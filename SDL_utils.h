
#ifndef SDL_UTILS__H
#define SDL_UTILS__H

#include <iostream>
#include <SDL.h>
#include <string>

const int W_WIDTH = 868;
const int W_HEIGHT = 696;
const int SCREEN_WIDTH = 522;
const int SCREEN_HEIGHT = 696;
const int NUM_BLOCK = 2;
const std::string WINDOW_TITLE = "a";
const int M = 24;
const int N = 18;
//size of block
const int b_w = (SCREEN_WIDTH/N);
const int b_h = (SCREEN_HEIGHT/M);

bool GAME_OVER = false;

void logSDLError(std::ostream& os,
                 const std::string &msg, bool fatal);

void initSDL(SDL_Window* &window, SDL_Renderer* &renderer) ;

void quitSDL(SDL_Window* window, SDL_Renderer* renderer);

void waitUntilKeyPressed();

SDL_Texture* loadTexture(const std::string &file, SDL_Renderer *ren);


void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y);


void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, int w, int h);

#endif
