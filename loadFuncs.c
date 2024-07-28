/*XM23p Loader function definitions*/
/*ECED 3403           JH 07 2024   */

#include "XM23p.h"
#include "load.h"
#include "execute.h"

//handles opening xme files
FILE *openFile(){

    char xmeFileName[MAXRECORDLENG];
    FILE *tempFile;
    printf("\nFile to load: ");
    scanf("%s", xmeFileName);

    tempFile = fopen(xmeFileName, "r");
    if(tempFile)
        printf("Opening %s...\n", xmeFileName);
    else
    printf("File not found!\n");

    return tempFile;
}


//reads SRecords into a buffer and
//passes them off to be interpreted and stored
void readRecords(FILE *inputFile)
{
    char recordBuffer[MAXRECORDLENG];
    char *tempPtr;
    //Counter to display line count
    int i = 0;

    while(fgets(recordBuffer, MAXRECORDLENG, inputFile)){

        //Using temporary pointer to check the second value of the record
        tempPtr = recordBuffer;
        tempPtr++;
        
        switch(*tempPtr){

            case '0':
                tempPtr++;
                parseS0(&tempPtr);
                break;
            case '1':
                tempPtr++;
                parseS12(&tempPtr, instruction);
                break;
            case '2':
                tempPtr++;
                parseS12(&tempPtr, data);
                break;
            case '9':
                tempPtr++;
                parseS9(&tempPtr);
                break;
            default:
                tempPtr++;
                printf("Invalid record at line %i\n", i);
                break;
        }
        i++;
    }
}

//Read s0 records checking checksum and storing
//source file name
void parseS0(char **recordPtr)
{
    //this data is reused for each record type,
    //should have made it a structure
    //two ASCII characters that represent a byte
    unsigned char a, b;
    //length of record
    int length;
    //record checksum
    unsigned char checkSum = 0;
    
    //grabbing length
    a = **recordPtr;
    (*recordPtr)++;
    b = **recordPtr;
    length = hexToByte(a, b);
    checkSum += length;

    //skip over address field
    for(int i = 0; i < 4; i++)
        (*recordPtr)++;
    //read through data
    for(int i = 0; i < (length-3); i++)
    {
        (*recordPtr)++;
        a = **recordPtr;
        (*recordPtr)++;
        b = **recordPtr;
        sourceFileName[i] = hexToByte(a, b);
        //Just keep appending null characters to the end
        //Likely not the most elegant solution
        sourceFileName[i+1] = '\0';
        checkSum += sourceFileName[i];
    }
    //calculate checksum
    (*recordPtr)++;
    a = **recordPtr;
    (*recordPtr)++;
    b = **recordPtr;
    checkSum += hexToByte(a, b);

    if(checkSum == CHECKSUM)
        printf("Source File: %s\n", sourceFileName);
    else
        printf("Invalid checksum for S0 Record!\n");
}

//parse S1&S2 records storing data in appropiate memory
//and checking checksum
void parseS12(char **recordPtr, int flag)
{
    //two ASCII characters that represent a byte
    unsigned char a, b;
    //length of record
    int length;
    //address in memory
    int address = 0;
    int startingAddress;
    //checksum that gets increased as we continue
    unsigned char checkSum = 0;

    //grabbing length
    a = **recordPtr;
    (*recordPtr)++;
    b = **recordPtr;

    length = hexToByte(a, b);
    checkSum += length;

    //grabbing address field
    (*recordPtr)++;
    a = **recordPtr;
    (*recordPtr)++;
    b = **recordPtr;
    address += hexToByte(a, b);
    checkSum += hexToByte(a, b);
    address = (address<<8);
    (*recordPtr)++;
    a = **recordPtr;
    (*recordPtr)++;
    b = **recordPtr;
    address += hexToByte(a, b);
    checkSum += hexToByte(a, b);

    //Save starting address for later dianostic
    startingAddress = address;

    //read through data
    for(int i = 0; i < (length-3); i++)
    {
        (*recordPtr)++;
        a = **recordPtr;
        (*recordPtr)++;
        b = **recordPtr;

        memBlock[flag].bytes[address] = hexToByte(a, b);
        checkSum += memBlock[flag].bytes[address];
        
        address++;
    }
    //calculate checksum
    (*recordPtr)++;
    a = **recordPtr;
    (*recordPtr)++;
    b = **recordPtr;
    checkSum += hexToByte(a, b);

    if(checkSum != CHECKSUM){
        printf("Invalid checksum! Bad data loaded to ");
        if(flag == instruction)
            printf("IMEM at locations 0x%04x->0x%04x\n", startingAddress, address);
        else
            printf("DMEM at locations 0x%04x->0x%04x\n", startingAddress, address);
    }
}

