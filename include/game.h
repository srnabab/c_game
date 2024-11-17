#ifndef GAME_H
#define GAME_H

// Function to poll SDL events and process keyboard input
int process_input(void *);
// Function to initialize our SDL window
//bool initialize_window(void);
// Setup function that runs once at the beginning of our program
void setup(void);
// Update function with a fixed time step
int update(void *);
// Render function to draw game objects in the SDL window
int render(void *);
// Function to destroy SDL window and renderer
int signal_trans (void *);

int flow_control(void *);

void destroy_window(void);

#endif