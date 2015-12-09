#ifndef __JUNIPEROPERATIONS_H
#define __JUNIPEROPERATIONS_H

#define OP_VERSION 0x00

#define OP_PEEK_8 0x01
#define OP_POKE_8 0x02

#define OP_PEEK_16 0x03
#define OP_POKE_16 0x04

#define OP_PEEK_32 0x05
#define OP_POKE_32 0x06

#define OP_PEEK_F 0x09
#define OP_POKE_F 0x0A

#define OP_PEEK_D 0x09
#define OP_POKE_D 0x0A

#define OP_WRITE_ARG 0x10
#define OP_CALL 0x20

#define OP_TEST_ARRAY_LEN 0x50
#define OP_TEST_ARRAY_SUM 0x51
#define OP_TEST_SYSCALL 0x52
#define OP_TEST_1234 1234
#define OP_TEST_CLASSREF 0x53

#endif
