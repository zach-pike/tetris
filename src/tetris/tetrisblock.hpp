#pragma once

#include <vector>
#include <tuple>
#include <array>
#include "types.hpp"

class TetrisPiece {
public:
    enum class TetrisPieces : std::uint8_t {
        T,
        L_1,
        L_2,
        Z_1,
        Z_2,
        CUBE
    };

    static const int nTetrisPieces = 6;

    using ColorArray = std::array<Color, TETRIS_GRID_N>;

private:
	std::vector<std::tuple<int, int>> parts;

    Color color;
    TetrisPieces type;
public:
	TetrisPiece(TetrisPieces p);
	~TetrisPiece();

	void rotate();

	void setColors(ColorArray& array);
};