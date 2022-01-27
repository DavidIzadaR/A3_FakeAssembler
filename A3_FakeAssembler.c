// A3_FakeAssembler.c
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

    FILE* oFile = fopen(argv[2], "w");
    if (oFile == NULL)
    {
        printf("Error: cannot open %s for writing\n", argv[2]);
        exit(2);
    }

    int B = (toupper(argv[3][0]) == 'B');
    printf("Output will be %s\n", B ? "Binary" : "Text");

    char op[10] = "operator";   // assembler statement/operator
    char op1[4] = "r99";  // parameter 1
    char op2[4] = "r99";  // parameter 2
    char op3[4] = "r99";  // parameter 3

    int RegA = -1;  // Register # for Op1
    int RegB = -1;  // Register # for Op2
    int RegC = -1;  // Register # for Op3
    int ValC = 0;   // Immediate constant instead of RegC
    unsigned char ByteC = 0;

    while (fscanf(iFile, "%s %s %s %s\n", op, op1, op2, op3) > 0)
    {
        RegA = RegNumber(op1);
        RegB = RegNumber(op2);
        RegC = RegNumber(op3);

        if (RegC == -1)
        {
            ValC = atoi(op3);
            ByteC = (unsigned char)((int)ValC & 0xF);
        }
        
        if ((strcmp(op, "add") == 0) && (RegC != -1))
            fprintf(oFile, "%#06x ", 0x0000 | RegA << 8 | RegB << 4 | RegC);
        else if ((strcmp(op, "addi") == 0) && (RegC == -1) && (ValC >= -8) & (ValC <= 7))
            fprintf(oFile, "%#06x ", 0x1000 | RegA << 8 | RegB << 4 | ByteC);
        else if ((strcmp(op, "nand") == 0) && (RegC != -1))
            fprintf(oFile, "%#06x ", 0x2000 | RegA << 8 | RegB << 4 | RegC);
        else if ((strcmp(op, "or") == 0) && (RegC != -1))
            fprintf(oFile, "%#06x ", 0x3000 | RegA << 8 | RegB << 4 | RegC);
        else if ((strcmp(op, "mul") == 0) && (RegC != -1))
            fprintf(oFile, "%#06x ", 0x4000 | RegA << 8 | RegB << 4 | RegC);
        else if ((strcmp(op, "lw") == 0) && (RegC == -1) && (ValC >= -8) & (ValC <= 7))
            fprintf(oFile, "%#06x ", 0x5000 | RegA << 8 | RegB << 4 | ByteC);
        else if ((strcmp(op, "sw") == 0) && (RegC == -1) && (ValC >= -8) & (ValC <= 7))
            fprintf(oFile, "%#06x ", 0x6000 | RegA << 8 | RegB << 4 | ByteC);
        else if ((strcmp(op, "bne") == 0) && (RegC == -1) && (ValC >= -8) & (ValC <= 7))
            fprintf(oFile, "%#06x ", 0x7000 | RegA << 8 | RegB << 4 | ByteC);
        else
            exit(3);

        fprintf(oFile, "  ; // % s % s % s ", op, op1, op2);
        if (RegC < 0)
            fprintf(oFile, "%d", ValC);
        else
            fprintf(oFile, "%s", op3);
        fprintf(oFile, "\n");
    }
    fclose(oFile);
    fclose(iFile);
}
