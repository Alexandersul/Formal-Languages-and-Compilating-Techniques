#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include<string.h>
#include<ctype.h>

//--------------------------------------INITIAL PENTRU ALEX------------------------------------------------------------

void err(const char*fmt,...)
{
    va_list va;
    va_start(va,fmt);
    fprintf(stderr,"error: ");
    vfprintf(stderr,fmt,va);
    fputc('\n',stderr);
    va_end(va);
    exit(-1);
}

#define SAFEALLOC(var,Type) if((var=(Type*)malloc(sizeof(Type)))==NULL)err("notenough memory");

char inBuf[30001];
char *pCrtCh;
int line=1;
int code;

bool unit();
bool structDef();
bool varDef();
bool typeBase();
bool arrayDecl();
bool fnDef();
bool fnParam();
bool stm();
bool stmCompound();
bool expr();
bool exprAssign();
bool exprOr();
bool exprAnd();
bool exprEq();
bool exprRel();
bool exprAdd();
bool exprMul();
bool exprCast();
bool exprUnary();
bool exprPostfix();
bool exprPrimary();

bool exprPostfixPrim();
bool exprMulPrim();
bool exprAddPrim();
bool exprRelPrim();
bool exprEqPrim();
bool exprAndPrim();
bool exprOrPrim();

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

char* createString(char* a,char* b){
char *s=(char*)malloc((b-a+5)*sizeof(char));
strncpy(s,a,b-a);
s[b-a]='\0';
return s;
}

void afisareAtomi()
{
Token *tk=tokens;
    FILE *fisierulo;
    fisierulo=fopen("out.txt","wb");

    if(fisierulo==NULL){
        printf("Eroare fisier\n");
        exit(EXIT_FAILURE);
    }

    while(tk!=NULL)
    {
        if(tk->code==CT_STRING)
        {
            fprintf(fisierulo,"%d %s:%s\n",tk->line,codename[tk->code],tk->text);
        }
        else if(tk->code==CT_INT)
        {
            fprintf(fisierulo,"%d %s:%d\n",tk->line,codename[tk->code],tk->i);
        }
        else if(tk->code==CT_REAL)
        {
            fprintf(fisierulo,"%d %s:%f\n",tk->line,codename[tk->code],tk->r);
        }
        else if(tk->code==ID)
        {
            fprintf(fisierulo,"%d %s:%s\n",tk->line,codename[tk->code],tk->text);
        }
        else if(tk->code==CT_CHAR)
        {
            fprintf(fisierulo,"%d %s:%c\n",tk->line,codename[tk->code],tk->i);
        }
        else
        {
            fprintf(fisierulo,"%d %s\n",tk->line,codename[tk->code]);
        }
    tk=tk->next;
    }
    fclose(fisierulo);
}

