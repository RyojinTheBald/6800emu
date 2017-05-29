#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>

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



#define ADDR_DIR()      (mem[reg->PC + 1])
#define ADDR_EXT()      ((mem[reg->PC + 1] << 8) + mem[reg->PC + 2])
#define ADDR_IND()      (reg->X + mem[reg->PC + 1])
#define ONE_COMP(in)    ((in) ^ 255)
#define TWO_COMP(in)    (ONE_COMP((in)) + 1)
#define STACK_PUSH(in)  (mem[reg->SP--] = (in))
#define STACK_POP()     (mem[++reg->SP])

//#define CALC_C(A, B, R)     (((A) & 0x80) && ((B) & 0x80)) + (((B) & 0x80) && (!((R) & 0x80))) + ((!((R) & 0x80)) && ((A) & 0x80))


#define CALC_C(A, B, R)     (!((A) & 0x80) && ((B) & 0x80)) + (((B) & 0x80) && ((R) & 0x80)) + (((R) & 0x80) && !((A) & 0x80))

#define CALC_V(A, B, R)     (((A) & 0x80) && ((B) & 0x80) && ((!((R) & 0x80)) + (!((A) & 0x80))) && ((!((B) & 0x80)) && ((R) & 0x80)))
#define CALC_Z(R)           ((R) == 0)
#define CALC_N(R)           (((R) & 0x80) != 0)
#define CALC_H(A, B, R)     ((((A) & 0x08) && ((B) & 0x08)) + (((B) & 0x08) && (!((R) & 0x08))) + ((!((R) & 0x08)) & ((A) & 0x08)))

void irq(registers_t* reg, uint8_t *mem)
{
    
}

void irq_nonmasked(registers_t* reg, uint8_t *mem)
{
    
}

