/* /tmp/autobuild/build-Vs61J3/build/target/linux-x86_64/include/jamaica_config.h.  Generated from jamaica_config.h.in by configure.  */
/* include/jamaica_config.h.in.  Generated from configure.in by autoheader.  */


#ifndef __JAMAICA_CONFIG_H__
#define __JAMAICA_CONFIG_H__


/* defined if arm 'asm()' is supported by C compiler */
/* #undef ARM_ASM */

/* define C compiler version */
#define CC_VERSION 4.4.7

/* define if the system has CLOCK_MONOTONIC constant in time.h. */
#define CLOCK_MONOTONIC_IN_TIME_H 1

/* define if the system has CLOCK_REALTIME constant in time.h. */
#define CLOCK_REALTIME_IN_TIME_H 1

/* defined if the C pre-processors supports the warning directive. */
#define CPP_HAS_WARNING 1

/* define if the C pre-processors supports varargs in macros (ellipses). */
#define CPP_VARARGS 1

/* define if the C pre-processors supports C99 varargs in macros (ellipses).
   */
#define CPP_VARARGS_C99 1

/* define if the system has CPU_SETSIZE constant is in sched.h. */
#define CPU_SETSIZE_IN_SCHED_H 1

/* defined if 'asm()' is supported by C compiler */
#define C_ASM 1

/* defined if local sockets are enabled. */
/* #undef ENABLE_LOCAL_SOCKETS */

/* defined if the system has FD_CLOEXEC constant is in fcntl.h. */
#define FD_CLOEXEC_IN_FCNTL_H 1

/* defined if filesystem used 8+3-filenames. */
/* #undef FILESYSTEM_8_3_NAMES */

/* defined if the system has FIONREAD constant is in sys/filio.h. */
/* #undef FIONREAD_IN_SYS_FILIO_H */

/* defined if the system has FIONREAD constant is in sys/ioctl.h. */
#define FIONREAD_IN_SYS_IOCTL_H 1

/* defined if the system has F_GETFL constant in fcntl.h. */
#define F_GETFL_IN_FCNTL_H 1

/* defined if the system has F_OK constant in unistd.h. */
#define F_OK_IN_UNISTD_H 1

/* defined if the system has F_SETFL constant in fcntl.h. */
#define F_SETFL_IN_FCNTL_H 1

/* defined if GNU intrinsics are supported by C compiler */
#define GCC_INTRINSICS 1

/* define GLIBC version */
#define GLIBC_VERSION 2.12

/* define if the system has accept() function. */
#define HAVE_ACCEPT 1

/* define if the system has access() function. */
#define HAVE_ACCESS 1

/* define if the system support &setjmp. */
#define HAVE_ADDRESS_OF_SETJMP 1

/* define if the system support &_setjmp. */
#define HAVE_ADDRESS_OF__SETJMP 1

/* Define to 1 if you have the <asm/ioctls.h> header file. */
#define HAVE_ASM_IOCTLS_H 1

/* Define to 1 if you have the <asm/termios.h> header file. */
#define HAVE_ASM_TERMIOS_H 1

/* unix64.S does not compile without PC relative relocs. */
#define HAVE_AS_X86_PCREL 1

/* define if the system has bind() function. */
#define HAVE_BIND 1

/* defined if bool is a built-in type */
/* #undef HAVE_BOOL */

/* define if the system has bsd style longjmp() functions. */
#define HAVE_BSD_LONGJMP 1

/* define if the system has bsd style setjmp() functions. */
#define HAVE_BSD_SETJMP 1

/* Define to 1 if you have the <bsp_api.h> header file. */
/* #undef HAVE_BSP_API_H */

/* define if the system has calloc() function. */
#define HAVE_CALLOC 1

/* define if the system has chmod() function. */
#define HAVE_CHMOD 1

/* defined if system has clock_getres() function */
#define HAVE_CLOCK_GETRES 1

/* defined if system has clock_gettime() function */
#define HAVE_CLOCK_GETTIME 1

/* define if the system has close() function. */
#define HAVE_CLOSE 1

/* define if the system has connect() function. */
#define HAVE_CONNECT 1

/* defined if bool is a C++-built-in type */
#define HAVE_CPP_BOOL 1

/* Define to 1 if you have the <cpuset.h> header file. */
/* #undef HAVE_CPUSET_H */

/* define if the system has cpu_set_t type in sched.h. */
#define HAVE_CPU_SET_T_DEFINED 1

/* defined if 'inline' is supported by C compiler */
#define HAVE_C_INLINE 1

/* defined if '__clear_cache()' is supported by C compiler */
#define HAVE_C___CLEAR_CACHE 1

/* defined if '__thread' is supported by C compiler */
#define HAVE_C___THREAD 1

/* Define to 1 if you have the <dirent.h> header file, and it defines `DIR'.
   */
#define HAVE_DIRENT_H 1

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* define if the system has dup2() function. */
#define HAVE_DUP2 1

/* defined if the system has variable environ in unistd.h */
#define HAVE_ENVIRON 1

/* Define to 1 if you have the <errno.h> header file. */
#define HAVE_ERRNO_H 1

/* define if the system has execve() function. */
#define HAVE_EXECVE 1

/* define if the system has fcntl() function. */
#define HAVE_FCNTL 1

/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* define if the system has fdatasync() function. */
#define HAVE_FDATASYNC 1

/* define if the system has finite() function. */
#define HAVE_FINITE 1

/* define if the system has fork() function. */
#define HAVE_FORK 1

/* define if the system has fstat() function. */
#define HAVE_FSTAT 1

/* define if the system has fstat64() function. */
#define HAVE_FSTAT64 1

/* define if the system has ftruncate() function. */
#define HAVE_FTRUNCATE 1

/* define if the system has ftruncate64() function. */
#define HAVE_FTRUNCATE64 1

/* defined if system have dlclose() function */
#define HAVE_FUNCTION_DLCLOSE 1

/* defined if system have dlerror() function */
#define HAVE_FUNCTION_DLERROR 1

/* defined if system have dlopen() function */
#define HAVE_FUNCTION_DLOPEN 1

/* defined if system have dlsym() function */
#define HAVE_FUNCTION_DLSYM 1

/* defined if system have getpid() function */
#define HAVE_FUNCTION_GETPID 1

/* defined if system have inb() function */
#define HAVE_FUNCTION_INB 1

/* defined if system have inl() function */
#define HAVE_FUNCTION_INL 1

/* defined if system have inw() function */
#define HAVE_FUNCTION_INW 1

/* defined if system have ioperm() function */
#define HAVE_FUNCTION_IOPERM 1

/* defined if system have outb() function */
#define HAVE_FUNCTION_OUTB 1

/* defined if system have outl() function */
#define HAVE_FUNCTION_OUTL 1

/* defined if system have outw() function */
#define HAVE_FUNCTION_OUTW 1

/* defined if system have utime() function */
#define HAVE_FUNCTION_UTIME 1

/* defined if system have waitpid() function */
#define HAVE_FUNCTION_WAITPID 1

/* define if the system has wcstombs() function. */
#define HAVE_FUNCTION_WCSTOMBS 1

/* defined if '__attribute__((noinline))' for functions is supported by C
   compiler */
#define HAVE_FUNCTION__ATTRIBUTE__NOINLINE 1

