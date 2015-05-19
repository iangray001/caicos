/*
 * The hardware memory layout is simplified from Jamaica's actual layout
 * so that it can be synthesised. The various pointer structures cannot
 * pass through the hardware tools so we have had to flatten them.
 * The porting layer is therefore responsible for ensuring that all memory
 * accesses still line up correctly.
 *
 * The offsets in this file are used by the porting layer to reference into
 * the structures maintained in the host's version of Jamaica.h.
 *
 * All values are word-offsets (i.e. the byte offset divided by 4)
 */


// Offsets into struct jamaica_thread
#define JAMAICA_OFFSET_M_GCENV 1
#define JAMAICA_OFFSET_M_CL 2
#define JAMAICA_OFFSET_M_CLI 3
#define JAMAICA_OFFSET_PLAINALLOC 31
#define JAMAICA_OFFSET_JAVASTACKSIZE 30

// Offsets into struct jamaica_GCEnv
#define JAMAICA_OFFSET_WHITE 1
#define JAMAICA_OFFSET_GREYLIST 0
