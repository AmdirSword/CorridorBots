//
// Created by leviz on 17.10.2021.
//

#include "tablegame_engine.h"

namespace game_engine
{

/*
 * === Exceptions ===
 */

//TODO is that correct way to overload constructor of std::exception?
    IllegalMoveException::IllegalMoveException( const char *const message ) : exception( message ) {}

    PlayerNotFoundException::PlayerNotFoundException( const char *const message ) : exception(message) {}

    TooFewPlayersException::TooFewPlayersException( const char *const message ) : exception(message) {}

/* === Class TablegameEngine_t ===
 * Represents an engine that controls the logic of a table game.
 *  This is an abstract class. It contains a board and an index of a current player, who makes the soonest turn.
 */

/*
 * Constructor TablegameEngine_t( Board_t board, int first_player ) builds an engine with predefined board and
 *  index of current player.
 */
    TablegameEngine_t::TablegameEngine_t( struct Board_t board, int first_player ) :
            m_board( std::move( board ) ),
            m_current_player( first_player ) {}

/*
 * Constructor TablegameEngine_t( Board_t board ) builds an engine with a predefined board.
 *  The current player is 0.
 */
    TablegameEngine_t::TablegameEngine_t( struct Board_t board ) :
            m_board( std::move( board ) ),
            m_current_player( 0 ) {}

/*
 * GetBoard() returns a ref-to-const board in an table game engine object.
 */
    const Board_t &TablegameEngine_t::GetBoard() const
    {
        return m_board;
    }

/*
 * GetPlayersList() returns a ref-to-const list of players from the board in an table game engine object.
 */
    const PlayersList_t &TablegameEngine_t::GetPlayersList() const
    {
        return m_board.players;
    }

/*
 * NextTurn() passes the turn to the next player. Current player index is updated.
 */
    void TablegameEngine_t::NextTurn()
    {
        int players_count = m_board.players.size();

        m_current_player++;
        m_current_player = ( m_current_player == players_count ) ? 0 : m_current_player;
    }

/*
 * findPlayer( const std::string& player_name ) returns an iterator in the players list in the board.
 *  The iterator points to a player with name player_name.
 *  .end() is returned if no player found.
 */
    auto TablegameEngine_t::findPlayer( const std::string& player_name ) const
    {
        auto& players_list = m_board.players;
        auto it = players_list.begin();

        for ( ; it != players_list.end(); it++ )
        {
            if ( it->name == player_name )
            {
                return it;
            }
        }

        return it;
    }

/*
 * GetPlayerPos(const std::string& player_name ) returns a ref-to-const position of
 *  a player with name player_name.
 *  Throws an exception if no player found.
 */
    const Pos_t& TablegameEngine_t::GetPlayerPos(const std::string &player_name ) const {
        auto player_piece = this->findPlayer( player_name );
        if ( player_piece == m_board.players.end() )
        {
            throw PlayerNotFoundException { ("Player " + player_name + " was not found." ).c_str() };
        }
        return player_piece->pos;
    }

/*
 * GetCurrentPlayer() return an player index who takes the soonest turn.
 */