//Parse s9 records, storing starting memory address
void parseS9(char **recordPtr)
{
    //two ASCII characters that represent a byte
    unsigned char a, b;
    //length of record
    int length;
    //starting address
    int address = 0;
    //record checksum
    int checkSum = 0;
    
    //grabbing length
    a = **recordPtr;
    (*recordPtr)++;
    b = **recordPtr;
    length = hexToByte(a, b);
    checkSum += length;

    //grabbing address field
    (*recordPtr)++;
    a = **recordPtr;
    (*recordPtr)++;
    b = **recordPtr;
    address += hexToByte(a, b);
    checkSum += hexToByte(a, b);
    address = (address<<8);
    (*recordPtr)++;
    a = **recordPtr;
    (*recordPtr)++;
    b = **recordPtr;
    address += hexToByte(a, b);
    checkSum += hexToByte(a, b);

    //calculate checksum
    (*recordPtr)++;
    a = **recordPtr;
    (*recordPtr)++;
    b = **recordPtr;
    checkSum += hexToByte(a, b);

    if(checkSum == CHECKSUM){
        regFile[0][PC].word = address;
        printf("Starting address is 0x%04x\n", address);
    }
    else
        printf("Invalid S9 checksum! Not saving program starting address!\n");

}

//converts a byte represented by two ascii characters into
//the equivalent byte value
unsigned char hexToByte(unsigned char char1, unsigned char char2)
{
    unsigned char result;
    
    //store the char with the appropiate offset
    if (isdigit(char1))
        result = (char1-'0');
    else if (isupper(char1))
        result = (char1-'A' + 10);
    else if (islower(char1))
        result = (char1-'a' + 10);
    //convert the lower four bits to the upper four bits
    result = result<<4;
    //repeat the process above for the second character
    if (isdigit(char2))
        result += (char2-'0');
    else if (isupper(char2))
        result += (char2-'A'+ 10);
    else if (islower(char2))
        result += (char2-'a' + 10);

    return result;
}

void displayMem()
{
    unsigned int lowerBound, upperBound;
    //placeholder integer that ensures we
    //print 16 bytes at a time
    int i = 17;

    printf("\nData Memory Bounds: ");
    scanf("%x %x", &lowerBound, &upperBound);
    printf("\n");

    while(lowerBound < upperBound){
        printf("%02x ", memBlock[data].bytes[lowerBound]);
        if(i % 16 == 0)
            printf("\n");
        lowerBound++;
        i++;
    }

    //reset counter
    i = 17;

    printf("\nInstruction Memory Bounds: ");
    scanf("%x %x", &lowerBound, &upperBound);
    printf("\n");

    while(lowerBound < upperBound){
        printf("%02x ", memBlock[instruction].bytes[lowerBound]);
        if(i % 16 == 0)
            printf("\n");
        lowerBound++;
        i++;
    }

    printf("\n");
}

//Initializes null characters in pipeline registers
void initializePipelines()
{
    instructionRegisters[MAR].word = NOP;
    instructionRegisters[CTRL].word = NOP;
    instructionRegisters[MBR].word = NOP;
    dataRegisters[MAR].word = NOP;
    dataRegisters[CTRL].word = NOP;
    dataRegisters[MBR].word = NOP;
    ir.value = NOP;
}