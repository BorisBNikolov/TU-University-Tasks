#include<stdio.h>
#include<string.h>

const int ARRAY_SIZE = 1024 ;     // don't work with more than that amount of lines            {Malloc use restriction}
const int ARRAY_LINE_SIZE = 1256; // don't work with more than that amount of symbols per line {Malloc use restriction}
const int FILENAME_SIZE = 100;

#define STR_QUIT_Q "Q"
#define STR_QUIT_q "q"
#define C_FILE_EXTENTION "c"

int DEBUG = 0; // 1 for showing debug information on screen, 6 for full info

#define ALL_OPERATORS  7 // number operators for which program will check
//operators for which program will check, add more operators and increase ALL_OPERATORS
const char* operatorsStr[ALL_OPERATORS] = {"if", "else", "switch", "goto", "while", "do", "for"};

enum states { TEXT,           //For Text
              SAW_SLASH,      // For
              SAW_STAR,
              SAW_APOSTROPHE,
              SAW_QUOTE,
              SINGLE_COMMENT,
              MULTI_COMMENT,
              NEXT_LINE
            } state = TEXT;

// read functions
int readFromFile    (char allLines[][ARRAY_LINE_SIZE]); // DONE
int readFromKeyboard(char allLines[][ARRAY_LINE_SIZE]); // DONE

// task functions
int lineWithLessSymbols(char allLines[][ARRAY_LINE_SIZE],char allLinesMeaningSymbols[][ARRAY_LINE_SIZE]); // DONE
int operatorsInProgram (char allLines[][ARRAY_LINE_SIZE], int* operationsResult); // DONE

// write functions
int  writeToFile  (char allLines[][ARRAY_LINE_SIZE], int minSymbolsLineNumber, int* operatorsNumberArr); // DONE
void writeToScreen(char allLines[][ARRAY_LINE_SIZE], int minSymbolsLineNumber, int* operatorsNumberArr); // DONE

// support functions
void loadMenu(char allLines[][ARRAY_LINE_SIZE]); //DONE
int  scanForSubStr(char* currentLine, const char* sub); //DONE
int  checkFileExtention(char FileName[], const char extention[]); // DONE
int  checkCharForLetter(char ch); // DONE
void zeroDoubleArray(char allLines[][ARRAY_LINE_SIZE]); // DONE

//debug functions
void  printDoubleArray     (char allLines[][ARRAY_LINE_SIZE]); // DONE
char* cleanExtraWhiteSpaces(char line[]); // DONE

//*****************************************************************************************
int main(){
  char allLines[ARRAY_SIZE][ARRAY_LINE_SIZE];
  zeroDoubleArray(allLines);
  loadMenu(allLines);
  return 0;
}
//*****************************************************************************************