int getNextToken()
{
    int state=0,nCh;
    char ch;
    char* pStartCh;
    Token *tk;
    for(;;){
        ch=*pCrtCh;
        switch(state){
            case 0:// testare tranzitii posibile din starea 0
                if(isalpha(ch)||ch=='_')
                {
                    pStartCh=pCrtCh;
                    pCrtCh++;// consuma caracterul
                    state=1;// trece la noua stare
                }
                else if(ch==' '||ch=='\r'||ch=='\t')
                {
                    pCrtCh++;// consuma caracterulsi ramane in starea 0
                }
                else if(ch=='\n') // tratat separat pentru a actualiza linia curenta
                {
                    line++;
                    pCrtCh++;
                }
                else if(ch>='0'&& ch<='9')
                {
                    pStartCh=pCrtCh;
                    pCrtCh++;
                    state=11;
                }
                else if(ch=='\'')
                {
                    pCrtCh++;
                    state=12;
                }
                else if(ch=='\"')
                {
                    pStartCh=pCrtCh;
                    pCrtCh++;
                    state=23;
                }
                else if(ch=='}')
                {
                    pCrtCh++;
                    addTk(RACC);
                    return RACC;
                }
                else if(ch=='{')
                {
                    pCrtCh++;
                    addTk(LACC);
                    return LACC;
                }
                else if(ch=='[')
                {
                    pCrtCh++;
                    addTk(LBRACKET);
                    return LBRACKET;
                }
                else if(ch==']')
                {
                    pCrtCh++;
                    addTk(RBRACKET);
                    return RBRACKET;
                }
                else if(ch=='(')
                {
                    pCrtCh++;
                    addTk(LPAR);
                    return LPAR;
                }
                else if(ch==')')
                {
                    pCrtCh++;
                    addTk(RPAR);
                    return RPAR;
                }
                else if(ch==';')
                {
                    pCrtCh++;
                    addTk(SEMICOLON);
                    return SEMICOLON;
                }

                else if(ch==',')
                {
                    pCrtCh++;
                    addTk(COMMA);
                    return COMMA;
                }
                else if(ch=='+')
                {
                    pCrtCh++;
                    addTk(ADD);
                    return ADD;
                }
                else if(ch=='-')
                {
                    pCrtCh++;
                    addTk(SUB);
                    return SUB;
                }
                else if(ch=='*')
                {
                    pCrtCh++;
                    addTk(MUL);
                    return MUL;
                }
                else if(ch=='.')
                {
                    pCrtCh++;
                    addTk(DOT);
                    return DOT;
                }
                else if(ch=='>')
                {
                    pCrtCh++;
                    state=141;
                }
                else if(ch=='/')
                {
                    pCrtCh++;
                    state=147;
                }
                else if(ch=='&')
                {
                    pCrtCh++;
                    state=126;
                }
                else if(ch=='|')
                {
                    pCrtCh++;
                    state=127;
                }
                else if(ch=='!')
                {
                    pCrtCh++;
                    state=129;
                }
                else if(ch=='=')
                {
                    pCrtCh++;
                    state=133;
                }
                else if(ch=='<')
                {
                    pCrtCh++;
                    state=137;
                }
                else if(ch=='\0')
                {
                    addTk(END);
                    return END;
                }
                else tkerr(addTk(END),"caracter invalid");
                break;
            case 1:
                if(isalnum(ch)||ch=='_')pCrtCh++;
                else state=2;
                break;
            case 2:
                nCh=pCrtCh-pStartCh;// lungimea cuvantului gasit
                // teste cuvinte cheie
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
                else
                {// daca nu este un cuvant cheie, atunci e un ID
                    tk=addTk(ID);
                    tk->text=createString(pStartCh,pCrtCh);
                }
                return tk->code;
            case 11:
                if(ch=='.')
                {
                    pCrtCh++;
                    state=31;
                }
                else if(ch=='e'||ch=='E')
                {
                    pCrtCh++;
                    state=71;
                }
                else if(ch>='0'&&ch<='9')
                {
                    pCrtCh++;
                }
                else
                {
                    state=21;
                }
                break;

            case 21:
                tk=addTk(CT_INT);
                tk->i=atoi(createString(pStartCh,pCrtCh));
                return CT_INT;
                break;
            case 31:
                if(ch>='0'&&ch<='9')
                {
                    pCrtCh++;
                    state=41;
                }
                else

                {
                    printf("Obligatoriu cifre dupa punct\n");
                    exit(-1);
                }
                break;
            case 41:
                if(ch>='0'&&ch<='9')
                {
                    pCrtCh++;
                }
                else if(ch=='e'||ch=='E')
                {
                    pCrtCh++;
                    state=71;
                }
                else
                {
                    tk=addTk(CT_REAL);
                    tk->r=atof(createString(pStartCh,pCrtCh));
                    return CT_REAL;
                }
                break;
            case 71:
                if(ch=='+'||ch=='-')
                {
                    pCrtCh++;
                    state=81;
                }
                else
                state=81;
                break;
            case 81:
                if(ch>='0'&& ch<='9')
                {
                    pCrtCh++;
                    state=91;
                }
                break;
            case 91:
                if(ch>='0'&&ch<='9')
                {
                    pCrtCh++;
                }
                else
                {
                    tk=addTk(CT_REAL);
                    tk->r=atof(createString(pStartCh,pCrtCh));
                    return CT_REAL;
                }
                break;
            case 12:
                if(ch!='\'')
                {
                    pCrtCh++;
                    state=22;
                }
                break;
            case 22:
                if(ch=='\'')
                {
                    tk=addTk(CT_CHAR);
                    tk->i=*(pCrtCh-1);
                    return CT_CHAR;
                }
                //else return;

            case 23:
                if(ch!='\"')
                    {
                        pCrtCh++;
                    }
                else
                    {
                        state=33;
                    }
                break;
            case 33:
                pCrtCh++;
                tk=addTk(CT_STRING);
                tk->text=createString(pStartCh+1,pCrtCh-1);
                return CT_STRING;
                break;
            case 147:
                if(ch=='/')
                {
                    state=148;
                }
                else
                {
                    addTk(DIV);
                    return DIV;
                }break;
            case 126:
                if(ch=='&')
                {
                    pCrtCh++;
                    addTk(AND);
                    return AND;
                }
                break;
            case 127:
                if(ch=='|')
                {
                    pCrtCh++;
                    addTk(OR);
                    return OR;
                }
                break;
            case 129:
                if(ch=='=')
                {
                    pCrtCh++;
                    addTk(NOTEQ);
                    return NOTEQ;
                }
                else
                {
                    addTk(NOT);
                    return NOT;
                }
                break;
            case 133:
                if(ch=='=')
                {
                    pCrtCh++;
                    addTk(EQUAL);
                    return EQUAL;
                }
                else
                {
                    addTk(ASSIGN);
                    return ASSIGN;
                }
                break;
            case 137:
                if(ch=='=')
                {
                    pCrtCh++;
                    addTk(LESSEQ);
                    return LESSEQ;
                }
                else
                {
                    addTk(LESS);
                    return LESS;
                }
            case 141:
                if(ch=='=')
                {
                    pCrtCh++;
                    addTk(GREATEREQ);
                    return GREATEREQ;
                }
                else
                {
                    addTk(GREATER);
                    return GREATER;
                }
                break;
            case 148:
                if(ch!='\0'&&ch!='\r'&&ch!='\n')
                {
                    pCrtCh++;
                    state=681;
                }
                break;
            case 681:
                if(ch!='\0'&&ch!='\r'&&ch!='\n')
                {
                    pCrtCh++;
                }
                else
                {
                    pCrtCh++;
                    state=0;
                }
            }
    }
}
//-----------------------------------------PANA AICI ALEX-----------------------------------------------