/* define if the system has F_RDLCK constant is in fcntl.h. */
#define HAVE_F_RDLCK 1

/* define if the system has F_SETLK constant is in fcntl.h. */
#define HAVE_F_SETLK 1

/* define if the system has F_SETLKW constant is in fcntl.h. */
#define HAVE_F_SETLKW 1

/* define if the system has F_UNLCK constant is in fcntl.h. */
#define HAVE_F_UNLCK 1

/* define if the system has F_WRLCK constant is in fcntl.h. */
#define HAVE_F_WRLCK 1

/* define if the system has getenv() function. */
#define HAVE_GETENV 1

/* define if the system has getenv_s() function. */
/* #undef HAVE_GETENV_S */

/* define if the system has gethostbyaddr() function. */
#define HAVE_GETHOSTBYADDR 1

/* define if the system has gethostbyname() function. */
#define HAVE_GETHOSTBYNAME 1

/* define if the system has gethostbyname_r() function. */
#define HAVE_GETHOSTBYNAME_R 1

/* define if the system has gethostname() function. */
#define HAVE_GETHOSTNAME 1

/* defined if the system has getifaddrs() function. */
#define HAVE_GETIFADDRS 1

/* defined if the system has getlogin() function. */
#define HAVE_GETLOGIN 1

/* define if the system has getpagesize() function. */
#define HAVE_GETPAGESIZE 1

/* define if the system has getsockname() function. */
#define HAVE_GETSOCKNAME 1

/* define if the system has getsockopt() function. */
#define HAVE_GETSOCKOPT 1

/* defined if the system has gettimeofday() function. */
#define HAVE_GETTIMEOFDAY 1

/* define if the system has gnu_get_libc_version() function. */
#define HAVE_GNU_GET_LIBC_VERSION 1

/* Used by OpenJDK */
#define HAVE_INT16_T_DEFINED 1

/* Used by OpenJDK */
#define HAVE_INT32_T_DEFINED 1

/* Used by OpenJDK */
#define HAVE_INT64_T_DEFINED 1

/* Used by OpenJDK */
#define HAVE_INT8_T_DEFINED 1

/* Used by OpenJDK */
#define HAVE_INTPTR_T_DEFINED 1

/* Define to 1 if you have the <intrin.h> header file. */
/* #undef HAVE_INTRIN_H */

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the <in.h> header file. */
/* #undef HAVE_IN_H */

/* define if the system has ioctl() function. */
#define HAVE_IOCTL 1

/* define if the system has finite() function. */
#define HAVE_ISFINITE 1

/* define if the system has isinf() function. */
#define HAVE_ISINF 1

/* define if the system has isnan() function. */
#define HAVE_ISNAN 1

/* defined if libffi is supported. */
#define HAVE_LIBFFI 1

/* define if the system has listen() function. */
#define HAVE_LISTEN 1

/* define if the system has localtime() function. */
#define HAVE_LOCALTIME 1

/* define if the system has localtime_r() function. */
#define HAVE_LOCALTIME_R 1

/* defined if long long constant have suffix LL */
#define HAVE_LONGLONG_SUFFIX_LL 1

/* defined if long long constant have suffix i64 */
/* #undef HAVE_LONGLONG_SUFFIX_i64 */

/* defined if long long data type available */
#define HAVE_LONG_LONG 1

/* define if the system has lseek() function. */
#define HAVE_LSEEK 1

/* define if the system has lseek64() function. */
#define HAVE_LSEEK64 1

/* define if the system has malloc() function. */
#define HAVE_MALLOC 1

/* Define to 1 if you have the <malloc.h> header file. */
#define HAVE_MALLOC_H 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* define if the system has memset() function. */
#define HAVE_MEMSET 1

/* defined if the system has mkdir() function. */
#define HAVE_MKDIR 1

/* define if the system has mktime() function. */
#define HAVE_MKTIME 1

/* define if the system has mmap() function. */
#define HAVE_MMAP 1

/* define if the system has mprotect() function. */
#define HAVE_MPROTECT 1

/* define if the system has MSG_NOSIGNAL constant is in socket.h. */
#define HAVE_MSG_NOSIGNAL 1

/* define if the system has msync() function. */
#define HAVE_MSYNC 1

/* define if the system has munmap() function. */
#define HAVE_MUNMAP 1

/* defined if the system has nanosleep() function. */
#define HAVE_NANOSLEEP 1

/* Define to 1 if you have the <ndir.h> header file, and it defines `DIR'. */
/* #undef HAVE_NDIR_H */

/* Define to 1 if you have the <netdb.h> header file. */
#define HAVE_NETDB_H 1

/* Define to 1 if you have the <netinet/in.h> header file. */
#define HAVE_NETINET_IN_H 1

/* Define to 1 if you have the <net/ifaddrs.h> header file. */
/* #undef HAVE_NET_IFADDRS_H */

/* defined if the system has off_t type in sys/types.h */
#define HAVE_OFF_T_DEFINED 1

/* define if the system has open() function. */
#define HAVE_OPEN 1

/* define if the system has PF_UNIX constant is in socket.h. */
#define HAVE_PF_UNIX 1

/* define if the system has pipe() function. */
#define HAVE_PIPE 1

/* Define to 1 if you have the <process.h> header file. */
/* #undef HAVE_PROCESS_H */

/* defined if pthread library available. */
/* #undef HAVE_PTHREAD */

/* define if the system has pthread_attr_destroy() function. */
#define HAVE_PTHREAD_ATTR_DESTROY 1

/* define if the system has pthread_attr_init() function. */
#define HAVE_PTHREAD_ATTR_INIT 1

/* define if the system has pthread_attr_setname() function. */
/* #undef HAVE_PTHREAD_ATTR_SETNAME */

/* define if the system has pthread_attr_setschedparam() function. */
#define HAVE_PTHREAD_ATTR_SETSCHEDPARAM 1

/* define if the system has pthread_attr_setstack() function. */
#define HAVE_PTHREAD_ATTR_SETSTACK 1

/* define if the system has pthread_attr_setstackaddr() function. */
#define HAVE_PTHREAD_ATTR_SETSTACKADDR 1

/* define if the system has pthread_attr_setstacksize() function. */
#define HAVE_PTHREAD_ATTR_SETSTACKSIZE 1

/* define if the system has pthread_cancel() function. */
#define HAVE_PTHREAD_CANCEL 1

/* define if the system has pthread_condattr_init() function. */
#define HAVE_PTHREAD_CONDATTR_INIT 1

/* define if the system has pthread_condattr_setclock() function. */
#define HAVE_PTHREAD_CONDATTR_SETCLOCK 1

/* define if the system has pthread_cond_destroy() function. */
#define HAVE_PTHREAD_COND_DESTROY 1

/* define if the system has pthread_cond_init() function. */
#define HAVE_PTHREAD_COND_INIT 1

/* define if the system has pthread_cond_signal() function. */
#define HAVE_PTHREAD_COND_SIGNAL 1

/* define if the system has pthread_cond_timedwait() function. */
#define HAVE_PTHREAD_COND_TIMEDWAIT 1

/* defined if the system has pthread_cond_t type in pthread.h */
#define HAVE_PTHREAD_COND_T_DEFINED 1

