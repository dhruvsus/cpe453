#include <stdio.h>
#define d "\\"
#define c "\n"
#define b "\""
#define a "#include <stdio.h>%s#define d %s%s%s%s%s#define c %s%sn%s%s#define b %s%s%s%s%s#define a %s%s%s%sint main(){printf(a,c,b,d,d,b,c,b,d,b,c,b,d,b,b,c,b,a,b,c,c);}%s"
int main(){printf(a,c,b,d,d,b,c,b,d,b,c,b,d,b,b,c,b,a,b,c,c);}