bool varDef();
bool arrayDecl();
bool fnParam();
bool typeBase();
bool stmCompound();
bool stm();
bool expr();
bool exprAssign();
bool structDef();
bool fnDef();
bool exprOR();
bool exprAnd();
bool exprEq();
bool exprRel();
bool exprAdd();
bool exprMul();
bool exprCast();
bool exprUnary();
bool exprPrimary();

Token *iTk;          ///iteratorul din lista de atomi care initial pointeaza la primul atom din lista
Token *consumedTk;   ///atomul care a fost consumat

///o functie care primeste ca parametru un cod de atom si ii returneaza numele
const char* tkCodeName(int code)
{
    return codename[code];
}


///primeste ca parametru un cod de atom
///daca la pozitia curenta a iteratorului se consuma atomul respectiv la returna true, else false
bool consume(int code)
{
    printf("consume(%s)",tkCodeName(code));
    if(iTk->code==code)
    {
        consumedTk=iTk;
        iTk=iTk->next;
        printf(" => consumed\n");
        return true;
    }
    printf(" => found %s\n",tkCodeName(iTk->code));
    return false;
}

/// typeBase: INT | DOUBLE | CHAR | STRUCT ID
///daca o ramura se indeplineste atunci nu vom mai testa pe celelalte
///daca vrem sa  testam un tipul de baza atunci apelam typeBase
bool typeBase()
{
    if(consume(INT))
    {
        return true;
    }
    if(consume(DOUBLE))
    {
        return true;
    }
    if(consume(CHAR))
    {
        return true;
    }
    if(consume(STRUCT))
    {
        if(consume(ID))
        {
            return true;
        }
    }
    return false;
}

///unit: ( structDef | fnDef | varDef )* END
bool unit()
{
    Token *pStartToken = iTk;       ///salvare pozitie initiala

    for(;;)
    {
        if(structDef())
        {
        }
        else if(fnDef())
        {
        }
        else if(varDef())
        {
        }
        else break;
    }

    if(consume(END))
    {
        return true;
    }
    else
    {
        tkerr(iTk,"Missing END token\n");
    }

    iTk = pStartToken;  ///refacere pozitie initiala daca atomul nu a fost consumat
    return false;
}
// structDef: STRUCT ID LACC varDef* RACC SEMICOLON
bool structDef()
{
    Token *start = iTk;

    if (consume(STRUCT))
    {
        if (consume(ID))
        {
            if (consume(LACC))
            {
                for (;;)
                {
                    if (varDef())
                    {
                    }
                    else
                        break;
                }
                if (consume(RACC))
                {
                    if (consume(SEMICOLON))
                    {
                        return true;
                    }
                    else
                        tkerr(iTk, "Missing ; after struct declaration\n");
                }
                else
                    tkerr(iTk, "Missing } after struct body\n");
            }
        }
        else
            tkerr(iTk, "Missing struct name\n");
    }

    iTk = start;
    return false;
}