void do_step(registers_t *reg, uint8_t *mem)
{
    uint8_t num_cycles = 0;
    
    uint16_t pc = reg->PC;
    uint16_t addr = 0;
    int32_t temp = -1;


    //0x00 - 0x0F
    //Condition code instructions (and friends)
    if ((reg->IR & 0xF0) == 0x00)
    {
        num_cycles = 2;
        reg->PC++;

        switch(reg->IR)
        {
            //NOP - No operation
            case(0x01):
                break;

            //TAP - CC = Accumulator A
            case(0x06):
                reg->CC.H = (reg->A & 0x32) >> 5;
                reg->CC.I = (reg->A & 0x16) >> 4;
                reg->CC.N = (reg->A & 0x08) >> 3;
                reg->CC.Z = (reg->A & 0x04) >> 2;
                reg->CC.V = (reg->A & 0x02) >> 1;
                reg->CC.C = reg->A & 0x01;
                break;
            
            //TPA - Accumulator A = CC
            case(0x07):
                reg->A = reg->CC.H << 5;
                reg->A += reg->CC.I << 4;
                reg->A += reg->CC.N << 3;
                reg->A += reg->CC.Z << 2;
                reg->A += reg->CC.V << 1;
                reg->A += reg->CC.C;
                break;


            //INX - Increment index register
            case(0x08):
                num_cycles = 4;
                reg->X++;
                
                reg->CC.Z = (reg->X == 0);
                break;

            //DEX - Decrement index register
            case(0x09):
                num_cycles = 4;
                reg->X--;

                reg->CC.Z = (reg->X == 0);
                break;

            
            //CLV - Clear overflow
            case(0x0A):
                reg->CC.V = 0;
                break;
            
            //SEV - Set overflow
            case(0x0B):
                reg->CC.V = 1;
                break;
            
            //CLC - Clear carry
            case(0x0C):
                reg->CC.C = 0;
                break;
            
            //SEC - Set carry
            case(0x0D):
                reg->CC.C = 1;
                break;
            
            //CLI - Clear interrupt
            case(0x0E):
                reg->CC.I = 0;
                break;
            
            //SEI - Set interrupt
            case(0x0F):
                reg->CC.I = 1;
                break;
        }
    }


    //0x10 - 0x1F
    //Dual accumulator instructions (and friend)
    else if ((reg->IR & 0xF0) == 0x10)
    {
        switch(reg->IR)
        {
            case(0x10):     //SBA
            {
                temp = reg->A + TWO_COMP(reg->B);
                reg->CC.N = (temp & 128) != 0;
                reg->CC.Z = (temp == 0);
                reg->CC.C = (temp & 128) != (reg->A & 128);
                reg->A = temp;
                reg->PC++;
                break;
            }
            
            //CBA - Compare accumulators
            case(0x11):
            {
                num_cycles = 2;
                uint8_t R = reg->A + TWO_COMP(reg->B);
                reg->CC.N = CALC_N(R);
                reg->CC.Z = CALC_Z(R);
                reg->CC.V = CALC_V(reg->A, reg->B, R);
                reg->CC.C = CALC_C(reg->A, reg->B, R);
                reg->PC++;
                break;
            }
            case(0x14):     //NBA
                reg->A &= reg->B;
                reg->CC.Z = (reg->A == 0);
                reg->PC++;
                break;
            
            //ABA - Add accumulator B to A
            case(0x1B):
            {
                uint8_t result = reg->A + reg->B;
                
                //Set condition-codes
                reg->CC.N = CALC_N(result);
                reg->CC.Z = CALC_Z(result);
                reg->CC.C = CALC_C(reg->A, reg->B, result);
                reg->CC.V = CALC_V(reg->A, reg->B, result);
                reg->CC.H = CALC_H(reg->A, reg->B, result);
                
                //Number of clock cycles required
                num_cycles = 2;
                
                reg->PC++;
                break;
            }
        }
    }
    
    //0x20 - 0x2F
    //Branch instruction
    else if ((reg->IR & 0xF0) == 0x20)
    {
        
        uint8_t tmp = mem[reg->PC + 1];
        
        int16_t addr_rel = mem[reg->PC + 1];    //Signed relative address
        
        if (tmp & 0x80)
        {
            addr_rel = -1 * (TWO_COMP(tmp));
        }
        
        reg->PC += 2;
        num_cycles = 4;

        switch(reg->IR)
        {
            //BRA - Branch always
            case(0x20):     
                reg->PC += addr_rel;
                break;
                
            //BHI - Branch if higher
            case(0x22):
                if((reg->CC.C + reg->CC.Z) == 0)
                    reg->PC += addr_rel;
                break;
                
            //BLS - Branch if lower or same
            case(0x23):
                if((reg->CC.C + reg->CC.Z) == 1)
                    reg->PC += addr_rel;
                break;
                
            //BCC - Branch if carry is clear
            case(0x24):
                if(reg->CC.C == 0)
                    reg->PC += addr_rel;
                break;
                
            //BCS - Branch if carry is set
            case(0x25):
                if(reg->CC.C == 1)
                    reg->PC += addr_rel;
                break;
                
            //BNE - Branch if not zero
            case(0x26):
                if(reg->CC.Z == 0)
                    reg->PC += addr_rel;
                break;
                
            //BEQ - Branch if equals zero
            case(0x27):
                if(reg->CC.Z == 1)
                    reg->PC += addr_rel;
                break;
                
            //BVC - Branch if overflow clear
            case(0x28):
                if(reg->CC.V == 0)
                    reg->PC += addr_rel;
                break;
                
            //BVS - Branch if overflow set
            case(0x29):
                if(reg->CC.V == 1)
                    reg->PC += addr_rel;
                break;
                
            //BPL - Branch if plus
            case(0x2A):
                if(reg->CC.N == 0)
                    reg->PC += addr_rel;
                break;
                
            //BMI - Branch if minus
            case(0x2B):
                if(reg->CC.N == 1)
                    reg->PC += addr_rel;
                break;
                
            //BGE - Branch if greater or equal to zero
            case(0x2C):
                if((reg->CC.N ^ reg->CC.V) == 0)
                    reg->PC += addr_rel;
                break;
                
            //BLT - Branch if less than zero
            case(0x2D):
                if((reg->CC.N ^ reg->CC.V) == 1)
                    reg->PC += addr_rel;
                break;
                
            //BGT - Branch if greater than zero
            case(0x2E):
                if((reg->CC.Z + (reg->CC.N ^ reg->CC.V)) == 0)
                    reg->PC += addr_rel;
                break;
                
            //BLE - Branch if less or equal to zero
            case(0x2F):
                if((reg->CC.Z + (reg->CC.N ^ reg->CC.V)) == 1)
                    reg->PC += addr_rel;
                break;
        }
    }

    //0x30 - 0x3F
    //Stack, subroutine, and interrupt instructions
    else if ((reg->IR & 0xF0) == 0x30)
    {
        switch(reg->IR)
        {
            
            case(0x36):     //PSHA - Implied
            {
                STACK_PUSH(reg->A);
                reg->PC++;
                break;
            }
            case(0x37):     //PSHB - B -> stack
            {
                STACK_PUSH(reg->B);
                reg->PC++;
                break;
            }

            case(0x32):     //PULA - Implied
            {
                reg->A = STACK_POP();
                reg->PC++;
                break;
            }
            case(0x33):     //PULB - Implied
            {
                reg->B = STACK_POP();
                reg->PC++;
                break;
            }


            //RTI - Return from interrupt (Implied)
            case(0x3B):
                break;
            
            //RTS - Return from subroutine (Implied)
            case(0x39):
                addr = STACK_POP() << 8;
                addr += STACK_POP();
                reg->PC = addr;
                break;
            
            //SWI - Software interrupt (Implied)
            case(0x3F):
                break;
            
            //WAI - Wait for interrupt (Implied)
            case(0x3E):
                break;

            //TXS - Index reg -> stack pointer (Implied)
            case(0x35):
                reg->SP = reg->X;
                break;
                
            //TSX - Stack pointer -> index reg (Implied)
            case(0x30):
                reg->X = reg->SP;
                break;

            //DES - Decrement stack pointer (Implied)
            case(0x34):
                reg->SP--;
                reg->PC++;
                break;

            //INS - Increment stack pointer (Implied)
            case(0x31):
                reg->SP++;
                reg->PC++;
                break;
        }
    }

    //0x40 - 0x7F
    //Single target instructions
    else if ((reg->IR & 0xC0) == 0x40)
    {
        uint8_t *X; //Target register
        
        switch(reg->IR & 0x30)
        {
            //Accumulator A
            case(0x00):
                X = &reg->A;
                num_cycles = 2;
                reg->PC++;
                break;
            
            //Accumulator B
            case(0x10):
                X = &reg->B;
                num_cycles = 2;
                reg->PC++;
                break;
            
            //Index mode
            case(0x20):
                X = &mem[ADDR_IND()];
                num_cycles = 7;
                reg->PC += 2;
                break;
            
            //Extended mode
            case(0x30):
                X = &mem[ADDR_EXT()];
                num_cycles = 6;
                reg->PC += 3;
                break;
        }
        
        switch(reg->IR & 0x0F)
        {
            //NEG - Negate, two's complement
            case(0x00):
            {
                uint8_t R = TWO_COMP(*X);
                
                reg->CC.N = CALC_N(R);
                reg->CC.Z = CALC_Z(R);
                reg->CC.V = CALC_V(0, TWO_COMP(*X), R);
                reg->CC.C = !(*X == 0);
                break;
            }
            
            //COM - Compliment, one's complement
            case(0x03):
            {
                uint8_t R = ONE_COMP(*X);
                
                reg->CC.N = CALC_N(R);
                reg->CC.Z = CALC_Z(R);
                reg->CC.V = 0;
                reg->CC.C = 1;
                break;
            }
            
            //LSR - Logical shift right
            case(0x04):
            {
                reg->CC.C = (*X & 0x01) != 0;
                
                uint8_t R = *X >> 1;
                
                reg->CC.N = 0;
                reg->CC.Z = CALC_Z(R);
                reg->CC.V = reg->CC.N ^ reg->CC.C;
                break;
            }
            
            //ROR - Rotate right
            case(0x06):
            {
                reg->CC.C = (*X & 0x01) != 0;
                
                uint8_t R = *X >> 1;
                R += reg->CC.C * 0x80;
                
                reg->CC.N = CALC_N(R);
                reg->CC.Z = CALC_Z(R);
                reg->CC.V = reg->CC.N ^ reg->CC.C;
                break;
            }
            
            //ASR - Arithmetic shift right
            case(0x07):
            {
                reg->CC.C = (*X & 0x01) != 0;
                
                uint8_t R = (*X >> 1) + (*X & 0x80);
                
                reg->CC.N = CALC_N(R);
                reg->CC.Z = CALC_Z(R);
                reg->CC.V = reg->CC.N ^ reg->CC.C;
                
                *X = R;
                break;
            }
            
            //ASL - Arithmetic shift left
            case(0x08):
            {
                reg->CC.C = (*X & 0x80) != 0;
                
                uint8_t R = *X << 1;
                
                reg->CC.N = CALC_N(R);
                reg->CC.Z = CALC_Z(R);
                reg->CC.V = reg->CC.N ^ reg->CC.C;
                
                *X = R;
                break;
            }
            
            //ROL - Rotate left
            case(0x09):
            {
                reg->CC.C = (*X & 0x80) != 0;
                
                uint8_t R = *X << 1;
                R += reg->CC.C;
                
                reg->CC.N = CALC_N(R);
                reg->CC.Z = CALC_Z(R);
                reg->CC.V = reg->CC.N ^ reg->CC.C;
                break;
            }

            //DEC - Decrement
            case(0x0A):
            {
                uint8_t R = *X + TWO_COMP(1);
                reg->CC.N = CALC_N(R);
                reg->CC.Z = CALC_Z(R);
                reg->CC.V = CALC_V(*X, TWO_COMP(1), R);
                
                *X = R;

                break;
            }
            
            //INC - Increment
            case(0x0C):
            {
                uint8_t R = *X + 1;
                reg->CC.N = CALC_N(R);
                reg->CC.Z = CALC_Z(R);
                reg->CC.V = CALC_V(*X, 1, R);
                
                *X = R;

                break;
            }

            //TST - Test
            case(0x0D):
            {
                reg->CC.N = CALC_N(*X);
                reg->CC.Z = CALC_Z(*X);
                reg->CC.V = 0;
                reg->CC.C = 0;
                break;
            }


            //JMP - Jump
            case(0x0E):
            {
                num_cycles -= 3;
                reg->PC = *X;
                break;
            }
            
            //CLR - Clear
            case(0x0F):
            {
                *X = 0;
                reg->CC.N = 0;
                reg->CC.Z = 1;
                reg->CC.V = 0;
                reg->CC.C = 0;

                break;
            }
        }
    }

    //0x80 - 0xFF
    //Dual target instructions
    else if ((reg->IR & 0x80) == 0x80)
    {
        
        //Select target accumulator
        uint8_t *Acc = (reg->IR & 0x40) ? &reg->B : &reg->A;
        
        //Determine addressing mode and get address
        switch(reg->IR & 0x30)
        {
            //Immediate
            case(0x00):
                addr = reg->PC + 1;
                reg->PC += 2;
                num_cycles = 2;
                break;
            
            //Direct
            case(0x10):
                addr = mem[reg->PC + 1];
                reg->PC += 2;
                num_cycles = 3;
                break;
            
            //Index
            case(0x20):
                addr = reg->X + mem[reg->PC + 1];
                reg->PC += 2;
                num_cycles = 5;
                break;
            
            //Extended
            case(0x30):
                addr = (mem[reg->PC + 1] << 8) + mem[reg->PC + 2];
                reg->PC += 3;
                num_cycles = 4;
                break;
        }
        
        
        //Determine operation
        switch(reg->IR & 0x0F)
        {

            //SUB - Subtract memory from accumulator
            case(0x00):
            {
                uint8_t tmp = (*Acc) + TWO_COMP(mem[addr]);
                reg->CC.N = (tmp & 0x80) != 0;
                reg->CC.Z = (tmp == 0);
                reg->CC.C = (tmp & 0x80) != (*Acc & 0x80);
                reg->CC.V = ((*Acc & 0x80) == (TWO_COMP(mem[addr]) & 0x80)) && ((*Acc & 0x80) != (tmp & 0x80));
                *Acc = tmp;
                break;
            }
            
            //CMP - Compare memory to accumulator
            case(0x01):
            {
                uint8_t R = *Acc + TWO_COMP(mem[addr]);
                
                reg->CC.N = CALC_N(R);
                reg->CC.Z = CALC_Z(R);
                reg->CC.C = CALC_C(*Acc, TWO_COMP(mem[addr]), R);
                
                //reg->CC.C = (!(*Acc & 0x80) && (mem[addr] & 0x80)) + ((mem[addr] & 0x80) && (R & 0x80)) + ((R & 0x80) && !(*Acc & 0x80));
                
                reg->CC.V = CALC_V(*Acc, TWO_COMP(mem[addr]), R);
                break;
            }
            
            //SBC - Subtract memory from accumulator, with carry
            case(0x02):
            {
                uint8_t tmp = (*Acc) + TWO_COMP(mem[addr] + reg->CC.C);
                reg->CC.N = (tmp & 0x80) != 0;
                reg->CC.Z = (tmp == 0);
                reg->CC.C = (tmp & 0x80) != (*Acc & 0x80);
                reg->CC.V = ((*Acc & 0x80) == (TWO_COMP(mem[addr]) & 0x80)) && ((*Acc & 0x80) != (tmp & 0x80));
                *Acc = tmp;
                break;
            }
            
            //AND - Logical AND against accumulator
            case(0x04):
            {
                uint8_t R = (*Acc) & mem[addr];

               
                //Set condition-codes
                reg->CC.N = CALC_N(R);
                reg->CC.Z = CALC_Z(R);
                reg->CC.V = 0;
                
                //Number of clock cycles required
                num_cycles = 2;
                
                //Extra cycle required for extended mode
                if (reg->IR & 0x30)
                    num_cycles = 3;
                
                reg->PC++;

                *Acc = R;
                break;
            }
            
            //BIT - Bit test - Logical and without assignment
            case(0x05):
            {
                uint8_t R = (*Acc) & mem[addr];
                reg->CC.N = CALC_N(R);
                reg->CC.Z = CALC_Z(R);
                reg->CC.V = 0;
                break;
            }
            
            //LDA - Load accumulator
            case(0x06):
            {
                uint8_t R = mem[addr];
                reg->CC.N = CALC_N(R);
                reg->CC.Z = CALC_Z(R);
                reg->CC.V = 0;
                *Acc = R;
                break;
            }
            
            //STA - Store accumulator
            case(0x07):
            {
                uint8_t tmp = *Acc;
                reg->CC.N = (tmp & 0x80) != 0;
                reg->CC.Z = (tmp == 0);
                reg->CC.V = 0;
                mem[addr] = tmp;
                break;
            }
            
            //EOR - Logical XOR, memory against accumulator
            case(0x08):
            {
                uint8_t tmp = (*Acc) ^ mem[addr];
                reg->CC.N = (tmp & 0x80) != 0;
                reg->CC.Z = (tmp == 0);
                reg->CC.V = 0;
                *Acc = tmp;
                break;
            }

            //ADC - Add memory to accumulator, with carry
            case(0x09):
            {
                uint8_t R = (*Acc) + mem[addr] + reg->CC.C;
                
                //Set condition-codes
                reg->CC.N = CALC_N(R);
                reg->CC.Z = CALC_Z(R);
                reg->CC.C = CALC_C(*Acc, mem[addr], R);
                reg->CC.V = CALC_V(*Acc, mem[addr], R);
                reg->CC.H = CALC_H(*Acc, mem[addr], R);
                
                //Number of clock cycles required
                num_cycles = 2;
                
                //Extra cycle required for extended mode
                if (reg->IR & 0x30)
                    num_cycles = 3;
                
                reg->PC++;

                *Acc = R;

                break;
            }

            //ORA - Logical inclusive-OR, memory against accumulator
            case(0x0A):
            {
                uint8_t tmp = (*Acc) | mem[addr];
                reg->CC.N = (tmp & 0x80) != 0;
                reg->CC.Z = (tmp == 0);
                reg->CC.V = 0;
                *Acc = tmp;
                break;
            }

            //ADD - Add memory to accumulator
            case(0x0B):
            {
                uint8_t R = (*Acc) + mem[addr];
                
                //Set condition-codes
                reg->CC.N = CALC_N(R);
                reg->CC.Z = CALC_Z(R);
                reg->CC.C = CALC_C(*Acc, mem[addr], R);
                reg->CC.V = CALC_V(*Acc, mem[addr], R);
                reg->CC.H = CALC_H(*Acc, mem[addr], R);
                
                //Number of clock cycles required
                num_cycles = 2;
                
                //Extra cycle required for extended mode
                if (reg->IR & 0x30)
                    num_cycles = 3;
                
                reg->PC++;

                *Acc = R;
                break;
            }
            
            //CPX - Compare index register to memory
            case(0x0C):
            {
                uint16_t tmp = reg->X - ((mem[addr] << 8) + mem[addr + 1]);
                reg->CC.N = (tmp & 0x8000) != 0;
                reg->CC.Z = (tmp == 0);

                //Set overflow flag - christ this is a mess
                reg->CC.V = (reg->X & 0x8000) && !(mem[addr] & 0x80) && (!(tmp & 0x8000) + !(reg->X & 0x8000)) && (mem[addr] & 0x80) && (tmp & 0x80);
                
                reg->X = tmp;
                
                //needed because of the extra length of immediate mode paramater
                if ((reg->IR & 0x30) == 0)
                    reg->PC++;
                    
                break;
            }
            
            //BSR/JSR - Branch to subroutine
            case(0x0D):
                num_cycles = 8;
                //8D - BSR
                //AD - JSR (index)
                //BD - JSR (extended)
                
                STACK_PUSH(reg->PC & 0xFF);
                STACK_PUSH((reg->PC >> 8) & 0xFF);
                
                if (reg->IR == 0x8D)    //BSR - relative branch
                    reg->PC += mem[addr];
                else                    //JSR - absolute jump
                    reg->PC = addr;
                
                break;
                
            //LDS/LDX - Load stack pointer/index register
            case(0x0E):
            {
                uint16_t *tmp = (reg->IR & 0x40) ? &reg->X : &reg->SP;
                *tmp = ((mem[addr] << 8) + mem[addr + 1]);

                reg->CC.N = (*tmp & 0x8000) != 0; //bit 15 == 1
                reg->CC.Z = (*tmp == 0);
                reg->CC.V = 0;
                
                break;
            }

            
            //STS/STX - Store stack pointer/index register
            case(0x0F):
            {
                uint16_t *tmp = (reg->IR & 0x40) ? &reg->X : &reg->SP;
                mem[addr] = ((*tmp >> 8) & 0xFF);
                mem[addr + 1] = (*tmp & 0xFF);


                reg->CC.N = (*tmp & 0x8000) != 0; //bit 15 == 1
                reg->CC.Z = (*tmp == 0);
                reg->CC.V = 0;
                
                break;
            }           
        }
    }
    
    if (pc == reg->PC)
    {
        printf("Unimplemented opcode 0x%02X at 0x%04X\n", reg->IR, reg->PC);
        exit(-1);
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

HANDLE consoleHandle;           //Console handle


void setCursorPosition(int x, int y)
{
    //Flush stdout buffer
    fflush(stdout);
    
    //Pack cursor coordinates into windows-friendly format
    COORD coord = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(consoleHandle, coord);
}

void cls()
{
    COORD coordScreen = {0, 0};
    DWORD cCharsWritten;
    CONSOLE_SCREEN_BUFFER_INFO bufferInfo;

    /* get the number of character cells in the current buffer */ 
    GetConsoleScreenBufferInfo(consoleHandle, &bufferInfo);
    int ConsoleSize = bufferInfo.dwSize.X * bufferInfo.dwSize.Y;

    /* fill the entire screen with blanks */ 
    FillConsoleOutputCharacter(consoleHandle, (TCHAR)' ', ConsoleSize, coordScreen, &cCharsWritten);

    /* now set the buffer's attributes accordingly */ 
    FillConsoleOutputAttribute(consoleHandle, bufferInfo.wAttributes, ConsoleSize, coordScreen, &cCharsWritten);

    setCursorPosition(0, 0);
}

void printScreen(uint8_t *mem)
{
    //cls();
    setCursorPosition(0,0);
    
    //54x20
    for (int y = 0; y < 20; y++)
    {
        for (int x = 0; x < 54; x++)
        {
            printf("%c", mem[0xFB00 + (y * 54) + x]);
        }
        printf("\n");
    }
    
}


int main(int argc, char **argv)
{
    
    consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    int stepping = 0;
    int breakpoint = -1; //0x003B;
    //breakpoint = 0x1E;
	registers_t reg;
    uint8_t mem[0x10000] = {0};
    registers_init(&reg);
    
    //load program into memory
    memcpy(mem, program, 0x120);
    
    while(1)
    {
        //Fetch instruction
        reg.IR = mem[reg.PC];

        //exit when we reach a null opcode
        if (reg.IR == 0x00)
        {
            printf("Reached null opcode, halting\n");
            break;
        }

        
        do_step(&reg, mem);


        printScreen(mem);
        printf("A:0x%02X B:0x%02X PC:0x%04X X:0x%04X SP:0x%04X IR:0x%02X    H:%d I:%d N:%d Z:%d V:%d C:%d\n", reg.A, reg.B, reg.PC, reg.X, reg.SP, reg.IR, reg.CC.H, reg.CC.I, reg.CC.N, reg.CC.Z, reg.CC.V, reg.CC.C);                


        if (!stepping && ((breakpoint > 0 && reg.PC == breakpoint)))
        {
            stepping = 1;
            printf("Breakpoint reached, press any key to step execution\n");
        }

            
        
            
        if (stepping)
        {
            getch();
            //stepping = false;
        }
    }

    
	return 0;
}


/*
 *  NOTE: Use wxWidgets for gui - same as codelite uses
 *
 */