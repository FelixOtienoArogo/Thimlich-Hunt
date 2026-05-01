#!/usr/bin/env bash

set -e

mkdir -p src
mkdir -p include
mkdir -p assets/textures
mkdir -p assets/maps
mkdir -p assets/audio
mkdir -p assets/fonts
mkdir -p build
mkdir -p docs
mkdir -p scripts

touch src/main.c
touch src/game.c
touch src/render.c
touch src/player.c
touch src/input.c
touch src/map.c
touch src/collision.c
touch src/minimap.c
touch src/enemy.c

touch include/game.h
touch include/render.h
touch include/player.h
touch include/input.h
touch include/map.h
touch include/collision.h
touch include/minimap.h
touch include/enemy.h

touch docs/architecture.md
touch docs/design.md

touch Makefile
touch README.md
touch LICENSE
touch .gitignore

echo "Project structure created successfully."
