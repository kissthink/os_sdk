/* Copyright (C) 1995 DJ Delorie, see COPYING.DJ for details */
#include <stdio.h>
#include <libc/file.h>
#include <libc/stdiohk.h>

FILE __dj_stdin = {
  0, 0, 0, 0,
  _IOREAD | _IONBF,
  0
};

FILE *stdin=&__dj_stdin;
  
FILE __dj_stdin2 = {
  0, 0, 0, 0,
  _IOREAD | _IOLBF,
  0
};