void loadMenu(char allLines[][ARRAY_LINE_SIZE]){
  char choice;
  int minSymbolsLineNumber = 0;
  do{
    printf("\n>------------------------------------------------------------<\n");
    printf(">Моля изберете номер на опция:                               <\n");
    printf(">1. Четене от Файл и запис във файл                          <\n");
    printf(">2. Четене от Файл и извеждане на резултата на екрана        <\n");
    printf(">3. Четене от клавиатурата и извеждане във Файл              <\n");
    printf(">4. Четене от клавиатурата и извеждане на резултата на екрана<\n");
    printf(">5. Изход от програмата ( 'Q' or 'q' )                       <\n");
    printf(">------------------------------------------------------------<\n");
    int operationResult[ALL_OPERATORS] = {0}; //reset each cicle, data result
    char allLinesMeaningSymbols[ARRAY_SIZE][ARRAY_LINE_SIZE];
    zeroDoubleArray(allLinesMeaningSymbols);

    scanf(" %c",&choice); // space before the %; this consumes the whitespace, so that the next scanf call should work
    switch(choice){
      //Menu options;
      case '1':
        printf(">>Избрахте: 1. Четене от Файл и запис във файл<\n");
        if (readFromFile(allLines)) break;
        minSymbolsLineNumber = lineWithLessSymbols(allLines,allLinesMeaningSymbols);
        if (minSymbolsLineNumber==-1) break;
        if (operatorsInProgram(allLinesMeaningSymbols, operationResult)) break;
        writeToFile(allLines, minSymbolsLineNumber, operationResult);
        break;
      case '2':
        printf(">>Избрахте: 2. Четене от Файл и извеждане на резултата на екрана<\n");
        if (readFromFile(allLines)) break;
        minSymbolsLineNumber = lineWithLessSymbols(allLines,allLinesMeaningSymbols);
        if (minSymbolsLineNumber==-1) break;
        if (operatorsInProgram(allLinesMeaningSymbols, operationResult)) break;
        writeToScreen(allLines, minSymbolsLineNumber,operationResult);
        break;
      case '3':
        printf(">>Избрахте: 3. Четене от клавиатурата и извеждане във Файл<\n");
        if (readFromKeyboard(allLines)) break;
        minSymbolsLineNumber = lineWithLessSymbols(allLines, allLinesMeaningSymbols);
        if (minSymbolsLineNumber==-1) break;
        if (operatorsInProgram( allLinesMeaningSymbols, operationResult)) break;
        writeToFile(allLines, minSymbolsLineNumber,operationResult);
        break;
      case '4':
        printf(">>Избрахте: 4. Четене от клавиатурата и извеждане на резултата на екрана\n");
        readFromKeyboard(allLines);
        minSymbolsLineNumber = lineWithLessSymbols(allLines,allLinesMeaningSymbols);
        if (minSymbolsLineNumber==-1) break;
        if (operatorsInProgram(allLinesMeaningSymbols, operationResult)) break;
        printDoubleArray(allLinesMeaningSymbols); //TEMP
        writeToScreen(allLines, minSymbolsLineNumber,operationResult);
        break;
      //Quit program options;
      case 'Q':
      case 'q':
      case '5':
        printf(">>Изход\n");
        return;
      default:
        zeroDoubleArray(allLines);
        printf(">>Невалиден избор, моля въведете пак!\n");
        break;
    }
  } while (1);
}

//Read functions  ******************************************************************************
int readFromFile(char allLines[][ARRAY_LINE_SIZE]){
  char inputFileName[FILENAME_SIZE];char line[ARRAY_LINE_SIZE];
  int i = 0;
  FILE *inputFile;
  do { /* open inputFile and check it until get right extention*/
    printf("Моля въведете името на файла, от който искате да четете!, 'Q' за връщане към менюто\n");
    scanf("%s", &inputFileName[0]);
    if (!(strcmp(inputFileName, STR_QUIT_Q) == 0 || strcmp(inputFileName, STR_QUIT_q))) {
      return 1; // return to Menu
    }else if (!checkFileExtention(inputFileName, C_FILE_EXTENTION)) {
      continue; // ask for new path
    } inputFile = fopen (inputFileName, "r");
    if ( inputFile == NULL ){ // check file for wrong
      printf("\n!!!Грешка с файла!!!\n");
      perror (inputFileName);
    }else break; // file is fine, continues to read it
  }while(1);

  while (fgets(line, ARRAY_LINE_SIZE, inputFile) != NULL){ // read data from array and write it to array
    //printf(">Line number:%d-> %s ",i, line);
    if (i < ARRAY_SIZE-1) {
      strcpy(allLines[i++], line);
    }else {
      printf("Файлът е твърде дълъг!!! Ще се обработи до ред: %d\n", i);
      break;
    }
  }
  strcpy(allLines[i],"\0"); // add termination sign in end
  fclose (inputFile);
  return 0;
};

int readFromKeyboard(char allLines[][ARRAY_LINE_SIZE]){
  char str[ARRAY_LINE_SIZE];
  int i,x= -1,y = 0;
  while (fgets(str,ARRAY_LINE_SIZE,stdin) != NULL) {
    for (i = 0; str[i] != '\0'; i ++) {
      char currentChar = str[i];
      if (currentChar == '\n'){//} || ((y-1) <= ARRAY_LINE_SIZE)) {
        y = 0;
        x++;
        if (!(x < ARRAY_SIZE-1)) {
          printf("Файлът е твърде дълъг!!! Ще се обработи до ред: %d\n", i);
          break;
        }
      }else{
        if (y < ARRAY_LINE_SIZE-1){
          allLines[x][y] = currentChar;
          y++;
        }else {
          allLines[x][y] = '\0';
          x++; y = 0;
        }
      }
    }
  }  y = 0;
  return 0;
};

