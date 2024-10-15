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

extern void start_test          (const char *name_fmt, ...);
extern void start_test_suite    (const char *name_fmt, ...);
extern void start_test_program  (const char *name_fmt, ...);

extern void end_test            (void);
extern void end_test_suite      (void);
extern void end_test_program    (void);

#endif
