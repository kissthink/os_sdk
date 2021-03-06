/* kill.c, created from kill.def. */
#line 23 "d:/jicamasdk/progs/bash-2.05/builtins/kill.def"

#line 36 "d:/jicamasdk/progs/bash-2.05/builtins/kill.def"

#include <config.h>

#include <stdio.h>
#include <errno.h>
#if defined (HAVE_UNISTD_H)
#  ifdef _MINIX
#    include <sys/types.h>
#  endif
#  include <unistd.h>
#endif

#include "../bashansi.h"

#include "../shell.h"
#include "../trap.h"
#include "../jobs.h"
#include "common.h"

/* Not all systems declare ERRNO in errno.h... and some systems #define it! */
#if !defined (errno)
extern int errno;
#endif /* !errno */

#if defined (JOB_CONTROL)
extern int posixly_correct;

#if !defined (CONTINUE_AFTER_KILL_ERROR)
#  define CONTINUE_OR_FAIL return (EXECUTION_FAILURE)
#else
#  define CONTINUE_OR_FAIL goto continue_killing
#endif /* CONTINUE_AFTER_KILL_ERROR */

/* Here is the kill builtin.  We only have it so that people can type
   kill -KILL %1?  No, if you fill up the process table this way you
   can still kill some. */
int
kill_builtin (list)
     WORD_LIST *list;
{
  int signal, any_succeeded, listing, saw_signal;
  char *sigspec, *word;
  pid_t pid;
  long pid_value;

  if (list == 0)
    {
      builtin_usage ();
      return (EXECUTION_FAILURE);
    }

  any_succeeded = listing = saw_signal = 0;
  signal = SIGTERM;
  sigspec = "TERM";

  /* Process options. */
  while (list)
    {
      word = list->word->word;

      if (ISOPTION (word, 'l'))
	{
	  listing++;
	  list = list->next;
	}
      else if (ISOPTION (word, 's') || ISOPTION (word, 'n'))
	{
	  list = list->next;
	  if (list)
	    {
	      sigspec = list->word->word;
	      if (sigspec[0] == '0' && sigspec[1] == '\0')
		signal = 0;
	      else
		signal = decode_signal (sigspec);
	      list = list->next;
	    }
	  else
	    {
	      builtin_error ("%s requires an argument", word);
	      return (EXECUTION_FAILURE);
	    }
	}
      else if (ISOPTION (word, '-'))
	{
	  list = list->next;
	  break;
	}
      else if (ISOPTION (word, '?'))
	{
	  builtin_usage ();
	  return (EXECUTION_SUCCESS);
	}
      /* If this is a signal specification then process it.  We only process
	 the first one seen; other arguments may signify process groups (e.g,
	 -num == process group num). */
      else if ((*word == '-') && !saw_signal)
	{
	  sigspec = word + 1;
	  signal = decode_signal (sigspec);
	  saw_signal++;
	  list = list->next;
	}
      else
	break;
    }

  if (listing)
    return (display_signal_list (list, 0));

  /* OK, we are killing processes. */
  if (signal == NO_SIG)
    {
      builtin_error ("bad signal spec `%s'", sigspec);
      return (EXECUTION_FAILURE);
    }

  if (list == 0)
    {
      builtin_usage ();
      return (EXECUTION_FAILURE);
    }

  while (list)
    {
      word = list->word->word;

      if (*word == '-')
	word++;

      if (*word && legal_number (word, &pid_value) && (pid_value == (pid_t)pid_value))
	{
	  /* Use the entire argument in case of minus sign presence. */
	  pid = (pid_t) pid_value;

	  if (kill_pid (pid, signal, 0) < 0)
	    goto signal_error;
	  else
	    any_succeeded++;
	}
      else if (*list->word->word && *list->word->word != '%')
	{
	  builtin_error ("%s: no such pid", list->word->word);
	  CONTINUE_OR_FAIL;
	}
      else if (*word && (interactive || job_control))
	/* Posix.2 says you can kill without job control active (4.32.4) */
	{			/* Must be a job spec.  Check it out. */
	  int job;
	  sigset_t set, oset;

	  BLOCK_CHILD (set, oset);
	  job = get_job_spec (list);

	  if (job < 0 || job >= job_slots || !jobs[job])
	    {
	      if (job != DUP_JOB)
		builtin_error ("%s: no such job", list->word->word);
	      UNBLOCK_CHILD (oset);
	      CONTINUE_OR_FAIL;
	    }

	  /* Job spec used.  Kill the process group. If the job was started
	     without job control, then its pgrp == shell_pgrp, so we have
	     to be careful.  We take the pid of the first job in the pipeline
	     in that case. */
	  pid = IS_JOBCONTROL (job) ? jobs[job]->pgrp : jobs[job]->pipe->pid;

	  UNBLOCK_CHILD (oset);

	  if (kill_pid (pid, signal, 1) < 0)
	    {
	    signal_error:
	      if (errno == EINVAL)
		builtin_error ("Invalid signal %d", signal);
	      else
		builtin_error ("(%ld) - %s", (long)pid, strerror (errno));
	      CONTINUE_OR_FAIL;
	    }
	  else
	    any_succeeded++;
	}
      else
	{
	  builtin_error ("`%s': not a pid or valid job spec", list->word->word);
	  CONTINUE_OR_FAIL;
	}
    continue_killing:
      list = list->next;
    }

  return (any_succeeded ? EXECUTION_SUCCESS : EXECUTION_FAILURE);
}
#endif /* JOB_CONTROL */
