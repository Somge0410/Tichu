#pragma once
#include "types.h"
#include <algorithm>
#include <random>
#include <stdexcept>
#include <vector>

class GameKnowlege {
public:
    GameKnowlege() {
        clear();
    }
    void give_cards_to_Location(CardStack stack, Locations location) {
        while (stack) {
            int cardIndex = pop_lsb(stack);
            cardLocations[cardIndex] =location;
            points[static_cast<int>(location)] += get_Points(static_cast<Cards>(cardIndex));
        }
    }
    void give_new_standings(const Standings new_standings) {
        standings = new_standings;
    }
    void clear() {
        cardLocations.fill(Locations::Unkown);
        standings = { 0, 0 };
        points.fill(0);
        tichu.fill(false);
        grandTichu.fill(false);
    }
    void set_tichu(const Tichu new_tichu) {
        tichu = new_tichu;
    }
    void set_grand_tichu(const GrandTichu new_grand_tichu) {
		grandTichu = new_grand_tichu;
    }
    void set_tichu(bool value, PlayerID player) {
        tichu[player] = value;
    }
    void set_grand_tichu(bool value,PlayerID player) {
        grandTichu[player] = value;
    }
    void set_player_turn(PlayerID new_turn) {
        turn = new_turn;
	}
    void set_last_player(PlayerID new_last_player) {
        player_who_played_cards_on_table = new_last_player;
	}
    void set_points(const Points new_points) {
        points = new_points;
	}
    CardStack random_unkown_cards(std::size_t count, std::mt19937& rng) const {
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
    }
    Standings get_standings() const {
        return standings;
	}
    GrandTichu get_grand_tichu() const {
        return grandTichu;
    }
    Tichu get_tichu() const {
		return tichu;
    }
    CardLocations get_card_locations() const {
        return cardLocations;
	}
private:
    CardLocations cardLocations={};
    Standings standings = { 0, 0 };
    Points points = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    Tichu tichu = { false, false, false, false };
    GrandTichu  grandTichu = { false, false, false, false };
    PlayerID turn;
	PlayerID player_who_played_cards_on_table;


};
class Player {
public:
    Player(PlayerID id) : id(id) {}
    void set_current_player(PlayerID id) {
        id = id;
        gameKnowlege.clear();
        CardStack hand = 0;
    }
    void ReceiveCardLocationChangeInfo(const CardStack stack, Locations global_location) {
		Locations local_location = convert_global_to_local(global_location);
        gameKnowlege.give_cards_to_Location(stack, local_location);
        if (Locations::P1Hand == local_location) {
            add_cards_to_hand(stack);
        }

    }
    void ReceiveTichuInfo(const Tichu tichu) {
		Tichu local_tichu = convert_global_to_local(tichu);
        gameKnowlege.set_tichu(local_tichu);
    }
    void ReceiveGrandTichuInfo(const GrandTichu grandTichu) {
        GrandTichu local_grand_tichu = convert_global_to_local(grandTichu);
        gameKnowlege.set_grand_tichu(local_grand_tichu);
	}

