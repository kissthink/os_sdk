#ifndef _GNU_SOURCE
#define	_GNU_SOURCE	1
#endif

#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>

#define GLOB_INTERFACE_VERSION 1

#include <stddef.h>
#include <unistd.h>

#ifndef POSIX
#ifdef	_POSIX_VERSION
#define	POSIX
#endif

#if !defined (_AMIGA) && !defined (VMS) && !defined(WIN32)
#include <pwd.h>
#endif

extern int errno;
#define __set_errno(val) errno = (val)

#ifndef	NULL
#define	NULL	0
#endif


#include <dirent.h>
#define NAMLEN(dirent) (dirent)->d_namlen

#define REAL_DIR_ENTRY(dp) 1

#include <stdlib.h>
#include <string.h>
#define	ANSI_STRING

extern char *getenv ();

#include <string.h>

extern char *malloc (), *realloc ();
extern void free ();

extern void qsort ();
extern void abort (), exit ();

#endif	/* Standard headers.  */


inline static char * my_realloc (p, n)
     char *p;
     unsigned int n;
{
 if (p == NULL)
  return (char *) malloc (n);
 return (char *) realloc (p, n);
}
#define	realloc	my_realloc

#undef	alloca
#define	alloca(n)	__builtin_alloca (n)

#ifndef S_ISDIR
#define S_ISDIR(mode) (((mode) & S_IFMT) == S_IFDIR)
#endif

/* Some system header files erroneously define these.
   We want our own definitions from <fnmatch.h> to take precedence.  */
#undef	FNM_PATHNAME
#undef	FNM_NOESCAPE
#undef	FNM_PERIOD
#include <fnmatch.h>

/* Some system header files erroneously define these.
   We want our own definitions from <glob.h> to take precedence.  */
#undef	GLOB_ERR
#undef	GLOB_MARK
#undef	GLOB_NOSORT
#undef	GLOB_DOOFFS
#undef	GLOB_NOCHECK
#undef	GLOB_APPEND
#undef	GLOB_NOESCAPE
#undef	GLOB_PERIOD
#include <glob.h>

static int glob_in_dir __P ((const char *pattern, const char *directory,
			     int flags,
			     int (*errfunc) __P ((const char *, int)),
			     glob_t *pglob));
static int prefix_array __P ((const char *prefix, char **array, size_t n));
static int collated_compare __P ((const __ptr_t, const __ptr_t));


/* Find the end of the sub-pattern in a brace expression.  We define
   this as an inline function if the compiler permits.  */
static
#if __GNUC__ - 0 >= 2
inline
#endif
const char *
next_brace_sub (const char *begin)
{
  unsigned int depth = 0;
  const char *cp = begin;

  while (1)
    {
      if (depth == 0)
	{
	  if (*cp != ',' && *cp != '}' && *cp != '\0')
	    {
	      if (*cp == '{')
		++depth;
	      ++cp;
	      continue;
	    }
	}
      else
	{
	  while (*cp != '\0' && (*cp != '}' || depth > 0))
	    {
	      if (*cp == '}')
		--depth;
	      ++cp;
	    }
	  if (*cp == '\0')
	    /* An incorrectly terminated brace expression.  */
	    return NULL;

	  continue;
	}
      break;
    }

  return cp;
}

/* Do glob searching for PATTERN, placing results in PGLOB.
   The bits defined above may be set in FLAGS.
   If a directory cannot be opened or read and ERRFUNC is not nil,
   it is called with the pathname that caused the error, and the
   `errno' value from the failing call; if it returns non-zero
   `glob' returns GLOB_ABEND; if it returns zero, the error is ignored.
   If memory cannot be allocated for PGLOB, GLOB_NOSPACE is returned.
   Otherwise, `glob' returns zero.  */
