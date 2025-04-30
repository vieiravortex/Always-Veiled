# Always Veiled

**Always Veiled** is a heartfelt 2D horror game prototype (version v18.05.1982) developed from scratch using C++ and SFML 2.6.2 (64-bit). Created by Vieira Vortex as a birthday tribute inspired by classics like *The Ring* and *The Omen*, this prototype is a lighthearted yet eerie starting point for a future psychological horror game in the style of *Silent Hill*. Dedicated to someone special (we’re a bit shy to name her 😉), it’s a labor of love with a hope for a small miracle—perhaps a glimpse of her attention. 🌟

This is not a full game but a prototype, showcasing two whimsical levels with a touch of mystery. The final vision will dive deeper into psychological horror, with a more immersive experience that may evolve beyond 2D. For now, enjoy the calming pace, custom-coded mechanics, and a spark of eerie atmosphere! 👁️

## Table of Contents
- [Prototype Overview](#prototype-overview)
- [Features](#features)
- [Project Structure](#project-structure)
- [Dependencies](#dependencies)
- [Building and Running](#building-and-running)
- [How to Play](#how-to-play)
- [License](#license)

## Prototype Overview
*Always Veiled* (version v18.05.1982) is a proof-of-concept for a psychological horror game, built entirely from raw C++ and SFML without a pre-made engine—a small triumph for coders! It features two distinct levels:
1. **"The Good Mother"**: Play as Anla, a 14-year-old boy, navigating a river to rescue a drowning girl. Dodge rocks and hostile creatures in a serene yet mysterious setting.
2. **"I Have the Honor"**: Control Parvis, a 27-year-old rebel, hijacking a train to free his friend. Battle enemies across the platform, train roof, and machinist cabin to reach the goal.

This prototype is a tribute, crafted with love and a nod to horror classics. While its current whimsical graphics and simple gameplay are far from the final vision, they lay the groundwork for a darker, more immersive experience. We hope it might one day reach the person it’s meant for—a miracle we quietly dream of. 💖

More details at [vieiravortex.github.io/alwaysveiled](https://vieiravortex.github.io/alwaysveiled/).

## Features
- **Two Prototype Levels**:
  - **River Level**: Swim through a scrolling river, avoiding obstacles and enemies to save a girl. Features animated sprites for the player, enemies, and environment.
  - **Station Level**: Progress through three phases (Platform, Train Roof, Machinist Cabin). Shoot enemies and interact with the environment to complete the mission.
- **Custom-Built with C++ and SFML**: No game engine—just pure code, showcasing the dedication behind this prototype.
- **Animated Sprites**: Smooth character and enemy animations using frame-based sprite sheets.
- **Narrative Introductions**: Each level begins with a story screen, setting the tone with a touch of mystery.
- **Dynamic Camera and Backgrounds**: Scrolling backgrounds and a player-following camera for an engaging experience.
- **Randomized Elements**: Enemies and obstacles spawn using a seeded random number generator for varied gameplay.
- **Pause Menu**: Pause the game to continue or return to the main menu.
- **Health System**: Players have a health bar (HP) that decreases upon collisions, leading to a game over if depleted.

## Project Structure
```
/Always Veiled
├── /Always Veiled           # Source code (.cpp and .h files)
├── /x64/Release             # Resources (sprites, fonts, .dll files)
└── /Always Veiled.sln       # Visual Studio solution file
```

Key source files:
- `Game.cpp`, `Game.h`: Core logic, state management, and resource loading.
- `main.cpp`: Entry point, initializes the SFML window and prototype.
- `Menu.cpp`: Handles the main menu and navigation.
- `RiverLevel.cpp`, `StationLevel.cpp`: Implement level-specific mechanics.
- `Renderer.cpp`, `Renderer.h`: Manages rendering of scenes, animations, and UI.
- `resource.h`: Defines resource IDs (game icon).

Resources in `/x64/Release` include:
- Sprite sheets for characters, enemies, and backgrounds (`Sprites/River/`, `Sprites/Train/`, etc.).
- Fonts for UI and story text (`Fonts/Univers 45 Light Regular.otf` and etc.).
- Game icon (`icon.png`).

## Dependencies
- **SFML 2.6.2 (64-bit)**: Simple and Fast Multimedia Library for graphics, windowing, and input handling. Download from [sfml-dev.org](https://www.sfml-dev.org/download/sfml/2.6.2/).
- **Visual Studio**: For building the project (solution file provided).
- **C++17 or later**: For modern C++ features used in the codebase.

## Building and Running
1. **Clone the Repository**:
   ```bash
   git clone <repository-url>
   cd Always-Veiled
   ```

2. **Set Up SFML**:
   - Download SFML 2.6.2 from [sfml-dev.org](https://www.sfml-dev.org/download/sfml/2.6.2/) and configure it in Visual Studio.

3. **Open the Solution**:
   - Open `Always Veiled.sln` in Visual Studio.
   - Set the build configuration to `x64` and `Release`.

4. **Copy Resources**:
   - Ensure the resources (sprites, fonts, `icon.png`) are in the `/x64/Release` directory relative to the executable.

5. **Build and Run**:
   - Build the solution in Visual Studio.
   - Run the executable from the `/x64/Release` directory to ensure resources load correctly.

**Note**: Ensure SFML 2.6.2 `.dll` files (e.g., `sfml-graphics-2.dll`, `sfml-window-2.dll`, `sfml-system-2.dll`) are in the same directory as the executable or in your system PATH.

## How to Play
- **Main Menu**:
  - Click "Start Game" to choose a level.
  - Select "Save the Girl" for the River Level or "Hijack a Train" for the Station Level.
- **River Level**:
  - Use arrow keys to swim (Left, Right, Up, Down).
  - Hold `LShift` to swim faster.
  - Avoid rocks and creatures to reach the drowning girl.
- **Station Level**:
  - Use arrow keys to move (Left, Right, Up, Down).
  - Hold `LShift` to run faster.
  - Press `LControl` to shoot (combine with Up/Down for angled shots in Platform/Train Roof phases).
  - Defeat enemies, progress through phases, and interact with the environment in the Machinist Cabin.
- **Pause**: Press `Escape` to pause and access the continue or main menu options.

## License
This project is licensed under the Creative Commons Zero v1.0 Universal License. See the [LICENSE](LICENSE) file for details.

---
Developed by Vieira Vortex as a tribute to someone special. 💫  
Check out the source code on GitHub and join us in hoping for a little miracle! 👀  
#VieiraVortex #AlwaysVeiled #AsiaVieira #TheOmen #TheRing #SilentHill #HorrorGame #IndieDev #SFML #C++
