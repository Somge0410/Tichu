#include "game_state.h"

#include "cards.h"

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <vector>

GameKnowlege::GameKnowlege() {
    clear();
}

void GameKnowlege::give_new_standings(const Standings new_standings) {
    standings = new_standings;
}

void GameKnowlege::clear() {
    cardLocations.fill(Locations::Unkown);
    points.fill(0);
    tichu.fill(false);
    grandTichu.fill(false);
    cards_in_hands.fill(0);
    wish.reset();
    first_finisher = -1;
    turn = 0;
    player_who_played_cards_on_table = -1;
    table_face = {};
}

void GameKnowlege::set_tichu(const Tichu new_tichu) {
    tichu = new_tichu;
}

void GameKnowlege::set_grand_tichu(const GrandTichu new_grand_tichu) {
    grandTichu = new_grand_tichu;
}

void GameKnowlege::set_last_player(PlayerID new_last_player) {
    player_who_played_cards_on_table = new_last_player;
}

void GameKnowlege::set_points(const Points new_points) {
    points = new_points;
}

CardStack GameKnowlege::random_unkown_cards(std::size_t count, std::mt19937& rng) const {
    std::vector<int> unkown_cards;
    for (int i = 0; i < static_cast<int>(Cards::Count); ++i) {
        if(cardLocations[i] == Locations::Unkown) {
            unkown_cards.push_back(i);
        }
    }
    if (count > unkown_cards.size()) {
        throw std::invalid_argument("Not enough unknown cards to select from.");
    }
    std::shuffle(unkown_cards.begin(), unkown_cards.end(), rng);
    CardStack result = 0;
    for(std::size_t i = 0; i < count; ++i) {
        result |= (CardStack{ 1 } << unkown_cards[i]);
    }
    return result;
}

Standings GameKnowlege::get_standings() const {
    return standings;
}

GrandTichu GameKnowlege::get_grand_tichu() const {
    return grandTichu;
}

Tichu GameKnowlege::get_tichu() const {
    return tichu;
}

CardLocations GameKnowlege::get_card_locations() const {
    return cardLocations;
}

PlayerID GameKnowlege::get_turn() const {
    return turn;
}

PlayerID GameKnowlege::get_last_player() const {
    return player_who_played_cards_on_table;
}

std::array<int,4> GameKnowlege::get_cards_in_hands() const {
    return cards_in_hands;
}

void GameKnowlege::set_cards_in_hands(const std::array<int, 4>& counts) {
    cards_in_hands = counts;
}

std::optional<CardType> GameKnowlege::get_wish() const {
    return wish;
}

void GameKnowlege::set_wish(std::optional<CardType> new_wish) {
    wish = new_wish;
}

CardStack GameKnowlege::get_cards_in_Location(Locations location) const {
    CardStack result = 0;
    for (int i = 0; i < static_cast<int>(Cards::Count); ++i) {
        if (cardLocations[i] == location) {
            result |= (CardStack{ 1 } << i);
        }
    }
    return result;
}

PlayerID GameKnowlege::get_first_finisher() const {
    return first_finisher;
}

void GameKnowlege::add_Points_to_team(int team, int points_to_add) {
    if (team < 0 || team >= 2) {
        throw std::out_of_range("Team index out of range. Must be 0 or 1.");
    }
    standings[team] += points_to_add;

}

int GameKnowlege::get_points_in_Location(Locations location) const {
    return points[static_cast<int>(location)];
}

const MoveInfo& GameKnowlege::get_table_face() const {
    return table_face;
}

template<bool with_log>
void GameKnowlege::give_cards_to_Location(const MoveInfo& move, Locations location) {
    if (location == Locations::Table) {
        table_face = move;
    }
    if(location == Locations::P0Bank || location == Locations::P1Bank || location == Locations::P2Bank || location == Locations::P3Bank) {
        table_face = {};
    }
    CardStack cards = move.cards & CARD_BITS_MASK;
    if constexpr (with_log) {
        std::cout << "Giving " << " cards to " << get_location_name(location) << std::endl;
        std::cout << "Cards: ";
        if (location == Locations::Table) {
            print(move);
        }
        else {
            print(cards);
        }
        std::cout << std::endl;
    }
    while (cards) {
        int cardIndex = pop_lsb(cards);
        if (cardIndex < 0 || cardIndex >= static_cast<int>(Cards::Count)) {
            throw std::out_of_range("Card stack contains an invalid card index.");
        }
        const int card_points = get_Points(static_cast<Cards>(cardIndex));
        const int previous_location = static_cast<int>(cardLocations[cardIndex]);
        if (cardLocations[cardIndex] != Locations::Unkown) {
            points[previous_location] -= card_points;
        }
        cardLocations[cardIndex] = location;
        points[static_cast<int>(location)] += card_points;
    }
}

template <bool with_log>
void GameKnowlege::set_tichu(bool value, PlayerID player) {
    if constexpr (with_log) {
        if (value) {
            std::cout << "Player " << static_cast<int>(player) << " called Tichu! " << std::endl;
        }
    }
    tichu[player] = value;
}

template<bool with_log>
void GameKnowlege::set_grand_tichu(bool value,PlayerID player) {
    if constexpr (with_log) {
        if (value) {
            std::cout << "Player " << static_cast<int>(player) << " called Grand -Tichu! " << std::endl;
        }
    }
    grandTichu[player] = value;
}

template<bool with_log>
void GameKnowlege::set_player_turn(PlayerID new_turn) {
    if constexpr (with_log) {
        std::cout << "It's now Player " << static_cast<int>(new_turn) << "'s turn." << std::endl;
    }
    turn = new_turn;
}

template<bool with_log>
void GameKnowlege::set_first_finisher(PlayerID player) {
    first_finisher = player;
}

// Instantiate both logging variants for callers in other source files.
template void GameKnowlege::give_cards_to_Location<false>(const MoveInfo&, Locations);
template void GameKnowlege::give_cards_to_Location<true>(const MoveInfo&, Locations);
template void GameKnowlege::set_tichu<false>(bool, PlayerID);
template void GameKnowlege::set_tichu<true>(bool, PlayerID);
template void GameKnowlege::set_grand_tichu<false>(bool, PlayerID);
template void GameKnowlege::set_grand_tichu<true>(bool, PlayerID);
template void GameKnowlege::set_player_turn<false>(PlayerID);
template void GameKnowlege::set_player_turn<true>(PlayerID);
template void GameKnowlege::set_first_finisher<false>(PlayerID);
template void GameKnowlege::set_first_finisher<true>(PlayerID);
