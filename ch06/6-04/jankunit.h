#ifndef JANKUNIT_H
#define JANKUNIT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>

#include "jankunit.internal.h"
#include "jankunit.assert.h"

#include "eprintf.h"

extern JankUnitContext *GLOBAL_CTX;

extern void init_ctx            (void);  

extern void start_test_block    (int, const char *name_fmt, ...);
extern void end_test_block      (int, int);

#endif
