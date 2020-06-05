//
// Created by bbkgl on 20-6-4.
//

#ifndef LJSTACK_PROCESS_HANDLER_H
#define LJSTACK_PROCESS_HANDLER_H

#include <signal.h>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <wait.h>

namespace ljstack {
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
                throw "read memory failed!";
        }

    private:
        int wait4stop() {
            int status = 99;
            do {
                if (waitpid(pid_, &status, 0) == -1 || WIFEXITED(status) || WIFSIGNALED(status))
                    return 0;
            } while(!WIFSTOPPED(status));
            return 1;
        }

        std::string mem_file_;
        int mem_fd_;
        pid_t pid_;
    };
}

#endif //LJSTACK_PROCESS_HANDLER_H
