Thimlich Hunt
3D Maze-Based Tactical Survival Game (C + Raylib)
🧠 Overview

Thimlich Hunt is a 3D maze-based tactical survival game inspired by the fortified stone architecture of Thimlich Ohinga.

Built in C using Raylib, the project demonstrates real-time rendering, collision detection, spatial navigation, and close-combat mechanics within a constrained labyrinth environment.

Players take on the role of a lone defender tasked with reclaiming a breached fortress by navigating narrow corridors, eliminating invading enemies, and securing inner enclosures.

🎮 Features
3D Rendering Engine – Real-time rendering using Raylib
Collision Detection – Prevents movement through walls and structures
Minimap System – Assists navigation within the maze
Directional Orientation – Differentiates wall directions for spatial awareness
Enemy AI (Planned) – Patrol and pursuit behavior inside the maze
Close Combat System (Planned) – Traditional weapon-based combat (no firearms)
Zone-Based Gameplay (Planned) – Clear areas and reclaim the fortress
🎯 Gameplay
Navigate a maze-like fortress structure
Use spatial awareness to avoid ambushes
Engage enemies in close combat
Clear zones and reclaim control of the stronghold
🗺️ Architecture & Design

The game’s layout is inspired by real-world defensive structures:

Narrow Entrances → Chokepoints and ambush zones
Corridors & Enclosures → Maze navigation paths
Central Area → Final objective zone
Layered Structure → Progressive difficulty
🛠️ Tech Stack
Language: C
Graphics Library: Raylib
Build System: Make
Platform: Linux (primary)
📦 Installation
git clone https://github.com/FelixOtienoArogo/alx-maze_project.git
cd alx-maze_project
make build
make run
🎮 Controls
Key	Action
↑ / ↓	Move forward / backward
← / →	Rotate camera
M	Enable minimap
N	Disable minimap
ESC	Exit game
🧪 Project Goals
Refactor a legacy SDL2-based maze project into a modern Raylib-based engine
Implement real-time rendering and game loop optimization
Introduce AI-driven enemies and combat mechanics
Extend the project into a cloud-deployable application (Docker + Terraform)
🚀 Future Improvements
Enemy AI with pathfinding
Health and damage system
Multiple levels / dynamic maps
Lighting and shadow effects
WebAssembly build for browser play
CI/CD pipeline and cloud deployment
🧠 Learning Outcomes

This project demonstrates:

Low-level graphics programming in C
Game loop design and state management
Spatial reasoning and collision handling
Modular software design and refactoring
Foundations for containerization and cloud deployment
👤 Author

Felix Otieno Arogo
Cloud Solutions Architect | AWS & GCP | Security-focused Engineer

GitHub: https://github.com/FelixOtienoArogo
Portfolio: https://felixotienoarogo.github.io
📜 License

This project is open-source and available under the MIT License.
