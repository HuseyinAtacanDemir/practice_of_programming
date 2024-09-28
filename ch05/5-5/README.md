Based on the man pages of the real vis, we can add folding functionality with the -f or -F flags [f- column-number].

We can also add, again unoriginally, the option to select which invisble characters to encode and which to print normally. Common choices might be not to encode tabs and newlines.

The real vis, in fact, does not encode tabs and newlines by default. In order to encode tabs, newlines, and spaces, the real vis expects the -w flag.

In our version, we will take the result of the isprint(char) function of ctype.h as our base, and -t -n flags will stop encoding tabs and newlines respectively.

I will make sure that flags that don't expect arguments can be combined: ./vis -f 10 -tn file1 ... will be correct usage.

As per the POSIX standard, the optional arguments to ./vis will come before the filenames.

