#include "simulator.h"


#define ONE_COMP(in)    ((in) ^ 255)
#define TWO_COMP(in)    (ONE_COMP((in)) + 1)
#define STACK_PUSH(in)  (mem[reg->SP--] = (in))
#define STACK_POP()     (mem[++reg->SP])


void irq(registers_t* reg, uint8_t *mem)
{
    //Interrupt mask not set, process interrupt
    if (!reg->CC.I)
    {
        reg->CC.I = 1;
        reg->Running = 1;
    }
}

void irq_nonmasked(registers_t* reg, uint8_t *mem)
{
    
}

uint8_t do_step(registers_t *reg, uint8_t *mem)
{
    uint8_t num_cycles = 0;
    
    uint16_t pc = reg->PC;

    //0x00 - 0x0F
    //Condition code instructions (and friends)
    if ((reg->IR & 0xF0) == 0x00)
    {
        reg->PC++;

        switch(reg->IR)
        {
            //NOP - No operation
            case(0x01):
                num_cycles = 2;
                break;

            //TAP - CC = Accumulator A
            case(0x06):
                num_cycles = 2;
                reg->CC.H = (reg->A & 0x32) >> 5;
                reg->CC.I = (reg->A & 0x16) >> 4;
                reg->CC.N = (reg->A & 0x08) >> 3;
                reg->CC.Z = (reg->A & 0x04) >> 2;
                reg->CC.V = (reg->A & 0x02) >> 1;
                reg->CC.C = reg->A & 0x01;
                break;
            
            //TPA - Accumulator A = CC
            case(0x07):
                num_cycles = 2;
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
                num_cycles = 2;
                reg->CC.V = 0;
                break;
            
            //SEV - Set overflow
            case(0x0B):
                num_cycles = 2;
                reg->CC.V = 1;
                break;
            
            //CLC - Clear carry
            case(0x0C):
                num_cycles = 2;
                reg->CC.C = 0;
                break;
            
            //SEC - Set carry
            case(0x0D):
                num_cycles = 2;
                reg->CC.C = 1;
                break;
            
            //CLI - Clear interrupt
            case(0x0E):
                num_cycles = 2;
                reg->CC.I = 0;
                break;
            
            //SEI - Set interrupt
            case(0x0F):
                num_cycles = 2;
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
            //SBA - Subtract reg->B from reg->A
            case(0x10):
            {
                num_cycles = 2;
                uint8_t A = reg->A, B = TWO_COMP(reg->B), R = A + B;
                
                reg->CC.N = (R & 0x80) != 0;
                reg->CC.Z = R == 0;
                reg->CC.V = ((A & 0x80) && !(B & 0x80) && !(R & 0x80)) + (!(A & 0x80) && (B & 0x80) && (R & 0x80));
                reg->CC.C = (!(A & 0x80) && (B & 0x80)) + ((B & 0x80) && (R & 0x80)) + ((R & 0x80) && !(A & 0x80));

                reg->A = R;
                reg->PC++;
                break;
            }
            
            //CBA - Compare accumulators
            case(0x11):
            {
                num_cycles = 2;
                uint8_t A = reg->A, B = TWO_COMP(reg->B), R = A + B;
                
                reg->CC.N = (R & 0x80) != 0;
                reg->CC.Z = R == 0;
                reg->CC.V = ((A & 0x80) && !(B & 0x80) && !(R & 0x80)) + (!(A & 0x80) && (B & 0x80) && (R & 0x80));
                reg->CC.C = (!(A & 0x80) && (B & 0x80)) + ((B & 0x80) && (R & 0x80)) + ((R & 0x80) && !(A & 0x80));

                reg->PC++;
                break;
            }
            /*
            case(0x14):     //NBA
                reg->A &= reg->B;
                reg->CC.Z = (reg->A == 0);
                reg->PC++;
                break;
            */
            
            //TAB - Transfer reg->A to reg->B
            case(0x16):
            {
                num_cycles = 2;
                reg->A = reg->B;
                
                reg->CC.N = (reg->A & 0x80) != 0;
                reg->CC.Z = reg->A == 0;
                reg->CC.V = 0;

                reg->PC++;
                break;
            }
            
            //TBA - Transfer reg->B to reg->A
            case(0x17):
            {
                num_cycles = 2;
                reg->B = reg->A;

                reg->CC.N = (reg->B & 0x80) != 0;
                reg->CC.Z = reg->B == 0;
                reg->CC.V = 0;
                
                reg->PC++;
                break;
            }
            
            //ABA - Add accumulator B to A
            case(0x1B):
            {
                num_cycles = 2;
                uint8_t A = reg->A, B = reg->B, R = A + B;
                
                reg->CC.N = (R & 0x80) != 0;
                reg->CC.Z = R == 0;
                reg->CC.V = ((A & 0x80) && (B & 0x80) && !(R & 0x80)) + (!(A & 0x80) && !(B & 0x80) && (R & 0x80));
                reg->CC.C = ((A & 0x80) && (B & 0x80)) + ((B & 0x80) && !(R & 0x80)) + (!(R & 0x80) && (A & 0x80));
                reg->CC.H = ((A & 0x08) && (B & 0x08)) + ((B & 0x08) && !(R & 0x08)) + (!(R & 0x08) && (A & 0x08));

                reg->A = R;

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
                num_cycles = 4;
                STACK_PUSH(reg->A);
                reg->PC++;
                break;
            }
            case(0x37):     //PSHB - B -> stack
            {
                num_cycles = 4;
                STACK_PUSH(reg->B);
                reg->PC++;
                break;
            }

            case(0x32):     //PULA - Implied
            {
                num_cycles = 4;
                reg->A = STACK_POP();
                reg->PC++;
                break;
            }
            case(0x33):     //PULB - Implied
            {
                num_cycles = 4;
                reg->B = STACK_POP();
                reg->PC++;
                break;
            }


            //RTI - Return from interrupt (Implied)
            case(0x3B):
            {
                num_cycles = 10;
                
                uint8_t CC = STACK_POP();

                reg->CC.H = (CC & 0x32) >> 5;
                reg->CC.I = (CC & 0x16) >> 4;
                reg->CC.N = (CC & 0x08) >> 3;
                reg->CC.Z = (CC & 0x04) >> 2;
                reg->CC.V = (CC & 0x02) >> 1;
                reg->CC.C = CC & 0x01;

                reg->B = STACK_POP();
                reg->A = STACK_POP();
                reg->X = (STACK_POP() << 8);
                reg->X += STACK_POP();
                reg->PC = STACK_POP() << 8;
                reg->PC += STACK_POP();
                break;
            }
            
            //RTS - Return from subroutine (Implied)
            case(0x39):
            {
                num_cycles = 5;
                reg->PC = STACK_POP() << 8;
                reg->PC += STACK_POP();
                break;
            }
            
            //SWI - Software interrupt (Implied)
            case(0x3F):
            {
                num_cycles = 12;
                
                //Save processor state to stack
                STACK_PUSH((reg->PC + 1) & 0x00FF);
                STACK_PUSH(((reg->PC + 1) & 0xFF00) >> 8);
                STACK_PUSH(reg->X & 0x00FF);
                STACK_PUSH((reg->X & 0xFF00) >> 8);
                STACK_PUSH(reg->A);
                STACK_PUSH(reg->B);

                uint8_t CC = reg->CC.H << 5;
                CC += reg->CC.I << 4;
                CC += reg->CC.N << 3;
                CC += reg->CC.Z << 2;
                CC += reg->CC.V << 1;
                CC += reg->CC.C;

                STACK_PUSH(CC);

                reg->CC.I = 1;
                
                //Jump to software interrupt pointer
                reg->PC = (mem[0x10000 - 5] << 8) + mem[0x10000 - 4];
            
                break;
            }
            
            //WAI - Wait for interrupt (Implied)
            case(0x3E):
            {
                num_cycles = 9;
                //Save processor state to stack
                STACK_PUSH((reg->PC + 1) & 0x00FF);
                STACK_PUSH(((reg->PC + 1) & 0xFF00) >> 8);
                STACK_PUSH(reg->X & 0x00FF);
                STACK_PUSH((reg->X & 0xFF00) >> 8);
                STACK_PUSH(reg->A);
                STACK_PUSH(reg->B);

                uint8_t CC = reg->CC.H << 5;
                CC += reg->CC.I << 4;
                CC += reg->CC.N << 3;
                CC += reg->CC.Z << 2;
                CC += reg->CC.V << 1;
                CC += reg->CC.C;

                STACK_PUSH(CC);

                //Jump to internal interrupt pointer
                reg->PC = (mem[0x10000 - 7] << 8) + mem[0x10000 - 6];
                
                //Halt execution to wait for interrupt
                reg->Running = 0;
            
                break;
        }
            //TXS - Index reg -> stack pointer (Implied)
            case(0x35):
                num_cycles = 4;
                reg->SP = reg->X;
                break;
                
            //TSX - Stack pointer -> index reg (Implied)
            case(0x30):
                num_cycles = 4;
                reg->X = reg->SP;
                break;

            //DES - Decrement stack pointer (Implied)
            case(0x34):
                num_cycles = 4;
                reg->SP--;
                reg->PC++;
                break;

            //INS - Increment stack pointer (Implied)
            case(0x31):
                num_cycles = 4;
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
                X = &mem[reg->X + mem[reg->PC + 1]];
                num_cycles = 7;
                reg->PC += 2;
                break;
            
            //Extended mode
            case(0x30):
                X = &mem[(mem[reg->PC + 1] << 8) + mem[reg->PC + 2]];
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

                reg->CC.N = (R & 0x80) != 0;
                reg->CC.Z = R == 0x00;
                reg->CC.V = *X == 0x80;
                reg->CC.C = *X != 0x00;
                
                break;
            }
            
            //COM - Compliment, one's complement
            case(0x03):
            {
                uint8_t R = ONE_COMP(*X);
                
                reg->CC.N = (R & 0x80) != 0;
                reg->CC.Z = R == 0x00;
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
                reg->CC.Z = R == 0x00;
                reg->CC.V = reg->CC.N ^ reg->CC.C;
                
                *X = R;
                break;
            }
            
            //ROR - Rotate right
            case(0x06):
            {
                reg->CC.C = (*X & 0x01) != 0;
                
                uint8_t R = *X >> 1;
                R += reg->CC.C * 0x80;
                
                reg->CC.N = (R & 0x80) != 0;
                reg->CC.Z = R == 0x00;
                reg->CC.V = reg->CC.N ^ reg->CC.C;
                
               *X = R; 
                break;
            }
            
            //ASR - Arithmetic shift right
            case(0x07):
            {
                reg->CC.C = (*X & 0x01) != 0;
                
                uint8_t R = (*X >> 1) + (*X & 0x80);
                
                reg->CC.N = (R & 0x80) != 0;
                reg->CC.Z = R == 0x00;
                reg->CC.V = reg->CC.N ^ reg->CC.C;
                
                *X = R;
                break;
            }
            
            //ASL - Arithmetic shift left
            case(0x08):
            {
                reg->CC.C = (*X & 0x80) != 0;
                
                uint8_t R = *X << 1;
                
                reg->CC.N = (R & 0x80) != 0;
                reg->CC.Z = R == 0x00;
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
                
                reg->CC.N = (R & 0x80) != 0;
                reg->CC.Z = R == 0x00;
                reg->CC.V = reg->CC.N ^ reg->CC.C;
                
                *X = R;
                break;
            }

            //DEC - Decrement
            case(0x0A):
            {
                uint8_t A = *X, B = TWO_COMP(1), R = A + B;
                
                reg->CC.N = (R & 0x80) != 0;
                reg->CC.Z = R == 0;
                reg->CC.V = (*X == 0x80) && (R == 0x7F);
                
                *X = R;
                break;
            }
            
            //INC - Increment
            case(0x0C):
            {
                uint8_t A = *X, B = 1, R = A + B;
                
                reg->CC.N = (R & 0x80) != 0;
                reg->CC.Z = R == 0;
                reg->CC.V = A == 0x7F;
                
                *X = R;
                break;
            }

            //TST - Test
            case(0x0D):
            {
                reg->CC.N = (*X & 0x80) != 0;
                reg->CC.Z = *X == 0;
                reg->CC.V = 0;
                reg->CC.C = 0;
                
                break;
            }


            //JMP - Jump
            case(0x0E):
            {
                num_cycles -= 3;
                reg->PC = X - mem; //get memory offset from variable pointer
                
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
        //target address
        uint16_t addr = 0;
        
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
                uint8_t A = *Acc, B = TWO_COMP(mem[addr]), R = A + B;
                
                reg->CC.N = (R & 0x80) != 0;
                reg->CC.Z = R == 0;
                reg->CC.V = (A & 0x80) && !(B & 0x80) && !(R & 0x80) && !(A & 0x80) && (B & 0x80) && (R & 0x80);
                reg->CC.C = (!(A & 0x80) && (B & 0x80)) + ((B & 0x80) && (R & 0x80)) + ((R & 0x80) && !(A & 0x80));
                
                *Acc = R;
                break;
            }
            
            //CMP - Compare memory to accumulator
            case(0x01):
            {
                uint8_t A = *Acc, B = TWO_COMP(mem[addr]), R = A + B;
                
                reg->CC.N = (R & 0x80) != 0;
                reg->CC.Z = R == 0;
                reg->CC.V = (A & 0x80) && !(B & 0x80) && !(R & 0x80) && !(A & 0x80) && (B & 0x80) && (R & 0x80);
                reg->CC.C = (!(A & 0x80) && (B & 0x80)) + ((B & 0x80) && (R & 0x80)) + ((R & 0x80) && !(A & 0x80));

                break;
            }
            
            //SBC - Subtract memory from accumulator, with carry
            case(0x02):
            {
                uint8_t A = *Acc, B = TWO_COMP(mem[addr] + reg->CC.C), R = A + B;
                
                reg->CC.N = (R & 0x80) != 0;
                reg->CC.Z = R == 0;
                reg->CC.V = (A & 0x80) && !(B & 0x80) && !(R & 0x80) && !(A & 0x80) && (B & 0x80) && (R & 0x80);
                reg->CC.C = (!(A & 0x80) && (B & 0x80)) + ((B & 0x80) && (R & 0x80)) + ((R & 0x80) && !(A & 0x80));
                
                *Acc = R;
                break;
            }
            
            //AND - Logical AND against accumulator
            case(0x04):
            {
                uint8_t A = *Acc, B = mem[addr], R = A & B;
                
                reg->CC.N = (R & 0x80) != 0;
                reg->CC.Z = R == 0;
                reg->CC.V = 0;

                *Acc = R;
                break;
            }
            
            //BIT - Bit test - Logical and without assignment
            case(0x05):
            {
                uint8_t A = *Acc, B = mem[addr], R = A & B;
                
                reg->CC.N = (R & 0x80) != 0;
                reg->CC.Z = R == 0;
                reg->CC.V = 0;
                
                break;
            }
            
            //LDA - Load accumulator
            case(0x06):
            {
                uint8_t R = mem[addr];
                
                reg->CC.N = (R & 0x80) != 0;
                reg->CC.Z = R == 0;
                reg->CC.V = 0;
                
                *Acc = R;
                break;
            }
            
            //STA - Store accumulator
            case(0x07):
            {
                uint8_t R = *Acc;
                
                reg->CC.N = (R & 0x80) != 0;
                reg->CC.Z = R == 0;
                reg->CC.V = 0;
                
                mem[addr] = R;
                break;
            }
            
            //EOR - Logical XOR, memory against accumulator
            case(0x08):
            {
                uint8_t R = (*Acc) ^ mem[addr];
                
                reg->CC.N = (R & 0x80) != 0;
                reg->CC.Z = (R == 0);
                reg->CC.V = 0;
                
                *Acc = R;
                break;
            }

            //ADC - Add memory to accumulator, with carry
            case(0x09):
            {
                uint8_t A = *Acc, B = mem[addr], R = A + B + reg->CC.C;
                
                reg->CC.N = (R & 0x80) != 0;
                reg->CC.Z = R == 0;
                reg->CC.V = ((A & 0x80) && (B & 0x80) && !(R & 0x80)) + (!(A & 0x80) && !(B & 0x80) && (R & 0x80));
                reg->CC.C = ((A & 0x80) && (B & 0x80)) + ((B & 0x80) && !(R & 0x80)) + (!(R & 0x80) && (A & 0x80));
                reg->CC.H = ((A & 0x08) && (B & 0x08)) + ((B & 0x08) && !(R & 0x08)) + (!(R & 0x08) && (A & 0x08));

                *Acc = R;
                break;
            }

            //ORA - Logical inclusive-OR, memory against accumulator
            case(0x0A):
            {
                uint8_t R = (*Acc) | mem[addr];
                
                reg->CC.N = (R & 0x80) != 0;
                reg->CC.Z = (R == 0);
                reg->CC.V = 0;
                
                *Acc = R;
                break;
            }

            //ADD - Add memory to accumulator
            case(0x0B):
            {
                uint8_t A = *Acc, B = mem[addr], R = A + B;
                
                reg->CC.N = (R & 0x80) != 0;
                reg->CC.Z = R == 0;
                reg->CC.V = ((A & 0x80) && (B & 0x80) && !(R & 0x80)) + (!(A & 0x80) && !(B & 0x80) && (R & 0x80));
                reg->CC.C = ((A & 0x80) && (B & 0x80)) + ((B & 0x80) && !(R & 0x80)) + (!(R & 0x80) && (A & 0x80));
                reg->CC.H = ((A & 0x08) && (B & 0x08)) + ((B & 0x08) && !(R & 0x08)) + (!(R & 0x08) && (A & 0x08));
                
                *Acc = R;
                break;
            }
            
            //CPX - Compare index register to memory
            case(0x0C):
            {
                uint16_t R = reg->X - ((mem[addr] << 8) + mem[addr + 1]);

                reg->CC.N = (R & 0x8000) != 0;
                reg->CC.Z = (R == 0);
                reg->CC.V = (reg->X & 0x8000) && !(mem[addr] & 0x80) && (!(R & 0x8000) + !(reg->X & 0x8000)) && (mem[addr] & 0x80) && (R & 0x80);
                
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
                uint16_t *R = (reg->IR & 0x40) ? &reg->X : &reg->SP;
                
                *R = ((mem[addr] << 8) + mem[addr + 1]);

                reg->CC.N = (*R & 0x8000) != 0; //bit 15 == 1
                reg->CC.Z = (*R == 0);
                reg->CC.V = 0;
                
                break;
            }

            
            //STS/STX - Store stack pointer/index register
            case(0x0F):
            {
                uint16_t *R = (reg->IR & 0x40) ? &reg->X : &reg->SP;
                
                mem[addr] = ((*R >> 8) & 0xFF);
                mem[addr + 1] = (*R & 0xFF);

                reg->CC.N = (*R & 0x8000) != 0; //bit 15 == 1
                reg->CC.Z = (*R == 0);
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
    return num_cycles;
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