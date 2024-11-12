#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include<string.h>
#include<ctype.h>
#include <stdbool.h>
//#include "ALEX.c"
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
Token *iTk; // iteratorul in lista de atomi. Initial pointeaza la primul atom din lista
Token *consumedTk; // atomul care tocmai a fost consumat. Va fi folosit în etapele urmatoare ale compilatorului
const char *tkCodeName(int code) //- o functie care primeste ca parametru un cod de atom si ii returneaza numele
{
    return "bla";
}
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
// stmCompound: LACC ( varDef | stm )* RACC
bool stmCompound()
{
    if(consume(LACC))  // se testeaza daca la pozitia curenta este {
    {
// daca da, atunci { este deja consumat si se continua cu interiorul blocului {...}
    }
}
// fnParam: typeBase ID arrayDecl?


// typeBase: INT | DOUBLE | CHAR | STRUCT ID
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

int arrayDecl(){
    return 1;
}

// fnParam: typeBase ID arrayDecl? ( secventa de 3 componente)
bool fnParam()
{
    Token *start=iTk; // se salveaza pozitia initiala a iteratorului
    if(typeBase())// se testeaza daca la pozitia curenta se poate consuma regula typeBase
    {
        if(consume(ID))  // se ajunge la ID doar daca s-a trecut de typeBase
        {
            // daca da, atunci typeBase este deja consumat si se continua cu restul regulii fnParam ...
            if(arrayDecl())  // se ajunge la arrayDecl doar daca s-a trecut de ID
            {
            }
            return true; // arrayDecl este optional, deci "return true;" nu depinde de existenta sa
        }
    }
// în caz ca oricare dintre componentele secventei nu se indeplineste, atunci se ajunge la ”return false;”
// se reface pozitia initiala a iteratorului, in caz ca unele if-uri exterioare au consumat atomi
    iTk=start;
    return false;
}

// unit: ( structDef | fnDef | varDef )* END
int structDef()
{
    return 1;
}
int fnDef()
{
    return 1;
}
int varDef()
{
    return 1;
}

bool unit()
{
    for(;;)  // bucla infinita
    {
        if(structDef()) {}
        else if(fnDef()) {}
        else if(varDef()) {}
        else break; // daca nu se poate consuma nimic la iteratia curenta, se iese din bucla
    }
    if(consume(END))  // se ajunge la END chiar si daca nu se consuma nimic în repetitie, deci ea este optionala
    {
        return true;
    }
}
int expr(){
    return 1;
}
void ceva(){
// ( COMMA expr )*
while(consume(COMMA))
{
    if(expr())
    {
    }
}
}
// exprOrPrim: OR exprAnd exprOrPrim | epsilon
// echivalent cu: exprOrPrim: ( OR exprAnd exprOrPrim )?
int exprAnd(){
    return 1;
}
bool exprOrPrim()
{
    if(consume(OR))  // prima alternativa: OR exprAnd exprOrPrim
    {
        if(exprAnd())
        {
            if(exprOrPrim())
            {
                return true;
            }
        }
    }
    return true; // epsilon - exprOrPrim returneaza true chiar daca nu consuma nimic
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
void altceva(){
if(arrayDecl()) {}
if(consume(SEMICOLON)) {} // se ajunge la SEMICOLON indiferent daca s-a consumat sau nu arrayDecl

if(consume(IF))
{
    if(consume(LPAR))
    {
       // ...
    }
    else tkerr(iTk,"lipseste ( dupa if");
}
}
int main()
{
    return 0;
}
