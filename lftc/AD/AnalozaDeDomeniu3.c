#pragma once

// analiza de domeniu

struct Symbol;typedef struct Symbol Symbol;

typedef enum{		// tipul de baza
	TB_INT,TB_DOUBLE,TB_CHAR,TB_VOID,TB_STRUCT
	}TypeBase;

typedef struct{		// tipul unui simbol
	TypeBase tb;
	Symbol *s;		// pentru TB_STRUCT, structura efectiva
	// n - dimensiunea pentru array
	//		n<0 - nu este un array
	//		n==0 - array fara dimensiune specificata: int v[]
	//		n>0 - array cu dimensiune specificata: double v[10]
	int n;
	}Type;

// returneaza dimensiunea in octeti a tipului t
int typeSize(Type *t);

typedef enum{		// felul unui simbol
	SK_VAR,SK_PARAM,SK_FN,SK_STRUCT
	}SymKind;

struct Symbol{
	const char *name;		// numele simbolului. Simbolul nu detine acest pointer, ci el este alocat in alta parte (ex: in Token)
	SymKind kind;		// felul
	Type type;		// tipul
	// owner:
	//		- NULL pentru simboluri globale
	//		- o structura pentru variabile definite in acea structura
	//		- o functie pentru parametri/variabile locale acelei functii
	Symbol *owner;
	Symbol *next;		// inlantuire la urmatorul simbol din lista
	union{		// date specifice pentru fiecare fel de simbol
		// indexul in fn.locals pentru variabile locale
		// indexul in globals pentru variabile globale
		// indexul in structura pentru membri de structura
		int varIdx;
		// indexul in fn.params pentru parametri
		int paramIdx;
		// membrii unei structuri
		Symbol *structMembers;
		struct{
			Symbol *params;		// parametrii functiei
			Symbol *locals;		// variabilele locale, inclusiv cele din subdomeniile fn
			}fn;
		};
	};

// aloca dinamic un nou simbol
Symbol *newSymbol(const char *name,SymKind kind);
// duplica simbolul dat
Symbol *dupSymbol(Symbol *symbol);
// adauga simbolul la sfarsitul listei
// list - adresa listei unde se face adaugarea
Symbol *addSymbolToList(Symbol **list,Symbol *s);
// numarul de simboluri din lista
int symbolsLen(Symbol *list);
// elibereaza memoria ocupata de un simbol
void freeSymbol(Symbol *s);

typedef struct _Domain{
	struct _Domain *parent;		// domeniul parinte
	Symbol *symbols;		// simbolurile din acest domeniu (lista simplu inlantuita)
	}Domain;

// domeniul curent (varful stivei de domenii)
extern Domain *symTable;

// adauga un domeniu in varful stivei de domenii
Domain *pushDomain();
// sterge domeniul din varful stivei de domenii
void dropDomain();
// afiseaza continutul domeniului curent
void showDomain(Domain *d,const char *name);
// cauta un simbol in domeniul specificat si il returneaza
// daca nu il gaseste, returneaza NULL
Symbol *findSymbolInDomain(Domain *d,const char *name);
// cauta un simbol in toate domeniile, incepand cu cel curent
Symbol *findSymbol(const char *name);
// adauga un simbol la domeniul curent
Symbol *addSymbolToDomain(Domain *d,Symbol *s);

// memoria in care sunt pastrate valorile variabilelor globale
extern char *globalMemory;
extern int nGlobalMemory;	// dimensiunea lui globalMemory, in octeti

// aloca in globalMemory un spatiu de nBytes octeti
// si ii returneaza indexul de inceput
int allocInGlobalMemory(int nBytes);

// TS={[]} - initial in TS se afla un singur domeniu, vid, cel global
int x,y; // {[x,y]} ; s-au adaugat variabilele in domeniul curent (global)
void f( // {[x,y,f]} ; la '(' incepe domeniul local al functiei
int x, // {[x,y,f], [x]} ; x global poate fi redefinit ca argument,
fiindca este in alt domeniu
int n) // {[x,y,f], [x,n]}
{
int n; // eroare: redefinire n (n exista deja in domeniul functiei)
if(x<y){ // {[x,y,f], [x,n], []} ; acoladele incep un nou domeniu
int n=x-1; // {[x,y,f], [x,n], [n]}
} // {[x,y,f], [x,n]} ; la '}' se iese din domeniul lui if
} // {[x,y,f]} ; la '}' se iese din domeniul functiei si raman doar simbolurile
din domeniul global

arrayDecl[inout Type *t]: LBRACKET
( CT_INT[tkSize] {t->n=tkSize->i;} | {t->n=0;} )
RBRACKET

