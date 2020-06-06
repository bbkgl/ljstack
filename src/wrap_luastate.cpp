#include <string>
#include "process_handler.h"
#include "wrap_luastate.h"

namespace ljstack {
    WrapLuaState::WrapLuaState(ProcessHandler *handler) {
        process_handler_ = handler;
        uintptr_t addr_globalL = process_handler_->get_addrbysymbol("globalL");
        if (!addr_globalL) {
            LOG_ERR("Can't found globalL...");
            exit(-1);
        }
        try {
            int ret = process_handler_->attach();
            if (ret < 0) {
                exit(-1);
            }
            globalL = process_handler_->geto((lua_State **)addr_globalL);
            ret = process_handler_->detach();
            if (ret < 0) {
                exit(-1);
            }
        } catch (const char *msg) {
            LOG_ERR("Can't get globalL...(%s)", msg);
            exit(-1);
        }
    }

    void WrapLuaState::update() {

    }
}
