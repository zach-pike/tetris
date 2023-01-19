#include "tetrisblock.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

static Color colors[] = {
    Color{ 0, 4, 255 },
    Color{ 255, 0, 0 },
    Color{ 94, 255, 0 },
    Color{ 0, 251, 255 },
    Color{ 255, 127, 0 }
};

static std::vector<std::tuple<int, int>> getBlockOffsets(TetrisPiece::TetrisPieces p) {
    switch (p) {
        case TetrisPiece::TetrisPieces::T: return std::vector{
            std::tuple(0, 0),
            std::tuple(0, -1),
            std::tuple(-1, 0),
            std::tuple(1, 0),
        };

        case TetrisPiece::TetrisPieces::L_1: return std::vector{
            std::tuple(0, 0),
            std::tuple(1, 0),
            std::tuple(0, 1),
            std::tuple(0, 2)
        };

        case TetrisPiece::TetrisPieces::L_2: return std::vector{
            std::tuple(0, 0),
            std::tuple(-1, 0),
            std::tuple(0, 1),
            std::tuple(0, 2)
        };

        case TetrisPiece::TetrisPieces::Z_1: return std::vector{
            std::tuple(0, 0),
            std::tuple(0, 1),

            std::tuple(-1, 1),
            std::tuple(1, 0)
        };

        case TetrisPiece::TetrisPieces::Z_2: return std::vector{
            std::tuple(0, 0),
            std::tuple(0, 1),

            std::tuple(1, 1),
            std::tuple(-1, 0)
        };

        case TetrisPiece::TetrisPieces::CUBE: return std::vector{
            std::tuple(0, 0),
            std::tuple(1, 0),
            std::tuple(1, 1),
            std::tuple(0, 1)
        };
    }
}

static int colorsLen = sizeof(colors) / sizeof(Color);

TetrisPiece::TetrisPiece(TetrisPieces p):
    type{p}
{
    // Pick a random color and get the correct block offsets
    parts = getBlockOffsets(p);

    srand(time(nullptr));

    color = Color{
        (std::uint8_t)(rand() % 256),
        (std::uint8_t)(rand() % 256),
        (std::uint8_t)(rand() % 256)
    };
}

TetrisPiece::~TetrisPiece() {}

void TetrisPiece::rotate() {
	for (auto& part : parts) {
		part = std::tuple(-std::get<1>(part), std::get<0>(part));
	}
}

void TetrisPiece::setColors(std::array<Color, TETRIS_GRID_N>& array) {
	int x = 4;
	int y = 4;

	for (auto part : parts) {
		// Calculate the position
		int xC = x + std::get<0>(part);
		int yC = y + std::get<1>(part);

		int idx = xC + (yC * TETRIS_GRID_X);

		array.at(idx) = color;
	}
}

