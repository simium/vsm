#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define DEBUG 1

typedef enum {
    PUSH,
    POP,
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    LOAD,
    STORE,
    CMP,
    JMP,
    JZ,
    JNZ,
    JE,
    JNE,
    JGT,
    JLT,
    JGE,
    JLE,
    DUP,
    HALT
} InstructionSet;

typedef enum {
    A, B, C, D, E, F, SP, PC, SF, ZF,
    NUM_OF_REGISTERS
} Registers;

static int16_t registers[NUM_OF_REGISTERS];

const static int16_t program[] = {
    PUSH, 25,
    PUSH, 10,
    MUL,
    PUSH, 50,
    ADD,
    POP,
    PUSH, 10,
    DUP,
    MUL,
    STORE, A,
    POP,
    LOAD, A,
    JMP, 11,
    HALT
};

#define sp (registers[SP])
#define pc (registers[PC])
#define sf (registers[SF])
#define zf (registers[ZF])

static bool running = true;
static int16_t stack[256];

static uint16_t fetch();
static void eval(int16_t instruction);

int main(int argc, char **argv)
{
    sp = -1;
    pc = 0;

    while(running)
    {
        eval(fetch());
        pc++;
    }

    return 0;
}

static uint16_t fetch()
{
    return program[pc];
}

static void eval(int16_t instruction)
{
    int16_t pop_val = 0;
    int16_t op_x = 0;
    int16_t op_y = 0;
    int16_t op_res = 0;
    int16_t op_reg = 0;

    switch(instruction)
    {
    case HALT:
        running = false;
        break;
    case PUSH:
        stack[++sp] = program[++pc];
        break;
    case POP:
        pop_val = stack[sp--];
        printf("POP: [%d]\n", pop_val);
        break;
    case DUP:
        op_x = stack[sp--];
        stack[++sp] = op_x;
        stack[++sp] = op_x;
        break;
    case ADD:
        op_y = stack[sp--];
        op_x = stack[sp--];
        op_res = op_x+op_y;
        stack[++sp] = op_res;
        break;
    case SUB:
        op_y = stack[sp--];
        op_x = stack[sp--];
        op_res = op_x-op_y;
        stack[++sp] = op_res;
        break;
    case MUL:
        op_y = stack[sp--];
        op_x = stack[sp--];
        op_res = op_x*op_y;
        stack[++sp] = op_res;
        break;
    case DIV:
        op_y = stack[sp--];
        op_x = stack[sp--];
        op_res = op_x/op_y;
        stack[++sp] = op_res;
        break;
    case MOD:
        op_y = stack[sp--];
        op_x = stack[sp--];
        op_res = op_x%op_y;
        stack[++sp] = op_res;
        break;
    case LOAD:
        op_reg = program[++pc];
        op_x   = registers[op_reg];
        stack[++sp] = op_x;
        break;
    case STORE:
        op_reg = program[++pc];
        op_x = stack[sp--];
        registers[op_reg] = op_x;
        break;
    case CMP:
        op_y = stack[sp--];
        op_x = stack[sp--];
        op_res = op_x-op_y;
        break;
    case JMP:
        op_reg = program[++pc];
        pc = op_reg-1;
        break;
    case JZ:
    case JE:
        op_reg = program[++pc];
        if (zf)
            pc = op_reg-1;
        break;
    case JNZ:
    case JNE:
        op_reg = program[++pc];
        if (!zf)
            pc = op_reg-1;
        break;
    case JGT:
        op_reg = program[++pc];
        if (!sf)
            pc = op_reg-1;
        break;
    case JLT:
        op_reg = program[++pc];
        if (sf)
            pc = op_reg-1;
        break;
    case JGE:
        op_reg = program[++pc];
        if (!sf || zf)
            pc = op_reg-1;
        break;
    case JLE:
        op_reg = program[++pc];
        if (sf || zf)
            pc = op_reg-1;
        break;
    }

    switch(instruction)
    {
    case ADD:
    case SUB:
    case MUL:
    case DIV:
    case MOD:
    case CMP:
        zf = (op_res == 0) ? 1 : 0;
        sf = (op_res < 0) ? 1 : 0;
        break;
    }

#ifdef DEBUG
    /* Debug current state */
    int i;

    printf("STACK: [");
    for (i = 0; i<8; i++)
    {
        //printf("%04X ", stack[i]);
        printf("%02d ", stack[i]);
    }
    printf("], REGISTERS: [");

    for (i = SP; i<NUM_OF_REGISTERS; i++)
    {
        printf("%04X ", registers[i]);
    }
    printf("]\n");
#endif
}

