#include "md5.h"
#include <stdio.h>
#include <string.h>



int main(int argc, char** argv){
    unsigned char * digest = md5String(argv[1]);
    
    char res[33];
    for(int i = 0; i < 16; ++i){
        sprintf(&res[i*2], "%02x", (int)digest[i]);
    }
    free(digest);
    char target[] = "7815696ecbf1c96e6894b779456d330e";

    printf("%s\n",res);

    printf("%d\n", strncmp(target, res, 32));
    
    return 1;
}