/* define if the system has pthread_cond_wait() function. */
#define HAVE_PTHREAD_COND_WAIT 1

/* define if the system has pthread_create() function. */
#define HAVE_PTHREAD_CREATE 1

/* define if the system has pthread_exit() function. */
#define HAVE_PTHREAD_EXIT 1

/* Define to 1 if you have the <pthread.h> header file. */
#define HAVE_PTHREAD_H 1

/* define if the system has pthread_join() function. */
#define HAVE_PTHREAD_JOIN 1

/* define if the system has pthread_kill() function. */
#define HAVE_PTHREAD_KILL 1

/* define if the system has pthread_mutex_destroy() function. */
#define HAVE_PTHREAD_MUTEX_DESTROY 1

/* define if the system has pthread_mutex_init() function. */
#define HAVE_PTHREAD_MUTEX_INIT 1

/* define if the system has pthread_mutex_lock() function. */
#define HAVE_PTHREAD_MUTEX_LOCK 1

/* defined if the system has pthread_mutex_t type in pthread.h */
#define HAVE_PTHREAD_MUTEX_T_DEFINED 1

/* define if the system has pthread_mutex_unlock() function. */
#define HAVE_PTHREAD_MUTEX_UNLOCK 1

/* define if the system has pthread_self() function. */
#define HAVE_PTHREAD_SELF 1

/* define if the system has pthread_setschedparam() function. */
#define HAVE_PTHREAD_SETSCHEDPARAM 1

/* defined if the system has pthread_t type in pthread.h */
#define HAVE_PTHREAD_T_DEFINED 1

/* define if the system has read() function. */
#define HAVE_READ 1

/* defined if realpath() is located in unistd.h - e.g. in libc4 or libc5
   environments */
#define HAVE_REALPATH_IN_UNISTD 1

/* define if the system has recvfrom() function. */
#define HAVE_RECVFROM 1

/* Define to 1 if you have the <sched.h> header file. */
#define HAVE_SCHED_H 1

/* define if the system has sched_setaffinity() function. */
#define HAVE_SCHED_SETAFFINITY 1

/* define if the system has select() function. */
#define HAVE_SELECT 1

/* Define to 1 if you have the <semaphore.h> header file. */
#define HAVE_SEMAPHORE_H 1

/* define if the system has sem_destroy() function. */
#define HAVE_SEM_DESTROY 1

/* define if the system has sem_init() function. */
#define HAVE_SEM_INIT 1

/* define if the system has sem_post() function. */
#define HAVE_SEM_POST 1

/* defined if the system has sem_t type in semaphore.h */
#define HAVE_SEM_T_DEFINED 1

/* define if the system has sem_wait() function. */
#define HAVE_SEM_WAIT 1

/* define if the system has send() function. */
#define HAVE_SEND 1

/* define if the system has sendfile() function. */
#define HAVE_SENDFILE 1

/* define if the system has sendfile64() function. */
#define HAVE_SENDFILE64 1

/* define if the system has sendto() function. */
#define HAVE_SENDTO 1

/* Define to 1 if you have the <setjmp.h> header file. */
#define HAVE_SETJMP_H 1

/* define if the system has setjmp()/longjmp() functions. */
#define HAVE_SETJMP_LONGJMP 1

/* define if the system has setsockopt() function. */
#define HAVE_SETSOCKOPT 1

/* defined if the system has shmat() function. */
#define HAVE_SHMAT 1

/* defined if the system has shmctl() function. */
#define HAVE_SHMCTL 1

/* defined if the system has shmget() function. */
#define HAVE_SHMGET 1

/* define if the system has shutdown() function. */
#define HAVE_SHUTDOWN 1

/* defined if the system has sigaction() function. */
#define HAVE_SIGACTION 1

/* defined if the system has sigfillset() function. */
#define HAVE_SIGFILLSET 1

/* defined if the system has signal() function. */
#define HAVE_SIGNAL 1

/* Define to 1 if you have the <signal.h> header file. */
#define HAVE_SIGNAL_H 1

/* define if the system has SA_RESTART constant is in signal.h. */
#define HAVE_SIGNAL_SA_RESTART 1

/* define if the system has snprintf() function. */
#define HAVE_SNPRINTF 1

/* define if the system has socket() function. */
#define HAVE_SOCKET 1

/* define if the system has socketpair() function. */
#define HAVE_SOCKETPAIR 1

/* check if the system defines socklen_t, */
#define HAVE_SOCKLEN_T 1

/* Define to 1 if you have the <sockLib.h> header file. */
/* #undef HAVE_SOCKLIB_H */

/* define if the system has SOCK_STREAM constant is in socket.h. */
#define HAVE_SOCK_STREAM 1

/* define if the system has SO_REUSEPORT constant is in sys/socket.h. */
/* #undef HAVE_SO_REUSEPORT */

/* define if the system has SO_TIMEOUT constant is in sys/socket.h. */
/* #undef HAVE_SO_TIMEOUT */

/* define if the system has stat() function. */
#define HAVE_STAT 1

/* define if the system has stat64() function. */
#define HAVE_STAT64 1

/* Used by OpenJDK */
#define HAVE_STAT64_DEFINED 1

/* define if the system has statvfs() function. */
#define HAVE_STATVFS 1

/* defined if the system has statvfs type in sys/statvfs.h */
#define HAVE_STATVFS_DEFINED 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* define if the system has strcasecmp() function. */
#define HAVE_STRCASECMP 1

/* define if the system has strdup() function. */
#define HAVE_STRDUP 1

/* define if the system has strerror() function. */
#define HAVE_STRERROR 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* define if the system has _strtod_r() function. */
/* #undef HAVE_STRTOD_R */

/* define if the system has sysconf() function. */
#define HAVE_SYSCONF 1

/* Define to 1 if you have the <sys/cdefs.h> header file. */
#define HAVE_SYS_CDEFS_H 1

/* Define to 1 if you have the <sys/config.h> header file. */
/* #undef HAVE_SYS_CONFIG_H */

/* Define to 1 if you have the <sys/cpuset.h> header file. */
/* #undef HAVE_SYS_CPUSET_H */

/* Define to 1 if you have the <sys/dir.h> header file, and it defines `DIR'.
   */
/* #undef HAVE_SYS_DIR_H */

/* Define to 1 if you have the <sys/filio.h> header file. */
/* #undef HAVE_SYS_FILIO_H */

/* Define to 1 if you have the <sys/ioctl.h> header file. */
#define HAVE_SYS_IOCTL_H 1

/* Define to 1 if you have the <sys/io.h> header file. */
#define HAVE_SYS_IO_H 1

/* Define to 1 if you have the <sys/ipc.h> header file. */
#define HAVE_SYS_IPC_H 1

/* Define to 1 if you have the <sys/mman.h> header file. */
#define HAVE_SYS_MMAN_H 1

/* Define to 1 if you have the <sys/ndir.h> header file, and it defines `DIR'.
   */
/* #undef HAVE_SYS_NDIR_H */

/* Define to 1 if you have the <sys/param.h> header file. */
#define HAVE_SYS_PARAM_H 1

/* Define to 1 if you have the <sys/sched.h> header file. */
/* #undef HAVE_SYS_SCHED_H */

/* Define to 1 if you have the <sys/select.h> header file. */
#define HAVE_SYS_SELECT_H 1

