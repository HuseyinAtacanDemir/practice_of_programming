#ifndef FREQ_TEST_HELPER_H 
#define FREQ_TEST_HELPER_H 

extern int  get_opt_idx       (int opt);        
extern int  is_mutex_opts     (char opt1, char opt2);

extern char **create_argv     (int argc, ...);
extern void destroy_argv      (char **argv);

extern char *get_eprintf_str  (const char *fmt, ...);
//char  *concat_str_arr   (char **arr, const char *delim);
//char  *rus_doll_fmt     (int n, ...);

#endif
