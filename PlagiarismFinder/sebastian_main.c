#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

void createWorkFile(char origin[], char subject[]);

int stringToNumber(char string[]);

int fourFirstStrings(char a[], char b[], char c[], char d[]);

void numberFile(char origin[], char location[]);

void numberAccumulator(char location[], int numberChunk);

void periodeRemover(char *dotString);

void compare(char oriFile[], char testFile[]);

void printNumbers(char location[]);

void inputFile(char originalWorkFile[], char testItFile[]);


int main() {

    char stringMan1[100] = "OriginalWork.txt", stringMan2[100] = "TextToBeTested.txt";
    printf(" Plagiarism Finder: \n\n");

    inputFile(stringMan1, stringMan2);

    //Opens the original doc, and the copy that we will work with
    //File copier/converter, Copies the original doc into our tester doc, turning uppercase letters into lowercase, and
    // removing non-letter, non-blankspace symbols
    createWorkFile(stringMan1, "OriginalDocTester.txt");
    createWorkFile(stringMan2, "HandInFromStudentTester.txt");

    
    numberFile("OriginalDocTester.txt", "NumberFileOrigin.txt");
    numberFile("HandInFromStudentTester.txt", "NumberFileTester.txt");

   /* printf(" \n\n\n\n Your File:\n");
    printNumbers("NumberFileOrigin.txt");
    printf(" File to be tested:\n");
    printNumbers("NumberFileTester.txt"); */

    compare("NumberFileOrigin.txt", "NumberFileTester.txt");
    //compare("NumberFileTester.txt", "NumberFileOrigin.txt");

    printf("\n You can now close the window \n");
    scanf(" %s", &stringMan1);

    return 0;
}

// Create workfile function
void createWorkFile(char origin[], char testFile[]) {
    FILE *fileIn = fopen(origin, "r");
    FILE *fileOut = fopen(testFile, "w");
    if (fileIn == NULL || fileOut == NULL) {
        printf("\n Error reading file %s :", origin);
        perror(" ");
        sleep(5);
        exit(0);
    } else {
        char StringBoy;
        while ((StringBoy = fgetc(fileIn)) != EOF) {
            if (StringBoy == '\n'){
                fputc(' ',fileOut);
            } else if (isalpha(StringBoy) != 0 || StringBoy == ' ' || StringBoy == '.') {
                StringBoy = tolower(StringBoy);
                fputc(StringBoy, fileOut);
            }
        }
        fclose(fileOut);
        fclose(fileIn);
    }
}



// Fingerprint function(s)

// converts text to number chunks and puts them into a file
void numberFile(char origin[], char location[]) {
    FILE *fileHandler1, *fileHandler2;
    char stringHandler1[100], stringHandler2[100], stringHandler3[100], stringHandler4[100], dotFinder;
    int numberChunk;
    fileHandler1 = fopen(origin, "r");
    fileHandler2 = fopen(location, "w+");
    if (fileHandler1 == NULL || fileHandler2 == NULL) {
        perror("\n Error at numberFile: ");
    } else {
        while (fscanf(fileHandler1, " %s %s %s %s", stringHandler1, stringHandler2, stringHandler3, stringHandler4) >
               -1) {
            //Fjerner punktummer til sidst
            periodeRemover(stringHandler1);
            periodeRemover(stringHandler2);
            periodeRemover(stringHandler3);
            periodeRemover(stringHandler4);

            numberChunk = fourFirstStrings(stringHandler1, stringHandler2, stringHandler3, stringHandler4);
            numberAccumulator(location, numberChunk);

            //Flytter dotten, medmindre der findes et slutpunktum
            fseek(fileHandler1, -2, SEEK_CUR);
            while ((dotFinder = fgetc(fileHandler1)) != EOF) {
                if (dotFinder == '.') {
                    break;
                }
            }
        }
        fclose(fileHandler1);
        fclose(fileHandler2);
    }
}

