#include <stdio.h>
#include <string.h>

int main(){
    char string[] = "string";
    char rev[] = "";

    for(int i= strlen(string); i>=0; i--){
        rev[strlen(string)-i] = string[i];
        rev[strlen(string)-i+1] = '\0';
    }

    printf("%s", rev);
}