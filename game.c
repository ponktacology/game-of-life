#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

enum State {
  DEAD = 0,
  ALIVE
};

struct Game {
  int **cells;
  int **changes;
  int width;
  int height;
  int generation;
};

int count_cell_neighbours(struct Game *game, int x, int y) {
  static int deltas[8][2] = { {1, 0}, {0, 1}, {-1, 0}, {0, -1}, {1, 1}, {-1, -1}, {1, -1}, {-1, 1} };
 
  int neighbours = 0;
  for (int i = 0; i < 8; i++) {
    int *delta = deltas[i];
    int neighbourY = y + delta[0];
    int neighbourX = x + delta[1];
    if (neighbourY >= game->height
      || neighbourX >= game->width
      || neighbourY < 0 
      || neighbourX < 0) continue;

    int neighbour = game->cells[y + delta[0]][x + delta[1]];
    if (neighbour == ALIVE) {
      neighbours++;
    }    
  }

  return neighbours;
}

void determine_life(struct Game *game, int x, int y) {
  int cell = game->cells[y][x];
  int neighbours = count_cell_neighbours(game, x, y);
  
  if (cell == ALIVE && (neighbours < 2 || neighbours > 3)) {
    game->changes[y][x] = DEAD;
    return;
  }
  
  if (cell == DEAD && neighbours == 3) {
    game->changes[y][x] = ALIVE;
    return;
  }

  game->changes[y][x] = game->cells[y][x];
}

void next_generation(struct Game *game) {
  game->generation++;
  
  for (int x = 0; x < game->width; x++) {
    for (int y = 0; y < game->height; y++) {
      determine_life(game, x, y);
    }        
  }


  for (int x = 0; x < game->width; x++) {
    for (int y = 0; y < game->height; y++) {
      game->cells[y][x] = game->changes[y][x];
    }        
  }
}


int count_population(struct Game *game) {
  int population = 0;

  for (int x = 0; x < game->width; x++) {
    for (int y = 0; y < game->height; y++) {
      if (game->cells[y][x] == ALIVE) {
        population++;
      }
    }
  }

  return population;
}

void draw(struct Game *game) {
  system("clear");
  
  printf("Generation: %d\n", game->generation);
  printf("Population: %d\n", count_population(game));

  for (int y = 0; y < game->height; y++) {
    for (int x = 0; x < game->width; x++) {
        char representation = game->cells[y][x] == ALIVE ? 'o' : ' ';
        printf("%c ", representation);
    }
    printf("\n");
  }
}

void init_game(struct Game *game, int width, int height) {
  game->width = width;
  game->height = height;
  game->cells = malloc(sizeof(int*) * height);
  game->changes = malloc(sizeof(int*) * height);

  for (int i = 0; i < height; i++) {
    game->cells[i] = malloc(sizeof(int) * width);
    game->changes[i] = malloc(sizeof(int) * width);
  }
}


int main() {
  struct Game game = {0};
  init_game(&game, 10, 10);
  
  game.cells[5][5] = ALIVE; 
  game.cells[6][5] = ALIVE;
  game.cells[4][5] = ALIVE;
  
  while (1) {
    draw(&game);
    next_generation(&game);
    sleep(1);
  }
}
