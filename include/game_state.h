#pragma once
#include "types.h"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <stdexcept>
#include <vector>
#include "move_generation.h"
class GameKnowlege {
public:
    GameKnowlege() {
        clear();
    }
    void give_cards_to_Location(const MoveInfo& move, Locations location) {
        if (location == Locations::Table) {
            table_face = move;
        }
        if(location == Locations::P1Bank || location == Locations::P2Bank || location == Locations::P3Bank || location == Locations::P4Bank) {
            table_face = {};
        }
        CardStack cards = move.cards & ((CardStack{ 1 } << static_cast<int>(Cards::Count)) - 1);
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
    void give_new_standings(const Standings new_standings) {
        standings = new_standings;
    }
    void clear() {
        cardLocations.fill(Locations::Unkown);
        points.fill(0);
        tichu.fill(false);
        grandTichu.fill(false);
		cards_in_hands.fill(0);
		first_finisher = -1;
        turn = 0;
        player_who_played_cards_on_table = -1;
        table_face = {};
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
        return result;
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
    PlayerID get_turn() const {
        return turn;
	}
    PlayerID get_last_player() const {
        return player_who_played_cards_on_table;
    }
    std::array<int,4> get_cards_in_hands() const {
        return cards_in_hands;
	}
    CardStack get_cards_in_Location(Locations location) const {
        CardStack result = 0;
        for (int i = 0; i < static_cast<int>(Cards::Count); ++i) {
            if (cardLocations[i] == location) {
                result |= (CardStack{ 1 } << i);
            }
        }
        return result;
	}
    PlayerID get_first_finisher() const {
        return first_finisher;
	}
    void set_first_finisher(PlayerID player) {
        first_finisher = player;
	}
    void add_Points_to_team(int team, int points_to_add) {
        if (team < 0 || team >= 2) {
            throw std::out_of_range("Team index out of range. Must be 0 or 1.");
        }
        standings[team] += points_to_add;
	}
    int get_points_in_Location(Locations location) const {
		return points[static_cast<int>(location)];
    }
    const MoveInfo& get_table_face() const {
        return table_face;
    }
private:
    CardLocations cardLocations={};
    Standings standings = { 0, 0 };
    Points points = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    Tichu tichu = { false, false, false, false };
    GrandTichu  grandTichu = { false, false, false, false };
    PlayerID turn;
	PlayerID player_who_played_cards_on_table;
	std::array<int, 4> cards_in_hands = { 0, 0, 0, 0 };
    PlayerID first_finisher = -1;
    MoveInfo table_face;

};
class Player {
public:
    Player(PlayerID id) : id(id) {}
    void set_current_player(PlayerID new_id) {
        id = new_id;
        gameKnowlege.clear();
        hand = 0;
        generated_moves.clear();
        moves_already_generated = false;
    }
    void ReceiveCardLocationChangeInfo(const MoveInfo& move, Locations global_location) {
		Locations local_location = convert_global_to_local(global_location);
        gameKnowlege.give_cards_to_Location(move, local_location);
        if (Locations::P1Hand == local_location) {
            add_cards_to_hand(move.cards);
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

    MoveInfo announce_move_to_table() {
        if (hand == 0) return {};
        const MoveInfo& table_move = gameKnowlege.get_table_face();
        // TODO: Implement a better strategy for selecting a move
        for (auto it = generated_moves.rbegin(); it != generated_moves.rend(); ++it) {
            const MoveInfo move = *it;
            if (table_move.length == 0 ||
                (move.type == MoveType::Bomb &&
                 (table_move.type != MoveType::Bomb || move.length > table_move.length)) ||
                (move.type == table_move.type && move.length == table_move.length &&
                 move.beat_value > table_move.beat_value)) {
                remove_cards_from_hand(move.cards);
                return move;
            }
        }
        return {};
    }
    std::array<CardStack,3> announce_change() {
        CardStack hand_copy = hand;
		std::array<CardStack, 3> change = { 0,0,0 };
        CardStack change_stack = 0;
        for(int i=0; i<3; ++i) {
            int lsb = pop_lsb(hand_copy);
            if (lsb < 0 || lsb >= static_cast<int>(Cards::Count)) {
                throw std::logic_error("Player does not have three valid cards to exchange.");
            }
            change[i] = (CardStack{ 1 } << lsb);
			change_stack |= change[i];
		}//TODO
        remove_cards_from_hand(change_stack);
        return change;
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
        if (!moves_already_generated) {
            generated_moves = generate_moves(hand);
            moves_already_generated = true;
        }
    }
    void receive_last_player_info(PlayerID global_id) {
        PlayerID local_id = convert_global_to_local(global_id);
        gameKnowlege.set_last_player(local_id);
    }
    bool has_cards_in_hand() const {
        return hand != 0;
	}
    int how_many_cards_in_hand() const {
        return popcount(hand);
	}
    bool has_cards_left() const {
        return hand != 0;
	}
    void receive_first_finisher(PlayerID global_id) {
        PlayerID local_id = convert_global_to_local(global_id);
        gameKnowlege.set_first_finisher(local_id);
	}

private:
    PlayerID id = 0;
    GameKnowlege gameKnowlege;
    CardStack hand = 0;
    void remove_cards_from_hand(const CardStack stack) {
        const CardStack cards = stack & ~LAST_FOUR_BITS_MASK;
        hand &= ~cards;
        generated_moves.erase(
            std::remove_if(generated_moves.begin(), generated_moves.end(),
                [cards](const MoveInfo& move) { return (move.cards & cards) != 0; }),
            generated_moves.end());
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
        const int location = static_cast<int>(global_location);
        const int player_id = static_cast<int>(id);
        const int first_hand = static_cast<int>(Locations::P1Hand);
        const int first_bank = static_cast<int>(Locations::P1Bank);

        if (location >= first_hand && location <= static_cast<int>(Locations::P4Hand)) {
            return static_cast<Locations>(first_hand + (location - first_hand - player_id + 4) % 4);
        }
        if (location >= first_bank && location <= static_cast<int>(Locations::P4Bank)) {
            return static_cast<Locations>(first_bank + (location - first_bank - player_id + 4) % 4);
        }
        return global_location;
    }
    Locations convert_local_to_global(Locations local_location) {
        const int location = static_cast<int>(local_location);
        const int player_id = static_cast<int>(id);
        const int first_hand = static_cast<int>(Locations::P1Hand);
        const int first_bank = static_cast<int>(Locations::P1Bank);

        if (location >= first_hand && location <= static_cast<int>(Locations::P4Hand)) {
            return static_cast<Locations>(first_hand + (location - first_hand + player_id) % 4);
        }
        if (location >= first_bank && location <= static_cast<int>(Locations::P4Bank)) {
            return static_cast<Locations>(first_bank + (location - first_bank + player_id) % 4);
        }
        return local_location;
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
	bool moves_already_generated = false;
	std::vector<MoveInfo> generated_moves;


};
class Game {
public:
    void play_game(int limit) {
        const auto start_time = std::chrono::steady_clock::now();
		Standings standings = gameKnowledge.get_standings();
        while ((standings[0]<limit && standings[1]<limit) || standings[0]==standings[1]) {
            play_round();
            standings = gameKnowledge.get_standings();
        }
        if (standings[0] > standings[1]) {
			std::cout << "Team 1 wins with " << standings[0] << ":" << standings[1] << " points!" << std::endl;
        }
        else {
			std::cout << "Team 2 wins with " << standings[1] << ":" << standings[0] << " points!" << std::endl;
        }
        const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - start_time).count();
        std::cout << "Game duration: " << elapsed << " ms" << std::endl;
        return;
    }
private:
    void play_round() {
        gameKnowledge.clear();
        for (PlayerID id = 0; id < 4; ++id) {
            players[id].set_current_player(id);
        }
        for (int id = 0; id < 4; ++id) {
            give_random_cards_to_player(id, 8);
            gameKnowledge.set_grand_tichu(request_player_for_grand_tichu(id), id);
        }
        notify_all_player_of_tichu();

        for (int id = 0; id < 4; ++id) {
            give_random_cards_to_player(id, 6);
            gameKnowledge.set_tichu(request_player_for_tichu(id), id);
        }
        notify_all_player_of_tichu();

        std::array<std::array<CardStack, 3>, 4> changes;
        for (int id = 0; id < 4; ++id) {
            changes[id] = request_player_for_change(id);
        }
        for (int from = 0; from < 4; ++from) {
            for (int to = 0; to < 3; ++to) {
                PlayerID target_id = (from + 3- to) % 4;
                Locations target_location = static_cast<Locations>(static_cast<int>(Locations::P1Hand) + target_id);
                gameKnowledge.give_cards_to_Location({ changes[from][to] }, target_location);
                notify_player_of_card_change(target_id, { changes[from][to] }, target_location);
                notify_player_of_card_change(from, { changes[from][to] }, target_location);
            }
        }
        gameKnowledge.set_player_turn(determine_starting_player());
        int pass_count = 0;
        while (round_ongoing()) {
            PlayerID current_player = gameKnowledge.get_turn();
            notify_all_players_of_turn();
            if (const MoveInfo move = request_player_for_move(current_player); move.cards != 0) {
                pass_count = 0;
                if (!players[current_player].has_cards_in_hand() && gameKnowledge.get_first_finisher() == -1) {
                    gameKnowledge.set_first_finisher(current_player);
                    notify_all_players_of_first_finisher();
                }
                gameKnowledge.set_last_player(current_player);
                gameKnowledge.give_cards_to_Location(move, Locations::Table);
                notify_all_players_of_card_change(move, Locations::Table);
                for (int i = 0; i < 3; ++i) {
                    if (request_player_for_tichu((current_player + i + 1) % 4)) {
                        gameKnowledge.set_tichu(true, (current_player + i + 1) % 4);
                        notify_all_player_of_tichu();
                    }
                    if (const MoveInfo bomb = request_player_for_bomb((current_player + i + 1) % 4); bomb.cards != 0) {
                        gameKnowledge.give_cards_to_Location(bomb, Locations::Table);
                        gameKnowledge.set_last_player((current_player + i + 1) % 4);
                        gameKnowledge.set_player_turn((current_player + i + 2) % 4);
                        notify_all_players_of_card_change(bomb, Locations::Table);
                        notify_all_players_of_last_player();
                        notify_all_players_of_turn();
                        int i = 0;
                        current_player = gameKnowledge.get_last_player();
                    }
                }
            }
            else {
                ++pass_count;
            }
            gameKnowledge.set_player_turn(determine_next_player(current_player));
            notify_all_players_of_turn();

            if (pass_count >= 3) {
                CardStack table_cards = gameKnowledge.get_cards_in_Location(Locations::Table);
                Locations last_player_location = static_cast<Locations>(static_cast<int>(Locations::P1Bank) + gameKnowledge.get_last_player());
                gameKnowledge.give_cards_to_Location({ table_cards }, last_player_location);
                notify_all_players_of_card_change({ table_cards }, last_player_location);
                pass_count = 0;
            }
        }
        finish_round();
        return;
    }
    std::array<Player, 4> players = { Player(0), Player(1), Player(2), Player(3) };
    GameKnowlege gameKnowledge;
    void notify_player_of_card_change(PlayerID id, const MoveInfo& move, Locations global_location) {
        players[id].ReceiveCardLocationChangeInfo(move, global_location);
    }
    void notify_all_players_of_card_change(const MoveInfo& move, Locations global_location) {
        for (int id = 0; id < 4; ++id) {
            notify_player_of_card_change(id, move, global_location);
        }
    }
    void notify_player_of_turn(PlayerID id) {
        players[id].receive_turn_info(gameKnowledge.get_turn());
    }
    void notify_all_players_of_turn() {
        for (int id = 0; id < 4; ++id) {
            notify_player_of_turn(id);
        }
    }
    void notify_player_of_last_player(PlayerID id) {
        players[id].receive_last_player_info(gameKnowledge.get_last_player());
    }
    void notify_all_players_of_last_player() {
        for (int id = 0; id < 4; ++id) {
            notify_player_of_last_player(id);
        }
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
    void notify_all_player_of_tichu() {
        for (int id = 0; id < 4; ++id) {
            notify_player_of_tichu(id);
        }
    }
    void notify_player_of_first_finisher(PlayerID id) {
        players[id].receive_first_finisher(gameKnowledge.get_first_finisher());
    }
    void notify_all_players_of_first_finisher() {
        for (int id = 0; id < 4; ++id) {
            notify_player_of_first_finisher(id);
        }
    }
    void notify_player_of_standings(PlayerID id) {
        players[id].receive_standings(gameKnowledge.get_standings());
	}
    void notify_all_players_of_standings() {
        for (int id = 0; id < 4; ++id) {
            notify_player_of_standings(id);
        }
	}
    bool request_player_for_tichu(PlayerID id) {
        return players[id].announce_tichu();
    }
    bool request_player_for_grand_tichu(PlayerID id) {
        return players[id].announce_grand_tichu();
    }
    MoveInfo request_player_for_move(PlayerID id) {
        return players[id].announce_move_to_table();
    }
    MoveInfo request_player_for_bomb(PlayerID id) {
        // Implement logic to request a bomb from the player
        return {};
    }
    std::array<CardStack, 3> request_player_for_change(PlayerID id) {
        std::array<CardStack, 3> change = players[id].announce_change();
        // Process the change and update game state
        return change; // Return the change
    }
    std::mt19937 rng{ std::random_device{}() };
    void give_random_cards_to_player(PlayerID id, size_t count) {
        CardStack stack = gameKnowledge.random_unkown_cards(count, rng);
        Locations location = static_cast<Locations>(static_cast<int>(Locations::P1Hand) + id);
        gameKnowledge.give_cards_to_Location({ stack }, location);
        notify_player_of_card_change(id, { stack }, location);
    }
    PlayerID determine_starting_player() {
        Locations mahjong_location = gameKnowledge.get_card_locations()[static_cast<int>(Cards::Mahjong)];
        if (mahjong_location == Locations::P1Hand) {
            return static_cast<PlayerID>(0);
        }
        else if (mahjong_location == Locations::P2Hand) {
            return static_cast<PlayerID>(1);
        }
        else if (mahjong_location == Locations::P3Hand) {
            return static_cast<PlayerID>(2);
        }
        else if (mahjong_location == Locations::P4Hand) {
            return static_cast<PlayerID>(3);
        }
        else {
            throw std::runtime_error("Mahjong card not found in any player's hand.");
        }
    }
    PlayerID determine_next_player(PlayerID current_player) {
        return static_cast<PlayerID>((current_player + 1) % 4);
    }
    bool round_ongoing() {
        bool team1 = players[0].has_cards_in_hand() || players[2].has_cards_in_hand();
        bool team2 = players[1].has_cards_in_hand() || players[3].has_cards_in_hand();
        return team1 && team2;
    }
    bool is_double_win() {
        bool team1 = players[0].has_cards_in_hand() && players[2].has_cards_in_hand();
        bool team2 = players[1].has_cards_in_hand() && players[3].has_cards_in_hand();
        return team1 || team2;
    }
    PlayerID determine_last() {
        for (int i = 0; i < 4; ++i) {
            if (players[i].has_cards_in_hand()) {
                return i;
            }
        }
        return -1;
    };
    void finish_round() {
        const Standings standings_before = gameKnowledge.get_standings();
        int card_points_total = 0;
        for (int location = 0; location <= static_cast<int>(Locations::P4Bank); ++location) {
            card_points_total += gameKnowledge.get_points_in_Location(static_cast<Locations>(location));
        }
        const bool double_win = is_double_win();
        if (double_win) {
			PlayerID first_winner = gameKnowledge.get_first_finisher();
			gameKnowledge.add_Points_to_team(first_winner % 2, 200);
            
        }
        else {
			PlayerID first_winner = gameKnowledge.get_first_finisher();
			PlayerID last_player = determine_last();
            for (int id = 0; id < 4; ++id) {
                if (id == last_player) {
					int points_in_hand = gameKnowledge.get_points_in_Location(static_cast<Locations>(static_cast<int>(Locations::P1Hand) + id));
					int points_in_bank = gameKnowledge.get_points_in_Location(static_cast<Locations>(static_cast<int>(Locations::P1Bank) + id));
					gameKnowledge.add_Points_to_team((id +1)% 2, points_in_hand);
					gameKnowledge.add_Points_to_team((first_winner) % 2, points_in_bank);
                }
                else {
					gameKnowledge.add_Points_to_team(id % 2, gameKnowledge.get_points_in_Location(static_cast<Locations>(static_cast<int>(Locations::P1Bank) + id)));
                }
            }
            gameKnowledge.add_Points_to_team((first_winner) % 2, gameKnowledge.get_points_in_Location(Locations::Table));
        }
        const Standings standings_after_cards = gameKnowledge.get_standings();
		Tichu tichu = gameKnowledge.get_tichu();
		GrandTichu grandTichu = gameKnowledge.get_grand_tichu();
        int tichu_count = 0;
        int grand_tichu_count = 0;
        for (int id = 0; id < 4; ++id) {
            tichu_count += tichu[id] ? 1 : 0;
            grand_tichu_count += grandTichu[id] ? 1 : 0;
        }
        for(int id = 0; id < 4; ++id) {
            if (tichu[id]) {
                if (id == gameKnowledge.get_first_finisher()) {
                    gameKnowledge.add_Points_to_team(id % 2, 100);
                }
                else {
					gameKnowledge.add_Points_to_team(id % 2, -100);
                }
            }
            if (grandTichu[id]) {
                if(id == gameKnowledge.get_first_finisher()) {
                    gameKnowledge.add_Points_to_team(id % 2, 200);
                }
                else {
                    gameKnowledge.add_Points_to_team(id % 2, -200);
				}
            }
		}
        const Standings standings_after = gameKnowledge.get_standings();
        std::cout << "Round audit: card points=" << card_points_total
                  << ", double win=" << double_win
                  << ", card delta=(" << standings_after_cards[0] - standings_before[0]
                  << ',' << standings_after_cards[1] - standings_before[1]
                  << "), Tichu calls=" << tichu_count
                  << ", Grand Tichu calls=" << grand_tichu_count
                  << ", final delta=(" << standings_after[0] - standings_before[0]
                  << ',' << standings_after[1] - standings_before[1]
                  << "), standings=(" << standings_after[0] << ',' << standings_after[1] << ")\n";
        notify_all_players_of_standings();
	}
};
