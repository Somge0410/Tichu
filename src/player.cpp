#include "player.h"

#include "cards.h"
#include "move_generation.h"

#include <algorithm>
#include <stdexcept>

Player::Player(PlayerID id) : id(id) {}

void Player::set_current_player(PlayerID new_id) {
    id = new_id;
    gameKnowlege.clear();
    hand = 0;
    generated_moves.clear();
    moves_already_generated = false;
    has_played = false;
}

void Player::ReceiveCardLocationChangeInfo(const MoveInfo& move, Locations global_location) {
    Locations local_location = convert_global_to_local(global_location);
    gameKnowlege.give_cards_to_Location(move, local_location);
    if (Locations::P0Hand == local_location) {
        add_cards_to_hand(move.cards);
    }

}

void Player::ReceiveTichuInfo(const Tichu tichu) {
    Tichu local_tichu = convert_global_to_local(tichu);
    gameKnowlege.set_tichu(local_tichu);
}

void Player::ReceiveGrandTichuInfo(const GrandTichu grandTichu) {
    GrandTichu local_grand_tichu = convert_global_to_local(grandTichu);
    gameKnowlege.set_grand_tichu(local_grand_tichu);
}

MoveInfo Player::announce_move_to_table() {
    if (hand == 0) return {};
    ensure_moves_generated();
    const MoveInfo& table_move = gameKnowlege.get_table_face();
    const auto wish = gameKnowlege.get_wish();
    const bool must_fulfill_wish = wish && std::any_of(generated_moves.begin(), generated_moves.end(),
        [&](const MoveInfo& move) { return beats(move, table_move) && contains_rank(move.cards, *wish); });
    // TODO: Implement a better strategy for selecting a move
    for (auto it = generated_moves.rbegin(); it != generated_moves.rend(); ++it) {
        const MoveInfo move = *it;
        if (beats(move, table_move) && (!must_fulfill_wish || contains_rank(move.cards, *wish))) {
            remove_cards_from_hand(move.cards);
            has_played = true;
            return move;
        }
    }
    return {};
}

MoveInfo Player::announce_bomb() {
    if (hand == 0 || gameKnowlege.get_table_face().length == 0) return {};
    ensure_moves_generated();
    for (auto it = generated_moves.rbegin(); it != generated_moves.rend(); ++it) {
        const MoveInfo move = *it;
        if (move.type == MoveType::Bomb && beats(move, gameKnowlege.get_table_face())) {
            remove_cards_from_hand(move.cards);
            has_played = true;
            return move;
        }
    }
    return {};
}

CardType Player::announce_wish() const {
    for (int rank = 14; rank >= 2; --rank) {
        const auto type = static_cast<CardType>(rank);
        if (!contains_rank(hand, type)) return type;
    }
    return CardType::Two;
}

bool Player::can_announce_tichu() const {
    return hand != 0 && !has_played && !gameKnowlege.get_tichu()[0] && !gameKnowlege.get_grand_tichu()[0];
}

bool Player::can_announce_grand_tichu() const {
    return can_announce_tichu() && popcount(hand) <= 8;
}