///varDef: typeBase ID arrayDecl? SEMICOLON
// varDef: typeBase ID arrayDecl? SEMICOLON
bool varDef()
{
    Token *start = iTk;

    if (typeBase())
    {
        if (consume(ID))
        {
            if (arrayDecl())
            {
            }
            if (consume(SEMICOLON))
            {
                return true;
            }
            else
                tkerr(iTk, "Missing ; after variable declaration\n");
        }
        else
            tkerr(iTk, "Missing variable name\n");
    }

    iTk = start;
    return false;
}



// arrayDecl: LBRACKET CT_INT? RBRACKET
bool arrayDecl()
{
    Token *start = iTk;

    if (consume(LBRACKET))
    {
        if (consume(CT_INT))
        {
        }
        if (consume(RBRACKET))
        {
            return true;
        }
        else
            tkerr(iTk, "Missing ] after array declaration\n");
    }
    iTk = start;
    return false;
}

// fnDef: ( typeBase | VOID ) ID LPAR ( fnParam (COMMA fnParam)* )? RPAR stmCompound
bool fnDef()
{
    Token *start = iTk;

    if (typeBase() || consume(VOID))
    {
        if (consume(ID))
        {
            if (consume(LPAR))
            {
                if (fnParam())
                {
                    for (;;)
                    {
                        if (consume(COMMA))
                        {
                            if (fnParam())
                            {
                            }
                            else
                                tkerr(iTk, "Missing argument from the function header\n");
                        }
                        else
                            break;
                    }
                }
                if (consume(RPAR))
                {
                    if (stmCompound())
                    {
                        return true;
                    }
                    else
                        tkerr(iTk, "Function has no body\n");
                }
                else
                    tkerr(iTk, "Missing ) after the function arguments\n");
            }
        }
        else
            tkerr(iTk, "Missing function name\n");
    }

    iTk = start;
    return false;
}

///fnParam: typeBase ID arrayDecl?
bool fnParam()
{
    Token *pStartToken = iTk;

    if(typeBase())
    {
        if(consume(ID))
        {
            if(arrayDecl())
            {
            }

            return true;
        }
        else
        {
            tkerr(iTk,"Missing array name\n");
        }
    }
    else
    {
        tkerr(iTk,"Missing function type\n");
    }
    iTk = pStartToken;
    return false;
}

bool stm()
{
    Token *start = iTk;

    if (stmCompound())
    {
        return true;
    }

    if (consume(IF))
    {
        if (consume(LPAR))
        {
            if (expr())
            {
                if (consume(RPAR))
                {
                    if (stm())
                    {
                        if (consume(ELSE))
                        {
                            if (stm())
                            {
                                return true;
                            }
                            else
                                tkerr(iTk, "The else instruction cannot have an empty body\n");
                        }
                        return true;
                    }
                    else
                        tkerr(iTk, "The if instruction must have a body\n");
                }
                else
                    tkerr(iTk, "Missing ) after the if condition\n");
            }
            else
                tkerr(iTk, "Missing condition from the if instruction\n");
        }
        else
            tkerr(iTk, "Missing ( after the if instruction\n");
    }

    if (consume(WHILE))
    {
        if (consume(LPAR))
        {
            if (expr())
            {
                if (consume(RPAR))
                {
                    if (stm())
                    {
                        return true;
                    }
                    else
                        tkerr(iTk, "The while instruction must have a body\n");
                }
                else
                    tkerr(iTk, "Missing ) after the while condition\n");
            }
        }
        else
            tkerr(iTk, "Missing ( after the while instruction\n");
    }

    if (consume(FOR))
    {
        if (consume(LPAR))
        {
            if (expr())
            {
            }
            if (consume(SEMICOLON))
            {
                if (expr())
                {
                }
                if (consume(SEMICOLON))
                {
                    if (expr())
                    {
                    }
                    if (consume(RPAR))
                    {
                        if (stm())
                        {
                            return true;
                        }
                        else
                            tkerr(iTk, "The for instruction must have a body\n");
                    }
                    else
                        tkerr(iTk, "Missing ) after the for condition\n");
                }
                else
                    tkerr(iTk, "Missing second ; from the for condition\n");
            }
            else
                tkerr(iTk, "Missing first ; from the for condition\n");
        }
        else
            tkerr(iTk, "Missing ( after the for instruction\n");
    }

    if (consume(BREAK))
    {
        if (consume(SEMICOLON))
        {
            return true;
        }
        else
            tkerr(iTk, "Missing ; after the break instruction");
    }

    if (consume(RETURN))
    {
        if (expr())
        {
        }
        if (consume(SEMICOLON))
        {
            return true;
        }
        else
            tkerr(iTk, "Missing ; after the return instruction");
    }

    if (expr())
    {
        if (consume(SEMICOLON))
        {
            return true;
        }
        else
            tkerr(iTk, "Missing ; after expression");
    }

    if (consume(SEMICOLON))
    {
        return true;
    }

    iTk = start;
    return false;
}