    int TablegameEngine_t::GetCurrentPlayer() const {
        return m_current_player;
    }

/*
 * === Class QuoridorEngine ===
 * Represents a Quoridor table game engine.
 */

/*
 * MovePlayer( const std::string& player_name, Pos_t new_pos ) moves a piece of a player with name player
 *  to a position new_pos.
 *  Throws an exception if the move is illegal.
 */
    void QuoridorEngine_t::MovePlayer( const std::string& player_name, Pos_t new_pos )
    {
        auto &player_pos = const_cast<Pos_t &>( GetPlayerPos( player_name ) );

        if ( !this->CheckMove( player_pos, new_pos ) )
        {
            throw IllegalMoveException { ( "Moving " + player_name + " to position ("
                                           + std::to_string( new_pos.x ) + ", "
                                           + std::to_string( new_pos.y ) + ") is illegal." ).c_str() };
        }

        player_pos = new_pos;
    }

/*
 * AddWall( WallPiece_t wall ) places a new wall on the board.
 *  Throws an exception if placing the wall on the board is illegal.
 */
    void QuoridorEngine_t::AddWall( WallPiece_t wall )
    {
        if ( !this->CheckAddWall( wall ) )
        {
            using namespace std::string_literals;
            throw IllegalMoveException {( "Adding "s + ( wall.vertical ? "vertical" : "horizontal" ) +
                                         " wall to position (" + std::to_string( wall.pos.x ) + ", " +
                                         std::to_string( wall.pos.y ) + ") is illegal." ).c_str() };
        }

        m_board.walls.push_back( wall );
        m_walls_on_board.emplace( wall );
    }

/*
 * QuoridorInit( const std::vector<std::string>& player_names ) sets up the board
 *  with the player pieces specified.
 *  There can be two or four players. The position of players corresponds to his name index in the input vector.
 */
    void QuoridorEngine_t::QuoridorInit( const std::vector<std::string>& player_names )
    {
        int player_names_size = player_names.size();
        if ( player_names_size < 2 )
        {
            throw TooFewPlayersException {
                    ( "Tried to init game with too few players:" +
                    std::to_string(player_names_size) + "." ).c_str() };
        }

        player_names_size = ( player_names_size >= 4 ) ? 4 : 2;

        for ( int i = 0; i < player_names_size; i++ )
        {
            m_board.players.push_back( PlayerPiece_t { .pos = player_positions[i], .name = player_names[i] } );
        }
    }

/*
 * CheckMove( const Pos_t& cur_pos, const Pos_t& new_pos ) returns whether a player piece move is legal
 *  from position cur_pos to position new_pos.
 *  The board does not need to contain a player piece at cur_pos.
 */
    bool QuoridorEngine_t::CheckMove( const Pos_t& cur_pos, const Pos_t& new_pos ) const
    {
        // TODO redo from a "return true" perspective?

        if ( !IsPosLegal( new_pos ) )
        {
            return false;
        }

        if ( !this->IsPosFree( new_pos ) )
        {
            return false;
        }

        int8_t x_diff = new_pos.x - cur_pos.x,
                y_diff = new_pos.y - cur_pos.y,
                pos_diff = ( abs( x_diff ) + abs( y_diff ) );
        if ( pos_diff == 0 || pos_diff > 2)
        {
            return false;
        }

        // TODO add comments
        if ( this->IsWallBetweenAdjPos( cur_pos, new_pos ) )
        {
            return false;
        }

        if ( pos_diff == 1 )
        {
            return true;
        }

        /* pos_diff == 2 */

        if ( abs( y_diff ) == 2 || abs ( x_diff ) == 2 ) {
            Pos_t pos_jump_over = { static_cast<int8_t>( cur_pos.x + x_diff / 2 ),
                                    static_cast<int8_t>( cur_pos.y + y_diff / 2 ) };
            if (this->IsPosFree( pos_jump_over ) )
            {
                return false;
            }

            return ( this->IsWallBetweenAdjPos( pos_jump_over, new_pos ) );
        }

        /* diagonal move */

        Pos_t pos_jump_over = { cur_pos.x, new_pos.y };

        for ( int i = 0; i < 2; i++ ) {
            Pos_t pos_straight_jump_to = { static_cast<int8_t>( 2 * cur_pos.x - pos_jump_over.x ),
                                           static_cast<int8_t>( 2 * cur_pos.y - pos_jump_over.y ) };
            if ( !this->IsPosFree( pos_jump_over ) &&
                 this->IsWallBetweenAdjPos( pos_jump_over, pos_straight_jump_to ) &&
                 !this->IsWallBetweenAdjPos( cur_pos, pos_jump_over ) &&
                 !this->IsWallBetweenAdjPos( pos_jump_over, new_pos ) )
            {
                return true;
            }

            pos_jump_over = { new_pos.x, cur_pos.y };
        }

        return false;
    }

/*
 * CheckAddWall( const WallPiece_t& wall ) returns whether placing the new wall on the board is legal.
 */
    bool QuoridorEngine_t::CheckAddWall( const WallPiece_t& wall ) const
    {
        if ( !IsWallLegal( wall ) )
        {
            return false;
        }

        auto& x = wall.pos.x;
        auto& y = wall.pos.y;
        auto& vertical = wall.vertical;

        return ( !this->findWall( wall ) &&
                 !this->findWall( { wall.pos, !vertical } ) &&
                 !this->findWall( { { static_cast<int8_t>( x + !vertical ),
                                      static_cast<int8_t>( y +  vertical ) }, vertical} ) &&
                 !this->findWall( { { static_cast<int8_t>( x - !vertical ),
                                      static_cast<int8_t>( y -  vertical ) }, vertical} ) );
    }

/*
 * CheckWallBlocksPath() returns whether on the board with the added wall
 *  there is a path to at least one goal position for each player.
 *  Warning! CheckWallBlocksPath does not check if wall adding is possible.
 */
    bool QuoridorEngine_t::CheckWallBlocksPath( WallPiece_t wall ) const
    {
        /* pathfinder */

        const Pos_t left = { -1, 0 }, right = { 1, 0}, up = { 0, -1 }, down = { 0, 1 };
        //TODO make left, etc global?
        //TODO make these directions indexed?

        for ( int player_index = 0; player_index < m_board.players.size(); player_index++ )
        {
            auto& player_piece = m_board.players[ player_index ];
            auto& player_pos = player_piece.pos;

            std::set<Pos_t> goal_pos = GetGoalPosForPlayerIndex( player_index );
            //TODO (for future) possible to change to a lambda stopping criterion function?

            //TODO (for future) isolate "pathfinder" class
            UPTR( std::set<Pos_t> ) reachable_pos {}, last_iter_pos {};
            std::multimap<Pos_t, Pos_t> to_from_check_pairs {};

            reachable_pos->insert( player_pos );

            //TODO (for future) make a faster algorithm that tries to go "forward"
            while( true )
            {
                for ( auto& pos : *reachable_pos )
                {
                    to_from_check_pairs.insert( { pos + left, pos } );
                    to_from_check_pairs.insert( { pos + right, pos } );
                    to_from_check_pairs.insert( { pos + up, pos } );
                    to_from_check_pairs.insert( { pos + down, pos } );
                }

                while ( !last_iter_pos->empty() )
                {
                    auto pos = last_iter_pos->begin();
                    to_from_check_pairs.erase( *pos );
                    last_iter_pos->erase( pos );
                }

                last_iter_pos = std::move( reachable_pos );
                //TODO check this works as intended
                reachable_pos = std::make_unique<std::set<Pos_t>>();
                //TODO check this works as intended

                bool do_break = false;
                for( auto& pos_pair : to_from_check_pairs)
                {
                    if ( !IsWallBetweenAdjPos( pos_pair.second, pos_pair.first ) &&
                         !IsWallBetweenAdjPos( pos_pair.second, pos_pair.first, wall ))
                    {
                        if ( goal_pos.find( pos_pair.second ) != goal_pos.end() )
                        {
                            do_break = true;
                            break;
                        }
                        reachable_pos->insert( pos_pair.second );
                    }
                }

                if ( do_break )
                {
                    break;
                }

                if ( reachable_pos->empty() )
                {
                    return true;
                }

                to_from_check_pairs.clear();
            }
        }

        return false;
    }

/*
 * IsPosFree( Pos_t pos ) returns whether position pos is in bounds of the board.
 */
    bool QuoridorEngine_t::IsPosFree( Pos_t pos ) const
    {
        for ( const auto& player_piece : m_board.players )
        {
            if ( player_piece.pos == pos )
            {
                return false;
            }
        }

        return true;
    }

/*
 * GetGoalPosForPlayerIndex( int player_index ) returns a set of goal positions
 *  for a player with index player_index
 */
    std::set<Pos_t> QuoridorEngine_t::GetGoalPosForPlayerIndex( int player_index )
    {

        std::set<Pos_t> goal_pos{};

        if ( player_index < 0 || player_index > 4 )
        {
            return goal_pos;
        }

        int8_t j = 0;
        int8_t *goal_pos_x_ptr = nullptr, *goal_pos_y_ptr = nullptr;
        auto opposite_side_coordinate =
                static_cast<int8_t>( m_board_side_length - 1 - player_positions[ player_index ].x );
        if ( player_positions[ player_index ].y == 4 )
        {
            goal_pos_x_ptr = &opposite_side_coordinate;
            goal_pos_y_ptr = &j;
        }
        else
        {
            goal_pos_x_ptr = &j;
            goal_pos_y_ptr = &opposite_side_coordinate;
        }

        for ( ; j < m_board_side_length; j++ )
        {
            goal_pos.insert( { *goal_pos_x_ptr, *goal_pos_y_ptr } );
        }

        return goal_pos;
    }

/*
 * IsPosLegal( const Pos_t& pos ) returns whether position pos is in bounds of the board.
 */
    bool QuoridorEngine_t::IsPosLegal( const Pos_t& pos )
    {
        return ( pos.x >= 0 && pos.x < m_board_side_length &&
                 pos.y >= 0 && pos.y < m_board_side_length );
    }

/*
 * IsWallLegal( const WallPiece_t& wall ) returns whether the wall is in bounds of the board.
 */
    bool QuoridorEngine_t::IsWallLegal( const WallPiece_t& wall )
    {
        Pos_t pos = wall.pos;
        return ( pos.x >= 0 && pos.x < m_wall_pos_per_side &&
                 pos.y >= 0 && pos.y < m_wall_pos_per_side );
    }

/*
 * IsWallBetweenAdjPos( Pos_t a, Pos_t b ) returns whether there is a wall
 *  between two adjacent positions a and b.
 *  Warning! Use method for adjacent positions only!
 */
    bool QuoridorEngine_t::IsWallBetweenAdjPos( Pos_t a, Pos_t b ) const
    {
        assert( abs( b.x - a.x ) + abs( b.y - a.y ) == 1 );

        /*
         * a or b may be adjacent but out of bounds, e.g. x = 255 or = 9.
         * If so, there is a wall between them.
         */
        if ( !IsPosLegal( a ) || !IsPosLegal( b ) )
        {
            return true;
        }

        int8_t x_min = ( a.x < b.x ) ? a.x : b.x,
                y_min = ( a.y < b.y ) ? a.y : b.y;
        bool vertical = ( y_min != 0 );

        /* find if there is on the board a perpendicular wall in one of two positions */
        return ( findWall( { { x_min , y_min }, !vertical } ) ||
                 findWall( { { static_cast<int8_t>( x_min -  vertical ),
                               static_cast<int8_t>( y_min - !vertical ) }, !vertical } ) );
    }

/*
 * IsWallBetweenAdjPos( Pos_t a, Pos_t b,  WallPiece_t wall  ) returns whether the wall is
 *  between two adjacent positions a and b.
 *  WallPiece_t wall may be not on the board.
 *  Warning! Use IsWallBetweenAdjPos for adjacent positions only!
 */
    bool QuoridorEngine_t::IsWallBetweenAdjPos( Pos_t a, Pos_t b, WallPiece_t wall )
    {
        assert( abs( b.x - a.x ) + abs( b.y - a.y ) == 1 );

        if ( !IsPosLegal( a ) || !IsPosLegal( b ) || !IsWallLegal( wall ) )
        {
            return false;
        }

        int8_t x_min = ( a.x < b.x ) ? a.x : b.x,
                y_min = ( a.y < b.y ) ? a.y : b.y;
        bool vertical = ( y_min != 0 );

        /* wall must be perpendicular and be in one of two positions */
        return ( vertical != wall.vertical &&
                 ( wall.pos == Pos_t{ x_min , y_min } ||
                   wall.pos == Pos_t{ static_cast<int8_t>( x_min -  vertical ) ,
                                      static_cast<int8_t>( y_min - !vertical ) } ) );
    }

/*
 * findWall( WallPiece_t wall ) returns whether the wall is placed on the board.
 */
    bool QuoridorEngine_t::findWall( WallPiece_t wall ) const
    {
        return ( m_walls_on_board.find( wall ) != m_walls_on_board.end() );
    }

/*
 * IsGameOver() signals whether there is a winner player on the board.
 */
    bool QuoridorEngine_t::IsGameOver() const {
        return ( !FindWinner().empty() );
    }

/*
 * FindWinner() returns a winner name. Winner is a player satisfying a winning criterion.
 * The criterion in Quoridor is player standing in one of goal positions on the board.
 */
    std::string QuoridorEngine_t::FindWinner() const {
        for ( int player_index = 0; player_index < m_board.players.size(); player_index ++ )
        {
            auto &player_piece = m_board.players[player_index];
            auto goal_pos = GetGoalPosForPlayerIndex( player_index );
            if ( goal_pos.find( player_piece.pos ) != goal_pos.end() )
            {
                return player_piece.name;
            }
        }
        return std::string();
    }

