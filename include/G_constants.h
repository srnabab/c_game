#ifndef G_CONSTANTS_H
#define G_CONSTANTS_H 1

#define PARTICLE_COUNT 8192
#define MAX_FRAMES_IN_FLIGHT 2

#define BALLCOUNT 2000

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

#endif