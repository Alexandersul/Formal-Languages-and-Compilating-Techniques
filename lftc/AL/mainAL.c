#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

void err(const char*fmt,...);

#define SAFEALLOC(var,Type) if((var=(Type*)malloc(sizeof(Type)))==NULL)err("not enough memory");

///folosit pentru a stoca codul fiecarui AL
enum {ID,
      BREAK, CHAR, DOUBLE, ELSE, FOR, IF, INT, RETURN, STRUCT, VOID, WHILE,
      CT_INT, CT_REAL, CT_CHAR, CT_STRING,
      COMMA, SEMICOLON, LPAR, RPAR, LBRACKET, RBRACKET, LACC, RACC, END,
      ADD, SUB, MUL, DIV, DOT, AND, OR, NOT, ASSIGN, EQUAL, NOTEQ, LESS, LESSEQ, GREATER, GREATEREQ
     };


///folosit pentru afisare
static const char *codeName[] =
{
    "ID",
    "BREAK", "CHAR", "DOUBLE", "ELSE", "FOR", "IF", "INT", "RETURN", "STRUCT", "VOID", "WHILE",
    "CT_INT", "CT_REAL", "CT_CHAR", "CT_STRING",
    "COMMA", "SEMICOLON", "LPAR", "RPAR", "LBRACKET", "RBRACKET", "LACC", "RACC", "END",
    "ADD", "SUB", "MUL", "DIV", "DOT", "AND", "OR", "NOT",
    "ASSIGN", "EQUAL", "NOTEQ", "LESS", "LESSEQ", "GREATER", "GREATEREQ",
    "SPACE", "LINECOMMENT"
};

int line = 1;

typedef struct _Token
{
    int code;   ///cod(nume)
    union
    {
        char* text;     ///folosit pentru ID, CT_STRING
        int i;       ///folosit pentru CT_INT, CT_CHAR
        double r;    ///folosit pentru CT_REAL

    };
    int line;       ///linia curenta din fisierul de intrare
    struct _Token * next;       ///inlantuire la urmatorul AL
} Token;

Token *lastToken = NULL;    ///sfarsitul listei de atomi lexicali
Token *tk;
Token *tokens = NULL;       ///inceputul listei de atomi lexicali
char *pCrtCh;       ///pointerul care citeste caracter cy caracter sirul
char inbuf[30001];  ///bufferul pentru a citi din fisier


///functie de adaugare a unui token in lista
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

///analogicul functiei printf
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

///pentru afisarea erorilor care pot aparea la o anumita pozitie in fisierul de intrare
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

///functie pentru a afisa AL
char *createString(const char *begin, const char *end)
{
    char *p;
    size_t n = (end - begin);
    SAFEALLOC(p, char);
    memcpy(p, begin, n);
    p[n] = '\0';
    return p;
}

