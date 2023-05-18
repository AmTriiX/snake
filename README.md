# Snake Game

This is a simple implementation of the classic Snake game in C using the SDL2 library. The game allows the player to control a snake and navigate it around a board to eat food and grow. The objective is to avoid collisions with the obstacles and the snake's own body.

## Features

- The snake can be controlled using the arrow keys.
- The snake grows in length each time it eats food.
- Food appears randomly on the board.
- Obstacles are present on the board and must be avoided.
- The game ends when the snake collides with obstacles or its own body.
- The player's score is displayed on the screen.
- The game speed increases as the snake grows longer.

## Dependencies

- SDL2 library
- SDL_ttf library

## Installation

1. Clone the repository: `git clone https://github.com/AmTriiX/snake.git`
2. Navigate to the project directory: `cd snake`
3. Compile the code using a C compiler: `make`
4. Run the game: `./snake`

Note: Make sure you have the SDL2 and SDL_ttf libraries installed on your system.

## Controls

- Arrow keys: Control the movement of the snake

## Gameplay

1. The game starts with a snake positioned on the board.
2. Use the arrow keys to navigate the snake.
3. The snake will move in the selected direction until it collides with an obstacle, or itself.
4. The objective is to eat the food that appears randomly on the board.
5. Each time the snake eats food, it grows longer.
6. Avoid collisions with the walls, obstacles, and the snake's own body.
7. The game ends when the snake collides with any of the obstacles, or its own body.
8. The player's score is displayed on the screen.

## Contributing

Contributions are welcome! If you find any issues or have suggestions for improvements, feel free to open an issue or submit a pull request.

## License

This project is licensed under the MIT License. See the LICENSE file for details.