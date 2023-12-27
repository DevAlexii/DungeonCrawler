#include <SDL.h>

SDL_Scancode exit_btn = SDL_SCANCODE_ESCAPE;
SDL_Scancode Map_input_exit(){return exit_btn;}

SDL_Scancode move_right_btn = SDL_SCANCODE_D;
SDL_Scancode Map_input_move_right(){return move_right_btn;}

SDL_Scancode move_left_btn = SDL_SCANCODE_A;
SDL_Scancode Map_input_move_left(){return move_left_btn;}

SDL_Scancode move_up_btn = SDL_SCANCODE_W;
SDL_Scancode Map_input_move_up(){return move_up_btn;}

SDL_Scancode move_down_btn = SDL_SCANCODE_S;
SDL_Scancode Map_input_move_down(){return move_down_btn;}