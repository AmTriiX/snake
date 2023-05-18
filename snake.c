#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <SDL2/SDL.h>
#include <SDL_ttf.h>

// Screen dimensions
#define SCREEN_WIDTH  900
#define SCREEN_HEIGHT 900

// Board dimensions
#define BOARD_WIDTH 30
#define BOARD_HEIGHT 30

// Cell dimensions
#define CELL_WIDTH ((SCREEN_WIDTH / BOARD_WIDTH))
#define CELL_HEIGHT ((SCREEN_HEIGHT / BOARD_HEIGHT))

// Snake movement speed
#define MAX_SNAKE_MOVEMENT 0.15
#define MIN_SNAKE_MOVEMENT 0.06
#define STEP_SNAKE_MOVEMENT 0.03

// Maximum length of the snake
#define MAX_SNAKE_LENGTH ((BOARD_WIDTH) * (BOARD_HEIGHT))

// Food spawn delay and count
#define DELAY_FOOD_SPAWN 3
#define FOODS_COUNT 1

// Number of obstacles
#define OBSTACLES_COUNT 20

// Font path for text rendering
#define FONT_PATH "./fonts/LiberationMono-Regular.ttf"

// Macros for defining obstacle patterns
#define STAR_OBSTACLE(game, off, x, y)                     \
  (game)->obs[off++] = (Obstacle){(Pos) {x    , y}    , 1}; \
  (game)->obs[off++] = (Obstacle){(Pos) {x + 1, y}    , 1}; \
  (game)->obs[off++] = (Obstacle){(Pos) {x - 1, y}    , 1}; \
  (game)->obs[off++] = (Obstacle){(Pos) {x    , y + 1}, 1}; \
  (game)->obs[off++] = (Obstacle){(Pos) {x    , y - 1}, 1};

#define HORIZONTAL_WALL_OBSACLE(game, off, x, y)           \
  (game)->obs[off++] = (Obstacle){(Pos) {x    , y}    , 1}; \
  (game)->obs[off++] = (Obstacle){(Pos) {x + 1, y}    , 1}; \
  (game)->obs[off++] = (Obstacle){(Pos) {x + 1, y}    , 1}; \
  (game)->obs[off++] = (Obstacle){(Pos) {x - 1, y}    , 1}; \
  (game)->obs[off++] = (Obstacle){(Pos) {x -2 , y}    , 1};

// Colors
#define BACKGROUND_COLOR 0x000000FF
#define GRID_COLOR       0xFFFFFFFF
#define SNAKE_COLOR      0xEE72F100
#define FOOD_COLOR       0x77B28C00
#define OBSTACLE_COLOR   0x964B0000
#define SCORE_COLOR      0xFFFFFF00

// Macro for extracting color components from a hexadecimal color value
#define HEX_COLOR(hex)                                          \
  ((hex) >> (3 * 8)) & 0xFF,                                    \
  ((hex) >> (2 * 8)) & 0xFF,                                    \
  ((hex) >> (1 * 8)) & 0xFF,                                    \
  ((hex) >> (0 * 8)) & 0xFF

// Enumeration for snake directions
typedef enum {
  DIR_RIGHT = 0,
  DIR_UP,
  DIR_LEFT,
  DIR_DOWN,
} Dir;

// Structure for representing positions
typedef struct {
  int x;
  int y;
} Pos;

// Structure for representing food
typedef struct {
  Pos pos;
  int score;
} Food;

// Structure for representing obstacles
typedef struct {
  Pos pos;
  int init;
} Obstacle;

// Structure for representing the snake
typedef struct {
  Pos body[MAX_SNAKE_LENGTH];
  int length;
  Dir dir;
} Snake;

// Structure for representing the game
typedef struct {
  Snake snake;
  Food food[FOODS_COUNT];
  Obstacle obs[OBSTACLES_COUNT];
  double game_speed;
  int quit;
  int global_score;
} Game;

// Function to handle SDL error codes
void scc(int code);

// Function to handle SDL pointer errors
void *scp(void *ptr);