    CardStack announce_move_to_table() {
        remove_cards_from_hand(0); //TODO
        return 0; //TODO
    }
    std::array<CardStack,3> announce_change() {
        remove_cards_from_hand(0); //TODO
        return {0,0,0}; //TODO
    }
    bool announce_tichu() {
        return false; //TODO
    }
    bool announce_grand_tichu() {
        return false; //TODO
    }
    void receive_standings(const Standings new_standings) {
		Standings local_standings = convert_global_to_local(new_standings);
        gameKnowlege.give_new_standings(local_standings);
	}
    void receive_turn_info(PlayerID global_id) {
        PlayerID local_id = convert_global_to_local(global_id);
		gameKnowlege.set_player_turn(local_id);
    }
    void receive_last_player_info(PlayerID global_id) {
        PlayerID local_id = convert_global_to_local(global_id);
        gameKnowlege.set_last_player(local_id);
    }

private:
    PlayerID id = 0;
    GameKnowlege gameKnowlege;
    CardStack hand = 0;
    void remove_cards_from_hand(const CardStack stack) {
        hand &= ~stack;
    }
    void add_cards_to_hand(const CardStack stack) {
        hand |= stack;
    }
    PlayerID convert_global_to_local(PlayerID global_id) {
        return (global_id - id + 4) % 4;
	}
    PlayerID convert_local_to_global(PlayerID local_id) {
        return (local_id + id) % 4;
	}
    Locations convert_global_to_local(Locations global_location) {
        if (global_location == Locations::P1Hand) {
            return static_cast<Locations>((static_cast<int>(Locations::P1Hand) + id) % 4);
        }
        else if (global_location == Locations::P2Hand) {
            return static_cast<Locations>((static_cast<int>(Locations::P2Hand) + id) % 4);
        }
        else if (global_location == Locations::P3Hand) {
            return static_cast<Locations>((static_cast<int>(Locations::P3Hand) + id) % 4);
        }
        else if (global_location == Locations::P4Hand) {
            return static_cast<Locations>((static_cast<int>(Locations::P4Hand) + id) % 4);
        }
        else if (global_location == Locations::Table) {
            return Locations::Table;
        }
        else if (global_location == Locations::P1Bank) {
            return static_cast<Locations>((static_cast<int>(Locations::P1Bank) + id) % 4);
        }
        else if (global_location == Locations::P2Bank) {
            return static_cast<Locations>((static_cast<int>(Locations::P2Bank) + id) % 4);
        }
        else if (global_location == Locations::P3Bank) {
            return static_cast<Locations>((static_cast<int>(Locations::P3Bank) + id) % 4);
        }
        else if (global_location == Locations::P4Bank) {
            return static_cast<Locations>((static_cast<int>(Locations::P4Bank) + id) % 4);
        }
    }
    Locations convert_local_to_global(Locations local_location) {
        if (local_location == Locations::P1Hand) {
            return static_cast<Locations>((static_cast<int>(Locations::P1Hand) - id + 4) % 4);
        }
        else if (local_location == Locations::P2Hand) {
            return static_cast<Locations>((static_cast<int>(Locations::P2Hand) - id + 4) % 4);
        }
        else if (local_location == Locations::P3Hand) {
            return static_cast<Locations>((static_cast<int>(Locations::P3Hand) - id + 4) % 4);
        }
        else if (local_location == Locations::P4Hand) {
            return static_cast<Locations>((static_cast<int>(Locations::P4Hand) - id + 4) % 4);
        }
        else if (local_location == Locations::Table) {
            return Locations::Table;
        }
        else if (local_location == Locations::P1Bank) {
            return static_cast<Locations>((static_cast<int>(Locations::P1Bank) - id + 4) % 4);
        }
        else if (local_location == Locations::P2Bank) {
            return static_cast<Locations>((static_cast<int>(Locations::P2Bank) - id + 4) % 4);
        }
        else if (local_location == Locations::P3Bank) {
            return static_cast<Locations>((static_cast<int>(Locations::P3Bank) - id + 4) % 4);
        }
        else if (local_location == Locations::P4Bank) {
            return static_cast<Locations>((static_cast<int>(Locations::P4Bank) - id + 4) % 4);
        }
    }
    Standings convert_global_to_local(Standings global_standings) {
        Standings local_standings;
        for (int i = 0; i < 2; ++i) {
            local_standings[i] = global_standings[(i + id) % 2];
        }
        return local_standings;
    }
    Points convert_global_to_local(Points global_points) {
        Points local_points;
        for (int i = 0; i < 10; ++i) {
            local_points[i] = global_points[(i + id) % 10];
        }
        return local_points;
	}
    GrandTichu convert_global_to_local(GrandTichu global_grand_tichu) {
        GrandTichu local_grand_tichu;
        for (int i = 0; i < 4; ++i) {
            local_grand_tichu[i] = global_grand_tichu[(i + id) % 4];
        }
        return local_grand_tichu;
	}
};
class Game {
public:
    void play_game();
private: 
    void play_round() {
        gameKnowledge.clear();
        for (int id = 0; id < 4; ++id) {
            give_random_cards_to_player(id, 8);
            gameKnowledge.set_grand_tichu(id, request_player_for_grand_tichu(id));
        }
        for (int id = 0; id < 4; ++id) {
            notify_player_of_grand_tichu(id);
        }
        for (int id = 0; id < 4; ++id) {
            give_random_cards_to_player(id, 4);
            gameKnowledge.set_tichu(id, request_player_for_tichu(id));
        }
        for (int id = 0; id < 4; ++id) {
            notify_player_of_tichu(id);
        }
        std::array<std::array<CardStack, 3>, 4> changes;
        for (int id = 0; id < 4; ++id) {
            changes[id] = request_player_for_change(id);
        }
        for (int from = 0; from < 4; ++from) {
            for (int to = 0; to < 3; ++to) {
                PlayerID target_id = (from + 2 * to - 1) % 4;
                notify_player_of_card_change(target_id, changes[from][to], static_cast<Locations>(static_cast<int>(Locations::P1Hand) + target_id));
            }
        }
    }
	std::array<Player, 4> players = { Player(0), Player(1), Player(2), Player(3) };
	GameKnowlege gameKnowledge;
    void notify_player_of_card_change(PlayerID id, CardStack stack, Locations global_location) {
        players[id].ReceiveCardLocationChangeInfo(stack, global_location);
		gameKnowledge.give_cards_to_Location(stack, global_location);
	}
    void notify_player_of_turn(PlayerID id) {
		players[id].receive_turn_info(turn);
    }
    void notify_player_of_last_player(PlayerID id) {
        players[id].receive_last_player_info(player_who_played_cards_on_table);
	}
    void notify_player_of_points(PlayerID id) {
        players[id].receive_standings(gameKnowledge.get_standings());
	}
    void notify_player_of_grand_tichu(PlayerID id) {
        players[id].ReceiveTichuInfo(gameKnowledge.get_grand_tichu());
    }
    void notify_player_of_tichu(PlayerID id) {
		players[id].ReceiveTichuInfo(gameKnowledge.get_tichu());
    }
    bool request_player_for_tichu(PlayerID id) {
        return players[id].announce_tichu();
    }
    bool request_player_for_grand_tichu(PlayerID id) {
        return players[id].announce_grand_tichu();
	}
    bool request_player_for_move(PlayerID id) {
        CardStack move = players[id].announce_move_to_table();
        // Process the move and update game state
        return true; // Return true if the move was valid, false otherwise
	}
    std::array<CardStack, 3> request_player_for_change(PlayerID id) {
        std::array<CardStack, 3> change = players[id].announce_change();
        // Process the change and update game state
        return change; // Return the change
	}
	std::mt19937 rng{std::random_device{}()};
    void give_random_cards_to_player(PlayerID id, size_t count) {
        CardStack stack = gameKnowledge.random_unkown_cards(count, rng);
        notify_player_of_card_change(id, stack, static_cast<Locations>(static_cast<int>(Locations::P1Hand) + id));
	}
    PlayerID determine_starting_player() {
		return gameKnowledge.get_card_locations()[static_cast<int>(Cards::Mahjong)];
    }
    PlayerID determine_next_player(PlayerID current_player) {
        return (current_player + 1) % 4;
	}
};