// stmCompound: LACC ( varDef | stm )* RACC
bool stmCompound()
{
    Token *start = iTk;

    if (consume(LACC))
    {
        for (;;)
        {
            if (varDef())
            {
            }
            else if (stm())
            {
            }
            else
                break;
        }
        if (consume(RACC))
        {
            return true;
        }
        else
            tkerr(iTk, "Missing } after body");
    }

    iTk = start;
    return false;
}


///expr: exprAssign
bool expr()
{
    Token *pStartToken = iTk;

    if(exprAssign())
    {
        return true;
    }
    /*else
    {
        tkerr(iTk,"Missing assign expression\n");
    }*/

    iTk = pStartToken;
    return false;
}

///exprAssign: exprUnary ASSIGN exprAssign | exprOr
bool exprAssign()       ///modificat
{
    Token *pStartToken = iTk;

    if(exprUnary())
    {
        if(consume(ASSIGN))
        {
            if(exprAssign())
            {
                return true;
            }
            else
            {
                tkerr(iTk,"Invalid expression after =\n");
            }
        }
        else
        {
            tkerr(iTk,"Missing assign statement\n");
        }
    }
    /*
    else
    {
        tkerr(iTk,"Invalid expression before =\n");
    }
*/
    if(exprOr())
    {
        return true;
    }
    /*else
    {
        tkerr(iTk,"Missing | statement =\n");
    }*/

    iTk = pStartToken;
    return false;
}

///exprOr: exprAnd | exprOrPrim
///exprOrPrim: OR exprAnd exprOrPrim | E
bool exprOr()
{
    Token *pStartToken = iTk;

    if(exprAnd())
    {
        if(exprOrPrim())
        {
            return true;
        }
        else
        {
            tkerr(iTk,"Invalid expression after &&\n");
        }
    }

    iTk = pStartToken;
    return false;
}

bool exprOrPrim()
{
    Token *pStartToken = iTk;

    if(consume(OR))
    {
        if(exprAnd())
        {
            if(exprOrPrim())
            {
                return true;
            }
        }
        else
        {
            tkerr(iTk,"Invalid expression after ||\n");
        }
    }
    else
    {
        tkerr(iTk,"Invalid ( after && =\n");
    }

    iTk = pStartToken;
    return false;
}


///exprAnd: exprEq exprAndPrim
///exprAndPrim: AND exprEq exprAndPrim | E
bool exprAnd()
{
    Token *pStartToken = iTk;

    if(exprEq())
    {
        if(exprAndPrim())
        {
            return true;
        }
        else
            tkerr(iTk, "Invalid expression after == \n");
    }

    iTk = pStartToken;
    return false;
}


bool exprAndPrim()
{
    Token *pStartToken = iTk;

    if(consume(AND))
    {
        if(exprEq())
        {
            if(exprAndPrim())
            {
                return true;
            }
            else
                tkerr(iTk, "Invalid expression after == \n");
        }
        else
            tkerr(iTk, "Invalid expression after && \n");
    }
    else
    {
        tkerr(iTk, "Missing && statement\n");
    }

    iTk = pStartToken;
    return false;
}

///exprEq: exprRel exprEqPrim
///exprEqPrim: ( EQUAL | NOTEQ ) exprRel  exprEqPrim | E
bool exprEq()
{
    Token *pStartToken = iTk;

    if(exprRel())
    {
        if(exprEqPrim())
        {
            return true;
        }
        else
            tkerr(iTk, "Invalid expression after ==\n");
    }

    iTk = pStartToken;
    return false;
}

