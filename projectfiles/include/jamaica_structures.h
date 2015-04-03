/*
 * These are the structures that are used by Jamaica on the FPGA. These are drastically reduced from the normal version on the target
 * because the FPGA code doesn't have to deal with allocation, garbage collection, or threading.
 *
 * Ideally these would not even be needed (they are pure overhead), but they are referenced by the code from Jamaica Builder. Also
 * as they are not used, for the most part they will be optimised away by HLS.
 *
 * This header should only be included from jamaica.h
 */

typedef struct {
	jamaica_ref greyList;
	jamaica_ref white;
} jamaica_GCEnv;

typedef struct jamaica_Mutator {
	jamaica_GCEnv     *gcenv;
	jamaica_ref *cl;
	jamaica_int32 cli;
} jamaica_Mutator;


struct jamaica_VM {
	jamaica_ref array_class[12];
};

struct jamaica_thread {
	jamaica_Mutator m;
	struct jamaica_VM *vm;
	jamaica_int32 id;
	jamaica_int32 f;
	jamaica_ref cT;
	jamaica_ref calledMethod;
	jamaica_int32 javaStackSize;
	volatile jamaica_bool plainAlloc;
	volatile jamaica_int32 state;
};
