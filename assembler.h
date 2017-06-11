#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <inttypes.h>
#include <string>



typedef struct parseNode_s{
    struct parseNode_s *prev;
    struct parseNode_s *next;
    
    struct parseNode_s *target;
    
    //.org defined address
    int32_t address;
    
    //line label
    std::string label;
    
    //line number
    uint16_t linenum;
    
    //pointer to relevant opcode data
    std::string mnemonic;
    
    //string containing operand
    std::string operand;
} parseNode_t;



void doTreeParse(std::string line);

int parseLine(char *line, uint16_t length);
void dump();
void dumpTree();
uint16_t decodeOperand(char *operands);
#endif