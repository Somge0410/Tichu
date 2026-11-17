#pragma once

#include "types.h"
#include "cards.h"
#include <algorithm>
#include <bitset>
constexpr CardStack notPhoenixMask = ~(cardBit(Cards::Phoenix) | LAST_FOUR_BITS_MASK);
void generate_Singles(const CardStack& hand, std::vector<MoveInfo>& moves) {
    for (int i = static_cast<int>(Cards::Mahjong); i < static_cast<int>(Cards::Count); ++i) {
        const Cards card = static_cast<Cards>(i);
        if (hand & cardBit(card)) {
            moves.push_back({ cardBit(card), MoveType::Single, get_beat_value(card), 1 });
        }
    }
    if (hand & cardBit(Cards::Phoenix)) {
        moves.push_back({ cardBit(Cards::Phoenix), MoveType::Single, Beat_Value::Phoenix, 1 });
    }
}
void generate_Pairs(const CardStack& hand,std::vector<MoveInfo>& moves) {
    for (int i = 3; i < 55; ++i) {
        Cards card1 = static_cast<Cards>(i);
        for (int j = i + 1; j < std::min(i + 4, static_cast<int>(Cards::Count)); ++j) {
            Cards card2 = static_cast<Cards>(j);
            if (get_CardType(card1) != get_CardType(card2)) {
                break; // Skip if the two cards are not of the same type
            }
            if ((hand & cardBit(card1) && (hand & cardBit(card2)))) {
                moves.push_back({ cardBit(card1) | cardBit(card2), MoveType::Pair, get_beat_value(card1), 2 });
            }
        }
    }
    if (hand & cardBit(Cards::Phoenix)) {
        std::vector<MoveInfo> singles = {};
        generate_Singles(hand, singles);
        for (MoveInfo single : singles) {
            if(!(single.cards & (cardBit(Cards::Phoenix)| cardBit(Cards::Dragon) | cardBit(Cards::Mahjong) | cardBit(Cards::Dog)))) {

                moves.push_back({ single.cards | cardBit(Cards::Phoenix) | push_CardType_To_last_bits(single.cards), MoveType::Pair, single.beat_value, 2 });
            }
        }
    }
}
void generate_Triples(const CardStack& hand, std::vector<MoveInfo>& moves) {
    for (int i = 3; i < 55; ++i) {
        Cards card1 = static_cast<Cards>(i);
        for (int j = i + 1; j < std::min(i + 3, static_cast<int>(Cards::Count)); ++j) {
            Cards card2 = static_cast<Cards>(j);
            if (get_CardType(card1) != get_CardType(card2)) {
                break; // Skip if the two cards are not of the same type
            }
            for (int k = j + 1; k < std::min(j + 2, static_cast<int>(Cards::Count)); ++k) {
                Cards card3 = static_cast<Cards>(k);
                if (get_CardType(card1) != get_CardType(card3)) {
                    break; // Skip if the three cards are not of the same type
                }
                if ((hand & cardBit(card1) && (hand & cardBit(card2)) && (hand & cardBit(card3)))) {
                    moves.push_back({ cardBit(card1) | cardBit(card2) | cardBit(card3), MoveType::Triple, get_beat_value(card1), 3 });
                }
            }
        }
    }
    if (hand & cardBit(Cards::Phoenix)) {
        std::vector<MoveInfo> pairs = {};
        generate_Pairs(hand & ~cardBit(Cards::Phoenix), pairs);
        for (MoveInfo pair : pairs) {
            moves.push_back({ pair.cards | cardBit(Cards::Phoenix) | push_CardType_To_last_bits(pair.cards), MoveType::Triple, pair.beat_value, 3 });
        }
    }

}
void generate_FullHouses(const CardStack hand, std::vector<MoveInfo>& moves) {
    std::vector<MoveInfo> pairs;
    generate_Pairs(hand, pairs);
    if(pairs.empty()) {
        return; // No pairs available, cannot form a full house
    }
    std::vector<MoveInfo> triples;
    generate_Triples(hand, triples);
    for(MoveInfo triple : triples) {
        for(MoveInfo pair : pairs) {
            if((triple.cards & pair.cards) == 0) { // Ensure the triple and pair do not overlap
                moves.push_back({triple.cards | pair.cards, MoveType::FullHouse, triple.beat_value, 5 });
            }
        }
    }

}

std::vector<CardStack> append_Straight_from_vector(const std::vector<MoveInfo>& vector, const Cards highest_card, int length, bool phoenix_available) {
    if (length <= 0) {
        return { 0 }; // Base case: return an empty straight
    }
    std::vector<CardStack> moves = {};
    if (highest_card == Cards::Mahjong ||
        highest_card == Cards::Phoenix ||
        highest_card == Cards::Dragon) {
        return {};
    }
	std::vector<CardStack> subs;
    for (const MoveInfo& move : vector) {
        const CardStack element = move.cards;
		bool contains_phoenix = (element & cardBit(Cards::Phoenix)) != 0;
        if(!phoenix_available && contains_phoenix) {
            continue; // Skip if Phoenix is not available and the element contains Phoenix
		}
        CardStack element_copy = element;
        CardStack Bits = 0;
        if (contains_phoenix) {
            element_copy &= notPhoenixMask; // Remove Phoenix and its metadata from the element copy
        }
        Cards next_highest;
        if(element_copy) {
            next_highest = static_cast<Cards>(pop_lsb(element_copy));
        }
        else {
            next_highest = static_cast<Cards>(next_trumping_card_index(highest_card));
		}
        if(contains_phoenix) {
            Bits = push_CardType_To_last_bits(next_highest);
		}
		int next_value = static_cast<int>(get_CardType(next_highest));
		int highest_value = static_cast<int>(get_CardType(highest_card));
        if(next_value!=highest_value+1) {
            continue; // Skip if the pair's highest card is not greater than the highest card on the table
        }
		subs=append_Straight_from_vector(vector, next_highest, length - 1,!contains_phoenix);
        for(CardStack sub_pair : subs) {

            moves.push_back((element & ~LAST_FOUR_BITS_MASK) | sub_pair | Bits);
		}
	}

    return moves;
}

