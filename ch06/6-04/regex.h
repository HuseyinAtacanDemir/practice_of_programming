#ifndef REGEX_H
#define REGEX_H

typedef enum { UNLOCKED, LOCKED } STATE;
typedef enum { PUSH, COIN } EVENT;

STATE next_state(STATE state, EVENT event);

#endif