// Generates the chunk number
int fourFirstStrings(char a[], char b[], char c[], char d[]) {
    int numb = 0;
    numb = numb + 1000 * stringToNumber(a);
    numb = numb + 100 * stringToNumber(b);
    numb = numb + 10 * stringToNumber(c);
    numb = numb + stringToNumber(d);
    return numb;
}

// Converts a string to a one-digit number
int stringToNumber(char string[]) {
    int sLenght = strlen(string);
    //printf(" sLenght: %d", sLenght);
    while (sLenght > 9)
        sLenght = sLenght - 10;
    return sLenght;
}

// Function that adds the fingerprint to a file, and increases its counter if it is a duplicate
void numberAccumulator(char location[], int numberChunk) {
    FILE *fileHandler3;
    int dubVal = 0, duplicateChecker;
    fileHandler3 = fopen(location, "r+");

    if (fileHandler3 == NULL) {
        perror("\n Error at numberAccumulator: ");
    } else {
        while (fscanf(fileHandler3, " %d", &duplicateChecker) != EOF) {
            if (duplicateChecker == numberChunk) {
                fscanf(fileHandler3, " %d", &dubVal);
                if (dubVal != 9) {
                    fseek(fileHandler3, -2, SEEK_CUR);
                    fprintf(fileHandler3, " %d", (dubVal + 1));
                    fseek(fileHandler3, 0, SEEK_CUR);
                }
                break;
            } else {
                fscanf(fileHandler3, " %d", &duplicateChecker);
            }
        }
        if (dubVal == 0) {
            fprintf(fileHandler3, " %d %d  \n", numberChunk, 1);
        }

        fclose(fileHandler3);
    }
}

void periodeRemover(char *dotString) {
    int len, i, j;
    len = strlen(dotString);
    for (i = 0; i < len; i++) {
        if (dotString[i] == '.') {
            for (j = i; j < len; j++) {
                dotString[j] = dotString[j + 1];
            }
        }
    }
}


void compare(char oriFile[], char testFile[]) {
    FILE *orif = fopen(oriFile, "r"), *testf = fopen(testFile, "r");
    if (orif == NULL || testf == NULL) {
        perror("\n Error at compare: ");
    } else {
        int oriNum, testNum, plagCount = 0, testCount = 0;
        while (fscanf(orif, " %d", &oriNum) != EOF) {
            while (fscanf(testf, " %d", &testNum) != EOF) {
                if (oriNum == testNum) {
                    plagCount++;
                }
                fscanf(testf, " %d", &testNum);
            }
            testCount++;
            rewind(testf);
            fscanf(orif, " %d", &oriNum);
        }
        printf(" \n\n\n\n\n\n\n\n Number of compared sentences = %d\n "
               "Number of plagiarism hits = %d\n "
               "Percentage plagiarism = %d%%\n",
               testCount, plagCount, 100 * plagCount / testCount);
        fclose(orif);
        fclose(testf);
    }
}

void printNumbers(char location[]){
    int chunkInt1, chunkInt2;
    FILE *fileHandler1 = fopen(location, "r");
    printf(" \n %s \n\n", location);
    if (fileHandler1 == NULL) {
        perror("\n Error at printNumbers: ");
    } else {
        while (fscanf(fileHandler1, " %d %d", &chunkInt1, &chunkInt2) != EOF) {
            printf(" %d %d\n", chunkInt1, chunkInt2);
        }
        printf("\n");
    }
    fclose(fileHandler1);
}

void inputFile(char originalWorkFile[], char testItFile[]){
    printf(" Input the file name of the file to be tested (include '.txt'): \n");
    scanf(" %[A-Za-z0-9 ._-!]",testItFile);
    printf("\n Input the file name of the file to it should be tested against (include '.txt'): \n");
    scanf(" %[A-Za-z0-9 ._-!]",originalWorkFile);

}
