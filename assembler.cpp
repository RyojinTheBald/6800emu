#include "assembler.h"

#include <string.h>
#include <stdio.h>

#include <ctype.h>

#include <map>
#include <vector>
#include <tuple>
#include <string>

#include <iostream>

//case insensitive string comparison
int strcicmp(char const *a, char const *b)
{
    for (;; a++, b++) {
        int d = toupper(*a) - *b;
        if (d != 0 || !*a)
            return d;
    }
}

struct char_cmp { 
    bool operator () (const char *a, const char *b) const 
    {
        return stricmp(a,b)<0;
    } 
};


struct opData
{
    uint8_t imm;
    uint8_t dir;
    uint8_t idx;
    uint8_t ext;
    uint8_t inh;
    
    uint8_t immLen;
    uint8_t dirLen;
    uint8_t idxLen;
    uint8_t extLen;
    uint8_t inhLen;
};

//map of mnemonics to opcode data
typedef std::map<const char *, struct opData, char_cmp> opMap_t;

/* map format
 * {Mnemonic {immediate, direct, index, extended, inherent, respective instruction lengths}}
 * 0x00 = no opcode in this mode
 */

opMap_t opMap = {
    {"ADDA", { 0x8B, 0x9B, 0xAB, 0xBB, 0x00, 2, 2, 2, 3, 0}},
    {"ADDB", { 0xCB, 0xDB, 0xEB, 0xFB, 0x00, 2, 2, 2, 3, 0}},
    {"ABA",  { 0x00, 0x00, 0x00, 0x00, 0x1B, 0, 0, 0, 0, 1}},
    {"ADCA", { 0x89, 0x99, 0xA9, 0xB9, 0x00, 2, 2, 2, 3, 0}},
    {"ADCB", { 0xC9, 0xD9, 0xE9, 0xF9, 0x00, 2, 2, 2, 3, 0}},
    {"ANDA", { 0x84, 0x94, 0xA4, 0xB4, 0x00, 2, 2, 2, 3, 0}},
    {"ANDB", { 0xC4, 0xD4, 0xE4, 0xF4, 0x00, 2, 2, 2, 3, 0}},
    {"BITA", { 0x85, 0x95, 0xA5, 0xB5, 0x00, 2, 2, 2, 3, 0}},
    {"BITB", { 0xC5, 0xD5, 0xE5, 0xF5, 0x00, 2, 2, 2, 3, 0}},
    {"CLR",  { 0x00, 0x00, 0x6F, 0x7F, 0x00, 0, 0, 2, 3, 0}},
    {"CLRA", { 0x00, 0x00, 0x00, 0x00, 0x4F, 0, 0, 0, 0, 1}},
    {"CLRB", { 0x00, 0x00, 0x00, 0x00, 0x5F, 0, 0, 0, 0, 1}},
    {"CMPA", { 0x81, 0x91, 0xA1, 0xB1, 0x00, 2, 2, 2, 3, 0}},
    {"CMPB", { 0xC1, 0xD1, 0xE1, 0xF1, 0x00, 2, 2, 2, 3, 0}},
    {"CBA",  { 0x00, 0x00, 0x00, 0x00, 0x11, 0, 0, 0, 0, 1}},
    {"COM",  { 0x00, 0x00, 0x63, 0x73, 0x00, 0, 0, 2, 3, 0}},
    {"COMA", { 0x00, 0x00, 0x00, 0x00, 0x43, 0, 0, 0, 0, 1}},
    {"COMB", { 0x00, 0x00, 0x00, 0x00, 0x53, 0, 0, 0, 0, 1}},
    {"NEG",  { 0x00, 0x00, 0x60, 0x70, 0x00, 0, 0, 2, 3, 0}},
    {"NEGA", { 0x00, 0x00, 0x00, 0x00, 0x40, 0, 0, 0, 0, 1}},
    {"NEGB", { 0x00, 0x00, 0x00, 0x00, 0x50, 0, 0, 0, 0, 1}},
    {"DAA",  { 0x00, 0x00, 0x00, 0x00, 0x10, 0, 0, 0, 0, 1}},
    {"DEC",  { 0x00, 0x00, 0x6A, 0x7A, 0x00, 0, 0, 2, 3, 0}},
    {"DECA", { 0x00, 0x00, 0x00, 0x00, 0x4A, 0, 0, 0, 0, 1}},
    {"DECB", { 0x00, 0x00, 0x00, 0x00, 0x5A, 0, 0, 0, 0, 1}},
    {"EORA", { 0x88, 0x98, 0xA8, 0xB8, 0x00, 2, 2, 2, 3, 0}},
    {"EORB", { 0xC8, 0xD8, 0xE8, 0xF8, 0x00, 2, 2, 2, 3, 0}},
    {"INC",  { 0x00, 0x00, 0x6C, 0x7C, 0x00, 0, 0, 2, 3, 0}},
    {"INCA", { 0x00, 0x00, 0x00, 0x00, 0x4C, 0, 0, 0, 0, 1}},
    {"INCB", { 0x00, 0x00, 0x00, 0x00, 0x5C, 0, 0, 0, 0, 1}},
    {"LDAA", { 0x86, 0x96, 0xA6, 0xB8, 0x00, 2, 2, 2, 3, 0}},
    {"LDAB", { 0xC6, 0xD6, 0xE6, 0xF6, 0x00, 2, 2, 2, 3, 0}},
    {"ORAA", { 0x8A, 0x9A, 0xAA, 0xBA, 0x00, 2, 2, 2, 3, 0}},
    {"ORAB", { 0xCA, 0xDA, 0xEA, 0xFA, 0x00, 2, 2, 2, 3, 0}},
    {"PSHA", { 0x00, 0x00, 0x00, 0x00, 0x36, 0, 0, 0, 0, 1}},
    {"PSHB", { 0x00, 0x00, 0x00, 0x00, 0x37, 0, 0, 0, 0, 1}},
    {"PULA", { 0x00, 0x00, 0x00, 0x00, 0x32, 0, 0, 0, 0, 1}},
    {"PULB", { 0x00, 0x00, 0x00, 0x00, 0x33, 0, 0, 0, 0, 1}},
    {"ROL" , { 0x00, 0x00, 0x69, 0x79, 0x00, 0, 0, 2, 3, 0}},
    {"ROLA", { 0x00, 0x00, 0x00, 0x00, 0x49, 0, 0, 0, 0, 1}},
    {"ROLB", { 0x00, 0x00, 0x00, 0x00, 0x59, 0, 0, 0, 0, 1}},
    {"ROR" , { 0x00, 0x00, 0x66, 0x76, 0x00, 0, 0, 2, 3, 0}},
    {"RORA", { 0x00, 0x00, 0x00, 0x00, 0x46, 0, 0, 0, 0, 1}},
    {"RORB", { 0x00, 0x00, 0x00, 0x00, 0x56, 0, 0, 0, 0, 1}},
    {"ASL" , { 0x00, 0x00, 0x68, 0x78, 0x00, 0, 0, 2, 3, 0}},
    {"ASLA", { 0x00, 0x00, 0x00, 0x00, 0x48, 0, 0, 0, 0, 1}},
    {"ASLB", { 0x00, 0x00, 0x00, 0x00, 0x58, 0, 0, 0, 0, 1}},
    {"ASR" , { 0x00, 0x00, 0x67, 0x77, 0x00, 0, 0, 2, 3, 0}},
    {"ASRA", { 0x00, 0x00, 0x00, 0x00, 0x47, 0, 0, 0, 0, 1}},
    {"ASRB", { 0x00, 0x00, 0x00, 0x00, 0x57, 0, 0, 0, 0, 1}},
    {"LSR" , { 0x00, 0x00, 0x64, 0x74, 0x00, 0, 0, 2, 3, 0}},
    {"LSRA", { 0x00, 0x00, 0x00, 0x00, 0x44, 0, 0, 0, 0, 1}},
    {"LSRB", { 0x00, 0x00, 0x00, 0x00, 0x54, 0, 0, 0, 0, 1}},
    {"STAA", { 0x00, 0x97, 0xA7, 0xB7, 0x00, 0, 2, 2, 3, 0}},
    {"STAB", { 0x00, 0xD7, 0xE7, 0xF7, 0x00, 0, 2, 2, 3, 0}},
    {"SUBA", { 0x80, 0x90, 0xA0, 0xB0, 0x00, 2, 2, 2, 3, 0}},
    {"SUBB", { 0xC0, 0xD0, 0xE0, 0xF0, 0x00, 2, 2, 2, 3, 0}},
    {"SBA",  { 0x00, 0x00, 0x00, 0x00, 0x10, 0, 0, 0, 0, 1}},
    {"SBCA", { 0x82, 0x92, 0xA2, 0xB2, 0x00, 2, 2, 2, 3, 0}},
    {"SBCB", { 0xC2, 0xD2, 0xE2, 0xF2, 0x00, 2, 2, 2, 3, 0}},
    {"TAB",  { 0x00, 0x00, 0x00, 0x00, 0x16, 0, 0, 0, 0, 1}},
    {"TBA",  { 0x00, 0x00, 0x00, 0x00, 0x17, 0, 0, 0, 0, 1}},
    {"TST",  { 0x00, 0x00, 0x6D, 0x7D, 0x00, 0, 0, 2, 3, 0}},
    {"TSTA", { 0x00, 0x00, 0x00, 0x00, 0x4D, 0, 0, 0, 0, 1}},
    {"TSTB", { 0x00, 0x00, 0x00, 0x00, 0x5D, 0, 0, 0, 0, 1}},

    {"CPX",  { 0x8C, 0x9C, 0xAC, 0xBC, 0x00, 2, 2, 2, 3, 0}},
    {"DEX",  { 0x00, 0x00, 0x00, 0x00, 0x09, 0, 0, 0, 0, 1}},
    {"DES",  { 0x00, 0x00, 0x00, 0x00, 0x34, 0, 0, 0, 0, 1}},
    {"INX",  { 0x00, 0x00, 0x00, 0x00, 0x08, 0, 0, 0, 0, 1}},
    {"INS",  { 0x00, 0x00, 0x00, 0x00, 0x31, 0, 0, 0, 0, 1}},
    {"LDX",  { 0xCE, 0xDE, 0xEE, 0xFE, 0x00, 3, 2, 2, 3, 0}},
    {"LDS",  { 0x8E, 0x9E, 0xAE, 0xBE, 0x00, 3, 2, 2, 3, 0}},
    {"STX",  { 0x00, 0xDF, 0xEF, 0xFF, 0x00, 0, 2, 2, 3, 0}},
    {"STS",  { 0x00, 0x9F, 0xAF, 0xBF, 0x00, 0, 2, 2, 3, 0}},
    {"TXS",  { 0x00, 0x00, 0x00, 0x00, 0x35, 0, 0, 0, 0, 1}},
    {"TSX",  { 0x00, 0x00, 0x00, 0x00, 0x30, 0, 0, 0, 0, 1}},

    {"BRA",  { 0x00, 0x20, 0x00, 0x00, 0x00, 0, 2, 0, 0, 0}},
    {"BCC",  { 0x00, 0x24, 0x00, 0x00, 0x00, 0, 2, 0, 0, 0}},
    {"BCS",  { 0x00, 0x24, 0x00, 0x00, 0x00, 0, 2, 0, 0, 0}},
    {"BEQ",  { 0x00, 0x27, 0x00, 0x00, 0x00, 0, 2, 0, 0, 0}},
    {"BGE",  { 0x00, 0x2C, 0x00, 0x00, 0x00, 0, 2, 0, 0, 0}},
    {"BGT",  { 0x00, 0x2E, 0x00, 0x00, 0x00, 0, 2, 0, 0, 0}},
    {"BHI",  { 0x00, 0x22, 0x00, 0x00, 0x00, 0, 2, 0, 0, 0}},
    {"BLE",  { 0x00, 0x2F, 0x00, 0x00, 0x00, 0, 2, 0, 0, 0}},
    {"BLS",  { 0x00, 0x23, 0x00, 0x00, 0x00, 0, 2, 0, 0, 0}},
    {"BLT",  { 0x00, 0x2D, 0x00, 0x00, 0x00, 0, 2, 0, 0, 0}},
    {"BMI",  { 0x00, 0x2B, 0x00, 0x00, 0x00, 0, 2, 0, 0, 0}},
    {"BNE",  { 0x00, 0x26, 0x00, 0x00, 0x00, 0, 2, 0, 0, 0}},
    {"BVC",  { 0x00, 0x28, 0x00, 0x00, 0x00, 0, 2, 0, 0, 0}},
    {"BVS",  { 0x00, 0x29, 0x00, 0x00, 0x00, 0, 2, 0, 0, 0}},
    {"BPL",  { 0x00, 0x2A, 0x00, 0x00, 0x00, 0, 2, 0, 0, 0}},

    {"BSR",  { 0x00, 0x8D, 0x00, 0x00, 0x00, 0, 2, 0, 0, 0}},
    {"JMP",  { 0x00, 0x00, 0x6E, 0x7E, 0x00, 0, 0, 2, 3, 0}},
    {"JSR",  { 0x00, 0x00, 0xAD, 0xBD, 0x00, 0, 0, 2, 3, 0}},
    {"NOP",  { 0x00, 0x00, 0x00, 0x00, 0x01, 0, 0, 0, 0, 1}},
    {"RTI",  { 0x00, 0x00, 0x00, 0x00, 0x3B, 0, 0, 0, 0, 1}},
    {"RTS",  { 0x00, 0x00, 0x00, 0x00, 0x39, 0, 0, 0, 0, 1}},
    {"SWI",  { 0x00, 0x00, 0x00, 0x00, 0x3F, 0, 0, 0, 0, 1}},
    {"WAI",  { 0x00, 0x00, 0x00, 0x00, 0x3E, 0, 0, 0, 0, 1}},

    {"CLC",  { 0x00, 0x00, 0x00, 0x00, 0x0C, 0, 0, 0, 0, 1}},
    {"CLI",  { 0x00, 0x00, 0x00, 0x00, 0x0E, 0, 0, 0, 0, 1}},
    {"CLV",  { 0x00, 0x00, 0x00, 0x00, 0x0A, 0, 0, 0, 0, 1}},
    {"SEC",  { 0x00, 0x00, 0x00, 0x00, 0x0D, 0, 0, 0, 0, 1}},
    {"SEI",  { 0x00, 0x00, 0x00, 0x00, 0x0F, 0, 0, 0, 0, 1}},
    {"SEV",  { 0x00, 0x00, 0x00, 0x00, 0x0B, 0, 0, 0, 0, 1}},
    {"TAP",  { 0x00, 0x00, 0x00, 0x00, 0x06, 0, 0, 0, 0, 1}},
    {"TPA",  { 0x00, 0x00, 0x00, 0x00, 0x07, 0, 0, 0, 0, 1}}
};



