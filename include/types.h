#pragma once
#include <cstdint>
#include <array>
#include <stdexcept>
#include <vector>
enum class Cards {
    Dog = 0,
    Phoenix = 1,
    Mahjong = 2,
    Blue2 = 3,
    Green2 = 4,
    Red2 = 5,
    Black2 = 6,
    Blue3 = 7,
    Green3 = 8,
    Red3 = 9,
    Black3 = 10,
    Blue4 = 11,
    Green4 = 12,
    Red4 = 13,
    Black4 = 14,
    Blue5 = 15,
    Green5 = 16,
    Red5 = 17,
    Black5 = 18,
    Blue6 = 19,
    Green6 = 20,
    Red6 = 21,
    Black6 = 22,
    Blue7 = 23,
    Green7 = 24,
    Red7 = 25,
    Black7 = 26,
    Blue8 = 27,
    Green8 = 28,
    Red8 = 29,
    Black8 = 30,
    Blue9 = 31,
    Green9 = 32,
    Red9 = 33,
    Black9 = 34,
    Blue10 =35,
    Green10=36,
	Red10 = 37,
    Black10=38,
    BlueJack = 39,
    GreenJack = 40,
    RedJack = 41,
    BlackJack = 42,
    BlueQueen = 43,
    GreenQueen = 44,
    RedQueen = 45,
    BlackQueen = 46,
    BlueKing = 47,
    GreenKing = 48,
    RedKing = 49,
    BlackKing = 50,
    BlueAce = 51,
    GreenAce = 52,
    RedAce = 53,
    BlackAce = 54,
    Dragon = 55,
    Count=56,
};
enum Color {
    Blue,
    Green,
    Red,
    Black,
    None
};
enum class CardType {
    Dog=-1,
	Phoenix = 0,
	Mahjong = 1,
    Two = 2,
    Three = 3,
    Four = 4,
    Five = 5,
    Six = 6,
    Seven = 7,
    Eight = 8,
    Nine = 9,
    Ten = 10,
    Jack = 11,
    Queen = 12,
    King = 13,
	Ace = 14,
	Dragon = 16,
};
enum class Beat_Value {
    Dog = 0,
    Mahjong = 1,
	Two = 2,
    Three = 3,
    Four = 4,
    Five = 5,
    Six = 6,
    Seven = 7,
    Eight = 8,
    Nine = 9,
    Ten = 10,
    Jack = 11,
    Queen = 12,
	King = 13,
    Ace = 14,
    Phoenix = 15,
	Dragon = 16,
};
enum Locations {
    P1Hand,
    P2Hand,
    P3Hand,
    P4Hand,
    Table,
    P1Bank,
    P2Bank,
    P3Bank,
    P4Bank,
    Unkown,
};
enum MoveType {
    Single,
    Pair,
    Triple,
	FullHouse,
    Straight,
	PairSequence,
    Bomb,
	NotDefined
};

using CardStack = std::uint64_t;
struct MoveInfo {
    CardStack cards = 0;
    MoveType type = MoveType::NotDefined;
    Beat_Value beat_value = Beat_Value::Dog;
    int length = 0;
};
using CardSet = std::uint64_t;
using CardLocations = std::array<Locations, 57>;
using Points = std::array<int, 10>;
using Standings = std::array<int, 2>;
using PlayerID = int;
using Tichu = std::array<bool, 4>;
using GrandTichu = std::array<bool, 4>;
constexpr int pop_lsb(CardStack& stack) {
    if (stack == 0) return -1;
    int lsb = 0;
    CardStack bit = 1;
    while ((stack & bit) == 0) {
        bit <<= 1;
        ++lsb;
    }
    stack &= stack - 1;
    return lsb;

}
constexpr int lsb(const CardStack& stack) {
    if (stack == 0) return -1;
    int lsb = 0;
    CardStack bit = 1;
    while ((stack & bit) == 0) {
        bit <<= 1;
        ++lsb;
    }
    return lsb;
}
constexpr int popcount(CardStack stack) {
    int count = 0;
    while (stack != 0) {
        stack &= stack - 1;
        ++count;
    }
    return count;
}
Beat_Value get_beat_value(Cards card) {
    switch (card) {
        case Cards::Dog:
            return Beat_Value::Dog;
        case Cards::Mahjong:
            return Beat_Value::Mahjong;
        case Cards::Phoenix:
            return Beat_Value::Phoenix;
        case Cards::Dragon:
            return Beat_Value::Dragon;
        default:
            return static_cast<Beat_Value>((static_cast<int>(card) - 3) / 4 + 2);
    }
}
Beat_Value get_beat_value(CardStack stack) {
    if (popcount(stack) != 1) {
        throw std::invalid_argument("Stack contains more than one card, cannot determine a single card.");
    }
    int lsb_index = lsb(stack);
    Cards card = static_cast<Cards>(lsb_index);
    return get_beat_value(card);
}
std::string get_location_name(Locations location) {
    switch (location) {
        case Locations::P1Hand: return "P1Hand";
        case Locations::P2Hand: return "P2Hand";
        case Locations::P3Hand: return "P3Hand";
        case Locations::P4Hand: return "P4Hand";
        case Locations::Table: return "Table";
        case Locations::P1Bank: return "P1Bank";
        case Locations::P2Bank: return "P2Bank";
        case Locations::P3Bank: return "P3Bank";
        case Locations::P4Bank: return "P4Bank";
        case Locations::Unkown: return "Unkown";
        default: throw std::invalid_argument("Invalid location value.");
    }
}
