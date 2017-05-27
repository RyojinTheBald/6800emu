#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>


uint8_t program[] = {0xFE, 0x01, 0x09, 0xA6, 0x00, 0x27, 0x6F, 0xB7, 0x01, 0x0B, 0xC6, 0x30, 0x10, 0xB7, 0x01, 0x0C,
0x27, 0x12, 0xF6, 0x01, 0x0C, 0xB6, 0x01, 0x0B, 0xBD, 0x00, 0x2A, 0xBD, 0x00, 0x55, 0x7C, 0x01,
0x0A, 0x7E, 0x00, 0x00, 0x7C, 0x01, 0x0A, 0x7E, 0x00, 0x00, 0xBD, 0x00, 0x31, 0x5A, 0x26, 0xFA,
0x39, 0xFE, 0x01, 0x0F, 0xA7, 0x00, 0xBD, 0x00, 0x3A, 0x39, 0x36, 0xB6, 0x01, 0x0D, 0x4C, 0xB1,
0x01, 0x13, 0x22, 0x0A, 0xB7, 0x01, 0x0D, 0x32, 0x7C, 0x01, 0x10, 0x29, 0x21, 0x39, 0x7F, 0x01,
0x0D, 0xBD, 0x00, 0x55, 0x39, 0xB6, 0x01, 0x10, 0x37, 0xB0, 0x01, 0x0D, 0x33, 0x29, 0x13, 0x7F,
0x01, 0x0D, 0x7C, 0x01, 0x0E, 0xBB, 0x01, 0x13, 0xB7, 0x01, 0x10, 0x25, 0x01, 0x39, 0x7C, 0x01,
0x0F, 0x39, 0x7A, 0x01, 0x0F, 0x39, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x35, 0x31, 0x30, 0x31, 0x39, 0x38, 0x35, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFB,
0x00, 0xFB, 0x00, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};




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
} registers_t;



uint8_t one_compliment(uint8_t in)
{
    return in ^ 255;
}

uint8_t two_compliment(uint8_t in)
{
    return one_compliment(in) + 1;
}

void stack_push(registers_t *reg, uint8_t *mem, uint8_t val)
{
    //printf("stackpush @ %02X\n", reg->PC);
    mem[reg->SP--] = val;
}

uint8_t stack_pop(registers_t *reg, uint8_t *mem)
{
    //printf("stackpop @ %02X\n", reg->PC);
    return mem[++reg->SP];
}


