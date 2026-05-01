# Thimlich Hunt
### *3D Tactical Survival Maze (C + Raylib)*

---

## 🧠 Overview

**Thimlich Hunt** is a 3D maze-based tactical survival game inspired by the fortified stone architecture of Thimlich Ohinga.

Built in C using **Raylib**, the project demonstrates real-time rendering, collision detection, spatial navigation, and close-combat mechanics within a constrained labyrinth environment.

You play as a lone defender tasked with reclaiming a breached fortress—navigating narrow corridors, eliminating invaders, and securing inner enclosures.

---
## 🏛️ Lore

Thimlich Hunt takes place within a fortified stone stronghold that has been breached by hostile invaders.

You are the last defender, navigating narrow corridors and layered enclosures to reclaim the fortress. Using your knowledge of the maze-like structure, you must eliminate enemies and secure the inner stronghold before it falls.

---

## 🎮 Features

- **3D Rendering Engine** — Real-time rendering with Raylib  
- **Collision Detection** — Prevents movement through walls and structures  
- **Minimap System** — Supports navigation within the maze  
- **Directional Orientation** — Improves spatial awareness  
- **Enemy AI** *(planned)* — Patrol and pursuit behaviors  
- **Close Combat System** *(planned)* — Traditional weapon-based combat (no firearms)  
- **Zone-Based Gameplay** *(planned)* — Clear areas and reclaim the fortress  

---

## 🎯 Gameplay

- Navigate a maze-like fortress structure  
- Use spatial awareness to avoid ambushes  
- Engage enemies in close combat  
- Clear zones and reclaim control of the stronghold  

---

## 🗺️ Architecture & Design

The level design is inspired by real-world defensive structures:

- **Narrow Entrances** → Chokepoints and ambush zones  
- **Corridors & Enclosures** → Maze navigation paths  
- **Central Area** → Final objective zone  
- **Layered Structure** → Progressive difficulty  

---

## 🛠️ Tech Stack

- **Language:** C  
- **Graphics:** Raylib  
- **Build System:** Make  
- **Platform:** Linux (primary)  

---

## 📦 Installation

    git clone https://github.com/FelixOtienoArogo/Thimlich-Hunt.git
    cd Thimlich-Hunt
    make build
    make run

---

## 🎮 Controls

| Key | Action |
|-----|--------|
| ↑ / ↓ | Move forward / backward |
| ← / → | Rotate camera |
| M | Enable minimap |
| N | Disable minimap |
| ESC | Exit game |

---

## 🧪 Project Goals

- Refactor a legacy SDL2-based maze project into a modern Raylib engine  
- Implement real-time rendering and optimized game loops  
- Introduce AI-driven enemies and combat mechanics  
- Extend into a cloud-deployable application (Docker + Terraform)  

---

## 🚀 Future Improvements

- Enemy AI with pathfinding  
- Health and damage system  
- Multiple levels / dynamic maps  
- Lighting and shadow effects  
- WebAssembly build (browser support)  
- CI/CD pipeline and cloud deployment  

---

## 🧠 Learning Outcomes

- Low-level graphics programming in C  
- Game loop design and state management  
- Spatial reasoning and collision handling  
- Modular system design and refactoring  
- Foundations for containerization and cloud deployment  

---

## 👤 Author

**Felix Otieno Arogo**  
Cloud Solutions Architect | AWS & GCP | Security-focused Engineer  

- GitHub: https://github.com/FelixOtienoArogo  
- Portfolio: https://felixotienoarogo.github.io  

---

## 📜 License

This project is open-source and available under the MIT License.
