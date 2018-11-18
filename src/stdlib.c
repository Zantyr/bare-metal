#define STDLIB

/* in stdLib from other places:
strLen
strRev
strFindCh
cmpStrHead
*/

char* strCopy(char *from){
	char* new = (char*) malloc((int) strLen(from));
	setStr(new, from);
	return new;
}
