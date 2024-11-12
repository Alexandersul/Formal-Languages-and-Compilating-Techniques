#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include<string.h>
#include<ctype.h>

const char *cc="not enough memory";
void err(const char *fmt,...)
{
    va_list va;
    va_start (va,fmt);
    fprintf(stderr,"error: ");
    vfprintf(stderr,fmt,va);
    fputc('\n',stderr);
    va_end(va);
    exit(-1);
}
#define SAFEALLOC(var,Type) if((var=(Type*)malloc(sizeof(Type)))==NULL)err(cc);

int line=1;

enum {ADD,AND,ASSIGN,EQUAL,COMMA,CT_INT,CT_REAL,CT_STRING,DIV,DOT,END,GREATER,GREATEREQ,
ID,LACC,LBRACKET,LESS,LESSEQ,
LPAR,MUL,NOT,NOTEQ,OR,RACC,RBRACKET,RPAR,SEMICOLON,SPACE,SUB,CT_CHAR,
BREAK,CHAR,DOUBLE,ELSE,FOR,IF,INT,RETURN,
STRUCT,VOID,WHILE,LINECOMMENT,ERR_CARACTER_INVALID,GHILIMEA,APOSTROF}; /// codurile AL

char *codename[] = {
"ADD","AND","ASSIGN","EQUAL","COMMA","CT_INT","CT_REAL","CT_STRING","DIV","DOT","END","GREATER","GREATEREQ",
"ID","LACC","LBRACKET","LESS","LESSEQ",
"LPAR","MUL","NOT","NOTEQ","OR","RACC","RBRACKET","RPAR","SEMICOLON","SPACE","SUB","CT_CHAR",
"BREAK","CHAR","DOUBLE","ELSE","FOR","IF","INT","RETURN",
"STRUCT","VOID","WHILE","LINECOMMENT","ERR_CARACTER_INVALID","GHILIMEA","APOSTROF","SPACE"
};

char *pCrtCh;
char **valoare;
int q=0;
char inBuf[30000];
void ini()
{
    pCrtCh=(char *)malloc(sizeof(char));
    valoare=(char **)malloc(sizeof(char*)*strlen(inBuf));
    for(int i=0;i<strlen(inBuf);i++)
        valoare[i]=(char*)malloc(sizeof(char));
}

int getNextToken()
{
    int state=0,nCh;
    char ch;
    char*pStartCh;
    for(;;) // bucla infinita
    {
        ch=*pCrtCh;
        switch(state)
        {
        case 0:// testare tranzitii posibile din starea 0
            if(ch=='=') { pCrtCh++; state=3; }
            else if(ch=='>') { pCrtCh++; state=6; }
            else if(ch=='<') { pCrtCh++; state=10; }
            else if(ch=='!') { pCrtCh++; state=13; }
            else if(ch=='/') { pCrtCh++; state=16; }
            else if(ch=='&') { pCrtCh++; state=31; }
            else if(ch=='|') { pCrtCh++; state=33; }
            else if(ch==',') { pCrtCh++; state=19; }
            else if(ch==';') { pCrtCh++; state=20; }
            else if(ch=='(') { pCrtCh++; state=21; }
            else if(ch==')') { pCrtCh++; state=22; }
            else if(ch=='[') { pCrtCh++; state=23; }
            else if(ch==']') { pCrtCh++; state=24; }
            else if(ch=='{') { pCrtCh++; state=25; }
            else if(ch=='}') { pCrtCh++; state=26; }
            else if(ch=='+') { pCrtCh++; state=27; }
            else if(ch=='-') { pCrtCh++; state=28; }
            else if(ch=='*') { pCrtCh++; state=29; }
            else if(ch=='.') { pCrtCh++; state=30; }
            else if(ch=='\'') { pCrtCh++; return APOSTROF; }
            else if(ch=='\"') { pCrtCh++; return GHILIMEA; }
            /**SPACE*/
            // consuma caracterulsi ramane in starea 0
            else if(ch==' '||ch=='\r'||ch=='\t') { pCrtCh++; return SPACE; }
            // tratat separat pentru a actualiza linia curenta
            else if(ch=='\n') { line++; pCrtCh++; return SPACE; }
            /**SPACE*/
            // sfarsit de sir
            else if(ch=='\0') return END;
            // memoreazainceputul ID-ului// consuma caracterul// trece la noua stare
            if(isalpha(ch)) { pStartCh=pCrtCh; pCrtCh++; state=1; }
            else if(isdigit(ch)){ pCrtCh++; state=36; }
            //else
                //return ERR_CARACTER_INVALID;
        break;
        case 1:
            if(isalnum(ch)||ch=='_') pCrtCh++;
            else state=2;
        break;
         case 2:
                nCh=pCrtCh-pStartCh;// lungimea cuvantului gasit
                // teste cuvinte cheie
                if(nCh==5&&!memcmp(pStartCh,"break",5)) return BREAK;
                else if(nCh==4&&!memcmp(pStartCh,"char",4)) return CHAR;
                else if(nCh==6&&!memcmp(pStartCh,"double",6)) return DOUBLE;
                else if(nCh==4&&!memcmp(pStartCh,"else",4)) return ELSE;
                else if(nCh==3&&!memcmp(pStartCh,"for",3)) return FOR;
                else if(nCh==2&&!memcmp(pStartCh,"if",2)) return IF;
                else if(nCh==3&&!memcmp(pStartCh,"int",3)) return INT;
                else if(nCh==6&&!memcmp(pStartCh,"return",6)) return RETURN;
                else if(nCh==6&&!memcmp(pStartCh,"struct",6)) return STRUCT;
                else if(nCh==4&&!memcmp(pStartCh,"void",4)) return VOID;
                else if(nCh==5&&!memcmp(pStartCh,"while",5)) return WHILE;
                else { /*strncpy(valoare[q],pStartCh,nCh); q++;  valoare[q][nCh]='\0';*/ return ID; }
                //dupa INT,CHAR etc obligatoriu vine un ID
        case 3: if(ch=='=') { pCrtCh++; state=4; }
                    else state=5;
        case 4: return EQUAL;
        case 5: return ASSIGN;
        case 6: if(ch=='=') { pCrtCh++; state=7; }
                    else state=8;
        case 7: return GREATEREQ;
        case 8: return GREATER;
        case 10: if(ch=='=') { pCrtCh++; state=11; }
                    else state=12;
        case 11: return LESSEQ;
        case 12: return LESS;
        case 13: if(ch=='=') { pCrtCh++; state=14; }
                 else state=15;
        case 14: return NOTEQ;
        case 15: return NOT;
        case 16: if(ch!='/') state=17;
                 else if(ch=='/') state=18;
        case 17: return DIV;
        case 18: { line++;  return LINECOMMENT; }
        case 19: return COMMA;
        case 20: return SEMICOLON;
        case 21: return LPAR;
        case 22: return RPAR;
        case 23: return LBRACKET;
        case 24: return RBRACKET;
        case 25: return LACC;
        case 26: return RACC;
        case 27: return ADD;
        case 28: return SUB;
        case 29: return MUL;
        case 30: return DOT;
        case 31: if(ch=='&') { pCrtCh++; state=32; }
        case 32: return AND;
        case 33: if(ch=='|') { pCrtCh++; state=34; }
        case 34: return OR;
        case 36: if(isdigit(ch)) { pCrtCh++; state=36; }
                 else return CT_INT;
      }
    }
}