// Function to generate a random integer within a range
int random_int_range(int low, int high);

// Function to generate a random position on the game board
Pos random_board_pos(void);

// Function to check if a position on the board is not occupied
int pos_is_not_empty(Game *game, Pos p);

// Function to generate a random empty position on the game board
Pos random_empty_board_pos(Game *game);

// Function to initialize the game state
void init_game(Game *game);

// Function to get the position of the snake's head
Pos *get_snake_head(Snake *snake);

// Function to check if snake movement is allowed
int allow_snake_movement(int manual, Game *game);

// Function to calculate the next position of the snake's head
Pos peak_next_pos(Snake *snake, Dir new_dir);

// Function to move the snake
void move_snake(Game *game, Dir new_dir, int manual);

// Function to handle the snake eating food
void eat_food(Game *game, Food *f);

// Function to initialize the food objects
void init_food(Game *game);

// Function to check if refreshing food is allowed
int allow_refresh_food(void);

// Function to check if there is food at the snake's head position
Food *check_for_food(Game *game);

// Function to update the position of the food objects
void update_food(Game *game);

// Function to check for obstacles at the snake's head position
int check_for_obstacles(Game *game);

// Function to update the game speed based on the snake's length
void update_game_speed(Game *game);

// Function to update the game state
void update_game_state(Game *game);

// Function to render the game on the screen
void render_game(SDL_Renderer *renderer, Game *game, TTF_Font *font);

// Function to render the snake on the screen
void render_snake(SDL_Renderer *renderer, Game *game);

// Function to render the food objects on the screen
void render_food(SDL_Renderer *renderer, Game *game);

// Function to render the obstacles on the screen
void render_obstacles(SDL_Renderer *renderer, Game *game);

// Function to render the game score on the screen
void render_game_score(SDL_Renderer *renderer, Game *game, TTF_Font *font);

// Function to render the game board on the screen
void render_board(SDL_Renderer *renderer);

