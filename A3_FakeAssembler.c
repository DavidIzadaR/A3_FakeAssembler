// A3_FakeAssembler.c
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
 
struct MachineStatement
{
    short Parm : 4;
    short RegB : 4;
    short RegA : 4;
    short op : 4;
};

union Statement
{
    struct  MachineStatement S;
    short   C;
};

int RegNumber(char s[])
{
    if (s[0] != 'r')
        return -1;

    switch (strlen(s))
    {
    case 1:
        return -1;
    case 2:
        return (int)(s[1] - '0');
    case 3:
        return (int)(s[1] - '0') * 10 + (int)(s[2] - '0');
    default:
        return -1;
    }
}

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        printf("Usage: %s <assembly file> <output file> <T(ext) | B(inary)>\n", argv[0]);
        exit(1);
    }

    FILE *iFile = fopen(argv[1], "r");
    if (iFile == NULL)
    {
        printf("Error: cannot open %s for reading\n", argv[1]);
        exit(2);
    }

    int B = (toupper(argv[3][0]) == 'B');
    printf("Output will be %s\n", B ? "Binary" : "Text");

    FILE* oFile = fopen(argv[2], B ? "wb" : "w");
    if (oFile == NULL)
    {
        printf("Error: cannot open %s for writing\n", argv[2]);
        exit(2);
    }

    char op[10] = "operator";   // assembler statement/operator
    char op1[4] = "r99";  // parameter 1
    char op2[4] = "r99";  // parameter 2
    char op3[4] = "r99";  // parameter 3

    int RegA = -1;  // Register # for Op1
    int RegB = -1;  // Register # for Op2
    int RegC = -1;  // Register # for Op3
    unsigned char ValC  = 0;   // Immediate constant instead of RegC
    unsigned char ByteC = 0;
    union Statement s;

    while (fscanf(iFile, "%s %s %s %s\n", op, op1, op2, op3) > 0)
    {
        RegA = RegNumber(op1);
        RegB = RegNumber(op2);
        RegC = RegNumber(op3);

        if (RegC == -1)
            ValC = atoi(op3) & 0xF;
        
        s.S.RegA = RegA;
        s.S.RegB = RegB;
        if ((strcmp(op, "add") == 0) && (RegC != -1))
        {
            s.S.op = 0x0;
            s.S.Parm = RegC;
        }
        else if ((strcmp(op, "addi") == 0) && (RegC == -1))
        {
            s.S.op = 0x1;
            s.S.Parm = ValC;
        }
        else if ((strcmp(op, "nand") == 0) && (RegC != -1))
        {
            s.S.op = 0x2;
            s.S.Parm = RegC;
        }
        else if ((strcmp(op, "or") == 0) && (RegC != -1))
        {
            s.S.op = 0x3;
            s.S.Parm = RegC;
        }
        else if ((strcmp(op, "mul") == 0) && (RegC != -1))
        {
            s.S.op = 0x4;
            s.S.Parm = ValC;
        }
        else if ((strcmp(op, "lw") == 0) && (RegC == -1))
        {
            s.S.op = 0x5;
            s.S.Parm = ValC;
        }
        else if ((strcmp(op, "sw") == 0) && (RegC == -1))
        {
            s.S.op = 0x6;
            s.S.Parm = ValC;
        }
        else if ((strcmp(op, "bne") == 0) && (RegC == -1))
        {
            s.S.op = 0x7;
            s.S.Parm = ValC;
        }
        else
            exit(3);

        if (B)
            fwrite(&s.C, sizeof(short), 1, oFile);
        else
        {
            fprintf(oFile, "%#06x ", s.C);

            fprintf(oFile, "  // % s % s % s ", op, op1, op2);
            if (RegC < 0)
                fprintf(oFile, "%d", ValC);
            else
                fprintf(oFile, "%s", op3);
            fprintf(oFile, "\n");
        };
    }
    fclose(oFile);
    fclose(iFile);
}
