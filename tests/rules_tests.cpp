#include "game.h"
#include "cards.h"
#include "move_generation.h"

#include <algorithm>
#include <iostream>
#include <random>
#include <regex>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>

namespace {
void require(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}

constexpr bool constant_bit_operations() {
    CardStack cards = (CardStack{1} << 63) | (CardStack{1} << 5);
    return popcount(cards) == 2 && lsb(cards) == 5 && pop_lsb(cards) == 5 &&
        pop_lsb(cards) == 63 && pop_lsb(cards) == -1 && cards == 0;
}
static_assert(constant_bit_operations(), "Bit operations must remain usable in constant expressions");
static_assert(lsb(CardStack{0}) == -1 && popcount(CardStack{0}) == 0, "Zero bit operations are incorrect");
static_assert(popcount(~CardStack{0}) == 64, "Population count must include all 64 bits");

void check_bit_operations() {
    const auto check = [](CardStack value) {
        int expected_count = 0;
        int expected_index = -1;
        for (int bit = 0; bit < 64; ++bit) {
            if (value & (CardStack{1} << bit)) {
                ++expected_count;
                if (expected_index == -1) expected_index = bit;
            }
        }
        require(popcount(value) == expected_count, "Fast population count is incorrect");
        require(card_bits::popcount_fallback(value) == expected_count, "Portable population count is incorrect");
        require(lsb(value) == expected_index, "Fast lowest-bit lookup is incorrect");
        CardStack remaining = value;
        require(pop_lsb(remaining) == expected_index, "Removing the lowest bit returned the wrong index");
        const CardStack expected_remaining = value == 0 ? 0 : value ^ (CardStack{1} << expected_index);
        require(remaining == expected_remaining, "Removing the lowest bit changed other bits");
    };
    check(0);
    check(~CardStack{0});
    for (int bit = 0; bit < 64; ++bit) check(CardStack{1} << bit);
    std::mt19937_64 random(12345);
    for (int sample = 0; sample < 2048; ++sample) check(random());
}

Player player_with(CardStack hand) {
    Player player(0);
    player.ReceiveCardLocationChangeInfo({hand}, Locations::P0Hand);
    return player;
}

void put_on_table(Player& player, const MoveInfo& move) {
    player.receive_last_player_info(1);
    player.ReceiveCardLocationChangeInfo(move, Locations::Table);
}

void check_phoenix() {
    const auto response = [](Cards card, Beat_Value base) {
        Player player = player_with(cardBit(card));
        put_on_table(player, {cardBit(Cards::Phoenix) | (static_cast<CardStack>(base) << 60),
            MoveType::Single, base, 1});
        return player.announce_move_to_table().cards;
    };
    require(response(Cards::Mahjong, Beat_Value::Mahjong) == 0, "Mahjong beat leading Phoenix");
    require(response(Cards::Blue2, Beat_Value::Mahjong) != 0, "Two failed to beat leading Phoenix");
    require(response(Cards::Blue8, Beat_Value::Eight) == 0, "Eight beat Phoenix over eight");
    require(response(Cards::Blue9, Beat_Value::Eight) != 0, "Nine failed to beat Phoenix over eight");
    require(response(Cards::BlueAce, Beat_Value::Ace) == 0, "Ace beat Phoenix over ace");
    require(response(Cards::Dragon, Beat_Value::Ace) != 0, "Dragon failed to beat Phoenix");

    std::ostringstream output;
    auto* previous = std::cout.rdbuf(output.rdbuf());
    print(MoveInfo{cardBit(Cards::Phoenix), MoveType::Single, Beat_Value::Mahjong, 1});
    print(MoveInfo{cardBit(Cards::Phoenix), MoveType::Single, Beat_Value::Eight, 1});
    print(MoveInfo{cardBit(Cards::Phoenix) | cardBit(Cards::BlueJack) |
        push_CardType_To_last_bits(Cards::BlueJack), MoveType::Pair, Beat_Value::Jack, 2});
    std::cout.rdbuf(previous);
    require(output.str().find("Phoenix=1.5") != std::string::npos, "Leading Phoenix log is wrong");
    require(output.str().find("Phoenix=8.5") != std::string::npos, "Phoenix response log is wrong");
    require(output.str().find("Phoenix=11") != std::string::npos, "Phoenix pair log is wrong");
}

void check_generators() {
    const CardStack triple = cardBit(Cards::Blue2) | cardBit(Cards::Green2) | cardBit(Cards::Black2);
    const auto triples = generate_moves(triple);
    require(std::any_of(triples.begin(), triples.end(), [&](const MoveInfo& move) {
        return move.type == MoveType::Triple && move.cards == triple;
    }), "Valid triple is missing");

    const CardStack straight = cardBit(Cards::Mahjong) | cardBit(Cards::Blue2) |
        cardBit(Cards::Green3) | cardBit(Cards::Red4) | cardBit(Cards::Black5);
    const auto straights = generate_moves(straight);
    require(std::any_of(straights.begin(), straights.end(), [&](const MoveInfo& move) {
        return move.type == MoveType::Straight && move.cards == straight;
    }), "Mahjong straight is missing");

    const CardStack four_twos = CardStack{0xF} << static_cast<int>(Cards::Blue2);
    const auto moves = generate_moves(four_twos | cardBit(Cards::Phoenix));
    require(std::none_of(moves.begin(), moves.end(), [](const MoveInfo& move) {
        return move.type == MoveType::FullHouse;
    }), "Five cards of the same rank formed a full house");
}

void check_wishes() {
    Player required = player_with(cardBit(Cards::Blue8) | cardBit(Cards::BlueAce));
    put_on_table(required, {cardBit(Cards::Blue7), MoveType::Single, Beat_Value::Seven, 1});
    required.receive_wish_info(CardType::Eight);
    require(required.announce_move_to_table().cards == cardBit(Cards::Blue8), "Wish was ignored");

    Player unavailable = player_with(cardBit(Cards::Blue8) | cardBit(Cards::BlueAce));
    put_on_table(unavailable, {cardBit(Cards::Blue9), MoveType::Single, Beat_Value::Nine, 1});
    unavailable.receive_wish_info(CardType::Eight);
    require(unavailable.announce_move_to_table().cards == cardBit(Cards::BlueAce), "Unplayable wish blocked a legal move");

    const CardStack fours = CardStack{0xF} << static_cast<int>(Cards::Blue4);
    Player forced_bomb = player_with(fours | cardBit(Cards::Dragon));
    put_on_table(forced_bomb, {cardBit(Cards::BlueAce), MoveType::Single, Beat_Value::Ace, 1});
    forced_bomb.receive_wish_info(CardType::Four);
    require(forced_bomb.announce_move_to_table().type == MoveType::Bomb, "Wish did not force a legal bomb");

    Player wildcard = player_with(cardBit(Cards::Phoenix) | cardBit(Cards::BlueAce));
    put_on_table(wildcard, {cardBit(Cards::Blue7), MoveType::Single, Beat_Value::Seven, 1});
    wildcard.receive_wish_info(CardType::Eight);
    require(wildcard.announce_move_to_table().cards != 0, "Phoenix was wrongly treated as a natural wished card");
}

void check_bombs_and_declarations() {
    const CardStack fours = CardStack{0xF} << static_cast<int>(Cards::Blue4);
    Player bomber = player_with(fours);
    require(bomber.announce_bomb().cards == 0, "Bomb played onto an empty table");
    put_on_table(bomber, {cardBit(Cards::Dragon), MoveType::Single, Beat_Value::Dragon, 1});
    require(bomber.announce_bomb().cards == fours, "Bomb failed to beat Dragon out of turn");
    require(!bomber.has_cards_in_hand(), "Bomb was not removed from hand");
    require(!bomber.can_announce_tichu(), "Player could declare after bombing");

    const CardStack fives = CardStack{0xF} << static_cast<int>(Cards::Blue5);
    Player counter = player_with(fives);
    put_on_table(counter, {fours, MoveType::Bomb, Beat_Value::Four, 4});
    require(counter.announce_bomb().cards == fives, "Higher bomb failed to counter");

    const CardStack aces = CardStack{0xF} << static_cast<int>(Cards::BlueAce);
    Player strong = player_with(aces | cardBit(Cards::Dragon) | cardBit(Cards::Phoenix));
    require(strong.announce_grand_tichu(), "Strong eight-card-or-smaller hand never declares Grand Tichu");
    require(strong.announce_tichu(), "Strong hand never declares Tichu");
    strong.ReceiveGrandTichuInfo({true, false, false, false});
    require(!strong.announce_tichu() && !strong.announce_grand_tichu(), "Duplicate declaration was allowed");

    Player played = player_with(cardBit(Cards::Blue2) | aces);
    require(played.announce_move_to_table().cards != 0, "Opening move failed");
    require(!played.can_announce_tichu(), "Player could declare after their first play");
}

void check_dog_transfers() {
    GameKnowlege knowledge;
    knowledge.give_cards_to_Location({cardBit(Cards::Dog)}, Locations::P0Hand);
    knowledge.give_cards_to_Location({cardBit(Cards::Dog)}, Locations::P1Hand);
    require(knowledge.get_last_player() == -1, "Dealing unexpectedly changed last player");
    knowledge.set_cards_in_hands({14, 14, 14, 14});
    knowledge.set_last_player(1);
    knowledge.give_cards_to_Location({cardBit(Cards::Dog)}, Locations::P1Bank);
    require(knowledge.get_cards_in_hands()[1] == 14, "Card transfer changed authoritative hand counts");
    knowledge.set_cards_in_hands({14, 13, 14, 14});
    require(knowledge.get_cards_in_hands()[1] == 13, "Hand-count notification was not stored");
}

void check_dealing() {
    std::ostringstream output;
    auto* previous = std::cout.rdbuf(output.rdbuf());
    try {
        Game game(12345);
        game.play_game<true>(1);
    }
    catch (...) {
        std::cout.rdbuf(previous);
        throw;
    }
    std::cout.rdbuf(previous);

    const std::string log = output.str();
    const std::string header = "Giving  cards to ";
    const std::regex card_pattern(R"(\{\s*([^{}\r\n]+?)\s*\})");
    std::set<std::string> seen;
    std::array<int, 4> hand_sizes = {};
    std::size_t start = log.find(header);
    for (int deal = 0; deal < 8; ++deal) {
        require(start != std::string::npos, "A dealing stage is missing");
        const std::size_t line_end = log.find('\n', start);
        require(log.substr(start + header.size(), line_end - start - header.size()) ==
            "P" + std::to_string(deal % 4) + "Hand", "Dealing changed the player order");
        const std::size_t next = log.find(header, line_end);
        const std::string block = log.substr(line_end, next - line_end);
        int card_count = 0;
        for (std::sregex_iterator match(block.begin(), block.end(), card_pattern), end; match != end; ++match) {
            std::istringstream words((*match)[1].str());
            std::string word;
            std::string card;
            while (words >> word) {
                if (!card.empty()) card += ' ';
                card += word;
            }
            require(seen.insert(card).second, "The same card was dealt more than once");
            ++card_count;
        }
        require(card_count == (deal < 4 ? 8 : 6), "The eight-card/six-card dealing stages changed");
        hand_sizes[deal % 4] += card_count;
        start = next;
    }
    require(seen.size() == static_cast<std::size_t>(Cards::Count), "The deck was not dealt completely");
    for (int count : hand_sizes) require(count == 14, "A player did not receive fourteen cards");
}

void check_complete_games() {
    std::ostringstream output;
    auto* previous = std::cout.rdbuf(output.rdbuf());
    try {
        for (unsigned seed = 0; seed < 40; ++seed) {
            Game game(seed);
            game.play_game<false>(500);
            const GameKnowlege& knowledge = game.get_knowledge();
            const auto standings = knowledge.get_standings();
            require(std::max(standings[0], standings[1]) >= 500 && standings[0] != standings[1], "Game ended before the point limit");
            const auto counts = knowledge.get_cards_in_hands();
            const auto locations = knowledge.get_card_locations();
            for (int card = 0; card < static_cast<int>(Cards::Count); ++card) {
                require(locations[card] != Locations::Unkown, "A card was never dealt");
            }
            int points = 0;
            for (int id = 0; id < 4; ++id) {
                const auto hand = static_cast<Locations>(static_cast<int>(Locations::P0Hand) + id);
                require(counts[id] == popcount(knowledge.get_cards_in_Location(hand)), "Hand count diverged from card locations");
            }
            for (int location = 0; location <= static_cast<int>(Locations::P3Bank); ++location) {
                points += knowledge.get_points_in_Location(static_cast<Locations>(location));
            }
            require(points == 100, "Card points were lost or duplicated");
            const PlayerID first = knowledge.get_first_finisher();
            require(first >= 0 && first < 4 && counts[first] == 0, "First finisher was not recorded correctly");
        }
    }
    catch (...) {
        std::cout.rdbuf(previous);
        throw;
    }
    std::cout.rdbuf(previous);
}
}

int main() {
    try {
        check_bit_operations();
        check_phoenix();
        check_generators();
        check_wishes();
        check_bombs_and_declarations();
        check_dog_transfers();
        check_dealing();
        check_complete_games();
        std::cout << "All bit, dealing, rule checks and 40 seeded games passed.\n";
    }
    catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