typedef std::map<std::string, std::vector<uint16_t>> labelMap_t;
typedef std::map<std::string, uint16_t> resolvedLabels_t;


//Holds unresolved label references
labelMap_t labelMap;

//Holds addresses of resolved labels
resolvedLabels_t resolvedLabels;

uint8_t assmProgram[0x10000] = {0};
uint16_t PC = 0;



parseNode_t * rootNode = nullptr;
parseNode_t * tailNode = nullptr;



int parseLine(char *line, uint16_t length)
{
    /*
     * label: everything before first whitespace
     * mnemonic: between first and second whitespace
     * opperand(s): between second whitespace and ;
     * comment: after ;
     * 
     * full line comment: first char is ;
    */
     
    char *start = line;
    
    char label[256] = {0};
    char mnem[256] = {0};
    char operands[256] = {0};
    
    //skip full-line comment
    if (*line == ';')
    {
        while (*line != '\n')
            line++;
        return line - start;
    }
        
    
    //check for label
    if (!isspace(*line))
    {
        //find end of label
        while ((*line != ' ') && (*line != '\t') && (*line != '\n'))
        {
            if (*line == 0)
                return -1;
            line++;
        }
        
        //extract label
        strncpy(label, start, line - start);
    }
    
    if (*line == '\n')
    {
        return line - start + 1;
    }

    //skip whitespace
    while ((*line == ' ') || (*line == '\t'))
    {
        if (*line == 0)
            return -1;
        line++;
    }
     
    char *mnemStart = line;
    
    //skip to end of mnemonic
    while ((*line != ' ') && (*line != '\t') && (*line != '\n'))
    {
        if (*line == 0)
            return -1;
        line++;
    }
    
    //extract mnemonic
    strncpy(mnem, mnemStart, line - mnemStart);
    
    
    
    //check for end of line (in case of implicit mnemonic)
    if (*line == '\n')
        return line - start + 1;
    
    char *opStart = line;
    
    //find end of operand(s)
    while ((*line != ';') && (*line != '\t') && (*line != '\n'))
    {
        if (*line == 0)
            return -1;
        line++;
    }
    
    //extract operand(s)
    strncpy(operands, ++opStart, line - opStart);
    operands[line - opStart] = 0;
    
    //seek to end of line if stopped at comment
    if (*line != '\n')
        while(*line != '\n')
        {
            if (*line == 0)
                return -1;
            line++;
        }
    

    //assembler directive
    if (*mnem == '.')
    {
        printf("PC:0x%04X label:%s\tDirective:%s\t operands:\"%s\"\n", PC, label, mnem, operands);
        if (stricmp(mnem, ".org") == 0)
        {
            PC = decodeOperand(operands);
        }
        else if (stricmp(mnem, ".str") == 0)
        {
            char *tmp = opStart + 1;

            while ((*tmp != '\"') && (*tmp != '\0'))
            {
                assmProgram[PC++] = *(tmp++);
            }
            //Null terminate
            assmProgram[PC++] = 0;
        }
        else if (stricmp(mnem, ".byte") == 0)
        {
            assmProgram[PC++] = decodeOperand(opStart);
        }
        else if (stricmp(mnem, ".word") == 0)
        {
            uint16_t word = decodeOperand(opStart);
            assmProgram[PC++] = word >> 8;
            assmProgram[PC++] = word & 0x00ff;
        }
        return line - start;
    }
    else
    {
        printf("PC:0x%04X label:%s\tMnemonic:%s\tOperands:\"%s\"\n", PC, label, mnem, operands);
    
    }




    
    //Resolve label
    if (strlen(label) > 0)
    {
        resolvedLabels[label] = PC;
        
        //This label has unresolved references
        if (labelMap.count(label) > 0)
        {
            printf("Resolving dangling references to: %s\n", label);
            for (auto it = std::begin(labelMap[label]); it != std::end(labelMap[label]); ++it)
            {
                assmProgram[*it] = PC;
            }
            labelMap.erase(label);
        }
    }
    

    
    //determine address mode
    if (strlen(operands) == 0)
    {
        //inherent mode, no opperands
        assmProgram[PC] = opMap[mnem].inh;
        //printf(" %02X\n", assmProgram[PC]);
        PC += opMap[mnem].inhLen;
    }
    else if(*(operands + 1) == '#')
    {
        //Immediate mode, single byte data
        assmProgram[PC] = opMap[mnem].imm;
        
        //skip '#'
        assmProgram[PC + 1] = decodeOperand(operands + 1);
        //printf(" %02X\n", assmProgram[PC]);
        PC += opMap[mnem].immLen;
    }
    else if(*(operands + strlen(operands) - 1) == 'x')
    {
        //Index mode, single byte index offset
        assmProgram[PC] = opMap[mnem].idx;
        assmProgram[PC + 1] = decodeOperand(operands);
        //printf(" %02X\n", assmProgram[PC]);
        PC += opMap[mnem].idxLen;
    }
    else
    {
        //Direct, extended, or relative
        //if value > 255, must be extended, otherwise relative or direct
        
        //printf("Direct address\n");
        
        
        assmProgram[PC++] = opMap[mnem].dir;
        
        //label not in map, add
        if (labelMap.count(operands) == 0)
            labelMap[operands] = std::vector<uint16_t>();

        labelMap[operands].push_back(PC++);
    }
    
    
    
    //add opcode
    //assmProgram[PC++] = opMap[mnem]

    
    //printf("mnem:%s\timm:0x%02X\tidx:0x%02X\text:0x%02X\tinh:0x%02X\n", mnem, opMap[mnem].imm, opMap[mnem].idx, opMap[mnem].ext, opMap[mnem].inh);

    return line - start + 1;
}


