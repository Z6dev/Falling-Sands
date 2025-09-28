#include <SDL2/SDL.h>
#include <emscripten.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "cells.h"

#define M_WIDTH 200
#define M_HEIGHT 150
#define SCALE 4

// I hate spamming return and printf so this function prints error and Exits with exit code 1.
void ThrowError(char* text) {
    printf("%s Error: %s\n", text, SDL_GetError());
    exit(1);
}

//--------------------------------------------------

SDL_Window* window;
SDL_Renderer* renderer;

CellType selected_cell = CELL_SAND;

void main_loop(void);

World Sandworld;

int main(void) {
    srand(time(NULL));

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        ThrowError("SDL Failed To Initialize!");
    }

    Sandworld = New_World(M_WIDTH, M_HEIGHT);

    window = SDL_CreateWindow(
        "Falling Sands", 0, 0, M_WIDTH * SCALE, M_HEIGHT * SCALE, SDL_WINDOW_SHOWN
    );
    if (window == NULL) {
        ThrowError("SDL Failed To Create Window!");
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        ThrowError("SDL Failed To Create Renderer!");
    }

    emscripten_set_main_loop(main_loop, 0, true);

    return 0;
}

//--------------------------------------------
void AddCell(int mx, int my, int size) {
    for (int dy = -size / 2; dy <= size / 2; dy++) {
        for (int dx = -size / 2; dx <= size / 2; dx++) {
            int nx = mx + dx;
            int ny = my + dy;
            if (nx < 0 || nx >= Sandworld.width || ny < 0 || ny >= Sandworld.height)
                continue;

            Sandworld.grid[ny * Sandworld.width + nx].Type = selected_cell;

            int randval = rand() % 30;
            switch (selected_cell) {
            case CELL_SAND:
                Sandworld.grid[ny * Sandworld.width + nx].Color =
                    (SDL_Color){252 - randval, 202 - randval, 0, 255};
                break;

            case CELL_WALL:
                Sandworld.grid[ny * Sandworld.width + nx].Color =
                    (SDL_Color){120 - randval, 120 - randval, 120 - randval, 255};
                break;

            case CELL_WATER:
                Sandworld.grid[ny * Sandworld.width + nx].Color =
                    (SDL_Color){64 - randval, 164 - randval, 233 - randval, 255};
                break;

            case CELL_EMPTY:
                break;
            }
        }
    }
}

void main_loop(void) {
    int mouseX, mouseY;
    Uint32 MouseState = SDL_GetMouseState(&mouseX, &mouseY);

    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            emscripten_cancel_main_loop();
        }

        if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
            case SDLK_r:
                Clear_World(&Sandworld);

            // Switch Materials
            case SDLK_1:
                selected_cell = CELL_SAND;
                break;
            case SDLK_2:
                selected_cell = CELL_WALL;
                break;
            case SDLK_3:
                selected_cell = CELL_WATER;
                break;
            case SDLK_0:
                selected_cell = CELL_EMPTY;
                break;
            }
        }

        bool mouseDown = MouseState & SDL_BUTTON_LMASK;

        // Spawn a 3x3 block when mouse moves and mouse is down
        if (mouseDown) {
            int mx = mouseX / SCALE;
            int my = mouseY / SCALE;

            AddCell(mx, my, 5);
        }
    }

    Update_World(&Sandworld);

    // Draw
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    for (int y = 0; y < Sandworld.height; y++) {
        for (int x = 0; x < Sandworld.width; x++) {
            if (Sandworld.grid[y * Sandworld.width + x].Type == CELL_EMPTY)
                continue;
            SDL_Color col = Sandworld.grid[y * Sandworld.width + x].Color;

            SDL_Rect r = {x * SCALE, y * SCALE, SCALE, SCALE};
            SDL_SetRenderDrawColor(renderer, col.r, col.g, col.b, 255);
            SDL_RenderFillRect(renderer, &r);
        }
    }

    SDL_RenderPresent(renderer);
}