#pragma once
enum Piece {
    NONE=0,
    QUEEN=1,
    ROOK=2,
    BISHOP=3,
    KNIGHT=4,
    KING=5,
    PAWN=6,
    WHITE=8,
    BLACK=16
    /* 5 bit numbers*/
};

constexpr int TYPE_MASK=0b00111;
constexpr int COLOR_MASK=0b11000;
constexpr int NUM_SQUARES=64;
constexpr int WIDTH=8;

enum Square {
    A1 = 0, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8,
    NO_SQUARE = -1
};

constexpr int rowOf(int sq) {return sq/WIDTH;}
constexpr int columnOf(int sq) {return sq%WIDTH;}
constexpr int boardPos(int r, int c) {return (r*WIDTH + c);}


constexpr int WHITE_KINGSIDE  = 1;
constexpr int WHITE_QUEENSIDE = 2;
constexpr int BLACK_KINGSIDE  = 4;
constexpr int BLACK_QUEENSIDE = 8;