int
glob (pattern, flags, errfunc, pglob)
     const char *pattern;
     int flags;
     int (*errfunc) __P ((const char *, int));
     glob_t *pglob;
{
  const char *filename;
  char *dirname;
  size_t dirlen;
  int status;
  int oldcount;

  if (pattern == NULL || pglob == NULL || (flags & ~__GLOB_FLAGS) != 0)
    {
      __set_errno (EINVAL);
      return -1;
    }

  if (flags & GLOB_BRACE)
    {
      const char *begin = strchr (pattern, '{');
      if (begin != NULL)
	{
	  /* Allocate working buffer large enough for our work.  Note that
	    we have at least an opening and closing brace.  */
	  int firstc;
	  char *alt_start;
	  const char *p;
	  const char *next;
	  const char *rest;
	  size_t rest_len;
#ifdef __GNUC__
	  char onealt[strlen (pattern) - 1];
#else
	  char *onealt = (char *) malloc (strlen (pattern) - 1);
	  if (onealt == NULL)
	    {
	      if (!(flags & GLOB_APPEND))
		globfree (pglob);
	      return GLOB_NOSPACE;
	    }
#endif

	  /* We know the prefix for all sub-patterns.  */
	  memcpy (onealt, pattern, begin - pattern);
	  alt_start = &onealt[begin - pattern];

	  /* Find the first sub-pattern and at the same time find the
	     rest after the closing brace.  */
	  next = next_brace_sub (begin + 1);
	  if (next == NULL)
	    {
	      /* It is an illegal expression.  */
#ifndef __GNUC__
	      free (onealt);
#endif
	      return glob (pattern, flags & ~GLOB_BRACE, errfunc, pglob);
	    }

	  /* Now find the end of the whole brace expression.  */
	  rest = next;
	  while (*rest != '}')
	    {
	      rest = next_brace_sub (rest + 1);
	      if (rest == NULL)
		{
		  /* It is an illegal expression.  */
#ifndef __GNUC__
		  free (onealt);
#endif
		  return glob (pattern, flags & ~GLOB_BRACE, errfunc, pglob);
		}
	    }
	  /* Please note that we now can be sure the brace expression
	     is well-formed.  */
	  rest_len = strlen (++rest) + 1;

	  /* We have a brace expression.  BEGIN points to the opening {,
	     NEXT points past the terminator of the first element, and END
	     points past the final }.  We will accumulate result names from
	     recursive runs for each brace alternative in the buffer using
	     GLOB_APPEND.  */

	  if (!(flags & GLOB_APPEND))
	    {
	      /* This call is to set a new vector, so clear out the
		 vector so we can append to it.  */
	      pglob->gl_pathc = 0;
	      pglob->gl_pathv = NULL;
	    }
	  firstc = pglob->gl_pathc;

	  p = begin + 1;
	  while (1)
	    {
	      int result;

	      /* Construct the new glob expression.  */
	      memcpy (alt_start, p, next - p);
	      memcpy (&alt_start[next - p], rest, rest_len);

	      result = glob (onealt,
			     ((flags & ~(GLOB_NOCHECK|GLOB_NOMAGIC))
			      | GLOB_APPEND), errfunc, pglob);

	      /* If we got an error, return it.  */
	      if (result && result != GLOB_NOMATCH)
		{
#ifndef __GNUC__
		  free (onealt);
#endif
		  if (!(flags & GLOB_APPEND))
		    globfree (pglob);
		  return result;
		}

	      if (*next == '}')
		/* We saw the last entry.  */
		break;

	      p = next + 1;
	      next = next_brace_sub (p);
	      assert (next != NULL);
	    }

#ifndef __GNUC__
	  free (onealt);
#endif

	  if (pglob->gl_pathc != firstc)
	    /* We found some entries.  */
	    return 0;
	  else if (!(flags & (GLOB_NOCHECK|GLOB_NOMAGIC)))
	    return GLOB_NOMATCH;
	}
    }

  /* Find the filename.  */
  filename = strrchr (pattern, '/');
  if (filename == NULL)
    {
      filename = pattern;
#ifdef _AMIGA
      dirname = (char *) "";
#else
      dirname = (char *) ".";
#endif
      dirlen = 0;
    }
  else if (filename == pattern)
    {
      /* "/pattern".  */
      dirname = (char *) "/";
      dirlen = 1;
      ++filename;
    }
  else
    {
      dirlen = filename - pattern;
      dirname = (char *) alloca (dirlen + 1);
      memcpy (dirname, pattern, dirlen);
      dirname[dirlen] = '\0';
      ++filename;
    }

  if (filename[0] == '\0' && dirlen > 1)
    /* "pattern/".  Expand "pattern", appending slashes.  */
    {
      int val = glob (dirname, flags | GLOB_MARK, errfunc, pglob);
      if (val == 0)
	pglob->gl_flags = (pglob->gl_flags & ~GLOB_MARK) | (flags & GLOB_MARK);
      return val;
    }

  if (!(flags & GLOB_APPEND))
    {
      pglob->gl_pathc = 0;
      pglob->gl_pathv = NULL;
    }

  oldcount = pglob->gl_pathc;

#ifndef VMS
  if ((flags & GLOB_TILDE) && dirname[0] == '~')
    {
      if (dirname[1] == '\0' || dirname[1] == '/')
	{
	  /* Look up home directory.  */
	  char *home_dir = getenv ("HOME");
#ifdef _AMIGA
	  if (home_dir == NULL || home_dir[0] == '\0')
	    home_dir = "SYS:";
#else
#ifdef WIN32
	  if (home_dir == NULL || home_dir[0] == '\0')
            home_dir = "c:/users/default"; /* poor default */
#else
	  if (home_dir == NULL || home_dir[0] == '\0')
	    {
	      extern char *getlogin __P ((void));
	      extern int getlogin_r __P ((char *, size_t));
	      int success;

#if defined HAVE_GETLOGIN_R || defined _LIBC
	      size_t buflen = sysconf (_SC_LOGIN_NAME_MAX) + 1;
	      char *name;

	      if (buflen == 0)
		/* `sysconf' does not support _SC_LOGIN_NAME_MAX.  Try
		   a moderate value.  */
		buflen = 16;
	      name = alloca (buflen);

	      success = getlogin_r (name, buflen) >= 0;
#else
	      char *name;
	      success = (name = getlogin ()) != NULL;
#endif
	      if (success)
		{
#if defined HAVE_GETPWNAM_R || defined _LIBC
		  size_t pwbuflen = sysconf (_SC_GETPW_R_SIZE_MAX);
		  char *pwtmpbuf;
		  struct passwd pwbuf, *p;

		  pwtmpbuf = alloca (pwbuflen);

		  success = (__getpwnam_r (name, &pwbuf, pwtmpbuf,
					   pwbuflen, &p) >= 0);
#else
		  struct passwd *p = getpwnam (name);
		  success = p != NULL;
#endif
		  if (success)
		    home_dir = p->pw_dir;
		}
	    }
	  if (home_dir == NULL || home_dir[0] == '\0')
	    home_dir = (char *) "~"; /* No luck.  */
#endif /* WIN32 */
#endif
	  /* Now construct the full directory.  */
	  if (dirname[1] == '\0')
	    dirname = home_dir;
	  else
	    {
	      char *newp;
	      size_t home_len = strlen (home_dir);
	      newp = alloca (home_len + dirlen);
	      memcpy (newp, home_dir, home_len);
	      memcpy (&newp[home_len], &dirname[1], dirlen);
	      dirname = newp;
	    }
	}
#if !defined _AMIGA && !defined WIN32
      else
	{
	  char *end_name = strchr (dirname, '/');
	  char *user_name;
	  char *home_dir;

	  if (end_name == NULL)
	    user_name = dirname + 1;
	  else
	    {
	      user_name = alloca (end_name - dirname);
	      memcpy (user_name, dirname + 1, end_name - dirname);
	      user_name[end_name - dirname - 1] = '\0';
	    }

	  /* Look up specific user's home directory.  */
	  {
#if defined HAVE_GETPWNAM_R || defined _LIBC
	    size_t buflen = sysconf (_SC_GETPW_R_SIZE_MAX);
	    char *pwtmpbuf = alloca (buflen);
	    struct passwd pwbuf, *p;
	    if (__getpwnam_r (user_name, &pwbuf, pwtmpbuf, buflen, &p) >= 0)
	      home_dir = p->pw_dir;
	    else
	      home_dir = NULL;
#else
	    struct passwd *p = getpwnam (user_name);
	    if (p != NULL)
	      home_dir = p->pw_dir;
	    else
	      home_dir = NULL;
#endif
	  }
	  /* If we found a home directory use this.  */
	  if (home_dir != NULL)
	    {
	      char *newp;
	      size_t home_len = strlen (home_dir);
	      size_t rest_len = end_name == NULL ? 0 : strlen (end_name);
	      newp = alloca (home_len + rest_len + 1);
	      memcpy (newp, home_dir, home_len);
	      memcpy (&newp[home_len], end_name, rest_len);
	      newp[home_len + rest_len] = '\0';
	      dirname = newp;
	    }
	}
#endif	/* Not Amiga && not Win32.  */
    }
#endif	/* Not VMS.  */

  if (__glob_pattern_p (dirname, !(flags & GLOB_NOESCAPE)))
    {
      /* The directory name contains metacharacters, so we
	 have to glob for the directory, and then glob for
	 the pattern in each directory found.  */
      glob_t dirs;
      register int i;

      status = glob (dirname,
		     ((flags & (GLOB_ERR | GLOB_NOCHECK | GLOB_NOESCAPE)) |
		      GLOB_NOSORT),
		     errfunc, &dirs);
      if (status != 0)
	return status;

      /* We have successfully globbed the preceding directory name.
	 For each name we found, call glob_in_dir on it and FILENAME,
	 appending the results to PGLOB.  */
      for (i = 0; i < dirs.gl_pathc; ++i)
	{
	  int oldcount;

#ifdef	SHELL
	  {
	    /* Make globbing interruptible in the bash shell. */
	    extern int interrupt_state;

	    if (interrupt_state)
	      {
		globfree (&dirs);
		globfree (&files);
		return GLOB_ABEND;
	      }
	  }
#endif /* SHELL.  */

	  oldcount = pglob->gl_pathc;
	  status = glob_in_dir (filename, dirs.gl_pathv[i],
				(flags | GLOB_APPEND) & ~GLOB_NOCHECK,
				errfunc, pglob);
	  if (status == GLOB_NOMATCH)
	    /* No matches in this directory.  Try the next.  */
	    continue;

	  if (status != 0)
	    {
	      globfree (&dirs);
	      globfree (pglob);
	      return status;
	    }

	  /* Stick the directory on the front of each name.  */
	  if (prefix_array (dirs.gl_pathv[i],
			    &pglob->gl_pathv[oldcount],
			    pglob->gl_pathc - oldcount))
	    {
	      globfree (&dirs);
	      globfree (pglob);
	      return GLOB_NOSPACE;
	    }
	}

      flags |= GLOB_MAGCHAR;

      if (pglob->gl_pathc == oldcount)
	/* No matches.  */
	if (flags & GLOB_NOCHECK)
	  {
	    size_t len = strlen (pattern) + 1;
	    char *patcopy = (char *) malloc (len);
	    if (patcopy == NULL)
	      return GLOB_NOSPACE;
	    memcpy (patcopy, pattern, len);

	    pglob->gl_pathv
	      = (char **) realloc (pglob->gl_pathv,
				   (pglob->gl_pathc +
				    ((flags & GLOB_DOOFFS) ?
				     pglob->gl_offs : 0) +
				    1 + 1) *
				   sizeof (char *));
	    if (pglob->gl_pathv == NULL)
	      {
		free (patcopy);
		return GLOB_NOSPACE;
	      }

	    if (flags & GLOB_DOOFFS)
	      while (pglob->gl_pathc < pglob->gl_offs)
		pglob->gl_pathv[pglob->gl_pathc++] = NULL;

	    pglob->gl_pathv[pglob->gl_pathc++] = patcopy;
	    pglob->gl_pathv[pglob->gl_pathc] = NULL;
	    pglob->gl_flags = flags;
	  }
	else
	  return GLOB_NOMATCH;
    }
  else
    {
      status = glob_in_dir (filename, dirname, flags, errfunc, pglob);
      if (status != 0)
	return status;

      if (dirlen > 0)
	{
	  /* Stick the directory on the front of each name.  */
	  if (prefix_array (dirname,
			    &pglob->gl_pathv[oldcount],
			    pglob->gl_pathc - oldcount))
	    {
	      globfree (pglob);
	      return GLOB_NOSPACE;
	    }
	}
    }

  if (flags & GLOB_MARK)
    {
      /* Append slashes to directory names.  */
      int i;
      struct stat st;
      for (i = oldcount; i < pglob->gl_pathc; ++i)
	if (((flags & GLOB_ALTDIRFUNC) ?
	     (*pglob->gl_stat) (pglob->gl_pathv[i], &st) :
	     stat (pglob->gl_pathv[i], &st)) == 0 &&
	    S_ISDIR (st.st_mode))
	  {
 	    size_t len = strlen (pglob->gl_pathv[i]) + 2;
	    char *new = realloc (pglob->gl_pathv[i], len);
 	    if (new == NULL)
	      {
		globfree (pglob);
		return GLOB_NOSPACE;
	      }
	    strcpy (&new[len - 2], "/");
	    pglob->gl_pathv[i] = new;
	  }
    }

  if (!(flags & GLOB_NOSORT))
    /* Sort the vector.  */
    qsort ((__ptr_t) &pglob->gl_pathv[oldcount],
	   pglob->gl_pathc - oldcount,
	   sizeof (char *), collated_compare);

  return 0;
}


