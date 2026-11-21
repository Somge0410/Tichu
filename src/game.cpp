#include "game.h"

#include "cards.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <stdexcept>

Game::Game(std::uint32_t seed) : rng(seed) {}

const GameKnowlege& Game::get_knowledge() const {
    return gameKnowledge;
}

void Game::notify_player_of_card_change(PlayerID id, const MoveInfo& move, Locations global_location) {
    players[id].ReceiveCardLocationChangeInfo(move, global_location);
}

void Game::notify_all_players_of_card_change(const MoveInfo& move, Locations global_location) {
    for (int id = 0; id < 4; ++id) {
        notify_player_of_card_change(id, move, global_location);
    }
}

void Game::notify_player_of_turn(PlayerID id) {
    players[id].receive_turn_info(gameKnowledge.get_turn());
}

void Game::notify_all_players_of_turn() {
    for (int id = 0; id < 4; ++id) {
        notify_player_of_turn(id);
    }
}

void Game::notify_player_of_last_player(PlayerID id) {
    players[id].receive_last_player_info(gameKnowledge.get_last_player());
}

void Game::notify_all_players_of_last_player() {
    for (int id = 0; id < 4; ++id) {
        notify_player_of_last_player(id);
    }
}

void Game::notify_player_of_points(PlayerID id) {
    players[id].receive_standings(gameKnowledge.get_standings());
}

void Game::notify_player_of_grand_tichu(PlayerID id) {
    players[id].ReceiveGrandTichuInfo(gameKnowledge.get_grand_tichu());
}

void Game::notify_player_of_tichu(PlayerID id) {
    players[id].ReceiveTichuInfo(gameKnowledge.get_tichu());
}

void Game::notify_all_player_of_tichu() {
    for (int id = 0; id < 4; ++id) {
        notify_player_of_tichu(id);
    }
}
void Game::notify_all_player_of_grand_tichu() {
    for (int id = 0; id < 4; ++id) {
        notify_player_of_grand_tichu(id);
    }
}
void Game::notify_player_of_first_finisher(PlayerID id) {
    players[id].receive_first_finisher(gameKnowledge.get_first_finisher());
}

void Game::notify_all_players_of_first_finisher() {
    for (int id = 0; id < 4; ++id) {
        notify_player_of_first_finisher(id);
    }
}

void Game::notify_player_of_standings(PlayerID id) {
    players[id].receive_standings(gameKnowledge.get_standings());
}

void Game::notify_all_players_of_standings() {
    for (int id = 0; id < 4; ++id) {
        notify_player_of_standings(id);
    }
}

void Game::notify_all_players_of_hand_counts() {
    std::array<int, 4> counts;
    for (int id = 0; id < 4; ++id) {
        counts[id] = players[id].how_many_cards_in_hand();
    }
    gameKnowledge.set_cards_in_hands(counts);
    for (Player& player : players) {
        player.receive_hand_counts(counts);
    }
}

void Game::notify_all_players_of_wish() {
    for (Player& player : players) {
        player.receive_wish_info(gameKnowledge.get_wish());
    }
}

template<bool with_log>
void Game::check_tichu(PlayerID player) {
    if (players[player].announce_tichu()) {
        gameKnowledge.set_tichu<with_log>(true, player);
        notify_all_player_of_tichu();
    }
}

bool Game::request_player_for_tichu(PlayerID id) {
    return players[id].announce_tichu();
}

bool Game::request_player_for_grand_tichu(PlayerID id) {
    return players[id].announce_grand_tichu();
}

MoveInfo Game::request_player_for_move(PlayerID id) {
    return players[id].announce_move_to_table();
}

MoveInfo Game::request_player_for_bomb(PlayerID id) {
    return players[id].announce_bomb();
}

std::array<CardStack, 3> Game::request_player_for_change(PlayerID id) {
    std::array<CardStack, 3> change = players[id].announce_change();
    // Process the change and update game state
    return change; // Return the change
}

PlayerID Game::determine_starting_player() {
    Locations mahjong_location = gameKnowledge.get_card_locations()[static_cast<int>(Cards::Mahjong)];
    if (mahjong_location == Locations::P0Hand) {
        return static_cast<PlayerID>(0);
    }
    else if (mahjong_location == Locations::P1Hand) {
        return static_cast<PlayerID>(1);
    }
    else if (mahjong_location == Locations::P2Hand) {
        return static_cast<PlayerID>(2);
    }
    else if (mahjong_location == Locations::P3Hand) {
        return static_cast<PlayerID>(3);
    }
    else {
        throw std::runtime_error("Mahjong card not found in any player's hand.");
    }
}

PlayerID Game::determine_next_player(PlayerID current_player) {
    return static_cast<PlayerID>((current_player + 1) % 4);
}

