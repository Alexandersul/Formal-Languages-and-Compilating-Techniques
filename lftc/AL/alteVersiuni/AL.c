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
STRUCT,VOID,WHILE,LINECOMMENT}; /// codurile AL

static const char *codename[] = {
"ADD","AND","ASSIGN","EQUAL","COMMA","CT_INT","CT_REAL","CT_STRING","DIV","DOT","END","GREATER","GREATEREQ",
"ID","LACC","LBRACKET","LESS","LESSEQ",
"LPAR","MUL","NOT","NOTEQ","OR","RACC","RBRACKET","RPAR","SEMICOLON","SPACE","SUB","CT_CHAR",
"BREAK","CHAR","DOUBLE","ELSE","FOR","IF","INT","RETURN",
"STRUCT","VOID","WHILE","LINECOMMENT"
};

char *createString(char *begin, char *end)
{
char *p;
size_t n = (end - begin);
SAFEALLOC(p, char);
memcpy(p, begin, n);
p[n] = '\0';
return p;
}
/*
typedef struct _Token
{
    int code;// codul (numele)
    union
    {
        char*text;// folosit pentruID, CT_STRING (alocat dinamic)
        int i;// folosit pentruCT_INT, CT_CHAR
        double r;// folosit pentruCT_REAL
    };
    int line;// linia din fisierulde intrare
    struct _Token *next;// inlantuirela urmatorul AL
} Token;

Token *lastToken;
Token *tokens;
*/
char *pCrtCh;
char inBuf[30000];

