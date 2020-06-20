#ifndef LJSTACK_WRAP_LUASTATE_H
#define LJSTACK_WRAP_LUASTATE_H

#include "lua_struct.h"
#include "process_handler.h"

namespace ljstack {
    class WrapLuaState {
    public:
        explicit WrapLuaState(ProcessHandler *handler);

        void update();

        inline lua_State *get_globalL() { return globalL_; }

        inline lua_State *get_nowL() { return nowL; }

        inline lua_State *getL() { return &contentL_; }

    private:
        ProcessHandler *process_handler_;

        lua_State *globalL_;

        lua_State *nowL;

        lua_State contentL_;
    };
}


#endif //LJSTACK_WRAP_LUASTATE_H
