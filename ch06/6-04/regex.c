#include "regex.h"

#include <stdio.h>
#include <string.h>

int next_state(int state, int event)
{
    return FSM[state][event];
}

int main(int argc, char **argv)
{
    int     state, event;
    char    *buffer;
    size_t  bufsize;

    state   = LOCKED;
    event   = PUSH;

    buffer  = NULL;
    bufsize = 0;

    while (getline(&buffer, &bufsize, stdin) != -1) {
        if (strcmp(buffer, "COIN\n") == 0)
            event = COIN;
        else if (strcmp(buffer, "PUSH\n") == 0)
            event = PUSH;
        else {
            printf("Unkown event\n");
            continue;
        }

        state = next_state(state, event);
        printf("%s\n", ((state == LOCKED) ? "LOCKED" : "UNLOCKED"));        
    }
    return 0;
}