///adauga in lista de tokens, urmatorul AL din sirul de intrare si ii returneaza codul
int getNextToken()
{
    int state=0;    ///starea curenta
    int nCh;
    char ch;
    char* pStartCh; ///pointer catre inceput
    Token *tk;
    SAFEALLOC(tk,Token)
    char *intValue;
    char *doubleValue;

    for(;;)
    {
        ch = *pCrtCh;
        switch(state)
        {
        case 0:
            if (isalpha(ch) || ch == '_')       ///ID[a-zA-Z_]
            {
                pStartCh = pCrtCh;  /// memoreaza inceputul ID-ului
                pCrtCh++;    /// consuma caracterul
                state=1;    /// trece la noua stare
            }

            ///CONSTANTE
            else if (isdigit(ch))
            {
                pStartCh = pCrtCh;      /// memoreaza inceputul INT-ului
                pCrtCh++;
                state = 3;
            }
            else if (ch == '\'')
            {
                pStartCh = pCrtCh + 1;  /// memoreaza inceputul CHAR-ului
                pCrtCh++;
                state = 12;
            }
            else if (ch == '\"')
            {
                pStartCh = pCrtCh + 1;   /// memoreaza inceputul STRING-ului
                pCrtCh++;
                state = 15;
            }

            ///DELIMITATORI
            else if (ch == ',')
            {
                pCrtCh++;
                state = 17;
            }
            else if(ch == ';')
            {
                pCrtCh++;
                state = 18;
            }
            else if(ch == '(')
            {
                pCrtCh++;
                state = 19;
            }
            else if(ch == ')')
            {
                pCrtCh++;
                state = 20;
            }
            else if(ch == '[')
            {
                pCrtCh++;
                state = 21;
            }
            else if(ch == ']')
            {
                pCrtCh++;
                state = 22;
            }
            else if(ch == '{')
            {
                pCrtCh++;
                state = 23;
            }
            else if(ch == '}')
            {
                pCrtCh++;
                state = 24;
            }
            else if(ch == '\0')  ///EOF
            {
                pCrtCh++;
                state = 25;
            }

            ///OPERATORI
            else if(ch == '+')
            {
                pCrtCh++;
                state = 26;
            }
            else if(ch == '-')
            {
                pCrtCh++;
                state = 27;
            }
            else if(ch == '*')
            {
                pCrtCh++;
                state = 28;
            }
            else if(ch == '.')
            {
                pCrtCh++;
                state = 29;
            }
            else if(ch == '&')
            {
                pCrtCh++;
                state = 30;
            }
            else if(ch == '|')
            {
                pCrtCh++;
                state = 32;
            }
            else if(ch == '!')
            {
                pCrtCh++;
                state = 34;
            }
            else if(ch == '=')
            {
                pCrtCh++;
                state = 37;
            }
            else if(ch == '<')
            {
                pCrtCh++;
                state = 40;
            }
            else if(ch == '>')
            {
                pCrtCh++;
                state = 43;
            }
            else if(ch == '>')
            {
                pCrtCh++;
                state = 43;
            }
            else if (ch == ' ' || ch == '\r' || ch == '\t')
            {
                pCrtCh++;
            }
            else if (ch == '\n')     /// tratat separat pentru a actualiza linia curenta
            {
                line++;
                pCrtCh++;
            }
            else if (ch == '/')
            {
                pCrtCh++;
                state = 46;
            }
            else
            {
                tkerr(addTk(END),"caracter invalid 1");
            }
            break;
        case 1:
            if (isalnum(ch) || ch == '_')
            {
                pCrtCh++;
            }
            else
            {
                state = 2;
            }
            break;
        case 2:
            ///pointerul de la pozitia curenta - cel de la inceputul cuvantului
            nCh = pCrtCh - pStartCh; /// lungimea cuvantului gasit
            /// testam toate cuvintele cheie
            if (nCh == 5 && !memcmp(pStartCh, "break", 5))
                tk = addTk(BREAK);
            else if (nCh == 4 && !memcmp(pStartCh, "char", 4))
                tk = addTk(CHAR);
            else if (nCh == 6 && !memcmp(pStartCh, "double", 6))
                tk = addTk(DOUBLE);
            else if (nCh == 4 && !memcmp(pStartCh, "else", 4))
                tk = addTk(ELSE);
            else if (nCh == 3 && !memcmp(pStartCh, "for", 3))
                tk = addTk(FOR);
            else if (nCh == 2 && !memcmp(pStartCh, "if", 2))
                tk = addTk(IF);
            else if (nCh == 3 && !memcmp(pStartCh, "int", 3))
                tk = addTk(INT);
            else if (nCh == 6 && !memcmp(pStartCh, "return", 6))
                tk = addTk(RETURN);
            else if (nCh == 6 && !memcmp(pStartCh, "struct", 6))
                tk = addTk(STRUCT);
            else if (nCh == 4 && !memcmp(pStartCh, "void", 4))
                tk = addTk(VOID);
            else if (nCh == 5 && !memcmp(pStartCh, "while", 5))
                tk = addTk(WHILE);

            /// daca nu este un cuvant cheie, atunci e un ID
            else
            {
                tk = addTk(ID);
                tk->text = createString(pStartCh,pCrtCh);
            }
            return tk->code;
        case 3:
            if (isdigit(ch))
            {
                pCrtCh++;
            }
            else if (ch == '.')
            {
                pCrtCh++;
                state = 5;
            }
            else if (ch == 'e' || ch == 'E')
            {
                pCrtCh++;
                state = 11;
            }
            else
            {
                state = 4;
            }
            break;
        case 4:
            tk = addTk(CT_INT);
            intValue = createString(pStartCh, pCrtCh);
            tk->i = strtol(intValue, NULL, 10);
            return CT_INT;
        case 5:
            if (isdigit(ch))
            {
                pCrtCh++;
                state = 6;
            }
            else
            {
                err("Format invalid");
            }
            break;
        case 6:
            if (isdigit(ch))
            {
                pCrtCh++;
            }
            else if (ch == 'e' || ch == 'E')
            {
                pCrtCh++;
                state = 7;
            }
            else
            {
                tk = addTk(CT_REAL);
                doubleValue = createString(pStartCh, pCrtCh);
                tk->r = atof(doubleValue);
                return CT_REAL;
            }
            break;
        case 7:
            if (ch == '+' || ch == '-')
            {
                pCrtCh++;
                state = 8;
            }
            else if (isdigit(ch))
            {
                pCrtCh++;
                state = 9;
            }
            break;
        case 8:
            if (isdigit(ch))
            {
                pCrtCh++;
                state = 9;
            }
            break;
        case 9:
            if (isdigit(ch))
            {
                pCrtCh++;
            }
            else
            {
                state = 10;
            }
            break;
        case 10:
            tk = addTk(CT_REAL);
            doubleValue = createString(pStartCh, pCrtCh);
            tk->r = atof(doubleValue);
            return CT_REAL;
        case 11:
            if (ch == '+' || ch == '-')
            {
                pCrtCh++;
                state = 8;
            }
            else if (isdigit(ch))
            {
                pCrtCh++;
                state = 9;
            }
            break;
        case 12:
            if (ch != ('\''))
            {
                pCrtCh++;
                state = 13;
            }
            break;
        case 13:
            if (ch == '\'')
            {
                pCrtCh++;
                state = 14;
            }
            break;
        case 14:
            tk = addTk(CT_CHAR);
            tk->text = createString(pStartCh, pCrtCh - 1);
            return CT_CHAR;
        case 15:
            if (ch != '\"')
            {
                pCrtCh++;
            }
            else if (ch == '\"')
            {
                pCrtCh++;
                state = 16;
            }
            break;
        case 16:
            tk = addTk(CT_STRING);
            tk->text = createString(pStartCh, pCrtCh - 1);
            return CT_STRING;
        case 17:
            addTk(COMMA);
            return COMMA;
        case 18:
            addTk(SEMICOLON);
            return SEMICOLON;
        case 19:
            addTk(LPAR);
            return LPAR;
        case 20:
            addTk(RPAR);
            return RPAR;
        case 21:
            addTk(LBRACKET);
            return LBRACKET;
        case 22:
            addTk(RBRACKET);
            return RBRACKET;
        case 23:
            addTk(LACC);
            return LACC;
        case 24:
            addTk(RACC);
            return RACC;
        case 25:
            addTk(END);
            return END;
        case 26:
            addTk(ADD);
            return ADD;
        case 27:
            addTk(SUB);
            return SUB;
        case 28:
            addTk(MUL);
            return MUL;
        case 29:
            addTk(DOT);
            return DOT;
        case 30:
            if(ch == '&')
            {
                pCrtCh++;
                state = 31;
            }
            break;
        case 31:
            addTk(AND);
            return AND;
        case 32:
            if(ch == '|')
            {
                pCrtCh++;
                state = 33;
            }
            break;
        case 33:
            addTk(OR);
            return OR;
        case 34:
            if(ch == '=')
            {
                pCrtCh++;
                state=36;
            }
            else
            {
                state=35;
            }
            break;
        case 35:
            addTk(NOT);
            return NOT;
        case 36:
            addTk(NOTEQ);
            return NOTEQ;
        case 37:
            if(ch == '=')
            {
                pCrtCh++;
                state=39;
            }
            else
            {
                state=38;
            }
            break;
        case 38:
            addTk(ASSIGN);
            return ASSIGN;
        case 39:
            addTk(EQUAL);
            return EQUAL;
        case 40:
            if(ch == '=')
            {
                pCrtCh++;
                state=42;
            }
            else
            {
                state=41;
            }
            break;
        case 41:
            addTk(LESS);
            return LESS;
        case 42:
            addTk(LESSEQ);
            return LESSEQ;
        case 43:
            if(ch == '=')
            {
                pCrtCh++;
                state=45;
            }
            else
            {
                state=44;
            }
            break;
        case 44:
            addTk(GREATER);
            return GREATER;
        case 45:
            addTk(GREATEREQ);
            return GREATEREQ;
        case 46:
            if (ch == '/')
            {
                pCrtCh++;
                state = 47;
            }
            else
            {
                state=48;
            }
            break;
        case 47:
            if (!(ch == '\n' || ch == '\r' || ch == '\0'))
            {
                pCrtCh++;
            }
            else
            {
                state = 0;
            }
            break;
        case 48:
            addTk(DIV);
            return DIV;
        }
    }
}

void showTokens()
{
    Token *atom = tokens;
    while (atom != NULL)
    {
        printf("\n%d\t", atom->line);
        printf("%s", codeName[atom->code]);
        switch(atom->code)
        {
        case ID:
            printf(": %s ", atom->text);
            break;
        case CT_STRING:
            printf(": %s ", atom->text);
            break;
        case CT_INT:
            printf(": %d ", atom->i);
            break;
        case CT_REAL:
            printf(": %g ", atom->r);
            break;
        case CT_CHAR:
            printf(": %s ", atom->text);
            break;
        default:
            break;
        }
        atom = atom->next;
    }
}

int main()
{
    FILE *fis;

    if((fis=fopen("ALEX.c","r"))==NULL)
    {
        printf("Error at opening file");
        return 1;
    }
    int n = fread(inbuf,1,30000,fis);
    inbuf[n] = '\0';

    pCrtCh = inbuf;
    while (getNextToken() != END) {}

    showTokens();

    while(tokens)
    {
        Token *aux = tokens;
        tokens = tokens->next;
        free(aux);
    }

    fclose(fis);
    return 0;
}
