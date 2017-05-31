#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct{
    uint8_t A;      //Accumulator A
    uint8_t B;      //Accumulator B
    uint16_t X;     //Index Register
    uint16_t PC;    //Program counter
    uint16_t SP;    //Stack pointer
    uint8_t IR;     //Instruction register
    
    struct{
        uint8_t H: 1;  //Half-carry
        uint8_t I: 1;  //Interrupt mask
        uint8_t N: 1;  //Negative
        uint8_t Z: 1;  //Zero
        uint8_t V: 1;  //Overflow
        uint8_t C: 1;  //Carry
    } CC;
    
    uint8_t Running;   //Keep running simulation thread?
} registers_t;


//Simulate hardware interrupt
void irq(registers_t* reg, uint8_t *mem);

//Simulate non-masked hardware interrupt
void irq_nonmasked(registers_t* reg, uint8_t *mem);

//Perform single simulation step/instruction
uint8_t do_step(registers_t *reg, uint8_t *mem);

//Initialise registers
void registers_init(registers_t *regs);

#endif