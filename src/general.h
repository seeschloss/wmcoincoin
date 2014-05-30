/*
    wmCoinCoin, the stupidest WindowMaker dock applet
    Copyright (C) 2001  Julien Pommier

    Utilities comming form GNU GLib

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

*/

#ifndef COINCOIN_GENERAL_H
#define COINCOIN_GENERAL_H

#include <stdlib.h>
#include <stdio.h>

#ifndef	NULL
#define	NULL	((void*) 0)
#endif

#ifndef	FALSE
#define	FALSE	(0)
#endif

#ifndef	TRUE
#define	TRUE	(!FALSE)
#endif

#undef	MAX
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))

#undef	MIN
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))

#undef	ABS
#define ABS(a)	   (((a) < 0) ? -(a) : (a))

#undef	CLAMP
#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))


/* Define G_VA_COPY() to do the right thing for copying va_list variables.
 * glibconfig.h may have already defined G_VA_COPY as va_copy or __va_copy.
 */
#if !defined (G_VA_COPY)
#  if defined (__GNUC__) && defined (__PPC__) && (defined (_CALL_SYSV) || defined (_WIN32))
#  define G_VA_COPY(ap1, ap2)	  (*(ap1) = *(ap2))
#  elif defined (G_VA_COPY_AS_ARRAY)
#  define G_VA_COPY(ap1, ap2)	  g_memmove ((ap1), (ap2), sizeof (va_list))
#  else /* va_list is a pointer */
#  define G_VA_COPY(ap1, ap2)	  ((ap1) = (ap2))
#  endif /* va_list is a pointer */
#endif /* !G_VA_COPY */

/* Wrap the gcc __PRETTY_FUNCTION__ and __FUNCTION__ variables with
 * macros, so we can refer to them as strings unconditionally.
 */
#ifdef	__GNUC__
#define	G_GNUC_FUNCTION		__FUNCTION__
#define	G_GNUC_PRETTY_FUNCTION	__PRETTY_FUNCTION__
#else	/* !__GNUC__ */
#define	G_GNUC_FUNCTION		""
#define	G_GNUC_PRETTY_FUNCTION	""
#endif	/* !__GNUC__ */

/* Provide simple macro statement wrappers (adapted from Perl):
 *  G_STMT_START { statements; } G_STMT_END;
 *  can be used as a single statement, as in
 *  if (x) G_STMT_START { ... } G_STMT_END; else ...
 *
 *  For gcc we will wrap the statements within `({' and `})' braces.
 *  For SunOS they will be wrapped within `if (1)' and `else (void) 0',
 *  and otherwise within `do' and `while (0)'.
 */
#if !(defined (G_STMT_START) && defined (G_STMT_END))
#  if defined (__GNUC__) && !defined (__STRICT_ANSI__) && !defined (__cplusplus)
#    define G_STMT_START	(void)(
#    define G_STMT_END		)
#  else
#    if (defined (sun) || defined (__sun__))
#      define G_STMT_START	if (1)
#      define G_STMT_END	else (void)0
#    else
#      define G_STMT_START	do
#      define G_STMT_END	while (0)
#    endif
#  endif
#endif


/* Provide macros for error handling. The "assert" macros will
 *  exit on failure. The "return" macros will exit the current
 *  function. Two different definitions are given for the macros
 *  if G_DISABLE_ASSERT is not defined, in order to support gcc's
 *  __PRETTY_FUNCTION__ capability.
 */

#ifdef G_DISABLE_ASSERT

#define g_assert(expr)
#define g_assert_not_reached()

#else /* !G_DISABLE_ASSERT */

#ifdef __GNUC__

#define g_assert(expr)			G_STMT_START{		\
     if (!(expr))						\
        fprintf(stderr,						\
 	      "file %s: line %d (%s): assertion failed: (%s)",	\
	      __FILE__,						\
	      __LINE__,						\
	      __PRETTY_FUNCTION__,				\
	      #expr);			}G_STMT_END

#define g_assert_not_reached()		G_STMT_START{		\
        fprintf(stderr,						\
	    "file %s: line %d (%s): should not be reached",	\
	    __FILE__,						\
	    __LINE__,						\
	    __PRETTY_FUNCTION__);	}G_STMT_END

