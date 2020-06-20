#include <luajit21/lua_struct.h>
#include "lj_getframe.h"

namespace ljstack {
    LJGetFrame::LJGetFrame(WrapLuaState *wrap_l, ProcessHandler *handler) {
        wrap_l_ = wrap_l;
        handler_ = handler;
    }

    TValue *LJGetFrame::lj_debug_frame(lua_State *L, int level, int &size) {
        TValue *frame = nullptr;
        TValue *nextframe = nullptr;
        TValue *bot = tvref(L->stack);
        for (nextframe = frame = L->base - 1; frame > bot;) {
            if (frame_gc(frame) == obj2gco(wrap_l_->get_nowL()))
                level++;
            if (level-- == 0) {
                size = (int) (nextframe - frame);
                return frame;
            }
            nextframe = frame;
            if (frame_islua(frame))
                frame = frame_prevl(frame);
            else {
                if (frame_isvarg(frame))
                    level++;
                frame = frame_prevd(frame);
            }
        }
        size = level;
        return nullptr;
    }

}
