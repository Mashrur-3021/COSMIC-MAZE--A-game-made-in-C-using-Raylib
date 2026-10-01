# COSMIC MAZE

A simple maze game built in C using the raylib library.

This project is a lightweight arcade-style maze game with a space-themed aesthetic, where the player navigates through a maze, avoids obstacles, and reaches the goal.

## Project Overview

COSMIC MAZE is a beginner-friendly C game project designed to showcase:

- raylib-based graphics and input handling
- simple game loop structure
- maze-based gameplay mechanics
- basic collision detection and win/lose conditions
- a clean, compact codebase suitable for learning and extension

## Features

- 2D maze gameplay
- Space-inspired visual theme
- Keyboard controls
- Objective-based progression
- Simple, easy-to-modify game logic
- Built entirely in C

## Controls

Use the keyboard to move the player through the maze:

- W / Up Arrow: Move up
- S / Down Arrow: Move down
- A / Left Arrow: Move left
- D / Right Arrow: Move right

Use the Escape key to quit the game.

## How to Play

1. Launch the game.
2. Move the player through the maze.
3. Reach the exit or goal tile.
4. Avoid walls and traps (if present in the specific version).
5. Complete the maze to win.

## Requirements

Before building this project, make sure you have:

- C compiler (gcc or clang)
- raylib installed on your system
- a terminal or IDE capable of running C programs

## Installation and Build

### Linux / macOS

If raylib is installed and available on your system, you can compile the project with a command similar to:

```bash
gcc main.c -o cosmic_maze -lraylib -lm
```

If the game is split into multiple source files, compile all of them together:

```bash
gcc src/*.c -o cosmic_maze -lraylib -lm
```

Then run:

```bash
./cosmic_maze
```

### Windows

Use MinGW or another C toolchain with raylib linked correctly, for example:

```bash
gcc main.c -o cosmic_maze.exe -lraylib -lm
```

If your project has a different file layout, adjust the command to match the actual source files.

## Project Structure

A typical structure for this project may look like this:

```text
COSMIC-MAZE--A-game-made-in-C-using-Raylib/
├── main.c
├── assets/
├── src/
├── README.md
└── ...
```

## Notes

This README is based on the repository description and the project’s C + raylib nature. If the project structure or controls differ from this template, feel free to update the file as the code evolves.

## License

This project does not currently list a license in the repository metadata. If you plan to publish or share it publicly, consider adding a license file such as MIT or GPL.

## Future Ideas

Possible enhancements include:

- multiple levels
- timer and score system
- enemy movement
- sound effects and music
- menu and game-over screens
- mobile or touch input support

## Contributing

Contributions are welcome. If you want to improve the game, add new levels, or clean up the code, open a pull request or fork the project and build from there.
