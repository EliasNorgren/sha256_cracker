#include <stdio.h>
#include <string.h>
#include "sha256.h"


void pad_string(BYTE src[], unsigned char dst[])
{
    int index = 0;
    for(int i = 0; i < SHA256_BLOCK_SIZE; i++){
        char temp[7];
        sprintf(temp, "%02hhx", src[i]);
        dst[index] = temp[0];
        dst[index+1] = temp[1];
        //printf("%d-%d -> %c%c\n", index, index+1, temp[0], temp[1]);
        index += 2;
    }
}

//test
int main(int argc, char **argv){
    SHA256_CTX ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, (unsigned char*)argv[1], strlen(((char*)argv[1])));
    BYTE buf[SHA256_BLOCK_SIZE];
    sha256_final(&ctx, buf);
    unsigned char padded_buf[(SHA256_BLOCK_SIZE * 2) + 1];
    memset(padded_buf, '\0', sizeof(unsigned char) * 65);
    pad_string(buf, padded_buf);
    printf("\n%s\n\n", padded_buf);

}