/* Define to 1 if you have the <sys/sendfile.h> header file. */
#define HAVE_SYS_SENDFILE_H 1

/* Define to 1 if you have the <sys/shm.h> header file. */
#define HAVE_SYS_SHM_H 1

/* Define to 1 if you have the <sys/socket.h> header file. */
#define HAVE_SYS_SOCKET_H 1

/* Define to 1 if you have the <sys/statvfs.h> header file. */
#define HAVE_SYS_STATVFS_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/syscall.h> header file. */
#define HAVE_SYS_SYSCALL_H 1

/* Define to 1 if you have the <sys/termios.h> header file. */
#define HAVE_SYS_TERMIOS_H 1

/* Define to 1 if you have the <sys/times.h> header file. */
/* #undef HAVE_SYS_TIMES_H */

/* Define to 1 if you have the <sys/time.h> header file. */
#define HAVE_SYS_TIME_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <sys/un.h> header file. */
#define HAVE_SYS_UN_H 1

/* Define to 1 if you have the <sys/utime.h> header file. */
/* #undef HAVE_SYS_UTIME_H */

/* Define to 1 if you have the <sys/utsname.h> header file. */
#define HAVE_SYS_UTSNAME_H 1

/* Define to 1 if you have the <sys/vfs.h> header file. */
#define HAVE_SYS_VFS_H 1

/* Define to 1 if you have the <sys/wait.h> header file. */
#define HAVE_SYS_WAIT_H 1

/* define if the system has time() function. */
#define HAVE_TIME 1

/* Define to 1 if you have the <time.h> header file. */
#define HAVE_TIME_H 1

/* Define to 1 if you have the <types/vxTypes.h> header file. */
/* #undef HAVE_TYPES_VXTYPES_H */

/* define if the system has tzname */
#define HAVE_TZNAME 1

/* Used by OpenJDK */
/* #undef HAVE_UINT16_DEFINED */

/* Used by OpenJDK */
#define HAVE_UINT16_T_DEFINED 1

/* Used by OpenJDK */
/* #undef HAVE_UINT32_DEFINED */

/* Used by OpenJDK */
#define HAVE_UINT32_T_DEFINED 1

/* Used by OpenJDK */
#define HAVE_UINT64_T_DEFINED 1

/* Used by OpenJDK */
/* #undef HAVE_UINT8_DEFINED */

/* Used by OpenJDK */
#define HAVE_UINT8_T_DEFINED 1

/* Used by OpenJDK */
#define HAVE_UINTPTR_T_DEFINED 1

/* Used by freetype */
/* #undef HAVE_UINT_DEFINED */

/* define if the system has uname() function. */
#define HAVE_UNAME 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* defined if the system has usleep() function. */
#define HAVE_USLEEP 1

/* Define to 1 if you have the <utime.h> header file. */
#define HAVE_UTIME_H 1

/* define if the system has vsnprintf() function. */
#define HAVE_VSNPRINTF 1

/* define if the system has variable vxWorksVersion */
/* #undef HAVE_VXWORKS_VERSION */

/* Define to 1 if you have the <wctype.h> header file. */
#define HAVE_WCTYPE_H 1

/* define if the system has write() function. */
#define HAVE_WRITE 1

/* defined if '__attribute__' is supported by C compiler */
#define HAVE__ATTRIBUTE__ 1

/* defined if '__attribute__((unused))' for variables is supported by C
   compiler */
#define HAVE__ATTRIBUTE__UNUSED 1

/* define if the system has __cpuid() function. */
/* #undef HAVE___CPUID */

/* defined if Jamaica is built as VxWorks ARINC 653 */
/* #undef JAMAICA_AS_VXWORKS_653 */

/* defined if Jamaica is built as VxWorks real time process */
/* #undef JAMAICA_AS_VXWORKS_RTP */

/* string containing the build date */
#define JAMAICA_BUILD_DATE_STRING "Thu Nov 14 16:41:42 UTC 2013"

/* define Jamaica unique build number. */
#define JAMAICA_BUILD_NUMBER "8142"

/* string containing the build number */
#define JAMAICA_BUILD_NUMBER_STRING "8142"

/* CDC Base Profile */
#define JAMAICA_CDC_BASE_PROFILE 2

/* CDC Foundation Profile */
#define JAMAICA_CDC_FOUNDATION_PROFILE 3

/* CDC Personal Basis Profile */
#define JAMAICA_CDC_PERSONAL_BASIS_PROFILE 4

/* defined if separate module to handle contiguous primitive arrays instead of
   tree arrays should be used */
/* #undef JAMAICA_CONTIGUOUS_PRIM_ARRAYS */

/* target cpu, e.g. i386, sparc8, ppc603 */
#define JAMAICA_CPU 100

/* target arm cpu */
#define JAMAICA_CPU_ARM 400

/* target armv4 cpu */
#define JAMAICA_CPU_ARMV4 404

/* target armv5 cpu */
#define JAMAICA_CPU_ARMV5 405

/* target armv6 cpu */
#define JAMAICA_CPU_ARMV6 406

/* target armv7 cpu */
#define JAMAICA_CPU_ARMV7 407

/* target blackfin cpu */
#define JAMAICA_CPU_BLACKFIN 1100

/* target c166 cpu */
#define JAMAICA_CPU_C166 901

/* target c167 cpu */
#define JAMAICA_CPU_C167 902

/* target c16x cpu */
#define JAMAICA_CPU_C16X 900

/* target coldfire cpu */
#define JAMAICA_CPU_COLDFIRE 800

/* target core cpu */
#define JAMAICA_CPU_CORE 107

/* target cris cpu */
#define JAMAICA_CPU_CRIS 1200

/* target cpu family, e.g. X86, SPARC, C16X, PPC, MC680XX */
#define JAMAICA_CPU_FAMILY 1

/* target ARM cpu family */
#define JAMAICA_CPU_FAMILY_ARM 5

/* target Blackfin cpu family */
#define JAMAICA_CPU_FAMILY_BLACKFIN 9

/* target C16X cpu family */
#define JAMAICA_CPU_FAMILY_C16X 3

/* target MC680XX cpu family */
#define JAMAICA_CPU_FAMILY_M68K 7

/* target MIPS cpu family */
#define JAMAICA_CPU_FAMILY_MIPS 10

/* target PPC cpu family */
#define JAMAICA_CPU_FAMILY_PPC 6

/* target SH cpu family */
#define JAMAICA_CPU_FAMILY_SH 8

/* target Sparc cpu family */
#define JAMAICA_CPU_FAMILY_SPARC 2

/* target v850 cpu family */
#define JAMAICA_CPU_FAMILY_V850 4

/* target x86 cpu family */
#define JAMAICA_CPU_FAMILY_X86 1

/* target i386 cpu */
#define JAMAICA_CPU_I386 101

/* target i486 cpu */
#define JAMAICA_CPU_I486 102

/* target m68000 cpu */
#define JAMAICA_CPU_M68000 601

/* target m68010 cpu */
#define JAMAICA_CPU_M68010 602

/* target m68020 cpu */
#define JAMAICA_CPU_M68020 603

/* target m68030 cpu */
#define JAMAICA_CPU_M68030 604

/* target m68040 cpu */
#define JAMAICA_CPU_M68040 605

