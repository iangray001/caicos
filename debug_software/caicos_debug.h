/*
 * Debugging header which is included into every source file if "debug=true" is included in the
 * caicos configuration.
 */

int caicos_docall(int op, int argc, ...);
void caicos_mdump(volatile void *mem, int count);
void caicos_check(int a, int b);

#define TEST_FUNCTION(NAME, TYPE, PEEKMODE, POKEMODE, DIV) void NAME(volatile TYPE *memory, int offset) {\
        int i;\
        for(i = 0; i < TEST_SIZE; i++) memory[i] = i+offset;\
        caicos_check(docall(PEEKMODE, 1, ((int)(&memory[10])) / DIV), 10+offset);\
        caicos_check(docall(PEEKMODE, 1, ((int)(&memory[40])) / DIV), 40+offset);\
        caicos_check(docall(PEEKMODE, 1, ((int)(&memory[70])) / DIV), 70+offset);\
        caicos_docall(POKEMODE, 2, ((int)(&memory[10])) / DIV, 10+0x20+offset);\
        caicos_docall(POKEMODE, 2, ((int)(&memory[40])) / DIV, 40+0x20+offset);\
        caicos_docall(POKEMODE, 2, ((int)(&memory[70])) / DIV, 70+0x20+offset);\
        caicos_check(docall(PEEKMODE, 1, ((int)(&memory[10])) / DIV), 10+0x20+offset);\
        caicos_check(docall(PEEKMODE, 1, ((int)(&memory[40])) / DIV), 40+0x20+offset);\
        caicos_check(docall(PEEKMODE, 1, ((int)(&memory[70])) / DIV), 70+0x20+offset);\
        caicos_check(memory[10], 10+0x20+offset);\
        caicos_check(memory[40], 40+0x20+offset);\
        caicos_check(memory[70], 70+0x20+offset);\
}

#define TEST_FUNCTION_PROTOTYPE(NAME, TYPE) void NAME(volatile TYPE *memory, int offset);

TEST_FUNCTION_PROTOTYPE(caicos_test8, char)
TEST_FUNCTION_PROTOTYPE(caicos_test16, short)
TEST_FUNCTION_PROTOTYPE(caicos_test32, int)
