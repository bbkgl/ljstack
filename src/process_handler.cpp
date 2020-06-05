//
// Created by bbkgl on 20-6-4.
//

#include "process_handler.h"
#include <sys/ptrace.h>
#include <iostream>

ljstack::ProcessHandler::ProcessHandler(pid_t pid) {
    pid_ = pid;
    mem_file_ = "/proc/" + std::to_string(pid) + "/mem";
    mem_fd_ = open(mem_file_.c_str(), O_RDONLY);
}

ljstack::ProcessHandler::~ProcessHandler() {
    close(mem_fd_);
}

int ljstack::ProcessHandler::attach() {
    if (-1 == ptrace(PTRACE_ATTACH, pid_, nullptr, nullptr)) {
        std::cerr << "attach error!" << std::endl;
        return -1;
    }
    if (!wait4stop()) {
        std::cerr << "wait error!" << std::endl;
        return -1;
    }
    return 0;
}

int ljstack::ProcessHandler::detach() {
    if (-1 == ptrace(PTRACE_DETACH, pid_, nullptr, nullptr)) {
        std::cerr << "detach error!" << std::endl;
        return -1;
    }
    return 0;
}
