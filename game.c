#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <raylib.h>

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
  bool setup;
};

int count_cell_neighbours(struct Game *game, int x, int y) {
  static const int deltas[8][2] = { {1, 0}, {0, 1}, {-1, 0}, {0, -1}, {1, 1}, {-1, -1}, {1, -1}, {-1, 1} };
 
  int neighbours = 0;
  for (int i = 0; i < 8; i++) {
    const int *delta = deltas[i];
    const int neighbourY = y + delta[0];
    const int neighbourX = x + delta[1];
    if (neighbourY >= game->height
      || neighbourX >= game->width
      || neighbourY < 0 
      || neighbourX < 0) continue;

    const int neighbour = game->cells[y + delta[0]][x + delta[1]];
    if (neighbour == ALIVE) {
      neighbours++;
    }    
  }

  return neighbours;
}

void determine_life(struct Game *game, int x, int y) {
  const int cell = game->cells[y][x];
  const int neighbours = count_cell_neighbours(game, x, y);
  
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
  BeginDrawing();
  ClearBackground(RAYWHITE);
  
  for (int x = 0; x < game->width; x++) {
    for (int y = 0; y < game->height; y++) {
      DrawRectangle(x * 50 + 1, y * 50 + 1, 48, 48, game->cells[y][x] == ALIVE ? RED : RAYWHITE);
      DrawRectangleLines(x * 50, y * 50, 50, 50, LIGHTGRAY); 
    }
  }

  static char text[64];
  snprintf(text, sizeof(text), "Generation: %d", game->generation);
  DrawText(text, 10, 0, 50, BLACK);
  snprintf(text, sizeof(text), "Population: %d", count_population(game));
  DrawText(text, 10, 50, 50, BLACK);

  EndDrawing();
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

void destroy_game(struct Game *game) {
  for (int y = 0; y < game->height; y++) {
      free(game->cells[y]);
  }
  free(game->cells);

  for (int y = 0; y < game->height; y++) {
      free(game->changes[y]);
  }
  free(game->changes);
}

void get_cell_from_click(Vector2 mousePos, int cell[2]) {
    cell[0] = mousePos.x / 50;
    cell[1] = mousePos.y / 50;
}

void process_initial_setup(struct Game *game) {
  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    static int cellPos[2];
    const Vector2 mousePos = GetMousePosition();    
    get_cell_from_click(mousePos, cellPos);
    game->cells[cellPos[1]][cellPos[0]] = !game->cells[cellPos[1]][cellPos[0]];
  } else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
    game->setup = true;
  }
}

int main() {
  struct Game game = {0};
  init_game(&game, 20, 20);
  
  const int screenWidth = game.width * 50;
  const int screenHeight = game.height * 50;

  InitWindow(screenWidth, screenHeight, "example");
  SetTargetFPS(10);

  while (!WindowShouldClose()) {
    if (!game.setup) {
      process_initial_setup(&game);
    } else {
      next_generation(&game);
    }
    draw(&game);  
  }

  CloseWindow();
  destroy_game(&game);
}