// Function to render a square with a specific color on the screen
void render_square(SDL_Renderer *renderer, Pos pos, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

// -------------------
// GLOBAL VARIABLES

Game GAME = {0};

// -------------------
// UTILS

// Function to handle SDL error codes
void scc(int code) {
  if (code < 0) {
    printf("SDL error: %s\n", SDL_GetError());
    exit(1);
  }

  return;
}

// Function to handle SDL pointer errors
void *scp(void *ptr) {
  if (ptr == NULL) {
    printf("SDL error: %s\n", SDL_GetError());
    exit(1);
  }

  return ptr;
}

// Function to generate a random integer within a range
int random_int_range(int low, int high) {
  return (rand() % (high - low)) + low;
}

// Function to generate a random position on the game board
Pos random_board_pos(void) {
  Pos p = {0};
  p.x = random_int_range(0, BOARD_WIDTH);
  p.y = random_int_range(0, BOARD_HEIGHT);

  return p;
}

// Function to generate a random direction for the snake
Dir random_dir(void) {
  return (Dir) random_int_range(0, 4);
}

// Check if a position on the game board is not empty
int pos_is_not_empty(Game *game, Pos p) {
  // Check if there is any food at the position
  for (int i = 0; i < FOODS_COUNT; i++) {
    if (p.x == game->food[i].pos.x && p.y == game->food[i].pos.y)
      return 1; // Position is not empty
  }

  // Check if there is any snake body segment at the position
  for (int i = 0; i < game->snake.length; i++) {
    if (p.x == game->snake.body[i].x && p.y == game->snake.body[i].y)
      return 1; // Position is not empty
  }  

  // Check if there is any obstacle at the position
  for (int i = 0; i < OBSTACLES_COUNT; i++) {
    if (p.x == game->obs[i].pos.x && p.y == game->obs[i].pos.y)
      return 1; // Position is not empty
  }  

  return 0; // Position is empty
}

// Generate a random empty position on the game board
Pos random_empty_board_pos(Game *game) {
  Pos p = {0};
  do {
    p = random_board_pos();
  } while (pos_is_not_empty(game, p));

  return p;
}

// Initialize the game state
void init_game(Game *game) {
  // Initialize the snake
  game->snake.body[0] = random_board_pos();
  game->snake.length = 1;
  game->snake.dir = random_dir();
  
  init_food(game);

  // Initialize the obstacles
  int obstacleOffset = 0;
  STAR_OBSTACLE(game, obstacleOffset, 10, 10);
  HORIZONTAL_WALL_OBSACLE(game, obstacleOffset, 20, 20)

  game->quit = 0;
  game->global_score = 0;
  game->game_speed = MAX_SNAKE_MOVEMENT;
}

// Get a pointer to the snake's head position
Pos *get_snake_head(Snake *snake) {
  return &snake->body[snake->length - 1];
}

// Determine if snake movement is allowed based on manual input and game speed
int allow_snake_movement(int manual, Game *game) {
  static struct timeval old_t = {0};
  static struct timeval new_t = {0};
  static int init = -1;
  double time_elapsed = -1;

  if (init == -1) {
    // First call to the function
    init = 1;
    gettimeofday(&old_t, NULL);

    return manual ? 1 : 0;
  }
    
  gettimeofday(&new_t, NULL);
  time_elapsed = (double)(new_t.tv_usec - old_t.tv_usec) / 1000000 +
    (double)(new_t.tv_sec - old_t.tv_sec);
  
  if (!manual && time_elapsed < game->game_speed) {
    // Not enough time has passed for automatic movement
    return 0;
  } else {
    old_t = new_t;
    return 1;
  }
}

// Determine the next position of the snake's head based on the new direction
Pos peak_next_pos(Snake *snake, Dir new_dir) {
  Pos new_pos;
  Pos *head_pos = get_snake_head(snake);

  switch (new_dir) {
    case DIR_RIGHT:
      new_pos.x = (head_pos->x + 1) % BOARD_WIDTH;
      new_pos.y = head_pos->y;
      break;
    
    case DIR_LEFT:
      new_pos.x = head_pos->x == 0 ? BOARD_WIDTH - 1 : head_pos->x - 1;
      new_pos.y = head_pos->y;
      break;

    case DIR_UP:
      new_pos.x = head_pos->x;    
      new_pos.y = head_pos->y == 0 ? BOARD_HEIGHT - 1 : head_pos->y - 1;
      break;

    case DIR_DOWN:
      new_pos.x = head_pos->x;    
      new_pos.y = (head_pos->y + 1) % BOARD_HEIGHT;
      break;
  }

  return new_pos;  
}

void move_snake(Game *game, Dir new_dir, int manual) {
  if (!allow_snake_movement(manual, game)) {
    return;
  }

  Snake *snake = &game->snake;
  Pos new_pos = peak_next_pos(snake, new_dir);

  // Check if the snake is moving back to its own tail
  if (snake->length >= 2 &&
      new_pos.x == snake->body[snake->length - 2].x &&
      new_pos.y == snake->body[snake->length - 2].y) {
    return;
  }

  // Perform movement
  Pos *head_pos = get_snake_head(snake);
  Pos old_pos = *head_pos;
  Pos tmp_pos = old_pos;

  *head_pos = new_pos;
  snake->dir = new_dir;

  for (int i = snake->length - 2; i >= 0; i--) {
    tmp_pos = snake->body[i];
    snake->body[i] = old_pos;
    old_pos = tmp_pos;
  }
}

// Eat the food and update the game state accordingly
void eat_food(Game *game, Food *f) {
  Snake *snake = &game->snake;

  // Increase the global score and reset the food's score
  game->global_score += f->score;
  f->score = 0;

  // Grow the snake's body
  Pos new_pos = peak_next_pos(snake, snake->dir);
  snake->length += 1;
  snake->body[snake->length - 1] = new_pos;
}

// Initialize the food positions and scores
void init_food(Game *game) {
  for (int i = 0; i < FOODS_COUNT; i++) {
    game->food[i].score = 1;
    game->food[i].pos = random_empty_board_pos(game);
  }
}

// Check if enough time has passed to refresh the food
int allow_refresh_food(void) {
  static struct timeval old_t = {0};
  static struct timeval new_t = {0};
  static int init = -1;
  double time_elapsed = -1;

  if (init == -1) {
    init = 1;
    gettimeofday(&old_t, NULL);
    return 1;
  }

  gettimeofday(&new_t, NULL);
  time_elapsed = (double)(new_t.tv_usec - old_t.tv_usec) / 1000000 + (double)(new_t.tv_sec - old_t.tv_sec);

  if (time_elapsed < DELAY_FOOD_SPAWN) {
    return 0;
  } else {
    old_t = new_t;
    return 1;
  }
}

// Check if the snake's head is on a food position and return the food object if found
Food *check_for_food(Game *game) {
  Snake *snake = &game->snake;
  Pos head_pos = *get_snake_head(snake);

  for (int i = 0; i < FOODS_COUNT; i++) {
    Food *f = &game->food[i];

    if (f->pos.x == head_pos.x && f->pos.y == head_pos.y && f->score > 0) {
      return f;
    }
  }

  return NULL;
}

// Update the food positions if enough time has passed
void update_food(Game *game) {
  if (allow_refresh_food()) {
    init_food(game);
  }
}

// Update the game speed based on the global score
void update_game_speed(Game *game) {
  double step_update = game->global_score * STEP_SNAKE_MOVEMENT;

  if (MAX_SNAKE_MOVEMENT - step_update < MIN_SNAKE_MOVEMENT) {
    game->game_speed = MIN_SNAKE_MOVEMENT;
  } else {
    game->game_speed = MAX_SNAKE_MOVEMENT - step_update;
  }
}

// Check if the snake has collided with any obstacles
int check_for_obstacles(Game *game) {
  Snake *s = &game->snake;
  Pos head_pos = *get_snake_head(s);

  // Check if the snake has collided with its own tail
  for (int i = 0; i < s->length - 2; i++) {
    if (s->body[i].x == head_pos.x && s->body[i].y == head_pos.y) {
      return 1;
    }
  }

  // Check if the snake has collided with any initialized obstacles
  for (int i = 0; i < OBSTACLES_COUNT; i++) {
    Obstacle ob = game->obs[i];
    if (ob.init && ob.pos.x == head_pos.x && ob.pos.y == head_pos.y) {
      return 1;
    }
  }

  return 0;
}

// Update the game state including snake movement, obstacle collision, food consumption, game speed, and food refresh
void update_game_state(Game *game) {
  move_snake(&GAME, GAME.snake.dir, 0);
  GAME.quit |= check_for_obstacles(&GAME);
  Food *f = check_for_food(&GAME);
  if (f) {
    eat_food(&GAME, f);
    update_game_speed(&GAME);
  }
  update_food(&GAME);
}

// Render the game on the screen
void render_game(SDL_Renderer *renderer, Game *game, TTF_Font *font) {
  scc(SDL_SetRenderDrawColor(renderer, HEX_COLOR(BACKGROUND_COLOR)));
  SDL_RenderClear(renderer);

  render_snake(renderer, game);
  render_food(renderer, game);
  render_obstacles(renderer, game);
  render_game_score(renderer, game, font);

  SDL_RenderPresent(renderer);
}

// Render the grid lines of the game board
void render_board(SDL_Renderer *renderer) {
  scc(SDL_SetRenderDrawColor(renderer, HEX_COLOR(GRID_COLOR)));

  for (int x = 0; x < BOARD_WIDTH; x++) {
    SDL_RenderDrawLine(renderer,
                       x * CELL_WIDTH, 0,
                       x * CELL_WIDTH, SCREEN_HEIGHT);
  }

  for (int y = 0; y < BOARD_HEIGHT; y++) {
    SDL_RenderDrawLine(renderer,
                       0, y * CELL_HEIGHT,
                       SCREEN_WIDTH, y * CELL_HEIGHT);
  }
}

// Render the snake on the screen
void render_snake(SDL_Renderer *renderer, Game *game) {
  for (int i = game->snake.length - 1; i >= 0; i--) {
    render_square(renderer, game->snake.body[i], HEX_COLOR(SNAKE_COLOR));
  }
}

// Render the food on the screen
void render_food(SDL_Renderer *renderer, Game *game) {
  for (int i = 0; i < FOODS_COUNT; i++) {
    if (game->food[i].score == 0) {
      continue; // Skip foods that have already been eaten
    }
    render_square(renderer, game->food[i].pos, HEX_COLOR(FOOD_COLOR));
  }
}

// Render the obstacles on the screen
void render_obstacles(SDL_Renderer *renderer, Game *game) {
  scc(SDL_SetRenderDrawColor(renderer, HEX_COLOR(OBSTACLE_COLOR)));

  for (int i = 0; i < OBSTACLES_COUNT; i++) {
    Obstacle ob = game->obs[i];

    if (!ob.init) {
      continue; // Skip uninitialized obstacles
    }

    render_square(renderer, ob.pos, HEX_COLOR(OBSTACLE_COLOR));
  }
}

// Render the game score on the screen
void render_game_score(SDL_Renderer *renderer, Game *game, TTF_Font *font) {
  static SDL_Surface *surface = NULL;
  static SDL_Texture *texture = NULL;
  static int prev_score = -1;

  if (prev_score == game->global_score) {
    return; // Nothing to update
  }

  if (surface != NULL) {
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
  }

  char str[32];
  sprintf(str, "Score: %d", game->global_score);

  surface = TTF_RenderText_Solid(font, str, (SDL_Color){HEX_COLOR(SCORE_COLOR)});

  if (!surface) {
    printf("Error: %s\n", TTF_GetError());
    exit(1);
  }

  texture = scp(SDL_CreateTextureFromSurface(renderer, surface));

  int textW = 0;
  int textH = 0;
  SDL_QueryTexture(texture, NULL, NULL, &textW, &textH);
  SDL_Rect text_rect = {0, 0, textW, textH};

  SDL_RenderCopy(renderer, texture, NULL, &text_rect);

  prev_score = game->global_score;
}

// Render a square on the screen with the specified color
void render_square(SDL_Renderer *renderer, Pos pos, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
  scc(SDL_SetRenderDrawColor(renderer, r, g, b, a));

  SDL_Rect rect = {
    (int)floorf(pos.x * CELL_WIDTH),
    (int)floorf(pos.y * CELL_HEIGHT),
    (int)floorf(CELL_WIDTH),
    (int)floorf(CELL_HEIGHT)
  };

  scc(SDL_RenderFillRect(renderer, &rect));
}

int main(void) {
  srand(time(0));

  // Initialize SDL
  SDL_Init(SDL_INIT_VIDEO);

  // Create a window
  SDL_Window *const window = scp(SDL_CreateWindow("Description", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE));
  
  // Create a renderer
  SDL_Renderer *const renderer = scp(SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED));

  // Initialize font
  TTF_Init();
  TTF_Font *font = TTF_OpenFont(FONT_PATH, 30);
  if (!font) {
    printf("Error loading font `%s`: %s\n", FONT_PATH, TTF_GetError());
    exit(1);
  }

  // Initialize the game
  init_game(&GAME);

  // Game loop
  while (!GAME.quit) {
    SDL_Event event;

    // Event handling
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        GAME.quit = 1;
      }

      if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
          case SDLK_UP:
            move_snake(&GAME, DIR_UP, 1);
            break;

          case SDLK_DOWN:
            move_snake(&GAME, DIR_DOWN, 1);
            break;

          case SDLK_LEFT:
            move_snake(&GAME, DIR_LEFT, 1);
            break;

          case SDLK_RIGHT:
            move_snake(&GAME, DIR_RIGHT, 1);
            break;
        }
      }
    }

    // Update game state
    update_game_state(&GAME);

    // Render the game
    render_game(renderer, &GAME, font);
  }

  // Clean up
  TTF_CloseFont(font);
  TTF_Quit();
  SDL_Quit();

  return 0;
}
