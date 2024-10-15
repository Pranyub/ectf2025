#include <stdio.h>
#include "passwords.h"
#include <stdint.h>

// Processes user input 
int process_in() {
    return 0; 
}

// Stores password 
int store_password() {
    return 0; 
}

// Generates user menu 
int usr_menu() {
    // User choice
    int usr_choice; 
    uint8_t pass[16]; 
    printf("Select an option ('1' or '2'): \n"); 
    printf("    - 1. Enter a new password\n"); 
    printf("    - 2. Log-In\n");
    printf("Choice: ");

    // Get user data
    scanf("%d", &usr_choice);

    switch (usr_choice) {
        case 1:
            printf("bruh 1\n");
            break;
        case 2:
            printf("bruh 2\n");
            break;
        default:
            printf("bro what did you do\n");
    }

    // for (int i = 0; i < 16; i++) {
    //     pass[i] = getc(stdin); 
    // }

    return 0; 
}

int main() {
    usr_menu(); 
    return 0; 
}