bool exprEqPrim()
{
    Token *pStartToken = iTk;

    if(consume(EQUAL))
    {
        if(exprRel())
        {
            if(exprEqPrim())
            {
                return true;
            }
            else
                tkerr(iTk, "Invalid expression \n");
            }
        else
            tkerr(iTk, "Invalid expression after == \n");
    }

    if(consume(NOTEQ))
    {
        if(exprRel())
        {
            if(exprEqPrim())
            {
                return true;
            }
            else
                tkerr(iTk, "Invalid expression \n");
        }
        else
            tkerr(iTk, "Invalid expression after != \n");
    }

    iTk = pStartToken;
    return false;
}


///exprRel: exprAdd exprRelPrim
///exprRelPrim: ( LESS | LESSEQ | GREATER | GREATEREQ ) exprAdd exprRelPrim | E
bool exprRel()
{
    Token *pStartToken = iTk;
    if(exprAdd())
    {
        if(exprRelPrim())
        {
            return true;
        }
        else
            tkerr(iTk, "Invalid expression \n");
    }

    iTk = pStartToken;
    return false;
}

bool exprRelPrim()
{
    Token *pStartToken = iTk;

    if(consume(LESS))
    {
        if(exprAdd())
        {
            if(exprRelPrim())
            {
                return true;
            }
            else
                tkerr(iTk, "Invalid expression \n");
        }
        else
            tkerr(iTk, "Invalid expression after < \n");
    }
    else
        tkerr(iTk, "Missing < statement\n");



    if(consume(LESSEQ))
    {
        if(exprAdd())
        {
            if(exprRelPrim())
            {
                return true;
            }
            else
                tkerr(iTk, "Invalid expression \n");
        }
        else
            tkerr(iTk, "Invalid expression after <= \n");
    }
    else
        tkerr(iTk, "Missing <= statement\n");


    if(consume(GREATER))
    {
        if(exprAdd())
        {
            if(exprRelPrim())
            {
                return true;
            }
            else
                tkerr(iTk, "Invalid expression \n");
        }
        else
            tkerr(iTk, "Invalid expression after > \n");
    }
    else
        tkerr(iTk, "Missing > statement\n");

    if(consume(GREATEREQ))
    {
        if(exprAdd())
        {
            if(exprRelPrim())
            {
                return true;
            }
            else
                tkerr(iTk, "Invalid expression \n");
        }
        else
            tkerr(iTk, "Invalid expression after >= \n");
    }
    else
        tkerr(iTk, "Missing >= statement\n");

    iTk = pStartToken;
    return false;
}



///exprAdd: exprMul exprAddPrim
///exprAddPrim: ( ADD | SUB ) exprMul exprMulPrim | E
bool exprAdd()
{
    Token *pStartToken = iTk;
    if(exprMul())
    {
        if(exprAddPrim())
        {
            return true;
        }
        else
            tkerr(iTk, "Invalid expression \n");
    }
    /*else
        tkerr(iTk, "Missing * statement\n");*/

    iTk = pStartToken;
    return false;
}


bool exprAddPrim()
{
    Token *pStartToken = iTk;

    if(consume(ADD))
    {
        if(exprMul())
        {
            if(exprAddPrim())
            {
                return true;
            }
            else
                tkerr(iTk, "Invalid expression \n");
        }
        else
            tkerr(iTk, "Invalid expression after + \n");
    }
    else
        tkerr(iTk, "Missing + statement\n");

    if(consume(SUB))
    {
        if(exprMul())
        {
            if(exprAddPrim())
            {
                return true;
            }
            else
                tkerr(iTk, "Invalid expression \n");
        }
        else
            tkerr(iTk, "Invalid expression after - \n");
    }
    else
        tkerr(iTk, "Missing - statement\n");

    iTk = pStartToken;
    return false;
}


///exprMul: exprCast exprMulPrim
///exprMulPrim: ( MUL | DIV ) exprCast exprMulPrim
bool exprMul()
{
    Token *pStartToken = iTk;

    if(exprCast())
    {
        if(exprMulPrim())
        {
            return true;
        }
        else
            tkerr(iTk, "Invalid expression after cast\n");
    }

    iTk = pStartToken;
    return false;
}

