#include "regex.h"

#include <stdio.h>
#include <string.h>

STATE next_state(STATE state, EVENT event)
{
    switch (event) {
        case PUSH:
            return LOCKED;
        case COIN:
            return UNLOCKED;
        default:
            return LOCKED;
    }
}

int main(int argc, char **argv)
{
    STATE s = LOCKED;

    char    *buffer = NULL;
    size_t  bufsize = 0;

    while (getline(&buffer, &bufsize, stdin) != -1) {
        EVENT e;

        if (strcmp(buffer, "COIN\n") == 0)
            e = COIN;
        else if (strcmp(buffer, "PUSH\n") == 0)
            e = PUSH;
        else {
            printf("Unkown event\n");
            continue;
        }

        s = next_state(s, e);
        printf("%s\n", ((s == LOCKED) ? "LOCKED" : "UNLOCKED"));        
    }
    return 0;
}