// Task functions ******************************************************************************
int lineWithLessSymbols(char allLines[][ARRAY_LINE_SIZE],char allLinesMeaningSymbols[][ARRAY_LINE_SIZE]){
  int i,j,jNew=0,minSymbolsLineNumber,minMeanfulSymbols;
  if (!strlen(allLines[0]) && !strlen(allLines[1])) {
    printf("Грешка - Липса на данни! \n");
    return -1; // with error
  }
  for (i = 0; strlen(allLines[i]); i++) {
    int lineMeanfulSymbols = 0,firstNote = 1;
    char currentLine[ARRAY_LINE_SIZE];
    strcpy(currentLine,allLines[i]);
    char str[22];
    DEBUG>5?printf("––line: <!>> %s <<!> number: %d ––\n",allLines[i], i+1):sprintf(str,"do Nothing");
    for (j = 0;/*checked down*/; j++){
      char currentChar = currentLine[j];
      if (currentChar == '\0' || currentChar == '\n' || state == SINGLE_COMMENT) { // go to next line
        if (state != MULTI_COMMENT ) {
          state = TEXT;
        }
        if (firstNote && lineMeanfulSymbols !=0) { // first time is minimal meanful symbols
          firstNote = 0;
          minMeanfulSymbols = lineMeanfulSymbols;
          minSymbolsLineNumber = i;
        } else if (minMeanfulSymbols > lineMeanfulSymbols && lineMeanfulSymbols!=0) { //compare and update minimum meanful symbols
          minMeanfulSymbols = lineMeanfulSymbols;
          minSymbolsLineNumber = i;
        }
        if (!strcmp(allLinesMeaningSymbols[i],"") || !strcmp(allLinesMeaningSymbols[i],"\n")) { // check if haven't meanful symbols and add space to save line
          allLinesMeaningSymbols[i][0]= ' ';
          allLinesMeaningSymbols[i][1]= '\0';
        }
        jNew = 0; // reset output array's line position to begginig
        break; // go to next line
      }
      switch(state){
        case TEXT : // normal text and whitespaces, count meanful symbols
          DEBUG>5?printf("\n--TEXT-- %c at %s--\n",currentChar,currentLine):sprintf(str,"do Nothing");
          switch(currentChar){
            case '/'  : state = SAW_SLASH;      break;
            case '\"' : state = SAW_QUOTE;      break;
            case '\'' : state = SAW_APOSTROPHE; break;
            case ' '  : // Don't count white spaces
            case '\t' : // but add them at array
              if (allLinesMeaningSymbols[i][jNew-1] != ' ' || allLinesMeaningSymbols[i][jNew-1] != '\t') { // remove extra whitespaces
                allLinesMeaningSymbols[i][jNew] = currentChar;
                jNew++; // if it's text
              }
              break;
            default   : if (state == TEXT) {
              allLinesMeaningSymbols[i][jNew] = currentChar; //save only meanful symbols
              lineMeanfulSymbols++ ; // count meanful symbols
              jNew++; // if it's text
            }break;
          } break;
        case SAW_SLASH : // saw / , link to single or multi comment functionality
          DEBUG>5?printf("\n--SAW SWASH-- %c--\n",currentChar):sprintf(str,"do Nothing");
          switch(currentChar){
            case '/'  : state = SINGLE_COMMENT; break;
            case '*'  : state = MULTI_COMMENT;  break;
            default   : state = TEXT;           break;
          } break;
        case SAW_STAR : // saw *
          DEBUG>5?printf("\n--SAW STAR-- %c--\n",currentChar):sprintf(str,"do Nothing");
          switch(currentChar){
            case '/'  : state = TEXT; break;
            case '*'  : break; // Stay at SAW_STAR state
            default   : state = MULTI_COMMENT;  break;
          } break;
        case SAW_QUOTE : // saw ""
          DEBUG>5?printf("\n--SAW QUOTE-- %c--\n",currentChar):sprintf(str,"do Nothing");
          switch (currentChar){
            case '\"' : state = TEXT; break;
            default : break; // under quote;
          } break;
        case SAW_APOSTROPHE :
          DEBUG>5?(printf("\n--SAW APOSTROPHE-- %c--\n",currentChar)):sprintf(str,"do Nothing");
          switch (currentChar){
            case '\'' : state = TEXT; break;
            default : break; // under apostrophe;
          }break;
        case SINGLE_COMMENT :
          DEBUG>5?printf("\n--COMMENT-- %c--\n",currentChar):sprintf(str,"do Nothing");;
          break; // skip to next line at if up
        case MULTI_COMMENT :
          DEBUG>5?(printf("\n--MULTI_COMMENT-- %c--\n",currentChar)):sprintf(str,"do Nothing");
          switch(currentChar){
            case '*'  : state = SAW_STAR; break;
            default   : break;
          }break;
        default: break; // ERROR
      } DEBUG>5?printf("\nline number: %d length: %d\n", i, lineMeanfulSymbols):sprintf(str,"do Nothing");
    }
  }
  //DEBUG?printf("%d %s\n", minSymbolsLineNumber, allLines[minSymbolsLineNumber]):sprintf(str,"do Nothing");
  return minSymbolsLineNumber;
}

