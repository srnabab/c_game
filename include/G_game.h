#include "SDL3/SDL_stdinc.h"

#ifndef G_GAME_H
#define G_GAME_H 1

#include "SDL3/SDL_begin_code.h"

// Function to poll SDL events and process keyboard input
extern bool SDLCALL process_input(void);
// Function to initialize our SDL window
//bool initialize_window(void);
// Setup function that runs once at the beginning of our program
extern void SDLCALL setup(int argc, char* argv[]);
// Update function with a fixed time step
extern int SDLCALL update(void *);
// Render function to draw game objects in the SDL window
extern int SDLCALL render(void *);
// Function to destroy SDL window and renderer
// extern int SDLCALL signal_trans (void *);

extern void SDLCALL pauseCode(void);

// int flow_control(void *);

extern void SDL_NORETURN destroy(void);

#include "SDL3/SDL_close_code.h"

#endif