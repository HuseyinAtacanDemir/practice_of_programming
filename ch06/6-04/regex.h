/*
Yeah.. I think I am a little bit too early into my proper CS journey to tackle
writing a regex parser right now.. so I will use the 

*/

#ifndef REGEX_H
#define REGEX_H

#define N_STATES 2
#define LOCKED 0
#define UNLOCKED 1

#define N_EVENTS 2
#define PUSH 0
#define COIN 1

const int FSM[N_STATES][N_EVENTS] = {
    [LOCKED]    = { LOCKED, UNLOCKED },
    [UNLOCKED]  = { LOCKED, UNLOCKED }
};

extern int  next_state    (int state, int event);
extern char *state_to_str (int state);

#endif
