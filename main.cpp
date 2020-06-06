#include <iostream>
#include "utils.h"
#include "process_handler.h"
#include "wrap_luastate.h"

using namespace ljstack;

int main() {
    LOG_OUT("LG_GC64:%d", LJ_GC64);

    ProcessHandler *handler = new ProcessHandler(9781);
    WrapLuaState *wrap_lua = new WrapLuaState(handler);
    delete wrap_lua;
    delete handler;
    return 0;
}