#include <stdio.h>
#include <string.h>
#include "passwords.h"

static const char *passwords[] = {
    PASS_1, PASS_2, PASS_3, PASS_4, PASS_5
};

int secure_strncmp(char *a, char *b, int len)
{
    char a_val, b_val;
    int i, res = 0;

    for (i = 0; i < len; i++) {
        a_val = *a;
        b_val = *b;
        res += (a_val - b_val);

        if (!a_val && !b_val) {
            return res;
        }

        if (a_val) {
            a++;
        }

        if (b_val) {
            b++;
        }
    }

    return res;
}

int main() {
    // Read input
    size_t sz;
    char input[16], *str;

    sz = read(0, input, sizeof(input));
    if (sz == -1) {
        return -1;
    }

    input[15] = 0;
    str = input;

    // Parse input
    int keynum;

    if (strncmp(str, "READ", 4)) {
        return -1;
    }

    str += 5;
    keynum = strtol(str, NULL, 10);
    if (!keynum || keynum > 5) {
        return -1;
    }

    str += 2;
    if (secure_strncmp(str, PRIMARY_PASS, sizeof(PRIMARY_PASS))) {
        return -1;
    }

    printf("PASS %s\n", passwords[keynum - 1]);
    return 0;
}