void generate_Straights(const CardStack hand, std::vector<MoveInfo>& moves, int length, bool same_color=false) {
    std::vector<MoveInfo> singles;
    generate_Singles(hand, singles);
    if (static_cast<int>(singles.size()) < length) {
        return; // Not enough singles to form a straight
    }
    const MoveType type = same_color ? MoveType::Bomb : MoveType::Straight;
    std::vector<MoveInfo> straights;
    for (const MoveInfo& single : singles) {
        if (single.cards & cardBit(Cards::Phoenix)) {
            for (int rank = 2; rank <= 15 - length; ++rank) {
                const Cards first_card = static_cast<Cards>(3 + 4 * (rank - 2));
                const auto sub_straights = append_Straight_from_vector(singles, first_card, length - 1, false);
                for (CardStack sub_straight : sub_straights) {
                    const CardStack cards = single.cards | sub_straight | push_CardType_To_last_bits(first_card);
                    straights.push_back({ cards, type, get_beat_value(first_card), length });
                }
            }
        }
        else {
            const Cards first_card = static_cast<Cards>(lsb(single.cards));
            const auto sub_straights = append_Straight_from_vector(singles, first_card, length - 1, true);
            for (CardStack sub_straight : sub_straights) {
                straights.push_back({ single.cards | sub_straight, type, single.beat_value, length });
            }
        }
    }

    for (const MoveInfo& straight : straights) {
        if (popcount(straight.cards & ~LAST_FOUR_BITS_MASK) == length &&
            All_Same_color(straight.cards) == same_color) {
            moves.push_back(straight);
        }
    }
}
void generate_Straights_of_Pairs(const CardStack hand, std::vector<MoveInfo>& moves, int length) {
    std::vector<MoveInfo> pairs;
    generate_Pairs(hand, pairs);
    if (static_cast<int>(pairs.size()) < length) {
        return; // Not enough pairs to form a straight of pairs
    }
    for (const MoveInfo& pair : pairs) {
        const bool contains_phoenix = (pair.cards & cardBit(Cards::Phoenix)) != 0;
        const Cards first_card = static_cast<Cards>(lsb(pair.cards & notPhoenixMask));
        const auto sub_straights = append_Straight_from_vector(pairs, first_card, length - 1, !contains_phoenix);
        for (CardStack sub_straight : sub_straights) {
            const CardStack cards = pair.cards | sub_straight;
            if (popcount(cards & ~LAST_FOUR_BITS_MASK) == length * 2) {
                moves.push_back({ cards, MoveType::PairSequence, pair.beat_value, length * 2 });
            }
        }
    }
}
void generate_4Bombs(const CardStack hand, std::vector<MoveInfo>& moves) {
    const CardStack BombMask4 = (1ull << 3) | (1ull << 4) | (1ull << 5) | (1ull << 6);
    for (int i = 0; i < 52; i += 4) {
        const CardStack mask = BombMask4 << i;
        if ((hand & mask) == mask) {
            moves.push_back({ mask, MoveType::Bomb, get_beat_value(static_cast<Cards>(i + 3)), 4 });
        }
    }
}
void generate_straight_Bombs(const CardStack hand, std::vector<MoveInfo>& moves, int length) {
    if (length < 5) return;
    generate_Straights(hand, moves, length, true);
}
void generate_Bombs(const CardStack hand, std::vector<MoveInfo>& moves) {
    generate_4Bombs(hand, moves);
    const int hand_cards = popcount(hand);
    for (int length = 5; length <= hand_cards; ++length) {
        generate_straight_Bombs(hand, moves, length);
    }
}
std::vector<MoveInfo> generate_moves(const CardStack hand) {
    std::vector<MoveInfo> moves = { {} };

    if (hand & cardBit(Cards::Dog)) {
        moves.push_back({ cardBit(Cards::Dog), MoveType::Single, Beat_Value::Dog, 1 });
    }
    generate_Bombs(hand, moves);
    generate_Singles(hand, moves);
    generate_Pairs(hand, moves);
    generate_FullHouses(hand, moves);
    generate_Triples(hand, moves);
    const int hand_cards = popcount(hand);
    for (int length = 5; length <= hand_cards; ++length) {
        generate_Straights(hand, moves, length);
    }
    for (int length = 2; length * 2 <= hand_cards; ++length) {
        generate_Straights_of_Pairs(hand, moves, length);
    }
    return moves;
}

