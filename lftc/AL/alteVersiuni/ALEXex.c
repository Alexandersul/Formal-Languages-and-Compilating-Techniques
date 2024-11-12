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
STRUCT,VOID,WHILE}; /// codurile AL

static const char *codename[] = {
"ADD","AND","ASSIGN","EQUAL","COMMA","CT_INT","CT_REAL","CT_STRING","DIV","DOT","END","GREATER","GREATEREQ",
"ID","LACC","LBRACKET","LESS","LESSEQ",
"LPAR","MUL","NOT","NOTEQ","OR","RACC","RBRACKET","RPAR","SEMICOLON","SPACE","SUB","CT_CHAR",
"BREAK","CHAR","DOUBLE","ELSE","FOR","IF","INT","RETURN",
"STRUCT","VOID","WHILE"
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

typedef struct _Token{
    int code;// codul (numele)
        union{
            char *text;// folosit pentruID, CT_STRING (alocat dinamic)
            int i;// folosit pentruCT_INT, CT_CHAR
            double r;// folosit pentruCT_REAL
            };
        int line;// linia din fisierulde intrare
        struct _Token *next;// inlantuirela urmatorul AL
        }Token;

Token *lastToken;
Token *tokens;
char *pCrtCh;
char inBuf[10000];

void ini(){
    pCrtCh=(char *)malloc(sizeof(char));
    tokens=(Token*)malloc(sizeof(Token));
    lastToken=(Token*)malloc(sizeof(Token));
    //inBuf=(char *)malloc(sizeof(char)*10000);
}

Token *addTk(int code)
{
    Token *tk;
    SAFEALLOC(tk,Token);
    tk->code=code;
    tk->line=line;
    tk->next=NULL;
    if(lastToken){
        lastToken->next=tk;
    }else{
        tokens=tk;
    }
    lastToken=tk;
    return tk;
}

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
int q;
char afis[10000];
int getNextToken()
{
    int state=0;
    char ch;
    char *pStartCh=(char *)malloc(sizeof(char));
    Token *tk;
    int nCh;
    for(;;){
        ch=*pCrtCh;
        switch(state){
            case 0:
                if(isalpha(ch)||ch=='_'){
                    pStartCh=pCrtCh;
                    pCrtCh++;// consuma caracterul
                    state=1;// trece la noua stare
                    }
                //CONSTANTE
                else if(ch=='\"'){
                    pCrtCh++;
                    state=23;
                }

                else if(ch=='\''){
                    pCrtCh++;
                    state=12;
                }

                else if(isdigit(ch)){
                    pCrtCh++;
                    state=11;
                }
                //CONSTANTE

                //DELIMITATORI
                else if(ch==','){
                    pCrtCh++;
                    state=14;
                }
                else if(ch==';'){
                    q++;
                    pCrtCh++;
                    strcpy(inBuf,pCrtCh);
                    state=15;
                }
                else if(ch=='('){
                    q++;
                    pCrtCh++;
                    //printf("%.1s",inBuf);
                    strcpy(inBuf,afis);
                    strcpy(inBuf,pCrtCh);
                    state=16;
                }
                else if(ch==')'){
                    pCrtCh++;
                    strcpy(inBuf,pCrtCh);
                    strcpy(afis,inBuf);
                    state=17;
                }
                else if(ch=='['){
                    pCrtCh++;
                    state=18;
                }
                else if(ch==']'){
                    pCrtCh++;
                    state=19;
                }
                else if(ch=='{'){
                    q++;
                    pCrtCh++;
                    strcpy(inBuf,pCrtCh);
                    state=63;
                }
                else if(ch=='}'){
                    pCrtCh++;
                    state=64;
                }
                else if((ch=='\0')||(ch==EOF)){
                    pCrtCh++;
                    state=65;
                }
                //DELIMITATORI

                //OPERATORI
                else if(ch=='+'){
                    q++;
                    pCrtCh++;
                    strcpy(inBuf,pCrtCh);
                    state=115;
                }
                else if(ch=='-'){
                    pCrtCh++;
                    state=117;
                }
                else if(ch=='*'){
                    pCrtCh++;
                    state=119;
                }
                else if(ch=='.'){
                    pCrtCh++;
                    state=123;
                }
                else if(ch=='&'){
                    pCrtCh++;
                    state=126;
                }
                else if(ch=='|'){
                    pCrtCh++;
                    state=127;
                }
                else if(ch=='!'){
                    pCrtCh++;
                    state=129;
                }
                else if(ch=='='){
                    q++;
                    pCrtCh++;
                    strcpy(inBuf,pCrtCh);
                    state=133;
                }
                else if(ch=='<'){
                    q++;
                    pCrtCh++;
                    strcpy(inBuf,pCrtCh);
                    state=137;
                }
                else if(ch=='>'){
                    pCrtCh++;
                    state=141;
                }

                //OPERATORI
                else if(ch==' '||ch=='\r'||ch=='\t'){
                    //q++;
                    pCrtCh++;
                    //strcpy(inBuf,pCrtCh);
                }
                else if(ch=='\n'){   // tratat separat pentru a actualiza liniacurenta
                    line++;
                    pCrtCh++;
                    }
                else if(ch==0){   // sfarsit de sir
                    addTk(END);
                    return END;
                    }
                else tkerr(addTk(END),"caracter invalid");
                break;
            case 1:
                if(isalnum(ch)||ch=='_')
                    {
                        pCrtCh++;
                        q++;
                    }
                    else state=2;
                break;
            case 2:
                nCh=pCrtCh-pStartCh;// lungimea cuvantuluigasit
                // teste cuvinte +nCh)
                if(nCh==5&&!memcmp(pStartCh,"break",5))tk=addTk(BREAK);
                else if(nCh==4&&!memcmp(pStartCh,"char",4))tk=addTk(CHAR);
                else if(nCh==6&&!memcmp(pStartCh,"double",6))tk=addTk(DOUBLE);
                else if(nCh==4&&!memcmp(pStartCh,"else",4))tk=addTk(ELSE);
                else if(nCh==3&&!memcmp(pStartCh,"for",3))tk=addTk(FOR);
                else if(nCh==2&&!memcmp(pStartCh,"if",2))tk=addTk(IF);
                else if(nCh==3&&!memcmp(pStartCh,"int",3))tk=addTk(INT);
                else if(nCh==6&&!memcmp(pStartCh,"return",6))tk=addTk(RETURN);
                else if(nCh==6&&!memcmp(pStartCh,"struct",6))tk=addTk(STRUCT);
                else if(nCh==4&&!memcmp(pStartCh,"void",4))tk=addTk(VOID);
                else if(nCh==5&&!memcmp(pStartCh,"while",5))tk=addTk(WHILE);
                else{// daca nu este un cuvantcheie, atunci e un ID
                    tk=addTk(ID);
                    //pStartCh--;
                    tk->text=createString(pStartCh,pCrtCh);
                    //printf("%s\n",tk->text);
                    }
                //q=nCh;
                //strcpy(afis,inBuf+nCh);
                //strcpy(inBuf,inBuf+nCh);
                return tk->code;
            //CONSTANTE
            case 23:
                if(ch=='\"'){
                    pCrtCh++;
                    state=33;
                }
                else{
                    pCrtCh++;
                    state=23;
                }
            case 33:
                addTk(CT_STRING);
                return CT_STRING;

            case 12:
                if(!(ch=='\'')){
                    pCrtCh++;
                    state=22;
                }
            case 22:
                if(ch=='\''){
                    pCrtCh++;
                    state=145;
                }
            case 145:
                addTk(CT_CHAR);
            return CT_CHAR;

            case 11:
                if(ch=='.'){
                    pCrtCh++;
                    strcpy(inBuf,pCrtCh);
                    state=31;
                    }
                else {
                        strcpy(inBuf,pCrtCh);
                        state=21;
                    }

            case 21:
                addTk(CT_INT);
                return CT_INT;

            case 31:
                if(isdigit(ch)){
                    pCrtCh++;
                    state=41;
                }
            case 41:
                if(isdigit(ch)){
                    pCrtCh++;
                    state=41;
                }
                else if(ch=='e'||ch=='E'){
                    pCrtCh++;
                    state=71;
                }
                else state=101;
            case 71:
                if(ch=='+'||ch=='-')
                {
                    pCrtCh++;
                    state=81;
                }
                else if(isdigit(ch)){
                    state=81;
                }//??????????   hereeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee
            case 81:
                if(isdigit(ch)){
                    pCrtCh++;
                    state=91;
                }
            case 91:
                if(isdigit(ch)){
                    pCrtCh++;
                    state=91;
                }
                else state=101;
            case 101:
                addTk(CT_REAL);
                return CT_REAL;
            //CONSTANTE


            //DELIMITATORI
            case 14:
                addTk(COMMA);
                return COMMA;
            case 15:
                addTk(SEMICOLON);
                return SEMICOLON;
            case 16:
                addTk(LPAR);
                return LPAR;
            case 17:
                addTk(RPAR);
                return RPAR;
            case 18:
                addTk(LBRACKET);
                return ASSIGN;
            case 19:
                addTk(RBRACKET);
                return RBRACKET;
            case 63:
                addTk(LACC);
                return LACC;
            case 64:
                addTk(RACC);
                return RACC;
            case 65:
                addTk(END);
                return END;
            //DELIMITATORI

            //OPERATORI
            case 115:
                addTk(ADD);
                return ADD;
            case 117:
                addTk(SUB);
                return SUB;
            case 119:
                addTk(MUL);
                return MUL;
            case 123:
                addTk(DOT);
                return DOT;

            case 126:
                if(ch=='&'){
                    pCrtCh++;
                    strcpy(inBuf,pCrtCh);
                    state=190;
                }
            case 190:
                addTk(AND);
                return AND;

            case 127:
                if(ch=='|'){
                    pCrtCh++;
                    state=128;
                }
            case 128:
                addTk(OR);
                return OR;

            case 129:
                if(ch=='='){
                    pCrtCh++;
                    state=131;
                    }
                else state=130;
                break;
            case 130:
                addTk(NOT);
                return NOT;
            case 131:
                addTk(NOTEQ);
                return NOTEQ;

            case 133:
                if(ch=='='){
                    pCrtCh++;
                    state=135;
                    }
                else state=134;
                break;
            case 135:
                addTk(EQUAL);
                return EQUAL;
            case 134:
                addTk(ASSIGN);
                return ASSIGN;

            case 137:
                if(ch=='='){
                    pCrtCh++;
                    state=139;
                    }
                else state=138;
                break;
            case 138:
                addTk(LESS);
                return LESS;
            case 139:
                addTk(LESSEQ);
                return LESSEQ;

            case 141:
                if(ch=='='){
                    pCrtCh++;
                    state=143;
                    }
                else state=142;
                break;
            case 132:
                addTk(GREATER);
                return GREATER;
            case 143:
                addTk(GREATEREQ);
                return GREATEREQ;
            //OPERATORI
            }
        }
}

int main()
{
    FILE *fis;
    int x=0;
    if((fis=fopen("1.c","rb")) == NULL)
    {
        printf("\n Eroare fisier!\n");
        return 1;
    }
    int n = fread(inBuf, 1, 10000, fis);
    fclose(fis);
    ini();
    //char* afis=(char *)malloc(sizeof(char)*10000);
    int z;
    strcpy(afis,inBuf);
    pCrtCh=inBuf;
    for(int i=0;i< strlen(inBuf);i++)
            {
                //pCrtCh=inBuf;
                //strcpy(afis,inBuf);
                z=getNextToken();
                //afis[q]='\0';
                printf("%s\n",codename[z]);
                  //  printf("%s",afis);
                //printf("\n");
            }
    return 0;
}