uint16_t decodeOperand(char *operands)
{
    //Convert from hex
    if (*operands == '$')
    {
        char *ptr;
        return strtol(++operands, &ptr, 16);
    }
    else
        return atoi(operands);
}


std::map<std::string, std::vector<parseNode_t*>> unresolvedReferences;
std::map<std::string, parseNode_t*> referenceMap;


uint16_t linenum = 0;

void doTreeParse(std::string line)
{
    linenum++;
    //full-line comment, ignore
    if (line[0] == ';')
        return;
    
    //skip blank lines
    if(line.find_first_not_of(" \t\n") == std::string::npos)
        return;

    parseNode_t * thisNode = new parseNode_t();

    thisNode->linenum = linenum;

    //check for line label
    std::size_t firstSpace = line.find_first_of(" \t");
    if (firstSpace > 0)
    {
        thisNode->label = line.substr(0, firstSpace);
        
        //Add label to reference map
        if (referenceMap.count(thisNode->label) > 0)
        {
            std::cerr << "Error: Multiple definitions of label \"" << thisNode->label << "\"" << std::endl;
            return;
        }
        else
            referenceMap[thisNode->label] = thisNode;
        
        //Resolve references pointing at this label
        if (unresolvedReferences.count(thisNode->label) > 0)
        {
            for (auto it = unresolvedReferences[thisNode->label].begin(); it != unresolvedReferences[thisNode->label].end(); it++)
                (*it)->target = thisNode;
            unresolvedReferences.erase(thisNode->label);
        }
    }
    
    //get mnemonic
    std::size_t startMnem = line.find_first_not_of(" \t", ++firstSpace);
    std::size_t endMnem = line.find_first_of(" \t\n", startMnem);
    thisNode->mnemonic = line.substr(startMnem, endMnem - startMnem);

    //get operand
    std::size_t startOp = line.find_first_not_of(" \t", endMnem);
    std::size_t endOp = line.find_first_of(" \t;\n", startOp);
    if ((startOp != endOp) && (endOp != std::string::npos))
    {
        thisNode->operand = line.substr(startOp, endOp - startOp);
        
        //parse for target instruction line
        if ((thisNode->operand[0] >= 'a' && thisNode->operand[0] <= 'z') || (thisNode->operand[0] >= 'A' && thisNode->operand[0] <= 'Z'))
        {
            if (referenceMap.count(thisNode->operand) > 0)
                thisNode->target = referenceMap[thisNode->operand];
            else
                unresolvedReferences[thisNode->operand].push_back(thisNode);
        }
        
    }

    if (rootNode == nullptr)
    {
        rootNode = thisNode;
        tailNode = thisNode;
    }
    else
    {
        thisNode->prev = tailNode;
        tailNode->next = thisNode;
        tailNode = thisNode;
    }
}



