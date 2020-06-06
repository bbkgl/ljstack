#ifndef LJSTACK_LUA_STRUCT_H
#define LJSTACK_LUA_STRUCT_H

// 在cmake中控制
// #define LJ_GC64

#if LJ_GC64
#define LJ_FR2			1
#else
#define LJ_FR2      0
#endif

/* GCobj reference */
typedef struct GCRef {
#if LJ_GC64
    uint64_t gcptr64;	/* True 64 bit pointer. */
#else
    uint32_t gcptr32;    /* Pseudo 32 bit pointer. */
#endif
} GCRef;

/* Memory reference */
typedef struct MRef {
#if LJ_GC64
    uint64_t ptr64;	/* True 64 bit pointer. */
#else
    uint32_t ptr32;    /* Pseudo 32 bit pointer. */
#endif
} MRef;

/* Common GC header for all collectable objects. */
#define GCHeader    GCRef nextgc; uint8_t marked; uint8_t gct

typedef uint32_t MSize;

/* Frame link. */
typedef union {
    int32_t ftsz;        /* Frame type and size of previous frame. */
    MRef pcr;        /* Or PC for Lua frames. */
} FrameLink;

/* Tagged value. */
#define LUA_NUMBER        double
#define lua_Number LUA_NUMBER
#define LJ_ENDIAN_LOHI(lo, hi)        lo hi
#define LJ_ALIGN(n)    __attribute__((aligned(n)))
/* Tagged value. */
typedef LJ_ALIGN(8) union TValue {
    uint64_t u64;        /* 64 bit pattern overlaps number. */
    lua_Number n;        /* Number object overlaps split tag/value object. */
#if LJ_GC64
    GCRef gcr;		/* GCobj reference with tag. */
  int64_t it64;
  struct {
    LJ_ENDIAN_LOHI(
      int32_t i;	/* Integer value. */
    , uint32_t it;	/* Internal object tag. Must overlap MSW of number. */
    )
  };
#else
    struct {
        LJ_ENDIAN_LOHI(
                union {
                    GCRef gcr;    /* GCobj reference (if any). */
                    int32_t i;    /* Integer value. */
                };, uint32_t it;    /* Internal object tag. Must overlap MSW of number. */
        )
    };
#endif
#if LJ_FR2
    int64_t ftsz;		/* Frame type and size of previous frame, or PC. */
#else
    struct {
        LJ_ENDIAN_LOHI(
                GCRef func;    /* Function for next frame (or dummy L). */
        , FrameLink tp;    /* Link to previous frame. */
        )
    } fr;
#endif
    struct {
        LJ_ENDIAN_LOHI(
                uint32_t lo;    /* Lower 32 bits of number. */
        , uint32_t hi;    /* Upper 32 bits of number. */
        )
    } u32;
} TValue;

typedef const TValue cTValue;

/* Per-thread state object. */
struct lua_State {
    GCHeader;
    uint8_t dummy_ffid;    /* Fake FF_C for curr_funcisL() on dummy frames. */
    uint8_t status;    /* Thread status. */
    MRef glref;        /* Link to global state. */
    GCRef gclist;        /* GC chain. */
    TValue *base;        /* Base of currently executing function. */
    TValue *top;        /* First free slot in the stack. */
    MRef maxstack;    /* Last free slot in the stack. */
    MRef stack;        /* Stack base. */
    GCRef openupval;    /* List of open upvalues in the stack. */
    GCRef env;        /* Thread environment (table of globals). */
    void *cframe;        /* End of C stack frame chain. */
    MSize stacksize;    /* True stack size (incl. LJ_STACK_EXTRA). */
};

#endif //LJSTACK_LUA_STRUCT_H
