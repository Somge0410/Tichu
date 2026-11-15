#pragma once
#include <cstdint>
#include <array>
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
constexpr int popcount(CardStack stack) {
    int count = 0;
    while (stack != 0) {
        stack &= stack - 1;
        ++count;
    }
    return count;
}
