//
// Created by leviz on 31.10.2021.
//

#include "Board.h"

//TODO where we place * and &?

/* === Struct Pos_t ===
 * Represents a two-dimensional integer position on a grid.
 *  Operations of addition, subtraction, assignment and comparison are available.
 */

Pos_t& Pos_t::operator=(const Pos_t &rhs) = default;


Pos_t Pos_t::operator+(const Pos_t &rhs) const
{
    return Pos_t { static_cast<int8_t>(x + rhs.x),
                   static_cast<int8_t>(y + rhs.y) };
}

Pos_t Pos_t::operator-(const Pos_t &rhs) const
{
    return Pos_t { static_cast<int8_t>(x - rhs.x),
                   static_cast<int8_t>(y - rhs.y) };
}

bool Pos_t::operator==(const Pos_t &rhs) const
{
    return ( x == rhs.x && y == rhs.y );
}

/* === Struct PlayerPiece_t ===
 * Represents a single piece, controlled by a player.
 *  The piece contains a player name and its position on the board.
 */

/* PlayerPiece_t has no methods */

/* === Struct WallPiece_t ===
 * Represents a piece of wall (specific for Quoridor).
 *  The piece contains its position on the board and the orientation: vertical or horizontal.
 *  Operations of assignment and comparison are available.
 */

WallPiece_t &WallPiece_t::operator=(const WallPiece_t &rhs) = default;

bool WallPiece_t::operator==(const WallPiece_t &rhs) const {
    return ( pos == rhs.pos && vertical == rhs.vertical );
}

/* === Struct Board_t ===
 * Represents a board with pieces on it.
 *  The board contains a list of player pieces and a list of wall pieces (specific for Quoridor).
 */

/* Board_t has no methods */