#else /* !__GNUC__ */

#define g_assert(expr)			G_STMT_START{		\
     if (!(expr))						\
        fprintf(stderr,						\
              "file %s: line %d: assertion failed: (%s)",	\
	      __FILE__,						\
	      __LINE__,						\
	      #expr);			}G_STMT_END

#define g_assert_not_reached()		G_STMT_START{	\
        fprintf(stderr,						\
	    "file %s: line %d: should not be reached",	\
	    __FILE__,					\
	    __LINE__);		}G_STMT_END

#endif /* __GNUC__ */

#endif /* !G_DISABLE_ASSERT */


#ifdef G_DISABLE_CHECKS

#define g_return_if_fail(expr)
#define g_return_val_if_fail(expr,val)

#else /* !G_DISABLE_CHECKS */

#ifdef __GNUC__

#define g_return_if_fail(expr)		G_STMT_START{			\
     if (!(expr))							\
       {								\
        fprintf(stderr,						\
		"file %s: line %d (%s): assertion `%s' failed.",	\
		__FILE__,						\
		__LINE__,						\
		__PRETTY_FUNCTION__,					\
		#expr);							\
	 return;							\
       };				}G_STMT_END

#define g_return_val_if_fail(expr,val)	G_STMT_START{			\
     if (!(expr))							\
       {								\
        fprintf(stderr,						\
		"file %s: line %d (%s): assertion `%s' failed.",	\
		__FILE__,						\
		__LINE__,						\
		__PRETTY_FUNCTION__,					\
		#expr);							\
	 return val;							\
       };				}G_STMT_END

#else /* !__GNUC__ */

#define g_return_if_fail(expr)		G_STMT_START{		\
     if (!(expr))						\
       {							\
        fprintf(stderr,						\
		"file %s: line %d: assertion `%s' failed.",	\
		__FILE__,					\
		__LINE__,					\
		#expr);						\
	 return;						\
       };				}G_STMT_END

#define g_return_val_if_fail(expr, val)	G_STMT_START{		\
     if (!(expr))						\
       {							\
        fprintf(stderr,						\
		"file %s: line %d: assertion `%s' failed.",	\
		__FILE__,					\
		__LINE__,					\
		#expr);						\
	 return val;						\
       };				}G_STMT_END

#endif /* !__GNUC__ */

#endif /* !G_DISABLE_CHECKS */


#ifdef G_DISABLE_CHECKS

#define g_return_if_reached()
#define g_return_val_if_reached(val)

#else   /* !G_DISABLE_CHECKS */

#ifdef __GNUC__

#define g_return_if_reached()          G_STMT_START{		\
        fprintf(stderr,						\
           "file %s: line %d (%s): should not be reached",	\
           __FILE__,						\
           __LINE__,						\
	   __PRETTY_FUNCTION__);				\
    return;			         }G_STMT_END

#define g_return_val_if_reached(val)      G_STMT_START{		\
        fprintf(stderr,						\
           "file %s: line %d (%s): should not be reached",	\
           __FILE__,						\
           __LINE__,						\
	   __PRETTY_FUNCTION__);				\
    return (val);			    }G_STMT_END

#else  /* !__GNUC__ */

#define g_return_if_reached()          G_STMT_START{		\
        fprintf(stderr,						\
           "file %s: line %d: should not be reached",		\
           __FILE__,						\
           __LINE__);						\
    return;			         }G_STMT_END

#define g_return_val_if_reached(val)      G_STMT_START{		\
        fprintf(stderr,						\
           "file %s: line %d: should not be reached",		\
           __FILE__,						\
           __LINE__);						\
    return (val);			    }G_STMT_END

#endif /* __GNUC__ */

#endif  /* G_DISABLE_CHECKS */



#endif
