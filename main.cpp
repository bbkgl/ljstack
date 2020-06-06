#include <iostream>
#include "utils.h"
#include "process_handler.h"
#include "wrap_luastate.h"

using namespace ljstack;

int main() {
    ProcessHandler *handler = new ProcessHandler(10600);
    WrapLuaState *wrap_lua = new WrapLuaState(handler);
    delete wrap_lua;
    delete handler;
    return 0;
}