    void QuoridorEngine_t::FinishGame() {
        //TODO what does this function do??
    }


/*
 * === Class PlayerQuoridorEngine ===
 * Represents a Quoridor engine for player class.
 */

/*
 * GetPossibleMoves( const std::string& player_name ) returns a vector of possible moves for a player
 *  with name player_name.
 */
    std::vector<Pos_t> PlayerQuoridorEngine_t::GetPossibleMoves( const std::string& player_name ) const
    {
        auto &player_pos = GetPlayerPos( player_name );
        const Pos_t left = { -1, 0 }, right = { 1, 0}, up = { 0, -1 }, down = { 0, 1 };
        Pos_t adj_moves[] = { player_pos + left, player_pos + right, player_pos + up, player_pos + down };
        std::set<Pos_t> moves_to_check {};

        for ( auto adj_move : adj_moves )
        {
            if ( this->IsPosFree( adj_move ) )
            {
                moves_to_check.insert( adj_move );
            }
            else
            {
                moves_to_check.insert( adj_move + left );
                moves_to_check.insert( adj_move + right );
                moves_to_check.insert( adj_move + up );
                moves_to_check.insert( adj_move + down );
            }
        }
        moves_to_check.erase( player_pos );

        std::vector<Pos_t> possible_moves {};
        for ( auto move_to_check : moves_to_check )
        {
            if ( CheckMove( player_pos, move_to_check) )
            {
                possible_moves.push_back( move_to_check );
            }
        }

        return possible_moves;
    }

/*
 * IsMovePossible( const std::string& player_name, Pos_t pos ) returns whether moving the player piece
 *  with name player_name to position pos on the board is possible.
 */
    bool PlayerQuoridorEngine_t::IsMovePossible( const std::string &player_name, Pos_t pos ) const
    {
        return this->CheckMove( GetPlayerPos( player_name ) , pos);
    }

/*
 * IsWallPossible( const WallPiece_t& wall ) returns whether placing the wall on the board is possible.
 */
    bool PlayerQuoridorEngine_t::IsWallPossible( const WallPiece_t& wall ) const
    {
        return this->CheckAddWall( wall );
    }

/*
 * UpdateBoard( const Board_t& board ) uploads a board to the player Quoridor engine.
 */
    void PlayerQuoridorEngine_t::UpdateBoard( const Board_t& board )
    {
        m_board = board;
    }
}
