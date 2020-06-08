#include "process_handler.h"
#include <sys/ptrace.h>
#include <iostream>
#include "utils.h"

namespace ljstack {
    ProcessHandler::ProcessHandler(pid_t pid) {
        pid_ = pid;
        elf_file_ = "/proc/" + std::to_string(pid) + "/exe";
        mem_file_ = "/proc/" + std::to_string(pid) + "/mem";
        maps_file_ = "/proc/" + std::to_string(pid) + "/maps";
        mem_fd_ = open(mem_file_.c_str(), O_RDONLY);
        status_ = RUNING;
        if (mem_fd_ < 0) {
            LOG_ERR("Can't open %s, maybe the process %d not exist!", mem_file_.c_str(), pid_);
            exit(-1);
        }
        get_elf_type();
    }

    ProcessHandler::~ProcessHandler() {
        close(mem_fd_);
    }

    int ProcessHandler::attach() {
        if (-1 == ptrace(PTRACE_ATTACH, pid_, nullptr, nullptr)) {
            LOG_ERR("attach process (%d) failed (%s)!", pid_, strerror(errno));
            return -1;
        }
        if (!wait4stop()) {
            LOG_ERR("wait process (%d) failed (%s)!", pid_, strerror(errno));
            return -1;
        }
        status_ = STOP;
        return 0;
    }

    int ProcessHandler::detach() {
        if (-1 == ptrace(PTRACE_DETACH, pid_, nullptr, nullptr)) {
            LOG_ERR("detach process (%d) failed (%s)!", pid_, strerror(errno));
            return -1;
        }
        status_ = RUNING;
        return 0;
    }
}
