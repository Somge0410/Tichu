#pragma once

#include "game_state.h"

#include <array>
#include <vector>

class Player {
public:
    Player(PlayerID id);
    void set_current_player(PlayerID new_id);
    void ReceiveCardLocationChangeInfo(const MoveInfo& move, Locations global_location);
    void ReceiveTichuInfo(const Tichu tichu);
    void ReceiveGrandTichuInfo(const GrandTichu grandTichu);

    MoveInfo announce_move_to_table();
    MoveInfo announce_bomb();
    CardType announce_wish() const;
    bool can_announce_tichu() const;
    bool can_announce_grand_tichu() const;
    std::array<CardStack,3> announce_change();
    bool announce_tichu();
    bool announce_grand_tichu();
    void receive_standings(const Standings new_standings);
    void receive_turn_info(PlayerID global_id);
    void receive_last_player_info(PlayerID global_id);
    void receive_hand_counts(const std::array<int, 4>& counts);
    void receive_wish_info(std::optional<CardType> wish);
    bool has_cards_in_hand() const;
    int how_many_cards_in_hand() const;
    bool has_cards_left() const;
    void receive_first_finisher(PlayerID global_id);
    PlayerID where_give_dragon_stack();

private:
    PlayerID id = 0;
    GameKnowlege gameKnowlege;
    CardStack hand = 0;
    void remove_cards_from_hand(const CardStack stack);
    void add_cards_to_hand(const CardStack stack);
    void ensure_moves_generated();
    int hand_strength() const;
    PlayerID convert_global_to_local(PlayerID global_id);
    PlayerID convert_local_to_global(PlayerID local_id);
    Locations convert_global_to_local(Locations global_location);
    Locations convert_local_to_global(Locations local_location);
    Standings convert_global_to_local(Standings global_standings);
    Points convert_global_to_local(Points global_points);
    GrandTichu convert_global_to_local(GrandTichu global_grand_tichu);
	bool moves_already_generated = false;
    bool has_played = false;
	std::vector<MoveInfo> generated_moves;


};