/* target m68060 cpu */
#define JAMAICA_CPU_M68060 607

/* target m68k cpu */
#define JAMAICA_CPU_M68K 600

/* target m68lc040 cpu */
#define JAMAICA_CPU_M68LC040 606

/* target mips cpu */
#define JAMAICA_CPU_MIPS 700

/* target mips32 cpu */
#define JAMAICA_CPU_MIPS32 701

/* target mips64 cpu */
#define JAMAICA_CPU_MIPS64 702

/* target nios cpu */
#define JAMAICA_CPU_NIOS 1300

/* target pentium cpu */
#define JAMAICA_CPU_PENTIUM 103

/* target pentium2 cpu */
#define JAMAICA_CPU_PENTIUM2 104

/* target pentium3 cpu */
#define JAMAICA_CPU_PENTIUM3 105

/* target pentium4 cpu */
#define JAMAICA_CPU_PENTIUM4 106

/* target ppc32 cpu */
#define JAMAICA_CPU_PPC32 317

/* target ppc403 cpu */
#define JAMAICA_CPU_PPC403 306

/* target ppc405 cpu */
#define JAMAICA_CPU_PPC405 307

/* target ppc405f cpu */
#define JAMAICA_CPU_PPC405F 310

/* target ppc440 cpu */
#define JAMAICA_CPU_PPC440 311

/* target ppc505 cpu */
#define JAMAICA_CPU_PPC505 312

/* target ppc509 cpu */
#define JAMAICA_CPU_PPC509 314

/* target ppc555 cpu */
#define JAMAICA_CPU_PPC555 313

/* target ppc600 cpu */
#define JAMAICA_CPU_PPC600 300

/* target ppc601 cpu */
#define JAMAICA_CPU_PPC601 301

/* target ppc602 cpu */
#define JAMAICA_CPU_PPC602 302

/* target ppc603 cpu */
#define JAMAICA_CPU_PPC603 303

/* target ppc604 cpu */
#define JAMAICA_CPU_PPC604 305

/* target ppc8540 cpu */
#define JAMAICA_CPU_PPC8540 315

/* target ppc860 cpu */
#define JAMAICA_CPU_PPC860 316

/* target ppcec603 cpu */
#define JAMAICA_CPU_PPCEC603 304

/* target sh cpu */
#define JAMAICA_CPU_SH 500

/* target sh2a cpu */
#define JAMAICA_CPU_SH2A 501

/* target sh3 cpu */
#define JAMAICA_CPU_SH3 502

/* target sh4 cpu */
#define JAMAICA_CPU_SH4 503

/* target sparc cpu */
#define JAMAICA_CPU_SPARC 200

/* target sparclite cpu */
#define JAMAICA_CPU_SPARCLITE 201

/* target sparcv8 cpu */
#define JAMAICA_CPU_SPARCV8 201

/* target sparcv9 cpu */
#define JAMAICA_CPU_SPARCV9 202

/* target strongarm cpu */
#define JAMAICA_CPU_STRONGARM 413

/* target ultrasparc cpu */
#define JAMAICA_CPU_ULTRASPARC 203

/* target v850 cpu */
#define JAMAICA_CPU_V850 1000

/* target x86 cpu */
#define JAMAICA_CPU_X86 100

/* defined if distribution version is built */
#define JAMAICA_DISTRIBUTION_VERSION 1

/* defined if dynamic library loading is supported */
#define JAMAICA_DYNAMIC_LIBRARY_LOADING 1

/* Jamaica Edition */
#define JAMAICA_EDITION JAMAICA_STANDARD_EDITION

/* target is big endian, e.g. m68, Sparc */
#define JAMAICA_ENDIANESS_BIG_ENDIAN 2

/* target is big endian, cross longs */
#define JAMAICA_ENDIANESS_BIG_ENDIAN_CROSS_LONGS 6

/* target is big endian, cross words */
#define JAMAICA_ENDIANESS_BIG_ENDIAN_CROSS_WORDS 4

/* target is little endian, e.g. x86, VAX, C166/C167 */
#define JAMAICA_ENDIANESS_LITTLE_ENDIAN 1

/* target is little endian, cross longs */
#define JAMAICA_ENDIANESS_LITTLE_ENDIAN_CROSS_LONGS 5

/* target is little endian, cross words */
#define JAMAICA_ENDIANESS_LITTLE_ENDIAN_CROSS_WORDS 3

/* target has unknown endianess */
#define JAMAICA_ENDIANESS_UNKNOWN 0

/* defined target byte ordering */
#define JAMAICA_ENDIAN_TYPE 1

/* defined target byte ordering for double */
#define JAMAICA_ENDIAN_TYPE_DOUBLE 1

/* defined target byte ordering for float */
#define JAMAICA_ENDIAN_TYPE_FLOAT 1

/* defined target byte ordering for long-long (integer 64 bits) */
#define JAMAICA_ENDIAN_TYPE_INT64 1

/* defined if filesystem functions should be supported */
//#define JAMAICA_FILESYSTEM 1

/* defined if graphics systems should be supported */
//#define JAMAICA_GRAPHICS 1

/* defined if DirectFB graphics system should be supported */
/* #undef JAMAICA_GRAPHICS_DIRECTFB */

/* defined if escher graphics system should be supported */
/* #undef JAMAICA_GRAPHICS_ESCHER */

/* defined if maui graphics system should be supported */
/* #undef JAMAICA_GRAPHICS_MAUI */

/* defined if MiniX graphics system should be supported */
/* #undef JAMAICA_GRAPHICS_MINIX */

/* defined if QNX GF graphics system should be supported */
/* #undef JAMAICA_GRAPHICS_QNX_GF */

/* defined if QNX Photon graphics system should be supported */
/* #undef JAMAICA_GRAPHICS_QNX_PHOTON */

/* defined if WinCE graphics system should be supported */
/* #undef JAMAICA_GRAPHICS_WINCE */

/* defined if WindML graphics system should be supported */
/* #undef JAMAICA_GRAPHICS_WINDML */

/* defined if XWindow graphics system should be supported */
//#define JAMAICA_GRAPHICS_XWINDOW 1

/* max. lenght of a classname */
#define JAMAICA_MAX_CLASSNAME_LENGTH 512

/* max. length of a directory+filename */
#define JAMAICA_MAX_FILENAME_LENGTH 512

/* JAMAICA_MAX_PATHNAME_LENGTH is the size of variables where Jamaica stores
   path information. On small devices it makes sense to choose values of 256
   bytes or less for this value. The default value is 4096 bytes on most *NIX
   systems. */
#define JAMAICA_MAX_PATHNAME_LENGTH 4096

/* defined if mmap() should be used to allocate heap */
#define JAMAICA_MMAP_FOR_HEAP 1

/* defined if network functions should be supported */
//#define JAMAICA_NETWORK 1

/* defined if online license checking should be used as for JamaicaVM Personal
   Edition */
/* #undef JAMAICA_ONLINE_LICENSE */

/* target operating system */
#define JAMAICA_OS "linux-gnu"

/* target operating system version */
#define JAMAICA_OS_VERSION ""

/* target operating system major version */
#define JAMAICA_OS_VERSION_MAJOR 

/* target operating system minor version */
#define JAMAICA_OS_VERSION_MINOR 

