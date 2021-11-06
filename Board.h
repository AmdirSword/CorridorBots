//
// Created by leviz on 23.10.2021.
//

#ifndef ENGINE_BOARD_H
#define ENGINE_BOARD_H

#include <string>
#include <vector>

/* === Struct Pos_t ===
 * Represents a two-dimensional integer position on a grid.
 *  Operations of addition, subtraction, assignment and comparison are available.
 */
struct Pos_t
{
    int8_t x {};
    int8_t y {};

    Pos_t& operator=( const Pos_t &rhs );
    Pos_t operator+( const Pos_t &rhs ) const;
    Pos_t operator-( const Pos_t &rhs ) const;
    bool operator==( const Pos_t &rhs ) const;
};

/* === Struct PlayerPiece_t ===
 * Represents a single piece, controlled by a player.
 *  The piece contains a player name and its position on the board.
 */
struct PlayerPiece_t
{
    Pos_t pos;
    std::string name {};
};

/* === Struct WallPiece_t ===
 * Represents a piece of wall (specific for Quoridor).
 *  The piece contains its position on the board and the orientation: vertical or horizontal.
 *  Operations of assignment and comparison are available.
 */
struct WallPiece_t
{
    Pos_t pos;
    bool vertical {};

    WallPiece_t& operator=( const WallPiece_t &rhs );
    bool operator==( const WallPiece_t &rhs ) const;
};

using PlayersList_t = std::vector<PlayerPiece_t>;
using WallsList_t = std::vector<WallPiece_t>;

/* === Struct Board_t ===
 * Represents a board with pieces on it.
 *  The board contains a list of player pieces and a list of wall pieces (specific for Quoridor).
 */
struct Board_t
{
    PlayersList_t players {};
    WallsList_t walls {};
};

#endif //ENGINE_BOARD_H
