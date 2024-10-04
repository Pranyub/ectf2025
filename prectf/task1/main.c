#include <stdio.h>
#include <string.h>
#include "passwords.h"
int main() {
    printf("Hello World!\n");
    while (1)
    {    
        char buffer[16];
        char* p;
        if (fgets(buffer, sizeof(buffer), stdin) != NULL)
        {
            if (strlen(buffer) == 15)
            {
                char command[6];
                char piss[6] = "READ \0";
                memcpy(command, &buffer[0], 5); 
                       
                if (strcmp(command, piss) == 0)
                {
                    if (buffer[5] > 0x30 && buffer[5] < 0x36)
                    {
                        char pass[9];
                        char buf[256];
                        strcpy(buf, PRIMARY_PASS);
                        strcat(buf, "");
                        strncpy(pass, &buffer[7], 8);
                        if (strcmp(pass,PRIMARY_PASS)  == 0)
                        {
                            switch (buffer[5])
                            {
                                case 0x31:
                                    printf("PASS %s\n", PASS_1);
                                    break;
                                case 0x32:
                                    printf("PASS %s\n", PASS_2);
                                    break;
                                case 0x33:
                                    printf("PASS %s\n", PASS_3);
                                    break;
                                case 0x34:
                                    printf("PASS %s\n", PASS_4);
                                    break;
                                case 0x35:
                                    printf("PASS %s\n", PASS_5);
                                    break;
                            }
                            break;
                        }
                        else {
                            printf("FAIL\n");
                            continue;
                        }
                    }
                    else {
                        printf("FAIL\n");
                        continue;
                    }
                }
                else {
                printf("FAIL\n");
                continue;
                }    
            }
            else {
            printf("FAIL\n");
            continue;
            }
        }
        else {
            printf("FAIL\n");
            continue;
        }
    }
    return 0;
}