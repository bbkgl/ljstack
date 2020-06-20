#ifndef LJSTACK_PROCESS_HANDLER_H
#define LJSTACK_PROCESS_HANDLER_H

#include <csignal>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <wait.h>
#include <libelfin/elf/elf++.hh>
#include <elf.h>
#include <fstream>
#include <string>
#include <sys/ptrace.h>
#include <exception>
#include "utils.h"

namespace ljstack {
    class ProcessHandler;

    enum Register {
        LJ_RIP,
        LJ_RDX,
    };

    enum PStatus {
        RUNING,
        STOP,
    };

    class AccessException : std::exception {
        const char* what() const throw() {
            return "read memory failed!";
        }
    };

    class PtraceException : std::exception {
    public:
        PtraceException(std::string &&msg) {
            errmsg_ = "ptrace failed!" + msg;
        }

        const char* what() const throw() {
            return errmsg_.c_str();
        }

    private:
        std::string errmsg_;
    };

    class ProcessHandler {
    public:
        explicit ProcessHandler(pid_t pid);
        virtual ~ProcessHandler();

        int attach();

        int detach();

        template <class T>
        inline T geto(T *pointer) {
            T obj;
            lj_memcpy(pointer, &obj, sizeof(T));
            return obj;
        }

        inline void lj_memcpy(void *src, void *dst, size_t len) {
            long offset = lseek(mem_fd_, reinterpret_cast<uintptr_t>(src), SEEK_SET);
            long ret = read(mem_fd_, dst, len);
            if (errno != 0 || offset < 0 || ret < 0)
                throw AccessException();
        }

        inline uintptr_t get_register(Register reg) {
            long reg_value = 0;
            char regname[20];
            switch (reg) {
                case LJ_RIP:
                    reg_value = ptrace(PTRACE_PEEKUSER, pid_, 8 * REG_RIP, nullptr);
                    strcpy(regname, "RIP");
                    break;
                case LJ_RDX:
                    reg_value = ptrace(PTRACE_PEEKUSER, pid_, 8 * REG_RDX, nullptr);
                    strcpy(regname, "RIP");
                    break;
                default:
                    reg_value = -1;
                    strcpy(regname, "Unknown register");
                    break;
            }
            if (reg_value >= 0)
                return static_cast<uintptr_t>(reg_value);
            else {
                LOG_OUT("Get the register %s failed!", regname);
                return 0xffffffffffffffff;  // -1
            }
        }

        inline void get_elf_type() {
            int fd = open(elf_file_.c_str(), O_RDONLY);
            Elf64_Ehdr header;
            ssize_t ret = read(fd, &header, sizeof(Elf64_Ehdr));
            close(fd);
            if (fd > 0 && ret > 0) {
                switch (header.e_type) {
                    case ET_DYN:
                        LOG_OUT("ELF TYPE: DYN");
                        get_text_addr();
                        break;
                    case ET_EXEC:
                        LOG_OUT("ELF TYPE: EXEC");
                        break;
                    default:
                        LOG_OUT("ELF TYPE: OTHER");
                        break;
                }
            }
        }

        inline uintptr_t get_addrbysymbol(std::string &&symbol) {
            int fd = open(elf_file_.c_str(), O_RDONLY);
            elf::elf f(elf::create_mmap_loader(fd));
            for (auto &sec : f.sections()) {
                if (sec.get_hdr().type != elf::sht::symtab && sec.get_hdr().type != elf::sht::dynsym)
                    continue;
                for (auto sym : sec.as_symtab()) {
                    auto data = sym.get_data();
                    if (sym.get_name() == symbol && data.value)
                        return text_addr_ + data.value;
                }
            }
            close(fd);
            return 0;
        }

        inline int get_status() {
            return status_;
        }

    private:
        inline void get_text_addr() {
            std::ifstream maps_steam(maps_file_.c_str());
            char line[300];
            while (maps_steam.getline(line, 300)) {
                std::string sline(line);
                if (sline.find("r-xp") != std::string::npos) {
                    unsigned long pos = sline.find('-');
                    std::string addrstr = sline.substr(0, pos);
                    text_addr_ = strtoul(addrstr.c_str(), nullptr, 16);
                    return ;
                }
            }
            maps_steam.close();
        }

        int wait4stop() {
            int status = 99;
            do {
                if (waitpid(pid_, &status, 0) == -1 || WIFEXITED(status) || WIFSIGNALED(status))
                    return 0;
            } while(!WIFSTOPPED(status));
            return 1;
        }

        std::string mem_file_;
        std::string maps_file_;
        std::string elf_file_;
        int mem_fd_;
        pid_t pid_;
        uintptr_t text_addr_;

        PStatus status_;
    };
}

#endif //LJSTACK_PROCESS_HANDLER_H