int operatorsInProgram(char allLines[][ARRAY_LINE_SIZE],int operationsResult[ALL_OPERATORS-1]){
  unsigned int i=0;
  if (!strlen(allLines[0]) && !strlen(allLines[1])) {
    printf("Грешка - Липса на данни или няма значещи символи! >%s< %zu\n",allLines[0],strlen(allLines[0]));
    return 1; // with error
  }
  for (i = 0; strlen(allLines[i]); i++) {
    char currentLine[ARRAY_LINE_SIZE];
    strcpy(currentLine,allLines[i]);
    //DEBUG >5?printf("––line: <!>> %s <<!> number: %d ––\n",allLines[i], i+1):sprintf(str,"do Nothing");
    for (int operatorNumber = 0; operatorNumber < ALL_OPERATORS; operatorNumber++) {
      operationsResult[operatorNumber] += scanForSubStr(currentLine,operatorsStr[operatorNumber]);
      //printf("operator: '%s' was found %d times\n", operatorsStr[operatorNumber], result);
    }
  }
  for (int operatorNumber = 0; operatorNumber < ALL_OPERATORS-1; operatorNumber++) { //DEBUG
    char str[22];
    DEBUG?printf("DEBUG-->operator: '%s' was found %d times\n", operatorsStr[operatorNumber], operationsResult[operatorNumber]):sprintf(str,"do Nothing");;
  }
  return 0;
}
// Write functions
void writeToScreen(char allLines[][ARRAY_LINE_SIZE], int minSymbolsLineNumber, int operatorsNumberArr[ALL_OPERATORS-1]){
  int operatorNumber;
  printf("Редът с най-малкък брой значещи символи е номер: %d \"%s\"\n", minSymbolsLineNumber+1, allLines[minSymbolsLineNumber]);
  for (operatorNumber = 0; operatorNumber < ALL_OPERATORS; operatorNumber++) {
    printf("operator: '%7s' was found %d times\n", operatorsStr[operatorNumber], operatorsNumberArr[operatorNumber]);
  }
}

int writeToFile(char inputAllLines[][ARRAY_LINE_SIZE], int minSymbolsLineNumber, int operatorsNumberArr[ALL_OPERATORS-1]){
  char outputFileName[FILENAME_SIZE];
  FILE *outputFile;
  do{ /* open output File and check it until get right extention*/
    printf("Моля въведете името на файла, в който искате да запишете резултата!, 'Q' за връщане към менюто\n");
    scanf("%s", &outputFileName[0]);
    if (!(strcmp(outputFileName, STR_QUIT_Q) == 0 || strcmp(outputFileName, STR_QUIT_q))) {
      return 1; // return to Menu
    }
    outputFile = fopen (outputFileName, "w");
    if ( outputFile == NULL ){ // check file for wrong
      printf("\n!!!Грешка с файла!!!\n");
      perror (outputFileName);
    }else break; // file is fine, continues to write in it
  }while(1);

  char stringForWrite[sizeof inputAllLines[minSymbolsLineNumber] + 100];
  sprintf(stringForWrite, "Редът с най-малкък брой значещи символи е номер: %d \"%s\"\n", minSymbolsLineNumber+1, inputAllLines[minSymbolsLineNumber]);
  fprintf(outputFile, "%s", stringForWrite);
  int operatorNumber;
  for (operatorNumber = 0; operatorNumber < ALL_OPERATORS; operatorNumber++) {
    char stringForWrite2[sizeof operatorsStr[operatorNumber] + 100];
    sprintf(stringForWrite2,"operator: '%7s' was found %d times\n", operatorsStr[operatorNumber], operatorsNumberArr[operatorNumber]);
    fprintf(outputFile,"%s", stringForWrite2);
  }
  fclose (outputFile);
  return 0;
}