PlayerID Game::determine_next_player_after_dog(PlayerID current_player) {
    PlayerID next = (current_player + 2) % 4;
    while (!players[next].has_cards_in_hand()) {
        next = determine_next_player(next);
    }
    return next;
}

bool Game::round_ongoing() {
    bool team1 = players[0].has_cards_in_hand() || players[2].has_cards_in_hand();
    bool team2 = players[1].has_cards_in_hand() || players[3].has_cards_in_hand();
    return team1 && team2;
}

bool Game::is_double_win() {
    bool team1 = players[0].has_cards_in_hand() && players[2].has_cards_in_hand();
    bool team2 = players[1].has_cards_in_hand() && players[3].has_cards_in_hand();
    return team1 || team2;
}

PlayerID Game::determine_last() {
    for (int i = 0; i < 4; ++i) {
        if (players[i].has_cards_in_hand()) {
            return i;
        }
    }
    return -1;
}

template< bool with_log>
void Game::play_game(int limit) {
    const auto start_time = std::chrono::steady_clock::now();
    Standings standings = gameKnowledge.get_standings();
    while ((standings[0]<limit && standings[1]<limit) || standings[0]==standings[1]) {
        play_round<with_log>();
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

template<bool with_log>
void Game::apply_move(PlayerID player, const MoveInfo& move) {
    MoveInfo played = move;
    if (move.type == MoveType::Single && (move.cards & cardBit(Cards::Phoenix))) {
        const MoveInfo& table = gameKnowledge.get_table_face();
        // Integer ranks store the value immediately below Phoenix's half-step.
        played.beat_value = table.length == 0 ? Beat_Value::Mahjong : table.beat_value;
        played.cards = (move.cards & CARD_BITS_MASK) |
            (static_cast<CardStack>(played.beat_value) << 60);
    }

    gameKnowledge.set_last_player(player);
    notify_all_players_of_last_player();
    const Locations location = (move.cards & cardBit(Cards::Dog))
        ? static_cast<Locations>(static_cast<int>(Locations::P0Bank) + player)
        : Locations::Table;
    gameKnowledge.give_cards_to_Location<with_log>(played, location);
    notify_all_players_of_card_change(played, location);
    notify_all_players_of_hand_counts();

    if (!players[player].has_cards_in_hand() && gameKnowledge.get_first_finisher() == -1) {
        gameKnowledge.set_first_finisher<with_log>(player);
        notify_all_players_of_first_finisher();
    }
    const auto wish = gameKnowledge.get_wish();
    if (wish && contains_rank(played.cards, *wish)) {
        gameKnowledge.set_wish(std::nullopt);
        notify_all_players_of_wish();
    }
    if (played.cards & cardBit(Cards::Mahjong)) {
        gameKnowledge.set_wish(players[player].announce_wish());
        notify_all_players_of_wish();
    }
}

template<bool with_log>
void Game::collect_table() {
    const CardStack cards = gameKnowledge.get_cards_in_Location(Locations::Table);
    if (cards == 0) return;
    const PlayerID winner = gameKnowledge.get_last_player();
    PlayerID recipient = winner;
    if (gameKnowledge.get_table_face().cards & cardBit(Cards::Dragon)) {
        recipient = players[winner].where_give_dragon_stack();
        if constexpr (with_log) {
            std::cout << "Dragon won! Giving cards to Player " << recipient << '\n';
        }
    }
    const Locations bank = static_cast<Locations>(static_cast<int>(Locations::P0Bank) + recipient);
    gameKnowledge.give_cards_to_Location<with_log>({cards}, bank);
    notify_all_players_of_card_change({cards}, bank);
}

template< bool with_log>
void Game::play_round() {
    gameKnowledge.clear();
    for (PlayerID id = 0; id < 4; ++id) {
        players[id].set_current_player(id);
    }
    std::array<Cards, static_cast<std::size_t>(Cards::Count)> deck;
    for (std::size_t index = 0; index < deck.size(); ++index) {
        deck[index] = static_cast<Cards>(index);
    }
    std::shuffle(deck.begin(), deck.end(), rng);
    std::size_t next_card = 0;
    const auto draw = [&](std::size_t count) {
        CardStack cards = 0;
        for (std::size_t index = 0; index < count; ++index) {
            cards |= cardBit(deck[next_card++]);
        }
        return cards;
    };
    for (int id = 0; id < 4; ++id) {
        give_cards_to_player<with_log>(id, draw(8));
        gameKnowledge.set_grand_tichu<with_log>(request_player_for_grand_tichu(id), id);
        notify_all_player_of_grand_tichu();
    }
    for (int id = 0; id < 4; ++id) {
        give_cards_to_player<with_log>(id, draw(6));
        check_tichu<with_log>(id);
    }

    std::array<std::array<CardStack, 3>, 4> changes;
    for (int id = 0; id < 4; ++id) {
        changes[id] = request_player_for_change(id);
    }
    for (int from = 0; from < 4; ++from) {
        for (int to = 0; to < 3; ++to) {
            const PlayerID target = (from + 3 - to) % 4;
            const Locations hand = static_cast<Locations>(static_cast<int>(Locations::P0Hand) + target);
            gameKnowledge.give_cards_to_Location<with_log>({changes[from][to]}, hand);
            notify_player_of_card_change(target, {changes[from][to]}, hand);
            notify_player_of_card_change(from, {changes[from][to]}, hand);
        }
    }
    notify_all_players_of_hand_counts();
    gameKnowledge.set_player_turn(determine_starting_player());
    int pass_count = 0;
    while (round_ongoing()) {
        PlayerID current = gameKnowledge.get_turn();
        notify_all_players_of_turn();
        check_tichu<with_log>(current);
        const MoveInfo move = request_player_for_move(current);
        if (move.cards != 0) {
            pass_count = 0;
            apply_move<with_log>(current, move);
            if (!round_ongoing()) break;
            if (move.cards & cardBit(Cards::Dog)) {
                gameKnowledge.set_player_turn<with_log>(determine_next_player_after_dog(current));
                continue;
            }
        }
        else {
            ++pass_count;
        }

        // Offer every player a bomb, including the owner of the current trick.
        // A successful bomb restarts the scan after the bomber.
        PlayerID candidate = determine_next_player(current);
        int checked = 0;
        while (checked < 4 && round_ongoing() && gameKnowledge.get_table_face().length != 0) {
            check_tichu<with_log>(candidate);
            const MoveInfo bomb = request_player_for_bomb(candidate);
            if (bomb.cards != 0) {
                apply_move<with_log>(candidate, bomb);
                pass_count = 0;
                current = candidate;
                checked = 0;
            }
            else {
                ++checked;
            }
            candidate = determine_next_player(candidate);
        }
        if (!round_ongoing()) break;
        gameKnowledge.set_player_turn<with_log>(determine_next_player(current));
        if (pass_count >= 3) {
            const PlayerID winner = gameKnowledge.get_last_player();
            collect_table<with_log>();
            PlayerID leader = winner;
            while (!players[leader].has_cards_in_hand()) {
                leader = determine_next_player(leader);
            }
            gameKnowledge.set_player_turn<with_log>(leader);
            pass_count = 0;
        }
    }
    if (!is_double_win()) collect_table<with_log>();
    finish_round<with_log>();
}

template <bool with_log>
void Game::give_cards_to_player(PlayerID id, CardStack cards) {
    Locations location = static_cast<Locations>(static_cast<int>(Locations::P0Hand) + id);
    gameKnowledge.give_cards_to_Location<with_log>({ cards }, location);
    notify_player_of_card_change(id, { cards }, location);
    notify_all_players_of_hand_counts();
}

template<bool with_logs>
void Game::finish_round() {
    const Standings standings_before = gameKnowledge.get_standings();
    int card_points_total = 0;
    for (int location = 0; location <= static_cast<int>(Locations::P3Bank); ++location) {
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
                int points_in_hand = gameKnowledge.get_points_in_Location(static_cast<Locations>(static_cast<int>(Locations::P0Hand) + id));
                int points_in_bank = gameKnowledge.get_points_in_Location(static_cast<Locations>(static_cast<int>(Locations::P0Bank) + id));
                gameKnowledge.add_Points_to_team((id +1)% 2, points_in_hand);
                gameKnowledge.add_Points_to_team((first_winner) % 2, points_in_bank);
            }
            else {
                gameKnowledge.add_Points_to_team(id % 2, gameKnowledge.get_points_in_Location(static_cast<Locations>(static_cast<int>(Locations::P0Bank) + id)));
            }
        }
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
    if constexpr (with_logs) {
        std::cout << "Round finished. Standings: Team 1 = " << standings_after[0] << ", Team 2 = " << standings_after[1] << std::endl;
        std::cout << "Round audit: card points=" << card_points_total
            << ", double win=" << double_win
            << ", card delta=(" << standings_after_cards[0] - standings_before[0]
            << ',' << standings_after_cards[1] - standings_before[1]
            << "), Tichu calls=" << tichu_count
            << ", Grand Tichu calls=" << grand_tichu_count
            << ", final delta=(" << standings_after[0] - standings_before[0]
            << ',' << standings_after[1] - standings_before[1]
            << "), standings=(" << standings_after[0] << ',' << standings_after[1] << ")\n";
    }
    notify_all_players_of_standings();
}

// Instantiate both logging variants for callers in other source files.
template void Game::play_game<false>(int);
template void Game::play_game<true>(int);
