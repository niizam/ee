#ifndef EE_PLATFORM_COMPAT_H
#define EE_PLATFORM_COMPAT_H

/*
 * Minimal portability helpers for building ee on modern UNIX-like hosts
 * and on Windows toolchains (MSYS2 / MinGW / MSVC).
 */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <ctype.h>
#include <locale.h>

#if defined(_WIN32)
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  include <windows.h>
#  include <io.h>
#  include <direct.h>
#  include <process.h>
#  include <stdint.h>
#  include <sys/types.h>
#  include <sys/stat.h>

/* Windows does not provide unistd.h in all toolchains */
#  define EE_HAVE_NO_PWD 1
#  define EE_HAVE_NO_UNISTD 1

#  ifndef PATH_MAX
#    define PATH_MAX MAX_PATH
#  endif

/* Provide access() wrapper for MSVC. */
#  ifdef _MSC_VER
#    define access _access
#    define unlink _unlink
#  endif

#  if !defined(strdup)
#    define strdup _strdup
#  endif

/* Provide portable wrapper for POSIX link() function on Windows */
static inline int ee_link(const char *oldpath, const char *newpath)
{
	if (CreateHardLinkA(newpath, oldpath, NULL)) {
		return 0;
	}
	/* Map Windows error to appropriate errno value */
	switch (GetLastError()) {
		case ERROR_FILE_NOT_FOUND:
		case ERROR_PATH_NOT_FOUND:
			errno = ENOENT;
			break;
		case ERROR_ACCESS_DENIED:
			errno = EACCES;
			break;
		case ERROR_ALREADY_EXISTS:
			errno = EEXIST;
			break;
		default:
			errno = EIO;
			break;
	}
	return -1;
}

#else /* !_WIN32 */
#  include <unistd.h>
#  include <pwd.h>
#  include <sys/wait.h>

/* On POSIX systems, use native link() */
#  define ee_link link

#endif

/* Some systems (notably Windows) lack catgets; guard it here. */
#if defined(_WIN32)
#  ifndef NO_CATGETS
#    define NO_CATGETS 1
#  endif
#endif

/*
 * Provide portable wrappers for uid lookups; the editor only needs the
 * current user's home directory to expand "~" and "~user".
 */
#if defined(EE_HAVE_NO_PWD)
struct passwd_compat {
	char *pw_dir;
};
#endif

#endif /* EE_PLATFORM_COMPAT_H */