int main()
{
    FILE *fis;
    char **text;
    int z=0,k=0,y=0,*u;
    if((fis = fopen("3.c","rb"))==NULL)
    {
        printf("\n Eroare fisier!\n");
        return 1;
    }
    fread(inBuf, 1, 30000, fis);
    fclose(fis);
    ini();
    pCrtCh=inBuf;
    text=(char **)malloc(sizeof(char*)*strlen(inBuf));
    for(int i=0;i<strlen(inBuf);i++)
        text[i]=(char*)malloc(sizeof(char));
    u=(int*)malloc(sizeof(int));
    while(codename[z]!="END")
        {
            z=getNextToken();
            u[k]=line;
            text[k]=codename[z];
            k++;
            //printf("%s\n",codename[z]);
        }
    k=0;
        while(text[k]!="END")
    {
        if(text[k]=="APOSTROF"&&text[k+1]=="APOSTROF")
        {
            printf("%d\t",u[k]);
            printf("CT_CHAR\n");
           k++;
        }
        else if(text[k]=="APOSTROF"&&text[k+2]=="APOSTROF")
        {
            printf("%d\t",u[k]);
            printf("CT_CHAR\n");
            k+=2;
        }
        else if(text[k]=="APOSTROF"&&text[k+2]!="APOSTROF")
        {
            printf("Format invalid in line: ");
            printf("%d\t",u[k]);
            exit(1);
        }
        else if(text[k]=="GHILIMEA"&&text[k+1]=="GHILIMEA")
        {
            printf("%d\t",u[k]);
            printf("CT_STRING\n");
            k++;
        }
        else if(text[k]=="GHILIMEA"&&text[k+2]=="GHILIMEA")
        {
            printf("%d\t",u[k]);
            printf("CT_STRING\n");
            k+=2;
        }
        else if(text[k]=="GHILIMEA"&&text[k+3]=="GHILIMEA")
        {
            printf("%d\t",u[k]);
            printf("CT_STRING\n");
            k+=3;
        }
         else if(text[k]=="GHILIMEA"&&text[k+3]!="GHILIMEA")
        {
            printf("Format invalid in line: ");
            printf("%d\t",u[k]);
            exit(1);
        }
        else if(text[k]=="CT_INT"&&text[k+1]=="DOT"&&text[k+2]=="CT_INT"&&text[k+3]=="ID")
        {
            printf("%d\t",u[k]);
            printf("CT_REAL\n");
            k+=3;
        }
        else if(text[k]=="CT_INT"&&text[k+1]=="DOT"&&text[k+2]=="CT_INT")
        {
            printf("%d\t",u[k]);
            printf("CT_REAL\n");
            k+=2;
        }
        else if(text[k]=="CT_INT"&&text[k+1]=="ID"&&text[k+2]=="SUB"&&text[k+3]=="CT_INT")
        {
            printf("%d\t",u[k]);
            printf("CT_REAL\n");
            k+=3;
        }
        else if(text[k]=="CT_INT"&&text[k+1]=="DOT"&&text[k+2]&&text[k+2]!="CT_INT")
        {
            printf("Format invalid in line: ");
            printf("%d\t",u[k]);
            exit(1);
        }
        else if(text[k]!="CT_INT"&&text[k+1]=="DOT"&&text[k+2]&&text[k+2]=="CT_INT")
        {
            printf("Formatinvalid in line: ");
            printf("%d\t",u[k]);
            exit(1);
        }
        else if(text[k]!="SPACE"&&text[k]!="GHILIMEA"&&text[k]!="APOSTROF"&&text[k]!="ID")
                {
                    printf("%d\t",u[k]);
                    printf("%s\n",text[k]);
                }
         else if(text[k]!="SPACE"&&text[k]!="GHILIMEA"&&text[k]!="APOSTROF"&&text[k]=="ID")
                {
                    printf("%d\t",u[k]);
                    printf("%s",text[k]);
                    printf(": %s\n",valoare[k]);
                }
        k++;
    }
    printf("%d\t",u[k-1]);
    printf("END");
    return 0;
}
