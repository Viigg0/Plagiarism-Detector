#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct
{
    int bucket;    // START INDEX i.e. BUCKET NUMBER
    int dubval; // DUBLICATE VALUE
} bucket;

void createWorkFile(char origin[], char subject[]);

int stringToNumber(char string[]);

int fourFirstStrings(char a[], char b[], char c[], char d[]);

void numberFile(char origin[], char location[]);

void numberAccumulator(char location[], bucket buckets[]);

void periodeRemover(char *dotString);

void compare(char oriFile[], char testFile[]);

void printNumbers(char location[]);

void inputFile(char originalWorkFile[], char testItFile[]);

int compare_buckets(const void* a, const void* b);

void sort_buckets(bucket buckets[]);
int main() {

    char stringMan1[100] = "OriginalWork.txt", stringMan2[100] = "TextToBeTested.txt";
    printf(" Plagiarism Finder: \n\n");



    inputFile(stringMan1, stringMan2);

    //Opens the original doc, and the copy that we will work with
    //File copier/converter, Copies the original doc into our tester doc, turning uppercase letters into lowercase, and
    // removing non-letter, non-blankspace symbols
    createWorkFile(stringMan1, "OriginalDocTester.txt");
    createWorkFile(stringMan2, "HandInFromStudentTester.txt");

    //printf(" OriginalWork:\n");
    numberFile("OriginalDocTester.txt", "NumberFileOrigin.txt");
    //printf(" TextToBeTested:\n");
    numberFile("HandInFromStudentTester.txt", "NumberFileTester.txt");


    //printNumbers("NumberFileOrigin.txt");
    //printNumbers("NumberFileTester.txt");

    compare( "NumberFileTester.txt", "NumberFileOrigin.txt");
    //compare("NumberFileTester.txt", "NumberFileOrigin.txt");

    remove("OriginalDocTester.txt");
    remove("HandInFromStudentTester.txt");
    remove("NumberFileOrigin.txt");
    remove("NumberFileTester.txt");


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
            } else if (StringBoy == '!'||StringBoy == '?'){
                fputc('.',fileOut);
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
    int i;
    /*int buckets[10000];
    for ( i = 0; i < 10000; ++i) {
        buckets[i]=0;
    }*/

    bucket buckets[10000];
    for(i = 0; i < 10000; i++){
        buckets[i].bucket = i;
        buckets[i].dubval = 0;
    }
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
            //buckets[numberChunk]++;
            buckets[numberChunk].dubval++;

            // Flytter pointeren til næste punktum.
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
    sort_buckets(buckets);
    numberAccumulator(location, buckets);
}

void sort_buckets(bucket buckets[]){
    qsort(buckets, 10000, sizeof(*buckets), compare_buckets);
}

int compare_buckets(const void* a, const void* b){
    int value1 = ((bucket*)a)->dubval;
    int value2 = ((bucket*)b)->dubval;
    return value2-value1;
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
    int sLength = strlen(string);
    while (sLength > 9)
        sLength = sLength - 10;
    return sLength;
}

// Function that adds the fingerprint to a file, and increases its counter if it is a duplicate
void numberAccumulator(char location[], bucket buckets[]) {

    FILE *fileHandler3;
    int i;
    fileHandler3 = fopen(location, "w");

    for ( i = 0; i < 10000; ++i) {
        if (buckets[i].dubval>0){
            fprintf(fileHandler3, " %d %d \n", buckets[i].bucket, buckets[i].dubval);
        }
    }
    fclose(fileHandler3);
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

// Function that compare the two hashed files
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
        printf(" Number of compared sentences = %d\n "
               "Number of plagiarism hits = %d\n "
               "Percentage plagiarism = %d%%\n",
               testCount, plagCount, 100 * plagCount / testCount);
        int plagPercent = 100 * plagCount / testCount;
        if(plagPercent>80) {
            printf(" |This is a placeholder text|\n Plagiarism score: 4\n Almost definitely plagiarism, most likely the whole text \n");
        } else if (plagPercent>60){
            printf(" |This is a placeholder text|\n Plagiarism score: 3\n Very high probability of containing plagiarism, most likely several sections\n");
        }else if (plagPercent>40){
            printf(" |This is a placeholder text|\n Plagiarism score: 2\n High probability of containing plagiarism, can be one or more sections \n");
        }else if (plagPercent>20){
            printf(" |This is a placeholder text|\n Plagiarism score: 1\n Small chance of plagiarism, might only be a small section \n");
        }else
            printf(" |This is a placeholder text|\n Plagiarism score: 0\n No plagiarism or only a few sentences \n");
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
    printf(" Input the file name of the file to be tested: \n");
    scanf(" %[A-Za-z0-9 ._-!]",testItFile);
    printf("\n Input the file name of the file to it should be tested against: \n");
    scanf(" %[A-Za-z0-9 ._-!]",originalWorkFile);

}

