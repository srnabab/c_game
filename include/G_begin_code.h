#ifndef G_BEGIN_CODE_H
#define G_BEGIN_CODE_H 1

#ifndef SEPRATOR
# if defined(_WIN32)
#  define SEPRATOR "\\"
# elif defined(__linux__)
#  define SEPRATOR "/"
# endif
#endif

#ifndef SEPRATOR_C
# if defined(_WIN32)
#  define SEPRATOR_C '\\'
# elif defined(__linux__)
#  define SEPRATOR_C '/'
# endif
#endif

#ifndef G_NODISCARD
#if ( (defined(__GNUC__) && (__GNUC__ >= 4)) || defined(__clang__) )
#define G_NODISCARD __attribute__((warn_unused_result))
#elif defined(_MSC_VER) && (_MSC_VER >= 1700)
#define G_NODISCARD _Check_return_
#else
#define G_NODISCARD
#endif /* C++17 or C23 */
#endif /* G_NODISCARD not defined */

#ifndef G_DECLSPEC
# if defined(__GNUC__) && __GNUC__ >= 4
#  define G_DECLSPEC __attribute__ ((visibility("default")))
# else
#  define G_DECLSPEC
# endif
#endif

#ifndef G_CALL
# if defined(_WIN32) && !defined(__GNUC__)
#  define G_CALL __cdecl
# else
#  define G_CALL
# endif
#endif

#ifndef G_MALLOC
# if defined(__GNUC__) && (__GNUC__ >= 3)
#define G_MALLOC __attribute__((malloc))
# else
#  define G_MALLOC
# endif
#endif

#ifndef G_ALLOC_SIZE
#if (defined(__clang__) && __clang_major__ >= 4) || (defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 3)))
#define G_ALLOC_SIZE(p) __attribute__((alloc_size(p)))
#elif defined(_MSC_VER)
#define G_ALLOC_SIZE(p)
#else
#define G_ALLOC_SIZE(p)
#endif
#endif /* G_ALLOC_SIZE not defined */

#ifndef G_ALLOC_SIZE2
#if (defined(__clang__) && __clang_major__ >= 4) || (defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 3)))
#define G_ALLOC_SIZE2(p1, p2) __attribute__((alloc_size(p1, p2)))
#elif defined(_MSC_VER)
#define G_ALLOC_SIZE2(p1, p2)
#else
#define G_ALLOC_SIZE2(p1, p2)
#endif
#endif /* G_ALLOC_SIZE2 not defined */

#endif