#pragma once

#include <cstdint>

#define TETRIS_GRID_X (int)(10)
#define TETRIS_GRID_Y (int)(20)

constexpr int TETRIS_GRID_N = TETRIS_GRID_X * TETRIS_GRID_Y;

struct Color {
	std::uint8_t r;
	std::uint8_t g;
	std::uint8_t b;
};