void do_step(registers_t *reg, uint8_t *mem)
{
    switch(reg->IR)
    {
        case(0x01):     //NOP
            reg->PC++;
            break;
        case(0x06):     //TAP
            reg->CC.H = (reg->A & 0x32) >> 5;
            reg->CC.I = (reg->A & 0x16) >> 4;
            reg->CC.N = (reg->A & 0x08) >> 3;
            reg->CC.Z = (reg->A & 0x04) >> 2;
            reg->CC.V = (reg->A & 0x02) >> 1;
            reg->CC.C = reg->A & 0x01;
            reg->PC++;
            break;
        case(0x07):     //TPA
            reg->A = reg->CC.H << 5;
            reg->A += reg->CC.I << 4;
            reg->A += reg->CC.N << 3;
            reg->A += reg->CC.Z << 2;
            reg->A += reg->CC.V << 1;
            reg->A += reg->CC.C;
            reg->PC++;
            break;
        case(0x08):     //INX
            reg->X++;
            reg->PC++;
            break;
        case(0x09):     //DEX
            reg->X--;
            reg->PC++;
            break;
        case(0x0A):     //CLV
            reg->CC.V = 0;
            reg->PC++;
            break;
        case(0x0B):     //SEV
            reg->CC.V = 1;
            reg->PC++;
            break;
        case(0x0C):     //CLC
            reg->CC.C = 0;
            reg->PC++;
            break;
        case(0x0D):     //SEC
            reg->CC.C = 1;
            reg->PC++;
            break;
        case(0x0E):     //CLI
            reg->CC.I = 0;
            reg->PC++;
            break;
        case(0x0F):     //SEI
            reg->CC.I = 1;
            reg->PC++;
            break;
        case(0x10):     //SBA
        {
            uint8_t result = reg->A + two_compliment(reg->B);
            reg->CC.N = (result & 128) != 0;    //Set negative flag
            reg->CC.Z = (result == 0);            //Set zero flag
            reg->CC.C = (result & 128) != (reg->A & 128);   //Set carry flag
            reg->A = result;
            reg->PC++;
            break;
        }
        case(0x11):     //CBA
        {
            uint8_t result = reg->A + two_compliment(reg->B);
            reg->CC.N = (result & 128) != 0;    //Set negative flag
            reg->CC.Z = (result == 0);            //Set zero flag
            reg->CC.C = (result & 128) != (reg->A & 128);   //Set carry flag
            reg->PC++;
            break;
        }
        case(0x14):     //NBA
            reg->A &= reg->B;
            reg->CC.Z = (reg->A == 0);
            reg->PC++;
            break;
        case(0x96):     //LDA A - direct
        {
            reg->A = mem[mem[++reg->PC]];
            reg->PC++;
            break;
        }
        case(0xD6):     //LDA B - direct
        {
            reg->B = mem[mem[++reg->PC]];
            reg->PC++;
            break;
        }
        case(0xFE):     //LDX - extended
        {
            uint16_t addr = (mem[reg->PC + 1] << 8) + mem[reg->PC + 2];
            
            reg->X = (mem[addr] << 8) + mem[addr + 1];
            reg->PC += 3;
            break;
        }
        case(0xA6):     //LDAA - index
        {
            reg->A = mem[reg->X + mem[reg->PC + 1]];
            reg->PC += 2;
            break;
        }
        case(0x27):     //BEQ - direct
        {
            if(reg->CC.Z)
                reg->PC = (mem[reg->PC + 1] << 8) + mem[reg->PC + 2];
            else
                reg->PC += 2;
            break;
        }
        case(0xA7):     //STAA - index
        {
            mem[reg->X + mem[reg->PC + 1]] = reg->A;
            reg->PC += 2;
            break;
        }
        case(0xB6):     //LDAA - extended
        {
            uint16_t addr = (mem[reg->PC + 1] << 8) + mem[reg->PC + 2];
            reg->A = mem[addr];
            reg->PC += 3;
            break;
        }
        case(0xB7):     //STAA - extended
        {
            uint16_t addr = (mem[reg->PC + 1] << 8) + mem[reg->PC + 2];
            mem[addr] = reg->A;
            reg->PC += 3;
            break;
        }
        case(0xBD):     //JSR - extended
        {
            stack_push(reg, mem, (reg->PC & 0xFF));
            stack_push(reg, mem, ((reg->PC >> 8) & 0xFF));
            reg->PC = (mem[reg->PC + 1] << 8) + mem[reg->PC + 2];
            break;
        }
        case(0xC6):     //LDAB - immediate
        {
            reg->B = mem[reg->PC + 1];
            reg->PC += 2;
            break;
        }
        case(0x36):     //PSHA - inherent
        {
            stack_push(reg, mem, reg->A);
            reg->PC++;
            break;
        }
        case(0x4C):     //INCA - inherent
        {
            reg->A++;
            reg->PC++;
            break;
        }
        case(0xB1):     //CMPA - extended
        {
            uint16_t addr = (mem[reg->PC + 1] << 8) + mem[reg->PC + 2];
            uint8_t result = reg->A - mem[addr];
            reg->CC.N = (result & 128) != 0;    //Set negative flag
            reg->CC.Z = (result == 0);            //Set zero flag
            reg->CC.C = (result & 128) != (reg->A & 128);   //Set carry flag
            reg->PC += 3;
            break;
        }
        case(0x22):     //BHI - direct
        {
            if(reg->CC.C + reg->CC.Z == 0)
                reg->PC = mem[reg->PC + 1];
            else
                reg->PC += 2;
            break;
        }
        case(0x32):     //PULA - inherent
        {
            reg->A = stack_pop(reg, mem);
            reg->PC++;
            break;
        }
        case(0x7C):     //INC - extended
        {
            uint16_t addr = (mem[reg->PC + 1] << 8) + mem[reg->PC + 2];
            mem[addr]++;
            reg->PC += 3;
            break;
        }
        case(0x29):     //BVS - direct
        {
            if(reg->CC.V)
                reg->PC = mem[reg->PC + 1];
            else
                reg->PC += 2;
            break;
        }
        case(0x7E):     //JMP - extended
        {
            uint16_t addr = (mem[reg->PC + 1] << 8) + mem[reg->PC + 2];
            reg->PC = mem[addr];
            break;
        }
        case(0xF6):     //LDAB - extended
        {
            uint16_t addr = (mem[reg->PC + 1] << 8) + mem[reg->PC + 2];
            reg->B = mem[addr];
            reg->PC += 3;
            break;
        }
        case(0x39):     //RTS - inherent
        {
            uint16_t addr = stack_pop(reg, mem) << 8;
            addr += stack_pop(reg, mem);
            reg->PC = addr;
            break;
        }
        
        default:
            printf("Unimplemented opcode 0x%02X at 0x%04X\n", reg->IR, reg->PC);
            exit(-1);
            break;
            
    }
}


void registers_init(registers_t *regs)
{
    regs->A = 0;
    regs->B = 0;
    regs->X = 0;
    regs->PC = 0;
    regs->SP = 0xF000;
    regs->IR = 0;

    regs->CC.H = 0;
    regs->CC.I = 0;
    regs->CC.N = 0;
    regs->CC.Z = 0;
    regs->CC.V = 0;
    regs->CC.C = 0;

}



int main(int argc, char **argv)
{
    int stepping = 0;
    int breakpoint = 0x03;
	registers_t reg;
    uint8_t mem[0xFFFF] = {0};
    registers_init(&reg);
    
    //load program into memory
    memcpy(mem, program, 0x120);
    
    while(1)
    {
        //Fetch instruction
        reg.IR = mem[reg.PC];
        
        //exit when we reach a null opcode
        if (reg.IR == 0x00)
            break;
        
        do_step(&reg, mem);
        printf("A:0x%02X B:0x%02X PC:0x%04X X:0x%04X SP:0x%04X IR:0x%02X    H:%d I:%d N:%d Z:%d V:%d C:%d\n", reg.A, reg.B, reg.PC, reg.X, reg.SP, reg.IR, reg.CC.H, reg.CC.I, reg.CC.N, reg.CC.Z, reg.CC.V, reg.CC.C);
        
        if (breakpoint > 0 && !stepping && reg.PC == breakpoint)
        {
            stepping = 1;
            printf("Breakpoint reached, press any key to step execution\n");
        }
            
        if (stepping)
            getch();
    }

    
	return 0;
}


/*
 *  NOTE: Use wxWidgets for gui - same as codelite uses
 *
 */