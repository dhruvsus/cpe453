#include "prog1.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if(argc<2){
        inputErr();
        return 1;
    }
    int quantum=atoi(argv[1]);
    printf("%d\n",quantum);
    return 0;
}
void inputErr(){
    fprintf(stderr, "erroneous input. input format:\n");
    fprintf(stderr, "prog1 quantum [prg 1 [args] [: prg 2 [args] [: prg3 [args] [: … ]]]]\n");
}