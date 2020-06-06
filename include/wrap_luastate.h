#ifndef LJSTACK_WRAP_LUASTATE_H
#define LJSTACK_WRAP_LUASTATE_H

#include "lua_struct.h"
#include "process_handler.h"

namespace ljstack {
    class WrapLuaState {
    public:
        explicit WrapLuaState(ProcessHandler *handler);

        void update();

    private:
        ProcessHandler *process_handler_;

        lua_State *globalL;
    };
}


#endif //LJSTACK_WRAP_LUASTATE_H
