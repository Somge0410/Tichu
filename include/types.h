#pragma once
#include <cstdint>
#include <array>
#include <vector>
enum class Cards = {
    Mahjong = 1,
    Dog = 2,
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
    BlackAce = 54
    Phoenix = 55,
    Dragon = 56,
    Count=57,
};
enum class Color {
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
enum class Locations {
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
enum class MoveType {
    Single,
    Pair,
    Triple,
	FullHouse,
    Straight,
	PairSequence,
    Bomb,
	NotDefined
};
Color color(Cards  card) {
    if (card == Cards::Mahjong || card == Cards::Dog || card == Cards::Phoenix || card == Cards::Dragon) {
        return Color::None;
    }
    int cardValue = static_cast<int>(card);
    int color_value = (cardValue - 3) % 4;
        switch (color_value) {
        case 0:
            return Color::Blue;
        case 1:
            return Color::Green;
        case 2:
            return Color::Red;
        case 3:
            return Color::Black;
        default:
            return Color::None;
        }
}
CardType CardType(Cards card) {
    if (card == Cards::Mahjong) {
        return CardType::Mahjong;
    }
    else if (card == Cards::Dog) {
        return CardType::Dog;
    }
    else if (card == Cards::Phoenix) {
        return CardType::Phoenix;
    }
    else if (card == Cards::Dragon) {
        return CardType::Dragon;
    }
    else {
        int cardValue = static_cast<int>(card);
        int value_value = (cardValue - 3) / 4+2; // Calculate the value based on the card index
        return static_cast<CardType>(value_value);
    }
}
int Points(Cards card) {
    if (card == Cards::Dragon) {
        return 25;
    }
    else if (card == Cards::Mahjong) {
        return 0;
    }
    else if (card == Cards::Phoenix) {
        return -25;
    }
    else if (card == Cards::Dog) {
        return 0;
    }
    else {
		CardType cardValue = CardType(card);
        if (cardValue == CardType::Five) {
            return 5;
        }
        else if (cardValue == CardType::Ten || cardValue == CardType::King) {
            return 10;
		}
        else {
            return 0;
        }
	}
}
int Index(Cards card) {
    return static_cast<int>(card);
}
int next_trumping_card_index(Cards card){
    if(card == Cards::Dragon) {
        return -2; // No card can trump the Dragon
    }
    else if (card == Cards::Phoenix) {
        return Index(Cards::Mahjong) // The Dragon trumps the Phoenix
    }
    else if (card == Cards::Mahjong) {
        return Index(Cards::Blue2); // The Dog trumps the Mahjong
    }
    else if (card == Cards::Dog) {
        return Index(Cards::Phoenix); // The Phoenix trumps the Dog
    }
    else {
        int index = Index(card);
		int next_index = index + 4-(index-3)%4;
        return next_index;
    }
}
int second_next_trump_index(Cards card) {
    Cards new_card = static_cast<Cards>(next_trumping_card_index);
    return next_trumping_card_index(new_card)
}

constexpr bool operator<=(CardType lhs, CardType rhs) {
    else return static_cast<int>(lhs) <= static_cast<int>(rhs);
}
constexpr bool operator<(CardType lhs, CardType rhs) {
	return lhs <= rhs && lhs != rhs;
}
constexpr bool operator>=(CardType lhs, CardType rhs) {
    return !(lhs < rhs);
}
constexpr bool operator>(CardType lhs, CardType rhs) {
    return !(lhs <= rhs);
}
constexpr bool operator <=(Cards lhs, Cards rhs) {
    return CardType(lhs) <= CardType(rhs);
}
constexpr bool operator <(Cards lhs, Cards rhs) {
    return CardType(lhs) < CardType(rhs);
}
constexpr bool operator >=(Cards lhs, Cards rhs) {
    return !(lhs < rhs);
}
constexpr bool operator >(Cards lhs, Cards rhs) {
    return !(lhs <= rhs);
}
using CardStack = std::uint64_t;
using CardSet = std::uint64_t;
using CardLocations = std::array<Locations, 57>;
using Points = std::array<int, 10>;
using Standings = std::array<int, 2>;
using PlayerID = int;
using Tichu = std::array<bool, 4>;
using GrandTichu = std::array<bool, 4>;
constexpr CardSet cardBit(unsigned index) {
	return CardSet{ 1 } << index;
}
constexpr CardSet cardBit(Cards card) {
    return cardBit(static_cast<int>(card));
}
bool All_Same_color(CardSet card_set) {
    if (!card_set) return true;
    int lsb = pop_lsb(card_set);
    Color color = Color(static_cast<Cards>(lsb));
    while (card_set) {
        lsb = pop_lsb(card_set);
        if (color != Color(static_cast<Cards>(lsb))) return false;
    }
    return true;
}
bool is_bomb(CardSet card_set) {
    return popcount(card_set) >= 4 && All_Same_color(card_set);
}
void addCard(CardStack& stack, Cards card) {
    int cardValue = static_cast<int>(card);
    stack |= (1ULL << cardValue);
}
void removeCard(CardStack& stack, Cards card) {
    int cardValue = static_cast<int>(card);
    stack &= ~(1ULL << cardValue);
}

constexpr int pop_lsb(CardStack& stack) {
    int lsb = __builtin_ctzll(stack); // Get the least significant bit
    stack &= stack-1; // Remove the least significant bit from the stack
    return lsb;
}
constexpr int popcount(CardStack stack) {
    return __builtin_popcountll(stack);
}
Locations convert_global_to_local(Locations global_location, PlayerID id) {
    if(global_location == Locations::P1Hand) {
        return static_cast<Locations>((static_cast<int>(Locations::P1Hand) + id) % 4);
    } else if(global_location == Locations::P2Hand) {
        return static_cast<Locations>((static_cast<int>(Locations::P2Hand) + id) % 4);
    } else if(global_location == Locations::P3Hand) {
        return static_cast<Locations>((static_cast<int>(Locations::P3Hand) + id) % 4);
    } else if(global_location == Locations::P4Hand) {
        return static_cast<Locations>((static_cast<int>(Locations::P4Hand) + id) % 4);
    } else if(global_location == Locations::Table) {
        return Locations::Table;
    } else if(global_location == Locations::P1Bank) {
        return static_cast<Locations>((static_cast<int>(Locations::P1Bank) + id) % 4);
    } else if(global_location == Locations::P2Bank) {
        return static_cast<Locations>((static_cast<int>(Locations::P2Bank) + id) % 4);
    } else if(global_location == Locations::P3Bank) {
        return static_cast<Locations>((static_cast<int>(Locations::P3Bank) + id) % 4);
    } else if(global_location == Locations::P4Bank) {
        return static_cast<Locations>((static_cast<int>(Locations::P4Bank) + id) % 4);
	}
}
Locations convert_local_to_global(Locations local_location, PlayerID id) {
    if(local_location == Locations::P1Hand) {
        return static_cast<Locations>((static_cast<int>(Locations::P1Hand) - id + 4) % 4);
    } else if(local_location == Locations::P2Hand) {
        return static_cast<Locations>((static_cast<int>(Locations::P2Hand) - id + 4) % 4);
    } else if(local_location == Locations::P3Hand) {
        return static_cast<Locations>((static_cast<int>(Locations::P3Hand) - id + 4) % 4);
    } else if(local_location == Locations::P4Hand) {
        return static_cast<Locations>((static_cast<int>(Locations::P4Hand) - id + 4) % 4);
    } else if(local_location == Locations::Table) {
        return Locations::Table;
    } else if(local_location == Locations::P1Bank) {
        return static_cast<Locations>((static_cast<int>(Locations::P1Bank) - id + 4) % 4);
    } else if(local_location == Locations::P2Bank) {
        return static_cast<Locations>((static_cast<int>(Locations::P2Bank) - id + 4) % 4);
    } else if(local_location == Locations::P3Bank) {
        return static_cast<Locations>((static_cast<int>(Locations::P3Bank) - id + 4) % 4);
    } else if(local_location == Locations::P4Bank) {
        return static_cast<Locations>((static_cast<int>(Locations::P4Bank) - id + 4) % 4);
    }
}
Standings convert_global_to_local(Standings global_standings, PlayerID id) {
    Standings local_standings;
    for (int i = 0; i < 2; ++i) {
        local_standings[i] = global_standings[(i + id) % 2];
    }
    return local_standings;
}

class GameKnowlege {
public:
    GameKnowlege() {
        clear();
	}
    void give_cards_to_Location(const CardStack stack, Locations global_location, PlayerID id) {
		Location local_location = convert_global_to_local(global_location, id);
        while (stack) {
            int cardIndex = pop_lsb(stack);
			cardLocations[cardIndex] = local_location;
            Points[static_cast<int>(local_location)] += Points(static_cast<Cards>(cardIndex));
        }
    }
    void give_new_standings(const Standings new_standings, PlayerID id) {
        Standings local_standings = convert_global_to_local(new_standings,id);
        standings = local_standings;
	}
    void clear() {
        cardLocations.fill(Locations::Unkown);
        standings = { 0, 0 };
        points.fill(0);
        tichu.fill(false);
		grandTichu.fill(false);
	}
    void set_tichu(const Tichu new_tichu, PlayerID id) {
        for (size_t i = 0; i < 4; ++i) {
            tichu[i] = new_tichu[(i + id) % 4];
        }
    }
    void set_grand_tichu(const GrandTichu new_grand_tichu, PlayerID id) {
        for (size_t i = 0; i < 4; ++i) {
            grandTichu[i] = new_grand_tichu[(i + id) % 4];
        }
	}
private:
	CardLocations cardLocations={};
	Standings = { 0, 0 };
	Points points = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	Tichu tichu = { false, false, false, false };
	GrandTichu  grandTichu = { false, false, false, false };
    
};
class Player {
public:
	Player(PlayerID id) : currentPlayer(id) {}
    void set_current_player(PlayerID id) {
        currentPlayer = id;
        gameKnowlege.clear();
		CardStack hand = 0;
    }
    void ReceiveCardLocationChangeInfo(const CardStack stack, Locations global_location) {
        gameKnowlege.give_cards_to_Location(stack, global_location, currentPlayer);
        if (Locations::P1Hand == convert_global_to_local(global_location, currentPlayer)) {
			add_cards_to_hand(stack);
        }

	}
    void ReceiveTichuInfo(const Tichu tichu, const GrandTichu grandTichu) {
        
        gameKnowlege.set_tichu(tichu, currentPlayer);
        gameKnowlege.set_grand_tichu(grandTichu, currentPlayer);
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

private:
    PlayerID currentPlayer = 0;
	GameKnowlege gameKnowlege;
	CardStack hand = 0;
    void remove_cards_from_hand(const CardStack stack) {
        hand &= ~stack;
    }
    void add_cards_to_hand(const CardStack stack) {
        hand |= stack;
	}
};

std::vector<CardStack> generate_moves( const CardStack hand, const CardStack table_face, const Cards highest_card, const MoveType type) {
    std::vector<CardStack> moves = {0};
	int active_cards = popcount(table_face);
	int hand_cards = popcount(hand);
    if(hand_cards<active_cards) {
        return moves; // Not enough cards to play
	}
	Cards highest_hand_card = static_cast<Cards>(pop_lsb(hand));
    if(CardType(highest_hand_card) <= CardType(highest_card)) {
        return moves; // No card in hand can beat the highest card on the table
	}
    generate_Bombs(hand, highest_card, active_cards, moves, type);

    switch (type)
    case MoveType::Single: generate_Singles(hand, highest_card, moves);
    case MoveType::Pair: generate_Pairs(hand, highest_card, moves);
    case MoveType::FullHouse: generate_FullHouses(hand, highest_card, moves);
    case MoveType::Straight: generate_Straights(hand, highest_card, moves);
    case MoveType::PairSequence: generate_Straights_of_Pairs(hand, highest_card, moves);
    case MoveType::Triple: generate_Triples(hand, highest_card, moves);
    default: {
        generate_Singles(hand, highest_card, moves);
        generate_Pairs(hand, highest_card, moves);
        generate_FullHouses(hand, highest_card, moves);
        generate_Straigths(hand, highest_card, moves);
        generate_Straight_of_Pairs(hand, highest_card, moves);
        generate_Triples(hand, highest_card, moves);
    }
           return moves;

}
void generate_Singles(const CardStack hand, const Cards highest_card, std::vector<CardStack>& moves) {
    for (size_t i = next_trumping_card_index(highest_card); i < static_cast<CardType>(Cards::Count)&& i>-2; ++i) {
        if (hand & cardBit(static_cast<int>(i))) {
            moves.push_back(cardBit(static_cast<int>(i)));
        }
    }
}
void generate_Pairs(const CardStack hand, const Cards highest_card, std::vector<CardStack>& moves) {
    if (highest_card in{ Cards::Mahjong, Cards::Dog, Cards::Phoenix, Cards::Dragon }) {
        return; // Cannot form pairs with these cards
    }
    for (size_t i = next_trumping_card_index(highest_card); i < static_cast<CardType>(Cards::Count)&& i>-2; ++i) {

        Cards card1 = cardBit(static_cast<int>(i));
        for (size_t j = i + 1; j < std::min(i + 4, static_cast<size_t>(Cards::Count)); ++j) {
            Cards card2 = cardBit(static_cast<int>(j));
            if (CardType(card1) != CardType(card2)) {
                break; // Skip if the two cards are not of the same type
            }
            if ((hand & cardBit(card1) && (hand & cardBit(card2)))) {
                moves.push_back(cardBit(card1) | cardBit(card2));
            }
        }
    }
    if (hand & CardBit(Cards::Phoenix)) {
        std::vector<CardStack> singles = {};
        generate_Singles(hand,highest_card, singles);
        for (CardStack single : singles) {
            if(!(single & (cardBit(Cards::Phoenix)| CardBit(Cards::Dragon) | CardBit(Cards::Mahjong) | CardBit(Cards::Dog)))) {
                moves.push_back(single | cardBit(Cards::Phoenix));
            }
        }
    }
}
void generate_Triples(const CardStack hand, const Cards highest_card, std::vector<CardStack>& moves) {
    if (highest_card in{ Cards::Mahjong, Cards::Dog, Cards::Phoenix, Cards::Dragon }) {
        return; // Cannot form pairs with these cards
    }
    for (size_t i = next_trumping_card_index(highest_card); i < static_cast<CardType>(Cards::Count)&& i>-2; ++i) {
        Cards card1 = cardBit(static_cast<int>(i));
        for (size_t j = i + 1; j < std::min(i + 3, static_cast<size_t>(Cards::Count)); ++j) {
            Cards card2 = cardBit(static_cast<int>(j));
            if (CardType(card1) != CardType(card2)) {
                break; // Skip if the two cards are not of the same type
            }
            for (size_t k = j + 1; k < std::min(j + 2, static_cast<size_t>(Cards::Count)); ++k) {
                Cards card3 = cardBit(static_cast<int>(k));
                if (CardType(card1) != CardType(card3)) {
                    break; // Skip if the three cards are not of the same type
                }
                if ((hand & cardBit(card1) && (hand & cardBit(card2)) && (hand & cardBit(card3)))) {
                    moves.push_back(cardBit(card1) | cardBit(card2) | cardBit(card3));
                }
            }
        }
    }
    if (hand & CardBit(Cards::Phoenix)) {
        std::vector<CardStack> pairs = {};
        generate_Pairs(hand & ~CardBit(Cards::Phoenix), highest_card, pairs);
        for (CardStack pair : pairs) {
            moves.push_back(pair | CardBit(Cards::Phoenix));
        }
    }

}
void generate_FullHouses(const CardStack hand, const Cards highest_card, std::vector<CardStack>& moves) {
    std::vector<CardStack> pairs;
	generate_Pairs(hand, Cards::Mahjong, pairs);
    if(pairs.empty()) {
        return; // No pairs available, cannot form a full house
	}
	std::vector<CardStack> triples;
	generate_Triples(hand, highest_card, triples);
    for(CardStack triple : triples) {
        for(CardStack pair : pairs) {
            if((triple & pair) == 0) { // Ensure the triple and pair do not overlap
                moves.push_back(triple | pair);
            }
        }
	}

}
std::vector<CardStack> append_Straight_card(const CardStack hand, const Cards highest_card, int length) {
    if(length <= 0) {
        return {0}; // Base case: return an empty straight
	}
    std::vector<CardStack> moves = {};
    if (highest_card in{ Cards::Mahjong, Cards::Dog, Cards::Phoenix, Cards::Dragon }) {
        return {}; // Cannot form pairs with these cards
    }
    for(size_t i=next_trumping_card_index(highest_card); i < next_trumping_card_index+4; ++i) {
		Cards card_new = static_cast<Cards>(i);
        if(hand & cardBit(static_cast<int>(i))) {
            std::vector<CardStack> sub_straights = append_Straight_card(hand, static_cast<Cards>(i), length - 1);
            for(CardStack sub_straight : sub_straights) {
                moves.push_back(cardBit(static_cast<int>(i)) | sub_straight);
            }
        }
	}
    if (hand & cardBit(static_cast<int>(Cards::Phoenix))) {
        std::vector<CardStack> sub_straights = append_Straight_card(hand & ~cardBit(static_cast<int>(Cards::Phoenix)), static_cast<Cards>(i), length - 1);
        for (CardStack sub_straight : sub_straights) {
            moves.push_back(cardBit(static_cast<int>(i)) | cardBit(static_cast<int>(Cards::Phoenix)) | sub_straight);
        }
    }

	return moves;
}
std::vector<CardStack> append_Straight_pairs(const CardStack hand, const Cards highest_card, int length) {
    if (length <= 0) {
        return { 0 }; // Base case: return an empty straight
    }
    std::vector<CardStack> moves = {};
    if (highest_card in{ Cards::Mahjong, Cards::Dog, Cards::Phoenix, Cards::Dragon }) {
        return {}; // Cannot form pairs with these cards
    }
    for (size_t i = next_trumping_card_index(highest_card); i < next_trumping_card_index(highest_card)+4; ++i) {
        phoenix_used = false;
        Cards card_new1 = static_cast<Cards>(i);
        if (!(hand & cardBit(static_cast<int>(i)))) {
            continue;
        }
        for (size_t j = i + 1; j < i + 4; ++j) {
            Cards card_new2=static_cast<Cards>(j);
            if (!(hand & cardBit(static_cast<int>(j)))) {
                    continue;
                }
            std::vector<CardStack> sub_straights = append_Straight_pairs(hand, card2_new, length - 1);
            for (CardStack sub_straight : sub_straights) {
                moves.push_back(cardBit(card_new1) | cardBit(card_new2) | sub_straight);
            }
        }
        if (hand & cardBit(Cards::Phoenx)) {
            std::vector<CardStack> sub_straights = append_Straight_pairs(hand & ~cardBit(Cards::Phonex), card_new1, length - 1);
            for (CardStack sub_straight : sub_straights) {
                moves.push_back(CardBit(card_new1) | cardBit(Cards::Phoenix) | sub_straight);
            }
        }
    }
    for (size_t j = next_trumping_card_index(highest_card); j < next_trumping_card_index(highest_card) + 4; ++j) {
        Cards card_new2 = static_cast<Cards>(j);
        if (!(hand & cardBit(static_cast<int>(j)))) {
            continue;
        }
        std::vector<CardStack> sub_straights = append_Straight_pairs(hand & ~cardBit(Cards::Phoenix), card_new2, length - 1);
        for (CardStack sub_straight : sub_straights) {
            moves.push_back(cardBit(Cards::Phoenix) | cardBit(card_new2) | sub_straight);
        }
    }

    return moves;
}

void generate_Straights(const CardStack hand, const Cards highest_card, std::vector<CardStack>& moves, int length, bool same_color=false) {
    std::vector<CardStack> straights = append_Straight_card(hand, highest_card, length);
    for (CardStack straight : straights) {
        if (popcount(straight) == length) {
            if (same_color) {
                if (All_Same_color(straight)) {
                    moves.pop_back(straight);
                }
                else {
                    if (!All_Same_color(straight)) {
                        moves.push_back(straight);
                    }
                }
            }
        }
    }
}
void generate_Straights_of_Pairs(const CardStack hand, const Cards highest_card, std::vector<CardStack>& moves, int length) {
    std::vector<CardStack> straights = append_Straight_pairs(hand, highest_card, length);
    for (CardStack straight : straights) {
        if (popcount(straight) == length * 2) {
            moves.push_back(straight);
        }
    }
}
void generate_4Bombs(const CardStack hand, const Cards highest_card, std::vector<CardStack>& moves) {
    CardStack 4BombMask = (1ull << 3) | (1ull << 4) | (1ull << 5) | (1ull << 6);
    CardStack mask = 4BombMask;
    for (int i = 0; i < 12; ++i) {
        mask = mask << i;
        if (hand & mask) {
            moves.push_back(mask);
        }
    }
}
void generate_straight_Bombs(const CardStack hand, const Cards highest_card, std::vector<CardStack>& moves, int length) {
    if (length < 5) return;
    generate_Straights(hand, highest_card, moves, length, true);
}
void generate_Bombs(const CardStack hand, const Cards highest_cards,int table_length,std::vector<CardStack>& moves,const MoveType type) {
    if (type == MoveType::Bomb) {
        if (table_length == 4) generate_4Bombs(hand, highest_cards, moves);
        for (int i = table_length; i < 14; ++i) {
            if (i == table_length) generate_straight_Bombs(hand, highest_cards, moves, i);
            else generate_straight_Bombs(hand, Cards::Dog, moves, i);
        }
    }
    else {
        generate_4Bombs(hand, Cards::Dog, moves);
        for (int i = 5; i < 14; ++i) {
            generate_straight_Bombs(hand, Cards::Dog, moves, i);
        }
    }
}