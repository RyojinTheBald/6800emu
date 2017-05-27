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



#define addr_dir()      (mem[reg->PC + 1])
#define addr_ext()      ((mem[reg->PC + 1] << 8) + mem[reg->PC + 2])
#define one_comp(in)    ((in) ^ 255)
#define two_comp(in)    (one_comp((in)) + 1)
#define stack_push(in)  (mem[reg->SP--] = (in))
#define stack_pop()     (mem[++reg->SP])

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
    
    switch(reg->IR)
    {
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
            temp = reg->A + two_comp(reg->B);
            reg->CC.N = (temp & 128) != 0;    //Set negative flag
            reg->CC.Z = (temp == 0);            //Set zero flag
            reg->CC.C = (temp & 128) != (reg->A & 128);   //Set carry flag
            reg->A = temp;
            reg->PC++;
            break;
        }
        case(0x11):     //CBA
        {
            temp = reg->A + two_comp(reg->B);
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
        case(0x96):     //LDA A - direct
        {
            reg->A = mem[addr_dir()];
            reg->PC++;
            break;
        }
        case(0xD6):     //LDA B - direct
        {
            reg->B = mem[addr_dir()];
            reg->PC +=2;
            break;
        }
        case(0xA6):     //LDAA - index
        {
            reg->A = mem[reg->X + mem[reg->PC + 1]];
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
            reg->A = mem[addr_ext()];
            reg->PC += 3;
            break;
        }
        case(0xB7):     //STAA - extended
        {
            mem[addr_ext()] = reg->A;
            reg->PC += 3;
            break;
        }
        case(0xC6):     //LDAB - immediate
        {
            reg->B = mem[reg->PC + 1];
            reg->PC += 2;
            break;
        }
        case(0x36):     //PSHA - Implied
        {
            stack_push(reg->A);
            reg->PC++;
            break;
        }
        case(0x4C):     //INCA - Implied
        {
            reg->A++;
            reg->PC++;
            break;
        }
        case(0xB1):     //CMPA - extended
        {
            temp = reg->A - mem[addr_ext()];
            reg->CC.N = (temp & 128) != 0;    //Set negative flag
            reg->CC.Z = (temp == 0);            //Set zero flag
            reg->CC.C = (temp & 128) != (reg->A & 128);   //Set carry flag
            reg->PC += 3;
            break;
        }
         case(0x32):     //PULA - Implied
        {
            reg->A = stack_pop();
            reg->PC++;
            break;
        }
        case(0x7C):     //INC - extended
        {
            mem[addr_ext()]++;
            reg->PC += 3;
            break;
        }
        case(0xF6):     //LDAB - extended
        {
            reg->B = mem[addr_ext()];
            reg->PC += 3;
            break;
        }

        /*
         * Branch if
         */
        
        //BRA - Branch always (Direct)
        case(0x20):     
            reg->PC = addr_dir();
            break;
            
        //BHI - Branch if higher (Direct)
        case(0x22):
            if((reg->CC.C + reg->CC.Z) == 0)
                reg->PC = addr_dir();
            else
                reg->PC += 2;
            break;
            
        //BLS - Branch if lower or same (Direct)
        case(0x23):
            if((reg->CC.C + reg->CC.Z) == 1)
                reg->PC = addr_dir();
            else
                reg->PC += 2;
            break;
            
        //BCC - Branch if carry is clear (Direct)
        case(0x24):
            if(reg->CC.C == 0)
                reg->PC = addr_dir();
            else
                reg->PC += 2;
            break;
            
        //BCS - Branch if carry is set (Direct)
        case(0x25):
            if(reg->CC.C == 1)
                reg->PC = addr_dir();
            else
                reg->PC += 2;
            break;
            
        //BNE - Branch if not zero (Direct)
        case(0x26):
            if(reg->CC.Z == 0)
                reg->PC = addr_dir();
            else
                reg->PC += 2;
            break;
            
        //BEQ - Branch if equals zero (Direct)
        case(0x27):
            if(reg->CC.Z == 1)
                reg->PC = addr_dir();
            else
                reg->PC += 2;
            break;
            
        //BVC - Branch if overflow clear (Direct)
        case(0x28):
            if(reg->CC.V == 0)
                reg->PC = addr_dir();
            else
                reg->PC += 2;
            break;
            
        //BVS - Branch if overflow set (Direct)
        case(0x29):
            if(reg->CC.V == 1)
                reg->PC = addr_dir();
            else
                reg->PC += 2;
            break;
            
        //BPL - Branch if plus (Direct)
        case(0x2A):
            if(reg->CC.N == 0)
                reg->PC = addr_dir();
            else
                reg->PC += 2;
            break;
            
        //BMI - Branch if minus (Direct)
        case(0x2B):
            if(reg->CC.N == 1)
                reg->PC = addr_dir();
            else
                reg->PC += 2;
            break;
            
        //BGE - Branch if greater or equal to zero (Direct)
        case(0x2C):
            if((reg->CC.N ^ reg->CC.V) == 0)
                reg->PC = addr_dir();
            else
                reg->PC += 2;
            break;
            
        //BLT - Branch if less than zero (Direct)
        case(0x2D):
            if((reg->CC.N ^ reg->CC.V) == 1)
                reg->PC = addr_dir();
            else
                reg->PC += 2;
            break;
            
        //BGT - Branch if greater than zero (Direct)
        case(0x2E):
            if((reg->CC.Z + (reg->CC.N ^ reg->CC.V)) == 0)
                reg->PC = addr_dir();
            else
                reg->PC += 2;
            break;
            
        //BLE - Branch if less or equal to zero (Direct)
        case(0x2F):
            if((reg->CC.Z + (reg->CC.N ^ reg->CC.V)) == 1)
                reg->PC = addr_dir();
            else
                reg->PC += 2;
            break;
        
        /*
         * Subroutine and interrupts
         */
         
        //BSR - Branch to subroutine
        case(0x8D):
            break;
        
        //JMP - Jump (index)
        case(0x6E):
            break;
        
        //JMP - Jump (extended)
        case(0x7E):
            reg->PC = mem[addr_ext()];
            break;
        
        //JSR - Jump to subroutine (index)
        case(0xAD):
            break;
        
        //JSR - Jump to subroutine (extended)
        case(0xBD):
            stack_push(reg->PC & 0xFF);
            stack_push((reg->PC >> 8) & 0xFF);
            reg->PC = (mem[reg->PC + 1] << 8) + mem[reg->PC + 2];
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
            addr = stack_pop() << 8;
            addr += stack_pop();
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
         
        /* CPX - Compare index register */
        
        //CPX - Compare index register (Immediate)
        case(0x8C):
            temp = mem[reg->PC + 1];
            reg->PC += 2;
            
        //CPX - Compare index register (Direct)
        case(0x9C):
            if(temp == -1)
            {
                temp = mem[addr_dir()];
                reg->PC += 2;
            }
            
        //CPX - Compare index register (Index)
        case(0xAC):
            if(temp == -1)
            {
                temp = mem[reg->X + addr_dir()];
                reg->PC += 2;
            }
            
        //CPX - Compare index register (Extended)
        case(0xBC):
            if(temp == -1)
            {
                temp = mem[addr_ext()];
                reg->PC += 3;
            }
            
            //Comparison value retrieved, update CC flags
            if(reg->X == temp)
            {
                reg->CC.N = 0;
                reg->CC.Z = 1;
                reg->CC.V = 0;
            }
            else if(reg->X < temp)
            {
                reg->CC.N = 1;
                reg->CC.Z = 0;
                reg->CC.V = 0;
            }
            else
            {
                reg->CC.N = 0;
                reg->CC.Z = 0;
                reg->CC.V = 0;
            }
            
            break;
            


        
        /* LDX - Load index register */
        
        //LDX - Load index register (Immediate)
        case(0xCE):
            temp = mem[reg->PC + 1];
            reg->PC += 2;
            
        //LDX - Load index register (Direct)
        case(0xDE):
            if(temp == -1)
            {
                temp = mem[addr_dir()];
                reg->PC += 2;
            }
            
        //LDX - Load index register (Index)
        case(0xEE):
            if(temp == -1)
            {
                temp = mem[reg->X + addr_dir()];
                reg->PC += 2;
            }
            
        //LDX - Load index register (Extended)
        case(0xFE):
            if(temp == -1)
            {
                temp = mem[addr_ext()];
                reg->PC += 3;
            }
        
            reg->X = temp & 0xFFFF;
            
            reg->CC.N = (reg->X & 0x8000) != 0; //bit 15 == 1
            reg->CC.Z = (reg->X == 0);
            reg->CC.V = 0;

            break;




        /* LDS - Load stack pointer */
        
        //LDS - Load stack pointer (Immediate)
        case(0x8E):
            temp = mem[reg->PC + 1];
            reg->PC += 2;
            
        //LDS - Load stack pointer (Direct)
        case(0x9E):
            if(temp == -1)
            {
                temp = mem[addr_dir()];
                reg->PC += 2;
            }
            
        //LDS - Load stack pointer (Index)
        case(0xAE):
            if(temp == -1)
            {
                temp = mem[reg->X + addr_dir()];
                reg->PC += 2;
            }
            
        //LDS - Load stack pointer (Extended)
        case(0xBE):
            if(temp == -1)
            {
                temp = mem[addr_ext()];
                reg->PC += 3;
            }
        
            reg->SP = temp & 0xFFFF;

            reg->CC.N = (reg->SP & 0x8000) != 0; //bit 15 == 1
            reg->CC.Z = (reg->SP == 0);
            reg->CC.V = 0;

            break;
       
        
        
        /* STX - Store index register */
        
        //STX - Store index register (Direct)
        case(0xDF):
            temp = addr_dir();
            reg->PC += 2;
            
        //STX - Store index register (Index)
        case(0xEF):
            if(temp == -1)
            {
                temp = reg->X + addr_dir();
                reg->PC += 2;
            }
            
        //STX - Store index register (Extended)
        case(0xFF):
            if(temp == -1)
            {
                temp = addr_ext();
                reg->PC += 3;
            }        
        
            mem[temp] = ((reg->X >> 8) & 0xFF);
            mem[temp + 1] = (reg->X & 0xFF);


            reg->CC.N = (reg->X & 0x8000) != 0; //bit 15 == 1
            reg->CC.Z = (reg->X == 0);
            reg->CC.V = 0;
            
            break;
        
    
        /* STS - Store stack pointer */
        
        //STS - Store stack pointer (Direct)
        case(0x9F):
            temp = addr_dir();
            reg->PC += 2;
            
        //STS - Store stack pointer (Index)
        case(0xAF):
            if(temp == -1)
            {
                temp = reg->X + addr_dir();
                reg->PC += 2;
            }
            
        //STS - Store stack pointer (Extended)
        case(0xBF):
            if(temp == -1)
            {
                temp = addr_ext();
                reg->PC += 3;
            }        
        
            mem[temp] = ((reg->SP >> 8) & 0xFF);
            mem[temp + 1] = (reg->SP & 0xFF);

            reg->CC.N = (reg->SP & 0x8000) != 0; //bit 15 == 1
            reg->CC.Z = (reg->SP == 0);
            reg->CC.V = 0;
            
            break;
        
        
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
        

        default:
            break;
            
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