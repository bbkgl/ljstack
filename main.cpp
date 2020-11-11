#include <iostream>
#include "utils.h"
#include "process_handler.h"
#include "wrap_luastate.h"
#include "lj_getframe.h"

using namespace ljstack;

int main() {
    LOG_OUT("LG_GC64:%d", LJ_GC64);
    auto handler = new ProcessHandler(12790);
    auto wrap_l = new WrapLuaState(handler);
    auto *geter = new LJGetFrame(wrap_l, handler);
    int size = 0;
    handler->attach();
    wrap_l->update();
    lua_State *L = wrap_l->getL();
    std::cout << geter->lj_debug_frame(L, 1, size) << std::endl;
    std::cout << geter->lj_debug_frame(L, 2, size) << std::endl;
    std::cout << geter->lj_debug_frame(L, 3, size) << std::endl;
    std::cout << geter->lj_debug_frame(L, 4, size) << std::endl;
    handler->detach();
    delete wrap_l;
    delete handler;
    return 0;
}
