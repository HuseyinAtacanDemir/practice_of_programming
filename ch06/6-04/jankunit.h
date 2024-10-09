#ifndef JANKUNIT_H
#define JANKUNIT_H

#include "jankunit.internal.h"
#include "jankunit.assert.h"

void start_test           (const char *name_fmt, ...);
void start_test_suite     (const char *name_fmt, ...);
void start_test_program   (const char *name_fmt, ...);

void end_test             (void);
void end_test_suite       (void);
void end_test_program     (void);

#endif

