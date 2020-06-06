#include <iostream>
#include "utils.h"
#include "process_handler.h"
#include "wrap_luastate.h"

using namespace ljstack;

int main() {
    LOG_OUT("LG_GC64:%d", LJ_GC64);

    ProcessHandler *handler = new ProcessHandler(5153);
//    WrapLuaState *wrap_lua = new WrapLuaState(handler);
//    delete wrap_lua;
    handler->attach();
    LOG_OUT("Reg: 0x%016lx", handler->get_register(LJ_RDX));
    handler->detach();
    delete handler;
    return 0;
}