std::array<CardStack,3> Player::announce_change() {
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

bool Player::announce_tichu() {
    return can_announce_tichu() && !gameKnowlege.get_tichu()[2] &&
        !gameKnowlege.get_grand_tichu()[2] && hand_strength() >= 5;
}

bool Player::announce_grand_tichu() {
    return can_announce_grand_tichu() && !gameKnowlege.get_grand_tichu()[2] && hand_strength() >= 4;
}

void Player::receive_standings(const Standings new_standings) {
    Standings local_standings = convert_global_to_local(new_standings);
    gameKnowlege.give_new_standings(local_standings);
}

void Player::receive_turn_info(PlayerID global_id) {
    PlayerID local_id = convert_global_to_local(global_id);
    gameKnowlege.set_player_turn(local_id);
    ensure_moves_generated();
}

void Player::receive_last_player_info(PlayerID global_id) {
    PlayerID local_id = convert_global_to_local(global_id);
    gameKnowlege.set_last_player(local_id);
}

void Player::receive_hand_counts(const std::array<int, 4>& counts) {
    std::array<int, 4> local_counts;
    for (int global_id = 0; global_id < 4; ++global_id) {
        local_counts[convert_global_to_local(global_id)] = counts[global_id];
    }
    gameKnowlege.set_cards_in_hands(local_counts);
}

void Player::receive_wish_info(std::optional<CardType> wish) {
    gameKnowlege.set_wish(wish);
}

bool Player::has_cards_in_hand() const {
    return hand != 0;
}

int Player::how_many_cards_in_hand() const {
    return popcount(hand);
}

bool Player::has_cards_left() const {
    return hand != 0;
}

void Player::receive_first_finisher(PlayerID global_id) {
    PlayerID local_id = convert_global_to_local(global_id);
    gameKnowlege.set_first_finisher(local_id);
}

PlayerID Player::where_give_dragon_stack() {
    return convert_local_to_global(static_cast<PlayerID>(1));
}

void Player::remove_cards_from_hand(const CardStack stack) {
    const CardStack cards = stack & CARD_BITS_MASK;
    hand &= ~cards;
    generated_moves.erase(
        std::remove_if(generated_moves.begin(), generated_moves.end(),
            [cards](const MoveInfo& move) { return (move.cards & cards) != 0; }),
        generated_moves.end());
}

void Player::add_cards_to_hand(const CardStack stack) {
    hand |= stack & CARD_BITS_MASK;
    moves_already_generated = false;
}

void Player::ensure_moves_generated() {
    if (!moves_already_generated) {
        generated_moves = generate_moves(hand);
        moves_already_generated = true;
    }
}

int Player::hand_strength() const {
    // A conservative declaration policy: high cards and four-of-a-kind bombs.
    int strength = popcount(hand & (CardStack{ 0xF } << static_cast<int>(Cards::BlueAce)));
    strength += (hand & cardBit(Cards::Dragon)) ? 2 : 0;
    strength += (hand & cardBit(Cards::Phoenix)) ? 1 : 0;
    for (int rank = 2; rank <= 14; ++rank) {
        const CardStack mask = CardStack{ 0xF } << (3 + 4 * (rank - 2));
        if ((hand & mask) == mask) strength += 2;
    }
    return strength;
}

PlayerID Player::convert_global_to_local(PlayerID global_id) {
    return (global_id - id + 4) % 4;
}

PlayerID Player::convert_local_to_global(PlayerID local_id) {
    return (local_id + id) % 4;
}

Locations Player::convert_global_to_local(Locations global_location) {
    const int location = static_cast<int>(global_location);
    const int player_id = static_cast<int>(id);
    const int first_hand = static_cast<int>(Locations::P0Hand);
    const int first_bank = static_cast<int>(Locations::P0Bank);

    if (location >= first_hand && location <= static_cast<int>(Locations::P3Hand)) {
        return static_cast<Locations>(first_hand + (location - first_hand - player_id + 4) % 4);
    }
    if (location >= first_bank && location <= static_cast<int>(Locations::P3Bank)) {
        return static_cast<Locations>(first_bank + (location - first_bank - player_id + 4) % 4);
    }
    return global_location;
}

Locations Player::convert_local_to_global(Locations local_location) {
    const int location = static_cast<int>(local_location);
    const int player_id = static_cast<int>(id);
    const int first_hand = static_cast<int>(Locations::P0Hand);
    const int first_bank = static_cast<int>(Locations::P0Bank);

    if (location >= first_hand && location <= static_cast<int>(Locations::P3Hand)) {
        return static_cast<Locations>(first_hand + (location - first_hand + player_id) % 4);
    }
    if (location >= first_bank && location <= static_cast<int>(Locations::P3Bank)) {
        return static_cast<Locations>(first_bank + (location - first_bank + player_id) % 4);
    }
    return local_location;
}

Standings Player::convert_global_to_local(Standings global_standings) {
    Standings local_standings;
    for (int i = 0; i < 2; ++i) {
        local_standings[i] = global_standings[(i + id) % 2];
    }
    return local_standings;
}

Points Player::convert_global_to_local(Points global_points) {
    Points local_points;
    for (int i = 0; i < 10; ++i) {
        local_points[convert_global_to_local(static_cast<Locations>(i))] = global_points[i];
    }
    return local_points;
}

GrandTichu Player::convert_global_to_local(GrandTichu global_grand_tichu) {
    GrandTichu local_grand_tichu;
    for (int i = 0; i < 4; ++i) {
        local_grand_tichu[i] = global_grand_tichu[(i + id) % 4];
    }
    return local_grand_tichu;
}
