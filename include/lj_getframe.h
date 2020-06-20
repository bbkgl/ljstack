#ifndef LJSTACK_LJ_GETFRAME_H
#define LJSTACK_LJ_GETFRAME_H

#include <luajit21/lua_struct.h>
#include "process_handler.h"
#include "lua_struct.h"
#include "wrap_luastate.h"

namespace ljstack {

    template <class T>
    T *mref(MRef r) {
#if LJ_GC64
        return ((T *)(void *)(r).ptr64);
#else
        return ((T *)(void *)(uintptr_t)((r).ptr32));
#endif
    }

    inline TValue *tvref(MRef r) {
        return mref<TValue>(r);
    }

    inline GCobj *gcref(GCRef r) {
        return (GCobj *)((uintptr_t)(r).gcptr32);
    }

    class LJGetFrame {
    public:
        explicit LJGetFrame(WrapLuaState *wrap_l, ProcessHandler *handler);

        TValue *lj_debug_frame(lua_State *L, int level, int &size);

        inline GCobj *obj2gco(lua_State *L) {
            return (GCobj *)L;
        }

        inline GCobj *frame_gc(TValue *f) {
            TValue rframe = handler_->geto(f);
            return gcref(rframe.fr.func);
        }

        inline BCIns *frame_pc(TValue *f) {
            TValue rframe = handler_->geto(f);
            return mref<BCIns>(rframe.fr.tp.pcr);
        }

        inline const BCReg bc_a(BCIns i) {
            return ((BCReg)((i >> 8) & 0xff));
        }

        inline TValue *frame_prevl(TValue *f) {
            BCIns *ins_ptr = frame_pc(f) - 1;
            BCIns ins = handler_->geto(ins_ptr);
            return (f - (1 + LJ_FR2 + bc_a(ins)));
        }

        inline TValue *frame_prevd(TValue *f) {
            return ((TValue *)((char *)(f) - frame_sized(f)));
        }

        inline TValue *frame_prev(TValue *f) {
            return (frame_islua(f)?frame_prevl(f):frame_prevd(f));
        }

        inline long frame_ftsz(TValue *f) {
            TValue rframe = handler_->geto(f);
            return ((long)(rframe.fr.tp.ftsz));
        }

        inline long frame_type(TValue *f) {
            return frame_ftsz(f) & FRAME_TYPE;
        }

        inline long frame_typep(TValue *f) {
            return frame_ftsz(f) & FRAME_TYPEP;
        }

        inline bool frame_isc(TValue *f) {
            return frame_type(f) == FRAME_C;
        }

        inline bool frame_islua(TValue *f) {
            return frame_type(f) == FRAME_LUA;
        }

        inline bool frame_isvarg(TValue *f) {
            return frame_typep(f) == FRAME_VARG;
        }

        inline long frame_sized(TValue *f) {
            return (frame_ftsz(f) & ~FRAME_TYPEP);
        }

        // 常量
        const int FRAME_TYPE = 3;
        const int FRAME_P = 4;
        const int FRAME_TYPEP = (FRAME_TYPE | FRAME_P);

        enum {
            FRAME_LUA, FRAME_C, FRAME_CONT, FRAME_VARG,
            FRAME_LUAP, FRAME_CP, FRAME_PCALL, FRAME_PCALLH
        };

    private:
        WrapLuaState *wrap_l_;
        ProcessHandler *handler_;
    };
}


#endif //LJSTACK_LJ_GETFRAME_H