/* target platform */
#define JAMAICA_PLATFORM "linux-x86_64"

/* defined if 64-bit references should be used */
//#define JAMAICA_REFS64 1

/* string containing the release number */
#define JAMAICA_RELEASE_STRING "5"

/* defined if stable version (version number is even) */
#define JAMAICA_STABLE_VERSION 1

/* 0 for stack growing up on the target system, 1 for stack growing down */
#define JAMAICA_STACK_GROWS_UP 0

/* Standard Edition */
#define JAMAICA_STANDARD_EDITION 1

/* define size of static VM heap in bytes. Not set by default, thus use
   dynamic memory via malloc(). */
/* #undef JAMAICA_STATIC_VM_HEAP_SIZE */

/* the version info string of the Jamaica VM which is used for output of
   jamaica -version */
#define JAMAICA_VERSION_INFO "Jamaica Virtual Machine Version 6.2 Release 5 (build 8142)"

/* string with the version number */
#define JAMAICA_VERSION_STRING "6.2"

/* defined if work-around for TLS bug should be included */
/* #undef JAMAICA_VXWORKS_TLS_BUG_WORKAROUND */

/* defined if C stack check needed */
#define JAMAICA_WITH_CHECK_C_STACK_OVERFLOW 1

/* defined if Java stack check needed */
#define JAMAICA_WITH_CHECK_JAVA_STACK_OVERFLOW 1

/* If enabled, Jamaica can read RPCube's JIP archives just like JAR and ZIP */
/* #undef JAMAICA_WITH_JIP_SUPPORT */

/* Java filename separator character */
#define JAVA_FILE_SEPARATOR_CHAR '/'

/* Java line separator string */
#define JAVA_LINE_SEPARATOR "\n"

/* Java paths separator character */
#define JAVA_PATH_SEPARATOR_CHAR ':'

/* libffi EH frame flags */
#define LIBFFI_EH_FRAME_FLAGS "aw"

/* define if the system has MAP_ANONYMOUS constant is in sys/mman.h. */
#define MAP_ANONYMOUS_IN_SYS_MMAN_H 1

/* define if the system has MAP_FIXED constant is in sys/mman.h. */
#define MAP_FIXED_IN_SYS_MMAN_H 1

/* define if the system has MAP_PRIVATE constant is in sys/mman.h. */
#define MAP_PRIVATE_IN_SYS_MMAN_H 1

/* define if the system has MAP_SHARED constant is in sys/mman.h. */
#define MAP_SHARED_IN_SYS_MMAN_H 1

/* Define to 1 if your C compiler doesn't accept -c and -o together. */
/* #undef NO_MINUS_C_MINUS_O */

/* defined if the system has O_NONBLOCK constant in fcntl.h. */
#define O_NONBLOCK_IN_FCNTL_H 1

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME ""

/* Define to the full name and version of this package. */
#define PACKAGE_STRING ""

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME ""

/* Define to the version of this package. */
#define PACKAGE_VERSION ""

/* needed for some Solaris functions, e.g. readdir_r */
/* #undef POSIX_PTHREAD_SEMANTICS */

/* define if the system has PROT_EXEC constant is in sys/mman.h. */
#define PROT_EXEC_IN_SYS_MMAN_H 1

/* define if the system has PROT_NONE constant is in sys/mman.h. */
#define PROT_NONE_IN_SYS_MMAN_H 1

/* define if the system has PROT_READ constant is in sys/mman.h. */
#define PROT_READ_IN_SYS_MMAN_H 1

/* define if the system has PROT_WRITE constant is in sys/mman.h. */
#define PROT_WRITE_IN_SYS_MMAN_H 1

/* defined if the system has R_OK constant in unistd.h. */
#define R_OK_IN_UNISTD_H 1

/* define if the system has SIGABRT constant is in signal.h. */
#define SIGABRT_IN_SIGNAL_H 1

/* define if the system has SIGALRM constant is in signal.h. */
#define SIGALRM_IN_SIGNAL_H 1

/* define if the system has SIGBUS constant is in signal.h. */
#define SIGBUS_IN_SIGNAL_H 1

/* define if the system has SIGCANCEL constant is in signal.h. */
/* #undef SIGCANCEL_IN_SIGNAL_H */

/* define if the system has SIGCHLD constant is in signal.h. */
#define SIGCHLD_IN_SIGNAL_H 1

/* define if the system has SIGCLD constant is in signal.h. */
#define SIGCLD_IN_SIGNAL_H 1

/* define if the system has SIGCONT constant is in signal.h. */
#define SIGCONT_IN_SIGNAL_H 1

/* define if the system has SIGEMT constant is in signal.h. */
/* #undef SIGEMT_IN_SIGNAL_H */

/* define if the system has SIGFPE constant is in signal.h. */
#define SIGFPE_IN_SIGNAL_H 1

/* define if the system has SIGFREEZE constant is in signal.h. */
/* #undef SIGFREEZE_IN_SIGNAL_H */

/* define if the system has SIGHUP constant is in signal.h. */
#define SIGHUP_IN_SIGNAL_H 1

/* define if the system has SIGILL constant is in signal.h. */
#define SIGILL_IN_SIGNAL_H 1

/* define if the system has SIGINFO constant is in signal.h. */
/* #undef SIGINFO_IN_SIGNAL_H */

/* define if the system has SIGINT constant is in signal.h. */
#define SIGINT_IN_SIGNAL_H 1

/* define if the system has SIGIOT constant is in signal.h. */
#define SIGIOT_IN_SIGNAL_H 1

/* define if the system has SIGIO constant is in signal.h. */
#define SIGIO_IN_SIGNAL_H 1

/* define if the system has SIGKILL constant is in signal.h. */
#define SIGKILL_IN_SIGNAL_H 1

/* define if the system has SIGLOST constant is in signal.h. */
/* #undef SIGLOST_IN_SIGNAL_H */

/* define if the system has SIGLWP constant is in signal.h. */
/* #undef SIGLWP_IN_SIGNAL_H */

/* define if the system has SIGPIPE constant is in signal.h. */
#define SIGPIPE_IN_SIGNAL_H 1

/* define if the system has SIGPOLL constant is in signal.h. */
#define SIGPOLL_IN_SIGNAL_H 1

/* define if the system has SIGPROF constant is in signal.h. */
#define SIGPROF_IN_SIGNAL_H 1

/* define if the system has SIGPWR constant is in signal.h. */
#define SIGPWR_IN_SIGNAL_H 1

/* define if the system has SIGQUIT constant is in signal.h. */
#define SIGQUIT_IN_SIGNAL_H 1

/* define if the system has SIGRTMAX constant is in signal.h. */
#define SIGRTMAX_IN_SIGNAL_H 1

/* define if the system has SIGRTMIN constant is in signal.h. */
#define SIGRTMIN_IN_SIGNAL_H 1

/* define if the system has SIGSEGV constant is in signal.h. */
#define SIGSEGV_IN_SIGNAL_H 1

/* define if the system has SIGSTKFLT constant is in signal.h. */
#define SIGSTKFLT_IN_SIGNAL_H 1

/* define if the system has SIGSTOP constant is in signal.h. */
#define SIGSTOP_IN_SIGNAL_H 1

/* define if the system has SIGSYS constant is in signal.h. */
#define SIGSYS_IN_SIGNAL_H 1

