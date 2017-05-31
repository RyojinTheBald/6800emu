#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>


#include "simulator.h"
#include "assembler.h"

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

    FILE *filePointer;
    
    filePointer = fopen("../test.txt", "r");
    
    if (filePointer == 0){
        printf("Error: Unable to open file\n");
        exit(-1);
    }
    else{
        char buffer[256];
    
        int len = 0;
    
        while (fgets(buffer, 256, filePointer) != NULL)
        {
            parseLine(buffer, strlen(buffer));
        }
        fclose(filePointer);
    }

    /*
    
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
    */
    
	return 0;
}


/*
 *  NOTE: Use wxWidgets for gui - same as codelite uses
 *
 */