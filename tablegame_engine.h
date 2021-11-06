//
// Created by leviz on 17.10.2021.
//

#ifndef ENGINE_TABLEGAME_ENGINE_H
#define ENGINE_TABLEGAME_ENGINE_H

#include "Board.h"

#include <array>
#include <cassert>
#include <map>
#include <memory>
#include <set>
#include <stdexcept>
#include <utility>

#define SPTR(x) std::shared_ptr<x>
#define UPTR(x) std::unique_ptr<x>

namespace game_engine
{

/*
 * === Exceptions ===
 */

    class IllegalMoveException : public std::exception
    {
    public:
        explicit IllegalMoveException( char const* message );
    };

    class PlayerNotFoundException : public std::exception
    {
    public:
        explicit PlayerNotFoundException( char const* message );
    };

    class TooFewPlayersException : public std::exception
    {
    public:
        explicit TooFewPlayersException( char const* message );
    };

/* === Class TablegameEngine_t ===
 * Represents an engine that controls the logic of a table game.
 *  This is an abstract class. It contains a board and an index of a current player, who makes the soonest turn.
 */

    class TablegameEngine_t
    {

    public:
        /*
         * Default constructor
         */
        TablegameEngine_t() = default;

        /*
         * Constructor TablegameEngine_t( Board_t board ) builds an engine with a predefined board.
         *  The current player is 0.
         */
        explicit TablegameEngine_t( Board_t board );

        /*
         * Constructor TablegameEngine_t( Board_t board, int first_player ) builds an engine with predefined board and
         *  index of current player.
         */
        TablegameEngine_t( Board_t board, int first_player );

        /*
         * GetBoard() returns a ref-to-const board in an table game engine object.
         */
        const Board_t& GetBoard() const;

        /*
         * GetPlayersList() returns a ref-to-const list of players from the board in an table game engine object.
         */
        const PlayersList_t& GetPlayersList() const;

        /*
         * IsGameOver() signals whether there is a winner player on the board.
         */
        virtual bool IsGameOver() const = 0;

        /*
         * FindWinner() returns a winner name. Winner is a player satisfying a winning criterion.
         */
        virtual std::string FindWinner() const = 0;

        /*
         * GetCurrentPlayer() return an player index who takes the soonest turn.
         */
        int GetCurrentPlayer() const;

        /*
         * TODO
         */
        virtual void FinishGame() = 0;

    private:

        /*
         * NextTurn() passes the turn to the next player. Current player index is updated.
         */
        void NextTurn();

        int m_current_player {};

    protected:
        /*
         * findPlayer( const std::string& player_name ) returns an iterator in the players list in the board.
         *  The iterator points to a player with name player_name.
         *  .end() is returned if no player found.
         */
        auto findPlayer( const std::string& player_name ) const;

        /*
         * GetPlayerPos(const std::string& player_name ) returns a ref-to-const position of
         *  a player with name player_name.
         *  Throws an exception if no player found.
         */
        const Pos_t & GetPlayerPos( const std::string& player_name ) const;

        struct Board_t m_board {};
    };


/*
 * === Class QuoridorEngine ===
 * Represents a Quoridor table game engine.
 */

    class QuoridorEngine_t : public TablegameEngine_t
    {
    public:

        /*
         * MovePlayer( const std::string& player_name, Pos_t new_pos ) moves a piece of a player with name player
         *  to a position new_pos.
         *  Throws an exception if the move is illegal.
         */
        void MovePlayer( const std::string& player_name, Pos_t new_pos );

        /*
         * AddWall( WallPiece_t wall ) places a new wall on the board.
         *  Throws an exception if placing the wall on the board is illegal.
         */
        void AddWall( WallPiece_t wall );

        /*
         * QuoridorInit( const std::vector<std::string>& player_names ) sets up the board
         *  with the player pieces specified.
         *  There can be two or four players. The position of players corresponds to his name index in the input vector.
         */
        void QuoridorInit( const std::vector<std::string>& player_names );

        /*
         * IsGameOver() signals whether there is a winner player on the board.
         */
        bool IsGameOver() const override;

