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
