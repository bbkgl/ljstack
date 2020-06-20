#ifndef LJSTACK_LUA_STRUCT_H
#define LJSTACK_LUA_STRUCT_H

// 在cmake中控制
// #define LJ_GC64

#include <cstdint>

#if LJ_GC64
#define LJ_FR2			1
#else
#define LJ_FR2      0
#endif

/* Types for handling bytecodes. Need this here, details in lj_bc.h. */
typedef uint32_t BCIns;  /* Bytecode instruction. */
typedef uint32_t BCPos;  /* Bytecode position. */
typedef uint32_t BCReg;  /* Bytecode register. */
typedef int32_t BCLine;  /* Bytecode line number. */

struct lua_State;

typedef int (*lua_CFunction) (lua_State *L);

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

/* GC header for generic access to common fields of GC objects. */
typedef struct GChead {
    GCHeader;
    uint8_t unused1;
    uint8_t unused2;
    GCRef env;
    GCRef gclist;
    GCRef metatable;
} GChead;

/* String object header. String payload follows. */
typedef struct GCstr {
    GCHeader;
    uint8_t reserved;	/* Used by lexer for fast lookup of reserved words. */
    uint8_t unused;
    MSize hash;		/* Hash of string. */
    MSize len;		/* Size of string. */
} GCstr;

typedef struct GCupval {
    GCHeader;
    uint8_t closed;	/* Set if closed (i.e. uv->v == &uv->u.value). */
    uint8_t immutable;	/* Immutable value. */
    union {
        TValue tv;		/* If closed: the value itself. */
        struct {		/* If open: double linked list, anchored at thread. */
            GCRef prev;
            GCRef next;
        };
    };
    MRef v;		/* Points to stack slot (open) or above (closed). */
    uint32_t dhash;	/* Disambiguation hash: dh1 != dh2 => cannot alias. */
} GCupval;

typedef struct GCproto {
    GCHeader;
    uint8_t numparams;	/* Number of parameters. */
    uint8_t framesize;	/* Fixed frame size. */
    MSize sizebc;		/* Number of bytecode instructions. */
#if LJ_GC64
    uint32_t unused_gc64;
#endif
    GCRef gclist;
    MRef k;		/* Split constant array (points to the middle). */
    MRef uv;		/* Upvalue list. local slot|0x8000 or parent uv idx. */
    MSize sizekgc;	/* Number of collectable constants. */
    MSize sizekn;		/* Number of lua_Number constants. */
    MSize sizept;		/* Total size including colocated arrays. */
    uint8_t sizeuv;	/* Number of upvalues. */
    uint8_t flags;	/* Miscellaneous flags (see below). */
    uint16_t trace;	/* Anchor for chain of root traces. */
    /* ------ The following fields are for debugging/tracebacks only ------ */
    GCRef chunkname;	/* Name of the chunk this function was defined in. */
    BCLine firstline;	/* First line of the function definition. */
    BCLine numline;	/* Number of lines for the function definition. */
    MRef lineinfo;	/* Compressed map from bytecode ins. to source line. */
    MRef uvinfo;		/* Upvalue names. */
    MRef varinfo;		/* Names and compressed extents of local variables. */
} GCproto;

/* Common header for functions. env should be at same offset in GCudata. */
#define GCfuncHeader \
  GCHeader; uint8_t ffid; uint8_t nupvalues; \
  GCRef env; GCRef gclist; MRef pc

typedef struct GCfuncC {
    GCfuncHeader;
    lua_CFunction f;	/* C function to be called. */
    TValue upvalue[1];	/* Array of upvalues (TValue). */
} GCfuncC;

typedef struct GCfuncL {
    GCfuncHeader;
    GCRef uvptr[1];	/* Array of _pointers_ to upvalue objects (GCupval). */
} GCfuncL;

typedef union GCfunc {
    GCfuncC c;
    GCfuncL l;
} GCfunc;

/* C data object. Payload follows. */
typedef struct GCcdata {
    GCHeader;
    uint16_t ctypeid;	/* C type ID. */
} GCcdata;

typedef struct GCtab {
    GCHeader;
    uint8_t nomm;		/* Negative cache for fast metamethods. */
    int8_t colo;		/* Array colocation. */
    MRef array;		/* Array part. */
    GCRef gclist;
    GCRef metatable;	/* Must be at same offset in GCudata. */
    MRef node;		/* Hash part. */
    uint32_t asize;	/* Size of array part (keys [0, asize-1]). */
    uint32_t hmask;	/* Hash part mask (size of hash part - 1). */
#if LJ_GC64
    MRef freetop;		/* Top of free elements. */
#endif
} GCtab;

/* Userdata object. Payload follows. */
typedef struct GCudata {
    GCHeader;
    uint8_t udtype;	/* Userdata type. */
    uint8_t unused2;
    GCRef env;		/* Should be at same offset in GCfunc. */
    MSize len;		/* Size of payload. */
    GCRef metatable;	/* Must be at same offset in GCtab. */
    uint32_t align1;	/* To force 8 byte alignment of the payload. */
} GCudata;

typedef union GCobj {
    GChead gch;
    GCstr str;
    GCupval uv;
    lua_State th;
    GCproto pt;
    GCfunc fn;
    GCcdata cd;
    GCtab tab;
    GCudata ud;
} GCobj;

#endif //LJSTACK_LUA_STRUCT_H