void ini(){
    pCrtCh=(char *)malloc(sizeof(char));
    //tokens=(Token*)malloc(sizeof(Token));
    //lastToken=(Token*)malloc(sizeof(Token));
    //inBuf=(char *)malloc(sizeof(char)*10000);
}
/*
Token *addTk(int code)
{
    Token *tk;
    SAFEALLOC(tk,Token)
    tk->code=code;
    tk->line=line;
    tk->next=NULL;
    if(lastToken)
    {
        lastToken->next=tk;
    }
    else
    {
        tokens=tk;
    }
    lastToken=tk;
    return tk;
}
*/
/*
void tkerr(const Token *tk,const char*fmt,...)
{
    va_list va;
    va_start(va,fmt);
    fprintf(stderr,"error in line %d: ",tk->line);
    vfprintf(stderr,fmt,va);
    fputc('\n',stderr);
    va_end(va);
    exit(-1);
}
*/
int getNextToken()
{
    int state=0,nCh;
    char ch;
    char*pStartCh;
    //Token *tk;
    for(;;) // bucla infinita
    {
        ch=*pCrtCh;
        switch(state)
        {
        case 0:// testare tranzitii posibile din starea 0
            if(isalpha(ch)||ch=='_')
            {
                pStartCh=pCrtCh;// memoreazainceputul ID-ului
                pCrtCh++;// consuma caracterul
                state=1;// trece la noua stare
            }
            else if(ch=='=')
            {
                pCrtCh++;
                state=3;
            }
            else if(ch=='>')
            {
                pCrtCh++;
                state=6;
            }
            else if(ch=='<')
            {
                pCrtCh++;
                state=9;
            }
            else if(ch=='!')
            {
                pCrtCh++;
                state=13;
            }
            else if(ch=='\\')
            {
                pCrtCh++;
                state=16;
            }
            else if(ch=='&')
            {
                pCrtCh++;
                state=31;
            }
            else if(ch=='|')
            {
                pCrtCh++;
                state=33;
            }
            else if(ch==',')
            {
                pCrtCh++;
                state=19;
            }
            else if(ch==';')
            {
                pCrtCh++;
                state=20;
            }
            else if(ch=='(')
            {
                pCrtCh++;
                state=21;
            }
            else if(ch==')')
            {
                pCrtCh++;
                state=22;
            }
             else if(ch=='[')
            {
                pCrtCh++;
                state=23;
            }
            else if(ch==']')
            {
                pCrtCh++;
                state=24;
            }
            else if(ch=='{')
            {
                pCrtCh++;
                state=25;
            }
            else if(ch=='}')
            {
                pCrtCh++;
                state=26;
            }
            else if(ch=='+')
            {
                pCrtCh++;
                state=27;
            }
            else if(ch=='-')
            {
                pCrtCh++;
                state=28;
            }
            else if(ch=='*')
            {
                pCrtCh++;
                state=29;
            }
            else if(ch=='.')
            {
                pCrtCh++;
                state=30;
            }
            else if(ch=='\'')
            {
                pCrtCh++;
                state=35;
            }
            else if(ch=='\"')
            {
                pCrtCh++;
                state=36;
            }
            /**SPACE*/
            else if(ch==' '||ch=='\r'||ch=='\t')
            {
                pCrtCh++;// consuma caracterulsi ramane in starea 0
            }
            else if(ch=='\n') // tratat separat pentru a actualiza linia curenta
            {
                line++;
                pCrtCh++;
            }
            /**SPACE*/
            else if(ch=='\0') // sfarsit de sir
            {
                //addTk(END);
                return END;
            }
            else if(isdigit(ch)){
                    pCrtCh++;
                    state=35;
                }
            //else tkerr(addTk(END),"caracter invalid");
        break;

        case 1:
            if(isalnum(ch)||ch=='_')
                pCrtCh++;
            else state=2;
        break;

         case 2:
                nCh=pCrtCh-pStartCh;// lungimea cuvantuluigasit
                // teste cuvinte cheie
                if(nCh==5&&!memcmp(pStartCh,"break",5))
                {
                        //tk=addTk(BREAK);
                        return BREAK;
                }
                else if(nCh==4&&!memcmp(pStartCh,"char",4))
                {
                        //tk=addTk(CHAR);
                        return CHAR;
                }
                else if(nCh==6&&!memcmp(pStartCh,"double",6))
                {
                    //tk=addTk(DOUBLE);
                    return DOUBLE;
                }
                else if(nCh==4&&!memcmp(pStartCh,"else",4))
                {
                    //tk=addTk(ELSE);
                    return ELSE;
                }
                else if(nCh==3&&!memcmp(pStartCh,"for",3))
                {
                        //tk=addTk(FOR);
                        return FOR;
                }
                else if(nCh==2&&!memcmp(pStartCh,"if",2))
                {
                    //tk=addTk(IF);
                    return IF;
                }
                else if(nCh==3&&!memcmp(pStartCh,"int",3))
                {
                    //tk=addTk(INT);
                    return INT;
                }
                else if(nCh==6&&!memcmp(pStartCh,"return",6))
                {
                    //tk=addTk(RETURN);
                    return RETURN;
                }
                else if(nCh==6&&!memcmp(pStartCh,"struct",6))
                {
                    //tk=addTk(STRUCT);
                    return STRUCT;
                }
                else if(nCh==4&&!memcmp(pStartCh,"void",4))
                {
                    //tk=addTk(VOID);
                    return VOID;
                }
                else if(nCh==5&&!memcmp(pStartCh,"while",5))
                {
                        //tk=addTk(WHILE);
                        return WHILE;
                }
                else{// daca nu este un cuvantcheie, atunci e un ID
                    //tk=addTk(ID);
                    //tk->text=createString(pStartCh,pCrtCh);
                    }
                return ID;

        case 3:
            if(ch=='=')
            {
                pCrtCh++;
                state=4;
            }
            else state=5;
        break;
        case 4:
            //addTk(EQUAL);
        return EQUAL;

        case 5:
            //addTk(ASSIGN);
        return ASSIGN;
        case 6:
            if(ch=='=')
            {
                pCrtCh++;
                state=7;
            }
            else state=8;
        break;
        case 7:
            //addTk(GREATEREQ);
        return GREATEREQ;
        case 8:
            //addTk(GREATER);
        return GREATER;
         case 10:
            if(ch=='=')
            {
                pCrtCh++;
                state=11;
            }
            else state=12;
        break;
        case 11:
            //addTk(LESSEQ);
        return LESSEQ;
        case 12:
            //addTk(LESS);
        return LESS;
        case 13:
            if(ch=='=')
            {
                pCrtCh++;
                state=14;
            }
            else state=15;
        case 14:
            //addTk(NOTEQ);
        return NOTEQ;
        case 15:
            //addTk(NOT);
        return NOT;
        case 16:
            if(ch=='\\')
            {
                pCrtCh++;
                state=17;
            }
            else state=18;
        case 17:
            //addTk(LINECOMMENT);
        return LINECOMMENT;
        case 18:
            //addTk(DIV);
        return DIV;
        case 19:
            //addTk(COMMA);
        return COMMA;
        case 20:
            //addTk(SEMICOLON);
        return SEMICOLON;
        case 21:
            //addTk(LPAR);
        return LPAR;
        case 22:
            //addTk(RPAR);
        return RPAR;
        case 23:
            //addTk(LBRACKET);
        return LBRACKET;
        case 24:
            //addTk(RBRACKET);
        return RBRACKET;
        case 25:
            //addTk(LACC);
        return LACC;
        case 26:
            //addTk(RACC);
        return RACC;
        case 27:
            //addTk(ADD);
        return ADD;
        case 28:
            //addTk(SUB);
        return SUB;
        case 29:
            //addTk(MUL);
        return MUL;
        case 30:
            //addTk(DOT);
        return DOT;
        case 31:
            if(ch=='&')
            {
                pCrtCh++;
                state=32;
            }
        case 32:
            //addTk(AND);
        return AND;
        case 33:
            if(ch=='|')
            {
                pCrtCh++;
                state=34;
            }
        case 34:
            //addTk(OR);
        return OR;
        case 35:
                if(ch=='.'){
                    pCrtCh++;
                    strcpy(inBuf,pCrtCh);
                    state=37;
                    }
                else {
                        strcpy(inBuf,pCrtCh);
                        state=36;
                    }
        case 36:
                //addTk(CT_INT);
        return CT_INT;
        case 37:
                if(isdigit(ch)){
                    pCrtCh++;
                    state=38;
                }
        case 38:
                if(isdigit(ch)){
                    pCrtCh++;
                    state=38;
                }
                else if(ch=='e'||ch=='E'){
                    pCrtCh++;
                    state=39;
                }
                else state=42;
        case 39:
                if(ch=='+'||ch=='-')
                {
                    pCrtCh++;
                    state=40;
                }
                else if(isdigit(ch)){
                    state=40;
                }
        case 40:
            if(isdigit(ch))
            {
                pCrtCh++;
                state=41;
            }
        case 41:
            if(isdigit(ch))
                {
                    pCrtCh++;
                    state=41;
                }
            else state=42;
        case 101:
            //addTk(CT_REAL);
        return CT_REAL;
      }
    }
}

int main()
{
    FILE *fis;
    if((fis = fopen("2.c","rb"))==NULL)
    {
        printf("\n Eroare fisier!\n");
        return 1;
    }
    int n=fread(inBuf, 1, 30000, fis);
    fclose(fis);
    //inBuf[n] = '\0';
    ini();
    int z;
//Afisiarea sirului care urmeaza a fi verificat lexical si sintactic
//printf("Sirul din fisier este:"\n%s\n\n", inBuf);
//pch=inBuf;
//while(getNextTk()!=END) {}
//afisare atomi
pCrtCh=inBuf;
     for(int i=0;i< strlen(inBuf);i++)
            {
                //strcpy(afis,inBuf);
                //printf("%s\n",pCrtCh);
                z=getNextToken();
                 printf("%s\n",codename[z]);
                 //printf("aici: %s\n",tokens->code);
                 //printf("%s\n",pCrtCh);
                //afis[q]='\0';
                  //  printf("%s",afis);
                //printf("\n");
            }
    return 0;
}