bool arrayDecl(Type *t){
if(consume(LBRACKET)){
if(consume(CT_INT)){
Token *tkSize=consumedTk;
t->n=tkSize->i;
}else{
t->n=0; // array fara dimensiune: int v[]
}
if(consume(RBRACKET)){
return true;
}else tkerr(iTk,"missing ] or invalid expression inside [...]");
}
return false;
}

// afisare atomi lexicali
pushDomain(); // creaza domeniul global
// analiza sintactica
showDomain(symTable,"global");
dropDomain(); // sterge domeniul global

// numele structurii trebuie sa fie unic in domeniu
// in interiorul structurii nu pot exista doua variabile cu acelasi nume
structDef: STRUCT ID[tkName] LACC
{
Symbol *s=findSymbolInDomain(symTable,tkName->text);
if(s)tkerr(iTk,"symbol redefinition: %s",tkName->text);
s=addSymbolToDomain(symTable,newSymbol(tkName->text,SK_STRUCT));
s->type.tb=TB_STRUCT;
s->type.s=s;
s->type.n=-1;
pushDomain();
owner=s;
}
varDef* RACC SEMICOLON
{
owner=NULL;
dropDomain();
}

// numele variabilei trebuie sa fie unic in domeniu
// variabilele de tip vector trebuie sa aiba dimensiunea data (nu se accepta: int v[])
varDef: {Type t;} typeBase[&t] ID[tkName]
( arrayDecl[&t]
{if(t.n==0)tkerr(iTk,"a vector variable must have a specified dimension");}
)? SEMICOLON
{
Symbol *var=findSymbolInDomain(symTable,tkName->text);
if(var)tkerr(iTk,"symbol redefinition: %s",tkName->text);
var=newSymbol(tkName->text,SK_VAR);
var->type=t;
var->owner=owner;
addSymbolToDomain(symTable,var);
if(owner){
switch(owner->kind){
case SK_FN:
var->varIdx=symbolsLen(owner->fn.locals);
addSymbolToList(&owner->fn.locals,dupSymbol(var));
break;
case SK_STRUCT:
var->varIdx=typeSize(&owner->type);
addSymbolToList(&owner->structMembers,dupSymbol(var));
break;
}
}else{
var->varIdx=allocInGlobalMemory(typeSize(&t));
}
}

// daca tipul de baza este o structura, ea trebuie sa fie deja definita anterior
typeBase[out Type *t]: {t->n=-1;}
(
INT {t->tb=TB_INT;}
| DOUBLE {t->tb=TB_DOUBLE;}
| CHAR {t->tb=TB_CHAR;}
| STRUCT ID[tkName]
{
t->tb=TB_STRUCT;
t->s=findSymbol(tkName->text);
if(!t->s)tkerr(iTk,"structura nedefinita: %s",tkName->text);
}
)

arrayDecl[inout Type *t]: LBRACKET
( CT_INT[tkSize] {t->n=tkSize->i;} | {t->n=0;} )
RBRACKET

// numele functiei trebuie sa fie unic in domeniu
// domeniul local functiei incepe imediat dupa LPAR
// corpul functiei {...} nu defineste un nou subdomeniu in domeniul local functiei
fnDef: {Type t;}
( typeBase[&t] | VOID {t.tb=TB_VOID;} ) ID[tkName] LPAR
{
Symbol *fn=findSymbolInDomain(symTable,tkName->text);
if(fn)tkerr(iTk,"symbol redefinition: %s",tkName->text);
fn=newSymbol(tkName->text,SK_FN);
fn->type=t;
addSymbolToDomain(symTable,fn);
owner=fn;
pushDomain();
}
( fnParam ( COMMA fnParam )* )? RPAR stmCompound[false]
{
dropDomain();
owner=NULL;
}

// numele parametrului trebuie sa fie unic in domeniu
// parametrii pot fi vectori cu dimensiune data, dar in acest caz li se sterge
dimensiunea ( int v[10] -> int v[] )
fnParam: {Type t;} typeBase[&t] ID[tkName]
(arrayDecl[&t] {t.n=0;} )?
{
Symbol *param=findSymbolInDomain(symTable,tkName->text);
if(param)tkerr(iTk,"symbol redefinition: %s",tkName->text);
param=newSymbol(tkName->text,SK_PARAM);
param->type=t;
param->paramIdx=symbolsLen(owner->fn.params);
// parametrul este adaugat atat la domeniul curent, cat si la parametrii fn
addSymbolToDomain(symTable,param);
addSymbolToList(&owner->fn.params,dupSymbol(param));
}

// corpul compus {...} al instructiunilor defineste un nou domeniu
stm: stmCompound[true] ...

// se defineste un nou domeniu doar la cerere
stmCompound[in bool newDomain]: LACC
{if(newDomain)pushDomain();}
( varDef | stm )* RACC
{if(newDomain)dropDomain();}
