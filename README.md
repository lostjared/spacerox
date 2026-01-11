# Spacerox 🚀

A classic 2D Asteroids-style arcade game built with SDL2.

![Asteroids](https://github.com/lostjared/spacerox/blob/main/spacerox.gif)

## About

Spacerox is an homage to the classic Asteroids arcade game. Navigate your spaceship through an asteroid field, destroy asteroids with your laser cannon, and survive as long as possible to achieve a high score!

## Gameplay

### Objective
Pilot your spacecraft and destroy asteroids while avoiding collisions. Survive as long as possible and rack up the highest score!

### Features
- **Classic arcade gameplay** - True to the original Asteroids experience
- **Asteroid physics** - Asteroids collide and bounce off each other realistically
- **Progressive difficulty** - Asteroids continuously spawn to keep the challenge going
- **Scoring system** - Different asteroid sizes award different points
- **Lives system** - Start with 3 lives, each collision costs one life
- **Weapon overheat** - Hold fire too long and your weapon overheats!
- **Launch sequence** - Dramatic countdown and launch animation at game start
- **Particle explosions** - Colorful explosion effects when ships and asteroids are destroyed
- **Starfield background** - Animated scrolling star background

## Controls

| Key | Action |
|-----|--------|
| **←** (Left Arrow) | Rotate ship counter-clockwise |
| **→** (Right Arrow) | Rotate ship clockwise |
| **↑** (Up Arrow) | Thrust forward |
| **Space** | Fire weapon (hold for rapid fire) |
| **Escape** | Quit game |
| **Space** (on Game Over) | Restart game |

## Scoring

| Asteroid Size | Points |
|---------------|--------|
| Large | 20 |
| Medium | 50 |
| Small | 100 |

**Tip:** Smaller asteroids are worth more points but are harder to hit!

## Mechanics

### Ship Movement
- Your ship has momentum-based physics with gradual deceleration
- Thrusting accelerates in the direction your ship is facing
- Maximum speed is capped to keep gameplay manageable
- The screen wraps around - exit one side and appear on the other

### Weapon System
- Fire rapid bursts of projectiles using the Space key
- Holding down fire continuously will cause your weapon to **overheat**
- When overheated, your ship glows red and you cannot fire for 3 seconds
- Release the fire button periodically to let your weapon cool down

### Asteroids
- Asteroids spawn at various sizes and drift across the screen
- Shooting a large asteroid splits it into 2-3 smaller pieces
- Asteroids wrap around the screen edges
- Asteroids bounce off each other with realistic collision physics
- New asteroids spawn from the edges when too few remain on screen

### Lives & Respawning
- You start with **3 lives**
- Colliding with an asteroid destroys your ship and costs 1 life
- After destruction, a countdown sequence prepares you for respawn
- Game over when all lives are lost

## Building

### Requirements
- GCC compiler
- SDL2 development libraries
- SDL2_ttf library
- Make

### Linux/macOS

```bash
# Install dependencies (Debian/Ubuntu)
sudo apt-get install libsdl2-dev libsdl2-ttf-dev

# Install dependencies (macOS with Homebrew)
brew install sdl2 sdl2_ttf

# Build
make

# Run
./space
```

### Windows

```bash
# Build with MinGW
make -f Makefile.win

# Run
space.exe
```

### Web (Emscripten)

```bash
# Build for web using Emscripten
make -f Makefile.em

# Serve space.html in a local web server
```

## Project Structure

```
spacerox/
├── space.c      # Main game loop, rendering, and game states
├── ship.c       # Ship, projectiles, asteroids, and physics
├── ship.h       # Data structures and function declarations
├── sdl.c        # SDL initialization and text rendering
├── sdl.h        # SDL-related declarations
├── assets/      # Game assets (fonts, icons)
├── Makefile     # Linux/macOS build
├── Makefile.win # Windows build
└── Makefile.em  # Emscripten/Web build
```

## Technical Details

- **Resolution:** 640x360 (scaled to 1280x720 window)
- **Target FPS:** 60
- **Written in:** C
- **Graphics:** SDL2 with vector-drawn graphics (no sprites)

## License

See [LICENSE](LICENSE) file for details.

## Author

Coded by Jared Bruni