/* Free storage allocated in PGLOB by a previous `glob' call.  */
void
globfree (pglob)
     register glob_t *pglob;
{
  if (pglob->gl_pathv != NULL)
    {
      register int i;
      for (i = 0; i < pglob->gl_pathc; ++i)
	if (pglob->gl_pathv[i] != NULL)
	  free ((__ptr_t) pglob->gl_pathv[i]);
      free ((__ptr_t) pglob->gl_pathv);
    }
}


/* Do a collated comparison of A and B.  */
static int
collated_compare (a, b)
     const __ptr_t a;
     const __ptr_t b;
{
  const char *const s1 = *(const char *const * const) a;
  const char *const s2 = *(const char *const * const) b;

  if (s1 == s2)
    return 0;
  if (s1 == NULL)
    return 1;
  if (s2 == NULL)
    return -1;
  return strcoll (s1, s2);
}


/* Prepend DIRNAME to each of N members of ARRAY, replacing ARRAY's
   elements in place.  Return nonzero if out of memory, zero if successful.
   A slash is inserted between DIRNAME and each elt of ARRAY,
   unless DIRNAME is just "/".  Each old element of ARRAY is freed.  */
static int
prefix_array (dirname, array, n)
     const char *dirname;
     char **array;
     size_t n;
{
  register size_t i;
  size_t dirlen = strlen (dirname);

  if (dirlen == 1 && dirname[0] == '/')
    /* DIRNAME is just "/", so normal prepending would get us "//foo".
       We want "/foo" instead, so don't prepend any chars from DIRNAME.  */
    dirlen = 0;

  for (i = 0; i < n; ++i)
    {
      size_t eltlen = strlen (array[i]) + 1;
      char *new = (char *) malloc (dirlen + 1 + eltlen);
      if (new == NULL)
	{
	  while (i > 0)
	    free ((__ptr_t) array[--i]);
	  return 1;
	}

      memcpy (new, dirname, dirlen);
      new[dirlen] = '/';
      memcpy (&new[dirlen + 1], array[i], eltlen);
      free ((__ptr_t) array[i]);
      array[i] = new;
    }

  return 0;
}