/* define if the system has SIGTERM constant is in signal.h. */
#define SIGTERM_IN_SIGNAL_H 1

/* define if the system has SIGTHAW constant is in signal.h. */
/* #undef SIGTHAW_IN_SIGNAL_H */

/* define if the system has SIGTRAP constant is in signal.h. */
#define SIGTRAP_IN_SIGNAL_H 1

/* define if the system has SIGTSTP constant is in signal.h. */
#define SIGTSTP_IN_SIGNAL_H 1

/* define if the system has SIGTTIN constant is in signal.h. */
#define SIGTTIN_IN_SIGNAL_H 1

/* define if the system has SIGTTOU constant is in signal.h. */
#define SIGTTOU_IN_SIGNAL_H 1

/* define if the system has SIGUNUSED constant is in signal.h. */
#define SIGUNUSED_IN_SIGNAL_H 1

/* define if the system has SIGURG constant is in signal.h. */
#define SIGURG_IN_SIGNAL_H 1

/* define if the system has SIGUSR1 constant is in signal.h. */
#define SIGUSR1_IN_SIGNAL_H 1

/* define if the system has SIGUSR2 constant is in signal.h. */
#define SIGUSR2_IN_SIGNAL_H 1

/* define if the system has SIGVTALRM constant is in signal.h. */
#define SIGVTALRM_IN_SIGNAL_H 1

/* define if the system has SIGWAITING constant is in signal.h. */
/* #undef SIGWAITING_IN_SIGNAL_H */

/* define if the system has SIGWINCH constant is in signal.h. */
#define SIGWINCH_IN_SIGNAL_H 1

/* define if the system has SIGXCPU constant is in signal.h. */
#define SIGXCPU_IN_SIGNAL_H 1

/* define if the system has SIGXFSZ constant is in signal.h. */
#define SIGXFSZ_IN_SIGNAL_H 1

/* The size of `int', as computed by sizeof. */
#define SIZEOF_INT 4

/* The size of `long', as computed by sizeof. */
#define SIZEOF_LONG 8

/* The size of `long long', as computed by sizeof. */
#define SIZEOF_LONG_LONG 8

/* The size of `unsigned int', as computed by sizeof. */
#define SIZEOF_UNSIGNED_INT 4

/* The size of `unsigned long', as computed by sizeof. */
#define SIZEOF_UNSIGNED_LONG 8

/* The size of `unsigned long long', as computed by sizeof. */
#define SIZEOF_UNSIGNED_LONG_LONG 8

/* The size of `void *', as computed by sizeof. */
#define SIZEOF_VOID_P 8

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Define to 1 if you can safely include both <sys/time.h> and <time.h>. */
#define TIME_WITH_SYS_TIME 1

/* defined if the system has TIOCMGET constant is in sys/ioctl.h. */
#define TIOCMGET_IN_SYS_IOCTL_H 1

/* defined if the system has TIOCMGET constant is in sys/termios.h. */
/* #undef TIOCMGET_IN_SYS_TERMIOS_H */

/* Define to 1 if your <sys/time.h> declares `struct tm'. */
/* #undef TM_IN_SYS_TIME */

/* defined if the system has VX_ALTIVEC_TASK constant in taskLib.h. */
/* #undef VX_ALTIVEC_TASK_IN_TASKLIB_H */

/* defined if no filesystem supported */
/* #undef WITHOUT_FILESYSTEM */

/* defined if no network supported */
/* #undef WITHOUT_NETWORK */

/* defined if the system has W_OK constant in unistd.h. */
#define W_OK_IN_UNISTD_H 1

/* defined if the system has X_OK constant in unistd.h. */
#define X_OK_IN_UNISTD_H 1

/* _HAS_C9X is needed for isnan and friends in math.h for vxworks RTP. */
/* #undef _HAS_C9X */

/* needed for some Solaris functions, e.g. readdir_r */
/* #undef _POSIX_PTHREAD_SEMANTICS */

/* needed to extension of XPG4 */
/* #undef _XOPEN_SOURCE */

/* needed for extension of XPG4 */
/* #undef __EXTENSION__ */

/* defined to some value if C-compiler does not support __FUNCTION__. */
/* #undef __FUNCTION__ */

/* int8_t is defined */
/* #undef __int8_t_defined */

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */

/* Define to `__inline__' or `__inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name.  */
#ifndef __cplusplus
/* #undef inline */
#endif

/* signed int 16 bit */
/* #undef int16_t */

/* signed int 32 bit */
/* #undef int32_t */

/* signed int 64 bit */
/* #undef int64_t */

/* unsigned int 8 bit */
/* #undef int8_t */

/* unsigned int 16 bit */
/* #undef u_int16_t */

/* unsigned int 32 bit */
/* #undef u_int32_t */

/* unsigned int 64 bit */
/* #undef u_int64_t */

/* unsigned int 8 bit */
/* #undef u_int8_t */

#endif /* __JAMAICA_CONFIG_H__ */


/* for special debugging disable inlining */
#ifndef JAMAICA_XDEBUG
  #ifdef HAVE_C_INLINE
    #define C_INLINE 1
  #endif
#endif /* JAMAICA_XDEBUG */



#if (! defined __ASSEMBLER__) || (__ASSEMBLER__ != 1)
/* check for os9 with _OS9000 macro */
#ifdef _OS9000
  /* cdefs.h defines __attribute__ on newer os9 versions */
  #if HAVE_SYS_CDEFS_H
    #include <sys/cdefs.h>
  #endif
  /* avoid typedef pid_t in wait.h */
  #ifndef pid_t
    #define pid_t pid_t
  #endif
#endif /* _OS9000 */
#endif /* (! defined __ASSEMBLER__) || (__ASSEMBLER__ != 1) */

#ifndef HAVE__ATTRIBUTE__
  #ifndef __attribute__
    #define __attribute__(n)
  #endif
#endif /* HAVE__ATTRIBUTE__ */

#ifndef MIN
  #define MIN(a,b) (((a)<(b)) ? (a) : (b))
#endif /* ndef MIN */

#ifndef MAX
  #define MAX(a,b) (((a)>(b)) ? (a) : (b))
#endif /* ndef MAX */

#if HAVE_DIRENT_H
  #define READ_DIR_BUFFER_SIZE sizeof(struct dirent)
#else
/* ??? 12? */
  #define READ_DIR_BUFFER_SIZE 12
#endif /* HAVE_DIRENT_H */




/* define missing types */
#if (! defined __ASSEMBLER__) || (__ASSEMBLER__ != 1)
#ifdef JAMAICA_VXWORKS
  /* Fix for some broken vxWorks 5.x/6.x header files: if stdint.h is not included before vxWorks.h
     some types, e.g. int32_t, in stdint.h become redefined in vxWorks.h.
  */
  #if HAVE_STDINT_H
    #include <stdint.h>
    #define _INTPTR_T
  #endif

  /* Fix for some broken vxWorks 5.x/6.x header files: if vxWorks.h is not included before some other
     VxWorks header, e.g. setjmp.h.
  */
  #include "vxWorks.h"

  /* Fix for some broken vxWorks 6.x RTP header files: if strings.h is not included before some other
     VxWorks header, e.g. sys/select.h for FD_ZERO
  */
  #ifdef __RTP__
    #include "strings.h"
  #endif

  /* Fix for some broke vxWorks header files if types/vxTypes.h is not included. This header file may
     contain some required type definitions
  */
  #if HAVE_TYPES_VXTYPES_H
    #include <types/vxTypes.h>
  #endif