bool exprMulPrim()
{
        Token *pStartToken = iTk;


    if (consume(MUL))
    {
        if (exprCast())
        {
            if (exprMulPrim())
            {
                return true;
            }
            else
                tkerr(iTk, "Invalid expression after cast \n");
        }
        else
            tkerr(iTk, "Invalid expression after * \n");
    }

    if (consume(DIV))
    {
        if (exprCast())
        {
            if (exprMulPrim())
            {
                return true;
            }
            else
                tkerr(iTk, "Invalid expression after cast \n");
        }
        else
            tkerr(iTk, "Invalid expression after / \n");
    }

    iTk = pStartToken;
    return true;
}

///exprCast: LPAR typeBase arrayDecl? RPAR exprCast | exprUnary
bool exprCast()
{
    Token *pStartToken = iTk;

    if(consume(LPAR))
    {
        if(typeBase())
        {
            if(arrayDecl())
            {
                return true;
            }

            if(consume(RPAR))
            {
                if(exprCast())
                {
                    return true;
                }
            }
        }
    }

    if(exprUnary())
    {
        return true;
    }

    iTk = pStartToken;
    return false;
}



///exprUnary: ( SUB | NOT ) exprUnary | exprPostfix
bool exprUnary()
{
    Token *pStartToken = iTk;

    if(consume(SUB))
    {
        if(exprUnary())
        {
            return true;
        }
    }

    if(consume(NOT))
    {
        if(exprUnary())
        {
            return true;
        }
    }

    if(exprPostfix())
    {
        return true;
    }

    iTk = pStartToken;
    return false;
}

///exprPostfix: exprPrimary exprPostfixPrim
///exprPostfixPrim: LBRACKET expr RBRACKET exprPostfixPrim
///         | DOT ID exprPostfixPrim
///         | E
bool exprPostfix()
{
    Token *pStartToken = iTk;

    if(exprPrimary())
    {
        if(exprPostfixPrim())
        {
            return true;
        }
    }

    iTk = pStartToken;
    return false;
}


bool exprPostfixPrim()
{
    Token *pStartToken = iTk;

    if(consume(LBRACKET))
    {
        if(expr())
        {
            if(consume(RBRACKET))
            {
                if(exprPostfixPrim())
                {
                    return true;
                }
            }
        }
    }

    if(consume(DOT))
    {
        if(consume(ID))
        {
            if(exprPostfixPrim())
            {
                return true;
            }
        }
    }

    iTk = pStartToken;
    return false;
}



///exprPrimary: ID ( LPAR ( expr ( COMMA expr )* )? RPAR )?
///         | CT_INT | CT_REAL | CT_CHAR | CT_STRING | LPAR expr RPAR


bool exprPrimary()
{
    Token *pStartToken = iTk;

    if(consume(ID))
    {
        if(consume(LPAR))
        {
            if(consume(expr()))
            {
                for(;;)
                {
                    if(consume(COMMA))
                    {
                        if(consume(expr()))
                        {

                        }
                    }
                    else break;
                }
            }

            if(consume(RPAR))
            {
                return true;
            }
        }

        return true;
    }


    if (consume(CT_INT))
    {
        return true;
    }

    if (consume(CT_REAL))
    {
        return true;
    }

    if (consume(CT_CHAR))
    {
        return true;
    }

    if (consume(CT_STRING))
    {
        return true;
    }

    if (consume(LPAR))
    {
        if (expr())
        {
            if (consume(RPAR))
            {
                return true;
            }
            else
                tkerr(iTk, "Missing ) after expression \n");
        }
        else
            tkerr(iTk, "Missing expression after (");
    }


    iTk = pStartToken;
    return false;
}






int main()
{

//--------------------------------------INITIAL PENTRU ALEX------------------------------------------------------------

    FILE *fis;
    fis=fopen("test.txt","rb");   //sint/testare
    if(fis==NULL)
    {
        printf("Eroare fisier\n");
        exit(EXIT_FAILURE);
    }
    int n=fread(inBuf,1,30000,fis);
    inBuf[n]='\n';
    pCrtCh=inBuf;
    fclose(fis);
    while(getNextToken()!=END)
    {

    }
    afisareAtomi();

//-----------------------------------------PANA AICI ALEX-----------------------------------------------

    iTk=tokens;
    printf("\n%d",unit());
    return 0;

}