void dumpTree()
{
    parseNode_t * node = rootNode;
    while (node->next != nullptr)
    {
        std::cout << node->linenum << " " << node->label << "\t" << node->mnemonic << "\t" << node->operand;
        if (node->target != nullptr)
            std::cout << "\ttargetting:" << node->target->mnemonic << "(" << node->target->linenum << ")";
        node = node->next;
        std::cout << std::endl;
    }
    
    std::cout << "unresolved references:" << std::endl;
    
    for (auto it = unresolvedReferences.begin(); it != unresolvedReferences.end(); it++)
    {
        std::cout << it->first << ": ";
        
        for (auto vecIt = it->second.begin(); vecIt != it->second.end(); vecIt++)
            std::cout << (*vecIt)->linenum << " ";
        
        std::cout << std::endl;
    }
}


void dump()
{
    for (int y = 0; y < 20; y++)
    {
        printf("0x%04X", y * 16);
        for (int x = 0; x < 16; x++)
        {
            printf("  %02X", assmProgram[x + (y * 16)]);
        }
        printf("\n");
    }
    
    printf("Resolved labels:\n");
    
    for (auto it = resolvedLabels.begin(); it != resolvedLabels.end(); it++)
    {
        printf("%s - 0x%04X\n", it->first.c_str(), it->second);
    }
    
    
    printf("Unresolved labels:\n");
    
    for (auto it = labelMap.begin(); it != labelMap.end(); it++)
    {
        printf("%s -", it->first.c_str());
        
        for (auto vecIt = it->second.begin(); vecIt != it->second.end(); vecIt++)
            printf(" 0x%04X", *vecIt);
        
        printf("\n");
    }
}