#ifndef G_GAME_H
#define G_GAME_H 1

#include "SDL3/SDL_begin_code.h"

// Function to poll SDL events and process keyboard input
extern int SDLCALL process_input(void *);
// Function to initialize our SDL window
//bool initialize_window(void);
// Setup function that runs once at the beginning of our program
extern void SDLCALL setup(void);
// Update function with a fixed time step
extern int SDLCALL update(void *);
// Render function to draw game objects in the SDL window
extern int SDLCALL render(void *);
// Function to destroy SDL window and renderer
extern int SDLCALL signal_trans (void *);

// int flow_control(void *);

extern void SDLCALL destroy_window(void);

#include "SDL3/SDL_close_code.h"

#endif