// Support functions ***************************************************************************
int scanForSubStr(char* currentLine, const char* sub){
  int foundMatches = 0;
  char currentLine_cpy[ARRAY_LINE_SIZE];
  strcpy(currentLine_cpy,currentLine);

  for (char *p = currentLine_cpy; (p = strstr(p, sub)) != NULL; p++) {
    char chBefore = *(p-1);
    char chAfter = *(p+strlen(sub));
    //printf("sub: '%s', line: %s,times:%d\n",sub, p, foundMatches);
    // for 'do': XdoX, doX, Xdo are ignored!!
    if ( checkCharForLetter(chBefore) || checkCharForLetter(chAfter)) {
      continue; // it's part of a word, don't count it
      //printf("IGNORED: sub: '%s', line: %s,chBefore: %c,chAfter: %c\n",sub, p, chBefore,chAfter);
    } foundMatches++; // substring found at offset
    //DEBUG==2?printf("DEBUG: Found at: %s pos: %tu count:%d\n", currentLine_cpy, p - currentLine_cpy, foundMatches):sprintf(str,"do Nothing");
    if (*p == '\0') break;
  }
  //DEBUG==2?printf("DEBUG: %s = %d\n",sub, foundMatches):sprintf(str,"do Nothing");;
  return foundMatches;
}

// return 1 for alfabet, 0 for other symbol
int checkCharForLetter(char ch){
  return (ch > 'a' && ch < 'z') || (ch > 'A' && ch < 'Z');
}

int checkFileExtention(char FileName[], const char extention[]){
  int i = 0;
  char tempFileName[FILENAME_SIZE];
  char *splitedArray[FILENAME_SIZE];
  strcpy(tempFileName,FileName);

  splitedArray[i] = strtok(tempFileName,".");
  while(splitedArray[i]!=NULL){
   splitedArray[++i] = strtok(NULL, ".");
  }
  if(!strcmp(extention, splitedArray[i-1]) == 0) { //get extention
    printf("'%s' не е правилното окончание: '%s'\n", splitedArray[i-1], extention);
    return 0;  // false Not ending with .c (extention[])
  }else return 1;  // for true ending with .c (extention[])
}

void zeroDoubleArray(char allLines[][ARRAY_LINE_SIZE]){
  int i,j;
  for(i = 0; i < ARRAY_SIZE; i++){ //allLines[i+1] != 0
    for(j = 0; j < ARRAY_LINE_SIZE; j++){
      allLines[i][j] = '\0';
    }
  }
}

// Debug functions *****************************************************************************
void printDoubleArray(char allLines[][ARRAY_LINE_SIZE]){
  int i,j;
  printf("DEBUG:\n%s\n", "VVVVVVVV");
  for (i = 0; ; i++){
    if (!strlen(allLines[i]) && !strlen(allLines[i+1])) {
      break;
    }
    for (j = 0; allLines[i][j] != '\0'; j++)
      printf("DEBUG: [%d][%d]>> '%c' <<\n",i,j,allLines[i][j]);
  }printf("DEBUG: %s\n","^^^^^^^^^^");
}

char* cleanExtraWhiteSpaces(char line[]){ //UNUSED FUNCTION
  int i=0,length=0,k=0;
  length=strlen(line);
  for(i=0;i<length;){
    if((line[i]==' ')&&(line[i+1]==' ')){
      for(k=i+1;k<length;k++){
        line[k]=line[k+1];
      }
      length= length - 1;
    }else i++;
  }
  return line;
}
