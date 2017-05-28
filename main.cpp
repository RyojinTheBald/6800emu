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
#define ONE_COMP(in)    ((in) ^ 255)
#define TWO_COMP(in)    (ONE_COMP((in)) + 1)
#define STACK_PUSH(in)  (mem[reg->SP--] = (in))
#define STACK_POP()     (mem[++reg->SP])


void irq(registers_t* reg, uint8_t *mem)
{
    
}

void irq_nonmasked(registers_t* reg, uint8_t *mem)
{
    
}

void do_step(registers_t *reg, uint8_t *mem)
{
    uint16_t pc = reg->PC;
    uint16_t addr = 0;
    int32_t temp = -1;
    
    //opcode uses accumulator and memory
    if ((reg->IR & 0x80) == 0x80)
    {
        
        //Select accumulator
        uint8_t *Acc = (reg->IR & 0x40) ? &reg->B : &reg->A;
        
        //Determine addressing mode and get address
        switch(reg->IR & 0x30)
        {
            //Immediate
            case(0x00):
                addr = reg->PC + 1;
                reg->PC += 2;
                break;
            
            //Direct
            case(0x10):
                addr = mem[reg->PC + 1];
                reg->PC += 2;
                break;
            
            //Index
            case(0x20):
                addr = reg->X + mem[reg->PC + 1];
                reg->PC += 2;
                break;
            
            //Extended
            case(0x30):
                addr = (mem[reg->PC + 1] << 8) + mem[reg->PC + 2];
                reg->PC += 3;
                break;
        }
        
        
        //Determine operation
        switch(reg->IR & 0x0F)
        {

            //SUB - Subtract memory from accumulator
            case(0x00):
            {
                uint8_t tmp = (*Acc) + TWO_COMP(mem[addr]);
                reg->CC.N = (tmp & 0x80) != 0;              //Set negative flag
                reg->CC.Z = (tmp == 0);                     //Set zero flag
                reg->CC.C = (tmp & 0x80) != (*Acc & 0x80);  //Set carry flag
                reg->CC.V = ((*Acc & 0x80) == (TWO_COMP(mem[addr]) & 0x80)) && ((*Acc & 0x80) != (tmp & 0x80));
                *Acc = tmp;
                break;
            }
            
            //CMP - Compare memory to accumulator
            case(0x01):
            {
                uint8_t tmp = *Acc + TWO_COMP(mem[addr]);
                reg->CC.N = (tmp & 0x80) != 0;              //Set negative flag
                reg->CC.Z = (tmp == 0);                     //Set zero flag
                reg->CC.C = (tmp & 0x80) != (*Acc & 0x80);  //Set carry flag
                reg->CC.V = ((*Acc & 0x80) == (TWO_COMP(mem[addr]) & 0x80)) && ((*Acc & 0x80) != (tmp & 0x80));
                break;
            }
            
            //SBC - Subtract memory from accumulator, with carry
            case(0x02):
            {
                uint8_t tmp = (*Acc) + TWO_COMP(mem[addr] + reg->CC.C);
                reg->CC.N = (tmp & 0x80) != 0;              //Set negative flag
                reg->CC.Z = (tmp == 0);                     //Set zero flag
                reg->CC.C = (tmp & 0x80) != (*Acc & 0x80);  //Set carry flag
                reg->CC.V = ((*Acc & 0x80) == (TWO_COMP(mem[addr]) & 0x80)) && ((*Acc & 0x80) != (tmp & 0x80));
                *Acc = tmp;
                break;
            }
            
            //AND - Logical AND against accumulator
            case(0x04):
            {
                uint8_t tmp = (*Acc) & mem[addr];
                reg->CC.N = (tmp & 0x80) != 0;              //Set negative flag
                reg->CC.Z = (tmp == 0);                     //Set zero flag
                reg->CC.V = 0;                              //Reset overflow flag
                *Acc = tmp;
                break;
            }
            
            //BIT - Bit test - Logical and without assignment
            case(0x05):
            {
                uint8_t tmp = (*Acc) & mem[addr];
                reg->CC.N = (tmp & 0x80) != 0;              //Set negative flag
                reg->CC.Z = (tmp == 0);                     //Set zero flag
                reg->CC.V = 0;                              //Reset overflow flag
                break;
            }
            
            //LDA - Load accumulator
            case(0x06):
            {
                uint8_t tmp = mem[addr];
                reg->CC.N = (tmp & 0x80) != 0;              //Set negative flag
                reg->CC.Z = (tmp == 0);                     //Set zero flag
                reg->CC.V = 0;                              //Reset overflow flag
                *Acc = tmp;
                break;
            }
            
            //STA - Store accumulator
            case(0x07):
            {
                uint8_t tmp = *Acc;
                reg->CC.N = (tmp & 0x80) != 0;              //Set negative flag
                reg->CC.Z = (tmp == 0);                     //Set zero flag
                reg->CC.V = 0;                              //Reset overflow flag
                mem[addr] = tmp;
                break;
            }
            
            //EOR - Logical XOR, memory against accumulator
            case(0x08):
            {
                uint8_t tmp = (*Acc) ^ mem[addr];
                reg->CC.N = (tmp & 0x80) != 0;              //Set negative flag
                reg->CC.Z = (tmp == 0);                     //Set zero flag
                reg->CC.V = 0;                              //Reset overflow flag
                *Acc = tmp;
                break;
            }

            //ADC - Add memory to accumulator, with carry
            case(0x09):
            {
                uint8_t tmp = (*Acc) + mem[addr] + reg->CC.C;
                reg->CC.N = (tmp & 0x80) != 0;              //Set negative flag
                reg->CC.Z = (tmp == 0);                     //Set zero flag
                reg->CC.C = (tmp & 0x80) != (*Acc & 0x80);  //Set carry flag
                reg->CC.V = ((*Acc & 0x80) == (TWO_COMP(mem[addr]) & 0x80)) && ((*Acc & 0x80) != (tmp & 0x80));
                /*
                 * TODO: Add half-carry flag logic
                 */
                *Acc = tmp;
                break;
            }

            //ORA - Logical inclusive-OR, memory against accumulator
            case(0x0A):
            {
                uint8_t tmp = (*Acc) | mem[addr];
                reg->CC.N = (tmp & 0x80) != 0;              //Set negative flag
                reg->CC.Z = (tmp == 0);                     //Set zero flag
                reg->CC.V = 0;                              //Reset overflow flag
                *Acc = tmp;
                break;
            }

            //ADD - Add memory to accumulator
            case(0x0B):
            {
                uint8_t tmp = (*Acc) + mem[addr];
                reg->CC.N = (tmp & 0x80) != 0;              //Set negative flag
                reg->CC.Z = (tmp == 0);                     //Set zero flag
                reg->CC.C = ((*Acc & 0x80) && (mem[addr] & 0x80)) + ((mem[addr] & 0x80) && !(tmp & 0x80)) + (!(tmp & 0x80) && (*Acc & 0x80));
                reg->CC.V = ((*Acc & 0x80) == (TWO_COMP(mem[addr]) & 0x80)) && ((*Acc & 0x80) != (tmp & 0x80));
                /*
                 * TODO: Add half-carry flag logic
                 */
                *Acc = tmp;
                break;
            }
            
            //CPX - Compare index register to memory
            case(0x0C):
            {
                uint16_t tmp = reg->X - ((mem[addr] << 8) + mem[addr + 1]);
                reg->CC.N = (tmp & 0x8000) != 0;              //Set negative flag
                reg->CC.Z = (tmp == 0);                     //Set zero flag

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
    else if ((reg->IR & 0xF0) == 0x20)    //Branch instruction
    {
        
        uint8_t tmp = mem[reg->PC + 1];
        
        int16_t addr_rel = mem[reg->PC + 1];
        
        if (tmp & 0x80)
        {
            addr_rel = -1 * (TWO_COMP(tmp));
        }
        
        //addr_rel = (reg->PC & 0x00FF) + mem[reg->PC + 1];
        
        
        
        reg->PC += 2;
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
                    reg->PC = addr_rel;
                break;
                
            //BLT - Branch if less than zero
            case(0x2D):
                if((reg->CC.N ^ reg->CC.V) == 1)
                    reg->PC = addr_rel;
                break;
                
            //BGT - Branch if greater than zero
            case(0x2E):
                if((reg->CC.Z + (reg->CC.N ^ reg->CC.V)) == 0)
                    reg->PC = addr_rel;
                break;
                
            //BLE - Branch if less or equal to zero
            case(0x2F):
                if((reg->CC.Z + (reg->CC.N ^ reg->CC.V)) == 1)
                    reg->PC = addr_rel;
                break;
        }
    }
    else
    {
        

        switch(reg->IR)
        {
            case(0x5A):     //DECB - Decrement B
            {
                reg->B--;
                reg->CC.N = (reg->B & 128) != 0;    //Set negative flag
                reg->CC.Z = (reg->B == 0);            //Set zero flag

                reg->PC++;
                break;
            }

            case(0x10):     //SBA
            {
                temp = reg->A + TWO_COMP(reg->B);
                reg->CC.N = (temp & 128) != 0;    //Set negative flag
                reg->CC.Z = (temp == 0);            //Set zero flag
                reg->CC.C = (temp & 128) != (reg->A & 128);   //Set carry flag
                reg->A = temp;
                reg->PC++;
                break;
            }
            case(0x11):     //CBA
            {
                temp = reg->A + TWO_COMP(reg->B);
                reg->CC.N = (temp & 128) != 0;    //Set negative flag
                reg->CC.Z = (temp == 0);            //Set zero flag
                reg->CC.C = (temp & 128) != (reg->A & 128);   //Set carry flag
                reg->PC++;
                break;
            }
            case(0x14):     //NBA
                reg->A &= reg->B;
                reg->CC.Z = (reg->A == 0);
                reg->PC++;
                break;
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

            case(0x4C):     //INCA - Implied
            {
                reg->A++;
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
            case(0x7C):     //INC - extended
            {
                uint8_t temp = mem[ADDR_EXT()] + 1;
                reg->CC.N = (temp & 128) != 0;    //Set negative flag
                reg->CC.Z = (temp == 0);            //Set zero flag
                reg->CC.C = (temp & 128) != (mem[ADDR_EXT()] & 128);   //Set carry flag
                
                mem[ADDR_EXT()] = temp;

                reg->PC += 3;
                break;
            }
            case(0x7F):     //CLR - extended
            {
                mem[ADDR_EXT()] = 0;
                reg->CC.N = 0;    //Set negative flag
                reg->CC.Z = 1;            //Set zero flag
                reg->CC.C = 0;   //Set carry flag


                reg->PC += 3;
                break;
            }

 
            /*
             * Subroutine and interrupts
             */
             
            //JMP - Jump (index)
            case(0x6E):
                break;
            
            //JMP - Jump (extended)
            case(0x7E):
                reg->PC++;
                reg->PC = mem[ADDR_EXT()];
                break;
            

            
           
            //NOP - No operation (Implied)
            case(0x01):
                reg->PC++;
                break;
            
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


            /* 
             * Index register and stack pointer 
             */



            //TXS - Index reg -> stack pointer (Implied)
            case(0x35):
                reg->SP = reg->X;
                break;
                
            //TSX - Stack pointer -> index reg (Implied)
            case(0x30):
                reg->X = reg->SP;
                break;

            //DEX - Decrement index register (Implied)
            case(0x09):
                reg->X--;
                reg->PC++;
                
                reg->CC.Z = (reg->X == 0);
                break;

            //DES - Decrement stack pointer (Implied)
            case(0x34):
                reg->SP--;
                reg->PC++;
                break;

            //INX - Increment index register (Implied)
            case(0x08):
                reg->X++;
                reg->PC++;
                
                reg->CC.Z = (reg->X == 0);
                break;
            
            //INS - Increment stack pointer (Implied)
            case(0x31):
                reg->SP++;
                reg->PC++;
                break;
            


            /* Condition code register manipulation */

            //TAP - CC = Accumulator A
            case(0x06):
                reg->CC.H = (reg->A & 0x32) >> 5;
                reg->CC.I = (reg->A & 0x16) >> 4;
                reg->CC.N = (reg->A & 0x08) >> 3;
                reg->CC.Z = (reg->A & 0x04) >> 2;
                reg->CC.V = (reg->A & 0x02) >> 1;
                reg->CC.C = reg->A & 0x01;
                reg->PC++;
                break;
            
            //TPA - Accumulator A = CC
            case(0x07):
                reg->A = reg->CC.H << 5;
                reg->A += reg->CC.I << 4;
                reg->A += reg->CC.N << 3;
                reg->A += reg->CC.Z << 2;
                reg->A += reg->CC.V << 1;
                reg->A += reg->CC.C;
                reg->PC++;
                break;
            
            //CLV - Clear overflow
            case(0x0A):
                reg->CC.V = 0;
                reg->PC++;
                break;
            
            //SEV - Set overflow
            case(0x0B):
                reg->CC.V = 1;
                reg->PC++;
                break;
            
            //CLC - Clear carry
            case(0x0C):
                reg->CC.C = 0;
                reg->PC++;
                break;
            
            //SEC - Set carry
            case(0x0D):
                reg->CC.C = 1;
                reg->PC++;
                break;
            
            //CLI - Clear interrupt
            case(0x0E):
                reg->CC.I = 0;
                reg->PC++;
                break;
            
            //SEI - Set interrupt
            case(0x0F):
                reg->CC.I = 1;
                reg->PC++;
                break;
            

            default:
                break;
                
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
    int breakpoint = -1;
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