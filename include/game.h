#pragma once

#include "game_state.h"
#include "player.h"

#include <array>
#include <cstddef>
#include <random>

class Game {
public:
    Game() = default;
    explicit Game(std::uint32_t seed);
    const GameKnowlege& get_knowledge() const;
    template< bool with_log> void play_game(int limit);
private:
    template< bool with_log=false> void play_round();
    std::array<Player, 4> players = { Player(0), Player(1), Player(2), Player(3) };
    GameKnowlege gameKnowledge;
    void notify_player_of_card_change(PlayerID id, const MoveInfo& move, Locations global_location);
    void notify_all_players_of_card_change(const MoveInfo& move, Locations global_location);
    void notify_player_of_turn(PlayerID id);
    void notify_all_players_of_turn();
    void notify_player_of_last_player(PlayerID id);
    void notify_all_players_of_last_player();
    void notify_player_of_points(PlayerID id);
    void notify_player_of_grand_tichu(PlayerID id);
	void notify_all_player_of_grand_tichu();
    void notify_player_of_tichu(PlayerID id);
    void notify_all_player_of_tichu();
    void notify_player_of_first_finisher(PlayerID id);
    void notify_all_players_of_first_finisher();
    void notify_player_of_standings(PlayerID id);
    void notify_all_players_of_standings();
    void notify_all_players_of_hand_counts();
    void notify_all_players_of_wish();
    template<bool with_log> void apply_move(PlayerID player, const MoveInfo& move);
    template<bool with_log> void collect_table();
    template<bool with_log> void check_tichu(PlayerID player);
    bool request_player_for_tichu(PlayerID id);
    bool request_player_for_grand_tichu(PlayerID id);
    MoveInfo request_player_for_move(PlayerID id);
    MoveInfo request_player_for_bomb(PlayerID id);
    std::array<CardStack, 3> request_player_for_change(PlayerID id);
    std::mt19937 rng{ std::random_device{}() };
    template <bool with_log=false> void give_cards_to_player(PlayerID id, CardStack cards);
    PlayerID determine_starting_player();
    PlayerID determine_next_player(PlayerID current_player);
    PlayerID determine_next_player_after_dog(PlayerID current_player);
    bool round_ongoing();
    bool is_double_win();
    PlayerID determine_last();
    template<bool with_logs=false>void finish_round();
};
