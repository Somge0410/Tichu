#pragma once

#include "types.h"

#include <array>
#include <cstddef>
#include <optional>
#include <random>

class GameKnowlege {
public:
    GameKnowlege();
    template<bool with_log=false> void give_cards_to_Location(const MoveInfo& move, Locations location);
    void give_new_standings(const Standings new_standings);
    void clear();
    void set_tichu(const Tichu new_tichu);
    void set_grand_tichu(const GrandTichu new_grand_tichu);
    template <bool with_log=false> void set_tichu(bool value, PlayerID player);
    template<bool with_log=false> void set_grand_tichu(bool value,PlayerID player);
    template<bool with_log=false> void set_player_turn(PlayerID new_turn);
    void set_last_player(PlayerID new_last_player);
    void set_points(const Points new_points);
    CardStack random_unkown_cards(std::size_t count, std::mt19937& rng) const;
    Standings get_standings() const;
    GrandTichu get_grand_tichu() const;
    Tichu get_tichu() const;
    CardLocations get_card_locations() const;
    PlayerID get_turn() const;
    PlayerID get_last_player() const;
    std::array<int,4> get_cards_in_hands() const;
    void set_cards_in_hands(const std::array<int, 4>& counts);
    std::optional<CardType> get_wish() const;
    void set_wish(std::optional<CardType> new_wish);
    CardStack get_cards_in_Location(Locations location) const;
    PlayerID get_first_finisher() const;
    template<bool with_log=false>void set_first_finisher(PlayerID player);
    void add_Points_to_team(int team, int points_to_add);
    int get_points_in_Location(Locations location) const;
    const MoveInfo& get_table_face() const;
private:
    CardLocations cardLocations={};
    Standings standings = { 0, 0 };
    Points points = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    Tichu tichu = { false, false, false, false };
    GrandTichu  grandTichu = { false, false, false, false };
    PlayerID turn;
	PlayerID player_who_played_cards_on_table;
    std::array<int, 4> cards_in_hands = {};
    std::optional<CardType> wish;
    PlayerID first_finisher = -1;
    MoveInfo table_face;

};
