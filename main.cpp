#include <iostream>
#include "utils.h"
#include "process_handler.h"
#include "wrap_luastate.h"

using namespace ljstack;

int main() {
    LOG_OUT("LG_GC64:%d", LJ_GC64);

    ProcessHandler *handler = new ProcessHandler(27877);
//    WrapLuaState *wrap_lua = new WrapLuaState(handler);
//    delete wrap_lua;
    int *ptr = (int *)0xfffffffffff;
    handler->attach();
    handler->geto(ptr);
    handler->detach();
    delete handler;
    return 0;
}