/* Return nonzero if PATTERN contains any metacharacters.
   Metacharacters can be quoted with backslashes if QUOTE is nonzero.  */
int
__glob_pattern_p (pattern, quote)
     const char *pattern;
     int quote;
{
  register const char *p;
  int open = 0;

  for (p = pattern; *p != '\0'; ++p)
    switch (*p)
      {
      case '?':
      case '*':
	return 1;

      case '\\':
	if (quote && p[1] != '\0')
	  ++p;
	break;

      case '[':
	open = 1;
	break;

      case ']':
	if (open)
	  return 1;
	break;
      }

  return 0;
}
#ifdef _LIBC
weak_alias (__glob_pattern_p, glob_pattern_p)
#endif


/* Like `glob', but PATTERN is a final pathname component,
   and matches are searched for in DIRECTORY.
   The GLOB_NOSORT bit in FLAGS is ignored.  No sorting is ever done.
   The GLOB_APPEND flag is assumed to be set (always appends).  */
static int
glob_in_dir (pattern, directory, flags, errfunc, pglob)
     const char *pattern;
     const char *directory;
     int flags;
     int (*errfunc) __P ((const char *, int));
     glob_t *pglob;
{
  __ptr_t stream;

  struct globlink
    {
      struct globlink *next;
      char *name;
    };
  struct globlink *names = NULL;
  size_t nfound = 0;

  if (!__glob_pattern_p (pattern, !(flags & GLOB_NOESCAPE)))
    {
      stream = NULL;
      flags |= GLOB_NOCHECK;
    }
  else
    {
      flags |= GLOB_MAGCHAR;

      stream = ((flags & GLOB_ALTDIRFUNC) ?
		(*pglob->gl_opendir) (directory) :
		(__ptr_t) opendir (directory));
      if (stream == NULL)
	{
	  if ((errfunc != NULL && (*errfunc) (directory, errno)) ||
	      (flags & GLOB_ERR))
	    return GLOB_ABEND;
	}
      else
	while (1)
	  {
	    const char *name;
	    size_t len;
	    struct dirent *d = ((flags & GLOB_ALTDIRFUNC) ?
				(*pglob->gl_readdir) (stream) :
				readdir ((DIR *) stream));
	    if (d == NULL)
	      break;
	    if (! REAL_DIR_ENTRY (d))
	      continue;

	    name = d->d_name;

	    if (fnmatch (pattern, name,
			 (!(flags & GLOB_PERIOD) ? FNM_PERIOD : 0) |
			 ((flags & GLOB_NOESCAPE) ? FNM_NOESCAPE : 0)
#ifdef _AMIGA
			 | FNM_CASEFOLD
#endif
			 ) == 0)
	      {
		struct globlink *new
		  = (struct globlink *) alloca (sizeof (struct globlink));
		len = NAMLEN (d);
		new->name
		  = (char *) malloc (len + 1);
		if (new->name == NULL)
		  goto memory_error;
		memcpy ((__ptr_t) new->name, name, len);
		new->name[len] = '\0';
		new->next = names;
		names = new;
		++nfound;
	      }
	  }
    }

  if (nfound == 0 && (flags & GLOB_NOMAGIC) &&
      ! __glob_pattern_p (pattern, !(flags & GLOB_NOESCAPE)))
    flags |= GLOB_NOCHECK;

  if (nfound == 0 && (flags & GLOB_NOCHECK))
    {
      size_t len = strlen (pattern);
      nfound = 1;
      names = (struct globlink *) alloca (sizeof (struct globlink));
      names->next = NULL;
      names->name = (char *) malloc (len + 1);
      if (names->name == NULL)
	goto memory_error;
      memcpy (names->name, pattern, len);
      names->name[len] = '\0';
    }

  pglob->gl_pathv
    = (char **) realloc (pglob->gl_pathv,
			 (pglob->gl_pathc +
			  ((flags & GLOB_DOOFFS) ? pglob->gl_offs : 0) +
			  nfound + 1) *
			 sizeof (char *));
  if (pglob->gl_pathv == NULL)
    goto memory_error;

  if (flags & GLOB_DOOFFS)
    while (pglob->gl_pathc < pglob->gl_offs)
      pglob->gl_pathv[pglob->gl_pathc++] = NULL;

  for (; names != NULL; names = names->next)
    pglob->gl_pathv[pglob->gl_pathc++] = names->name;
  pglob->gl_pathv[pglob->gl_pathc] = NULL;

  pglob->gl_flags = flags;

  if (stream != NULL)
    {
      int save = errno;
      if (flags & GLOB_ALTDIRFUNC)
	(*pglob->gl_closedir) (stream);
      else
	closedir ((DIR *) stream);
      __set_errno (save);
    }
  return nfound == 0 ? GLOB_NOMATCH : 0;

 memory_error:
  {
    int save = errno;
    if (flags & GLOB_ALTDIRFUNC)
      (*pglob->gl_closedir) (stream);
    else
      closedir ((DIR *) stream);
    __set_errno (save);
  }
  while (names != NULL)
    {
      if (names->name != NULL)
	free ((__ptr_t) names->name);
      names = names->next;
    }
  return GLOB_NOSPACE;
}
