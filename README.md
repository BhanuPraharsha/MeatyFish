# MeatyFish

A chess engine written from scratch in C++17. It speaks the UCI protocol, so it runs inside any UCI chess GUI (tested with Cute Chess).

## Features

- **Board:** 64-square array (a1 = 0, h8 = 63). A piece is a 5-bit value: the piece type in bits 0–2, the colour flag (`WHITE = 8`, `BLACK = 16`) in bits 3–4.
- **FEN loading** and a text board printer.
- **Full legal move generation:** all pieces, castling, en passant and promotion. Pseudo-legal moves are filtered by making each move and checking whether the mover's king is attacked.
- **Make/unmake** with an undo stack. Castling rights are updated through a per-square mask.
- **Search:** negamax with alpha-beta pruning, at a fixed depth of 6. Mate scores include the distance from the root, so the engine prefers faster mates and delays being mated.
- **Move ordering:** MVV-LVA (captures of valuable pieces by cheap pieces first), promotions first.
- **Evaluation:** material plus piece-square tables (the "simplified evaluation function" values), scored from the side to move's point of view.
- **UCI:** `uci`, `isready`, `ucinewgame`, `position startpos|fen … [moves …]`, `go`, `quit`.
- **Debug commands:** `perft` and `bench` (see below).

## Build

```bash
cmake -S . -B build      # configure (Release by default)
cmake --build build      # compile
./build/engine           # run; the engine waits for UCI commands on stdin
```

Requires CMake 3.16+ and a C++17 compiler. Warnings are enabled (`-Wall -Wextra -Wpedantic`).

## Usage

### In a GUI
In Cute Chess: *Tools → Settings → Engines → +*, set the command to the path of `build/engine` and the protocol to UCI.

### From the terminal
```
position startpos moves e2e4 e7e5
go
info depth 6 nodes …
bestmove …
```

### perft
Counts the leaf nodes of the legal-move tree and prints the count for each root move:
```
perft <depth> <fen>
```
Example: `printf 'perft 4 rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1\nquit\n' | ./build/engine`

### bench
Searches a fixed set of positions and prints the score, node count and time, with and without alpha-beta pruning (plain negamax runs only up to depth 4). The scores of the two searches must be equal.
```
bench [depth]        # default depth 4
```

## Correctness

Move generation matches the published perft results:

| Position | Depth | Nodes |
|---|---|---|
| Start position | 5 | 4,865,609 |
| Kiwipete | 4 | 4,085,603 |
| Position 3 | 5 | 674,624 |
| Position 4 | 4 | 422,333 |
| Position 5 | 4 | 2,103,487 |
| Position 6 | 4 | 3,894,594 |

`bench 4` reports identical scores for alpha-beta and plain negamax. Alpha-beta with move ordering visits 72,434 nodes in total, against 11,224,275 for plain negamax.

## Project layout

```
src/
├── types.h          piece and square enums, masks, constants, piece values
├── move.h/.cpp      Move struct, move to UCI string
├── board.h/.cpp     Board class: FEN, print, makeMove/unmakeMove
├── movegen.h/.cpp   pseudo-legal and legal moves, attack detection, castling
├── perft.h/.cpp     perft and divide
├── pst.h            piece-square tables
├── eval.h/.cpp      evaluation
├── search.h/.cpp    negamax, alpha-beta, move ordering, benchmark
├── uci.h/.cpp       UCI command loop, perft and bench commands
└── main.cpp         entry point
```

Dependencies flow: `types → move → board → movegen → eval/search → uci → main`.