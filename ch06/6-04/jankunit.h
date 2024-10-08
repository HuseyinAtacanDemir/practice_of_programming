#ifndef JANKUNIT_H
#define JANKUNIT_H

#include "jankunit.internal.h"

void start_test           (const char *name);
void start_test_suite     (const char *name);
void start_test_program   (const char *name);

void end_test             (void);
void end_test_suite       (void);
void end_test_program     (void);

#endif