        /*
         * FindWinner() returns a winner name. Winner is a player satisfying a winning criterion.
         * The criterion in Quoridor is player standing in one of goal positions on the board.
         */
        std::string FindWinner() const override;

        /*
         * TODO
         */
        void FinishGame() override;

    protected:

        /*
         * CheckMove( const Pos_t& cur_pos, const Pos_t& new_pos ) returns whether a player piece move is legal
         *  from position cur_pos to position new_pos.
         *  The board does not need to contain a player piece at cur_pos.
         */
        bool CheckMove( const Pos_t& cur_pos, const Pos_t& new_pos ) const;

        /*
         * CheckAddWall( const WallPiece_t& wall ) returns whether placing the new wall on the board is legal.
         */
        bool CheckAddWall( const WallPiece_t& wall ) const;

        /*
         * CheckWallBlocksPath() returns whether on the board with the added wall
         *  there is a path to at least one goal position for each player.
         *  Warning! CheckWallBlocksPath does not check if wall adding is possible.
         */
        bool CheckWallBlocksPath( WallPiece_t wall ) const;

        /*
         * IsPosFree( Pos_t pos ) returns whether position pos is NOT occupied by any player piece.
         */
        bool IsPosFree( Pos_t pos ) const;

    private:

        /*
         * GetGoalPosForPlayerIndex( int player_index ) returns a set of goal positions
         *  for a player with index player_index
         */
        static std::set<Pos_t> GetGoalPosForPlayerIndex( int player_index );

        /*
         * IsPosLegal( const Pos_t& pos ) returns whether position pos is in bounds of the board.
         */
        static bool IsPosLegal( const Pos_t& pos );

        /*
         * IsWallLegal( const WallPiece_t& wall ) returns whether the wall is in bounds of the board.
         */
        static bool IsWallLegal( const WallPiece_t& wall );

        /*
         * IsWallBetweenAdjPos( Pos_t a, Pos_t b ) returns whether there is a wall
         *  between two adjacent positions a and b.
         *  Warning! Use method for adjacent positions only!
         */
        bool IsWallBetweenAdjPos( Pos_t a, Pos_t b ) const;

        /*
         * IsWallBetweenAdjPos( Pos_t a, Pos_t b,  WallPiece_t wall  ) returns whether the wall is
         *  between two adjacent positions a and b.
         *  WallPiece_t wall may be not on the board.
         *  Warning! Use IsWallBetweenAdjPos for adjacent positions only!
         */
        static bool IsWallBetweenAdjPos( Pos_t a, Pos_t b, WallPiece_t wall ) ;

        /*
         * findWall( WallPiece_t wall ) returns whether the wall is placed on the board.
         */
        bool findWall( WallPiece_t wall ) const;

        static const int m_board_side_length { 9 };
        static const int m_wall_pos_per_side { m_board_side_length - 1 };

        constexpr static Pos_t player_positions[4] = { { 0, 4 },
                                                       { 8, 4 },
                                                       { 4, 0 },
                                                       { 4, 8 } };

        // TODO change in Board_t to set?
        std::set<WallPiece_t> m_walls_on_board {};
    };


/*
 * === Class PlayerQuoridorEngine ===
 * Represents a Quoridor engine for player class.
 */

    class PlayerQuoridorEngine_t : private QuoridorEngine_t
    {
        // TODO leave inheritance as private?
    public:

        /*
         * GetPossibleMoves( const std::string& player_name ) returns a vector of possible moves for a player
         *  with name player_name.
         */
        std::vector<Pos_t> GetPossibleMoves( const std::string& player_name ) const;

        /*
         * IsMovePossible( const std::string& player_name, Pos_t pos ) returns whether moving the player piece
         *  with name player_name to position pos on the board is possible.
         */
        bool IsMovePossible( const std::string& player_name, Pos_t pos ) const;

        /*
         * IsWallPossible( const WallPiece_t& wall ) returns whether placing the wall on the board is possible.
         */
        bool IsWallPossible( const WallPiece_t& wall ) const;

        /*
         * UpdateBoard( const Board_t& board ) uploads a board to the player Quoridor engine.
         */
        void UpdateBoard( const Board_t& board );
    };
}


#endif //ENGINE_TABLEGAME_ENGINE_H