#endif /* JAMAICA_VXWORKS */

#ifndef HAVE_INTPTR_T_DEFINED
  #if   SIZEOF_VOID_P == SIZEOF_INT
    typedef int intptr_t;
  #elif SIZEOF_VOID_P == SIZEOF_LONG
    typedef long intptr_t;
  #elif SIZEOF_VOID_P == SIZEOF_LONG_LONG
    typedef long long intptr_t;
  #else
    #error No usable data type for intptr_t! Please check configure script.
  #endif

  #define HAVE_INTPTR_T_DEFINED 1
#endif

#ifndef HAVE_UINTPTR_T_DEFINED
  #if   SIZEOF_VOID_P == SIZEOF_UNSIGNED_INT
    typedef unsigned int uintptr_t;
  #elif SIZEOF_VOID_P == SIZEOF_UNSIGNED_LONG
    typedef unsigned long uintptr_t;
  #elif SIZEOF_VOID_P == SIZEOF_UNSIGNED_LONG_LONG
    typedef unsigned long long uintptr_t;
  #else
    #error No usable data type for uintptr_t! Please check configure script.
  #endif

  #define HAVE_UINTPTR_T_DEFINED 1
#endif
#endif /* (! defined __ASSEMBLER__) || (__ASSEMBLER__ != 1) */



/* sizes */

/* The unit of memory that is accessed on the heap or the Java stack
 * is a 32 bit slot.  Even though the size is fixed and cannot be
 * changed, constants are defined here to avoid use of inline
 * constants.
 */
#define JAMAICA_LOG2_OF_BITS_PER_SLOT 5
#define JAMAICA_BITS_PER_SLOT         (1 << JAMAICA_LOG2_OF_BITS_PER_SLOT)
#define JAMAICA_BYTES_PER_SLOT        (JAMAICA_BITS_PER_SLOT / 8)

/*
 * Heap memory is split into blocks of equal size. The size is given
 * in number of slots. A power of 2 is strongly recommended.  If
 * memory blocks have to be 64-bit aligned, the block size must be an
 * even value.
 *
 * Currently, the GC code is fixed to a block size of 8, so this
 * cannot be modified.
 */
#ifndef JAMAICA_BLOCK_SIZE
  #define JAMAICA_BLOCK_SIZE 8
#endif
#define JAMAICA_BLOCK_SIZE_BYTES (JAMAICA_BLOCK_SIZE*JAMAICA_BYTES_PER_SLOT)

/*
 * If JAMAICA_BLOCK_SIZE is a power of two, this has to be set to the
 * two's logarithm of JAMAICA_BLOCK_SIZE. If JAMAICA_BLOCK_SIZE is
 * not a power of two, this has to be 0.
 */
#if   JAMAICA_BLOCK_SIZE == 2
  #define JAMAICA_LOG2_OF_BLOCK_SIZE 1
#elif JAMAICA_BLOCK_SIZE == 4
  #define JAMAICA_LOG2_OF_BLOCK_SIZE 2
#elif JAMAICA_BLOCK_SIZE == 8
  #define JAMAICA_LOG2_OF_BLOCK_SIZE 3
#elif JAMAICA_BLOCK_SIZE == 16
  #define JAMAICA_LOG2_OF_BLOCK_SIZE 4
#elif JAMAICA_BLOCK_SIZE == 32
  #define JAMAICA_LOG2_OF_BLOCK_SIZE 5
#elif JAMAICA_BLOCK_SIZE == 64
  #define JAMAICA_LOG2_OF_BLOCK_SIZE 6
#elif JAMAICA_BLOCK_SIZE == 128
  #define JAMAICA_LOG2_OF_BLOCK_SIZE 7
#else
  #define JAMAICA_LOG2_OF_BLOCK_SIZE 0
#endif

/*
 * The array width has to be the highest power of two that is less
 * or equal JAMAICA_BLOCK_SIZE. Only JAMAICA_ARRAY_WIDTH words can
 * be used within blocks used for arrays. This is why JAMAICA_BLOCK_SIZE
 * should be a power of two.
 */
#if   JAMAICA_BLOCK_SIZE >= 128
  #define JAMAICA_ARRAY_WIDTH 128
#elif JAMAICA_BLOCK_SIZE >=  64
  #define JAMAICA_ARRAY_WIDTH  64
#elif JAMAICA_BLOCK_SIZE >=  32
  #define JAMAICA_ARRAY_WIDTH  32
#elif JAMAICA_BLOCK_SIZE >=  16
  #define JAMAICA_ARRAY_WIDTH  16
#elif JAMAICA_BLOCK_SIZE >=   8
  #define JAMAICA_ARRAY_WIDTH   8
#elif JAMAICA_BLOCK_SIZE >=   4
  #define JAMAICA_ARRAY_WIDTH   4
#elif JAMAICA_BLOCK_SIZE >=   2
  #define JAMAICA_ARRAY_WIDTH   2
#else
  #error unsupported JAMAICA_BLOCK_SIZE!
#endif

#if   JAMAICA_ARRAY_WIDTH == 2
  #define JAMAICA_LOG2_OF_ARRAY_WIDTH 1
#elif JAMAICA_ARRAY_WIDTH == 4
  #define JAMAICA_LOG2_OF_ARRAY_WIDTH 2
#elif JAMAICA_ARRAY_WIDTH == 8
  #define JAMAICA_LOG2_OF_ARRAY_WIDTH 3
#elif JAMAICA_ARRAY_WIDTH == 16
  #define JAMAICA_LOG2_OF_ARRAY_WIDTH 4
#elif JAMAICA_ARRAY_WIDTH == 32
  #define JAMAICA_LOG2_OF_ARRAY_WIDTH 5
#elif JAMAICA_ARRAY_WIDTH == 64
  #define JAMAICA_LOG2_OF_ARRAY_WIDTH 6
#elif JAMAICA_ARRAY_WIDTH == 128
  #define JAMAICA_LOG2_OF_ARRAY_WIDTH 7
#else
  #define JAMAICA_LOG2_OF_ARRAY_WIDTH 0
#endif



/* includes with standard types */
#if (! defined __ASSEMBLER__) || (__ASSEMBLER__ != 1)
#if HAVE_STDINT_H
  #include <stdint.h>
#endif
#if HAVE_INTTYPES_H
  #include <inttypes.h>
#endif
#if HAVE_SYS_TYPES_H
  #include <sys/types.h>
#endif
#if HAVE_SYS_PARAM_H
  #include <sys/param.h>
#endif
#if HAVE_DIRENT_H
  #include <dirent.h>
#endif
#endif /* (! defined __ASSEMBLER__) || (__ASSEMBLER__ != 1) */



/* special for OpenJDK */
#ifndef HAVE_ERRNO_H
  // required in zutil.h
  #define NO_ERRNO_H
#endif
#ifndef HAVE_GETENV
  // required by OpenJDK
  #define NO_GETENV
#endif



/* special for FreeType */



/* special for libffi */
#define FFI_HIDDEN

