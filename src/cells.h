#ifndef CELLS_H
#define CELLS_H

#ifndef SDL_h_
#include <SDL2/SDL.h>
#endif

typedef enum { CELL_EMPTY, CELL_SAND, CELL_WALL, CELL_WATER } CellType;

typedef struct {
    CellType Type;
    SDL_Color Color;
} Cell;

typedef struct {
    int width, height;
    Cell* grid;
} World;

//------------------------------

World New_World(int w, int h) {
    World world = {w, h};
    world.grid = malloc((h * w) * sizeof(Cell));

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            world.grid[y * world.width + x] = (Cell){CELL_EMPTY};
        }
    }

    return world;
}

void Clear_World(World* world) {
    int w = world->width, h = world->height;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            world->grid[y * world->width + x] = (Cell){CELL_EMPTY};
        }
    }
}

void Update_World(World* world) {
    int w = world->width;
    int h = world->height;

    // iterate bottom-to-top so moved grains don't move again this frame
    for (int y = h - 1; y >= 0; y--) {
        for (int x = 0; x < w; x++) {
            int idx = y * w + x;

            int belowY = y + 1;
            if (belowY >= h)
                continue;

            int down = belowY * w + x;
            int downLeft = belowY * w + (x - 1);
            int downRight = belowY * w + (x + 1);

            if (world->grid[idx].Type == CELL_SAND) {

                // move straight down if empty
                if (world->grid[down].Type == CELL_EMPTY || world->grid[down].Type == CELL_WATER) {
                    Cell temp = world->grid[down];
                    world->grid[down] = world->grid[idx];
                    world->grid[idx] = temp;
                    continue;
                }

                // Randomize Direction
                int dir = rand() % 2;

                // try down-left
                if (x > 0 && dir == 0) {
                    if (world->grid[downLeft].Type == CELL_EMPTY ||
                        world->grid[downLeft].Type == CELL_WATER) {
                        Cell temp = world->grid[downLeft];
                        world->grid[downLeft] = world->grid[idx];
                        world->grid[idx] = temp;
                        continue;
                    }
                }

                // try down-right
                if (x + 1 < w && dir == 1) {
                    if (world->grid[downRight].Type == CELL_EMPTY ||
                        world->grid->Type == CELL_WATER) {
                        world->grid[downRight] = world->grid[idx];
                        world->grid[idx].Type = CELL_EMPTY;
                        continue;
                    }
                }
            } else if (world->grid[idx].Type == CELL_WATER) {
                if (world->grid[down].Type == CELL_EMPTY) {
                    Cell temp = world->grid[down];
                    world->grid[down] = world->grid[idx];
                    world->grid[idx] = temp;
                    continue;
                }

                // Randomize Direction
                int dir = rand() % 2 ? 1 : -1;

                for (int distance = 1; distance <= 10; distance++) {
                    int sideX = x + (dir * distance);
                    if (sideX >= 0 && sideX < w) {
                        int side = y * w + sideX;
                        if (world->grid[side].Type == CELL_EMPTY) {
                            // Move to the first available empty cell
                            world->grid[side] = world->grid[idx];
                            world->grid[idx].Type = CELL_EMPTY;
                            break; // Stop after moving to the first available spot
                        }
                    } else {
                        break; // Stop if we hit the boundary
                    }
                }
            }
        }
    }
}

#endif