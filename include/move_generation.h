#pragma once

#include "types.h"
#include "cards.h"
#include <bitset>
void generate_Singles(const CardStack hand, const Cards highest_card, std::vector<CardStack>& moves) {
    for (int i = next_trumping_card_index(highest_card); (i < static_cast<int>(Cards::Count)) && i>-2; ++i) {
        if (i != static_cast<int>(Cards::Phoenix) &&
            (hand & cardBit(i))) {
            moves.push_back(cardBit(i));
        }
    }
    if (highest_card != Cards::Dragon) {
        if (hand & cardBit(Cards::Phoenix)) {
			int highest_index = static_cast<int>(get_CardType(highest_card));
            CardStack bits = (static_cast<CardStack>(highest_index) & 0xFULL) << 60;
            moves.push_back(cardBit(Cards::Phoenix)| bits);
		}
    }
}
void generate_Pairs(const CardStack hand, const Cards highest_card, std::vector<CardStack>& moves) {
    if (highest_card == Cards::Mahjong ||
        highest_card == Cards::Phoenix ||
        highest_card == Cards::Dragon) {
        return;
    }
    int k = next_trumping_card_index(highest_card);
    int j = static_cast<int>(Cards::Count);
    for (int i = next_trumping_card_index(highest_card); (i < static_cast<int>(Cards::Count)) && i>-2; ++i) {
        if (highest_card == Cards::Mahjong ||
            highest_card == Cards::Phoenix ||
            highest_card == Cards::Dragon) {
            continue;
        }
        Cards card1 = static_cast<Cards>(i);
        for (int j = i + 1; j < std::min(i + 4, static_cast<int>(Cards::Count)); ++j) {
            Cards card2 = static_cast<Cards>(j);
            if (get_CardType(card1) != get_CardType(card2)) {
                break; // Skip if the two cards are not of the same type
            }
            if ((hand & cardBit(card1) && (hand & cardBit(card2)))) {
                moves.push_back(cardBit(card1) | cardBit(card2));
            }
        }
    }
    if (hand & cardBit(Cards::Phoenix)) {
        std::vector<CardStack> singles = {};
        generate_Singles(hand,highest_card, singles);
        for (CardStack single : singles) {
            if(!(single & (cardBit(Cards::Phoenix)| cardBit(Cards::Dragon) | cardBit(Cards::Mahjong) | cardBit(Cards::Dog)))) {

                moves.push_back(single | cardBit(Cards::Phoenix) | push_CardType_To_last_bits(single));
            }
        }
    }
}
void generate_Triples(const CardStack hand, const Cards highest_card, std::vector<CardStack>& moves) {
    if (highest_card == Cards::Mahjong ||
        highest_card == Cards::Phoenix ||
        highest_card == Cards::Dragon) {
        return;
    }
    for (int i = next_trumping_card_index(highest_card); i < static_cast<int>(Cards::Count)&& i>-2; ++i) {
        if (highest_card == Cards::Mahjong ||
            highest_card == Cards::Phoenix ||
            highest_card == Cards::Dragon) {
            continue;
        }
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
                    moves.push_back(cardBit(card1) | cardBit(card2) | cardBit(card3));
                }
            }
        }
    }
    if (hand & cardBit(Cards::Phoenix)) {
        std::vector<CardStack> pairs = {};
        generate_Pairs(hand & ~cardBit(Cards::Phoenix), highest_card, pairs);
        for (CardStack pair : pairs) {
            moves.push_back(pair | cardBit(Cards::Phoenix) | push_CardType_To_last_bits(pair));
        }
    }

}
void generate_FullHouses(const CardStack hand, const Cards highest_card, std::vector<CardStack>& moves) {
    std::vector<CardStack> pairs;
    generate_Pairs(hand, Cards::Dog, pairs);
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

std::vector<CardStack> append_Straight_from_vector(const std::vector<CardStack> vector, const Cards highest_card, int length, bool phoenix_available) {
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
    for(CardStack element : vector) {
		bool contains_phoenix = (element & cardBit(Cards::Phoenix)) != 0;
        if(!phoenix_available && contains_phoenix) {
            continue; // Skip if Phoenix is not available and the element contains Phoenix
		}
        CardStack element_copy = element;
        CardStack Bits = 0;
        if (contains_phoenix) {
            element_copy &= ~(cardBit(Cards::Phoenix) | LAST_FOUR_BITS_MASK); // Remove Phoenix and its metadata from the element copy
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

void generate_Straights(const CardStack hand, const Cards highest_card, std::vector<CardStack>& moves, int length, bool same_color=false) {
    std::vector<CardStack> straights = {};
	std::vector<CardStack> singles;
	generate_Singles(hand, highest_card, singles);
    if(singles.size() < length) {
        return; // Not enough singles to form a straight
	}
    for (CardStack single : singles) {
        CardStack single_copy = single;
        if (single & cardBit(Cards::Phoenix)) {
			int first_rank = std::max(2, static_cast<int>(get_CardType(highest_card)) + 1);
			int last_rank = 15 - length;
          for(int rank=first_rank;rank<=last_rank;rank++) {
                Cards next_highest = static_cast<Cards>(3+4*(rank-2));
                if (next_highest == Cards::Dragon || next_highest==Cards::Dog) break;
                std::vector<CardStack> sub_straights = append_Straight_from_vector(singles, next_highest, length - 1, false);
                for(CardStack sub_straight : sub_straights) {
                    straights.push_back(
                        (single & ~LAST_FOUR_BITS_MASK) |
                        sub_straight |
                        push_CardType_To_last_bits(next_highest)
                    );
                }
		  }
        }
        else {
            Cards next_highest = static_cast<Cards>(pop_lsb(single_copy));
            std::vector<CardStack> sub_straights = append_Straight_from_vector(singles, next_highest, length - 1, true);
            for (CardStack sub_straight : sub_straights) {
                straights.push_back(single | sub_straight);
            }
        }
    }

    for (CardStack straight : straights) {
        if (popcount(straight&~LAST_FOUR_BITS_MASK) == length) {
            if (same_color) {
                if (All_Same_color(straight)) {
                    moves.push_back(straight);
                }
            }
            else {
                    if (!All_Same_color(straight)) {
                        moves.push_back(straight);
                    }
                }
        }
    }
}
void generate_Straights_of_Pairs(const CardStack hand, const Cards highest_card, std::vector<CardStack>& moves, int length) {
	std::vector<CardStack> pairs;
	std::vector<CardStack> straights_of_pairs;
	generate_Pairs(hand,highest_card, pairs);
	if(pairs.size() < length) {
        return; // Not enough pairs to form a straight of pairs
	}
    for(CardStack pair : pairs) {

        bool contains_phoenix = (pair & cardBit(Cards::Phoenix)) != 0;
        CardStack pair_copy = pair & ~ cardBit(Cards::Phoenix);
        Cards next_highest = static_cast<Cards>(pop_lsb(pair_copy));
        
        if(get_CardType(next_highest) <= get_CardType(highest_card)) {
            continue; // Skip if the pair's highest card is not greater than the highest card on the table
		}
        std::vector<CardStack> sub_straights = append_Straight_from_vector(pairs, next_highest, length - 1,!contains_phoenix);
        for(CardStack sub_straight : sub_straights) {
            straights_of_pairs.push_back(pair | sub_straight);
		}
	}
    for (CardStack straight_of_pairs : straights_of_pairs) {
        if (popcount(straight_of_pairs & ~LAST_FOUR_BITS_MASK) == length * 2) {
            moves.push_back(straight_of_pairs);
        }
    }
}
void generate_4Bombs(const CardStack hand, const Cards highest_card, std::vector<CardStack>& moves) {
    CardStack BombMask4 = (1ull << 3) | (1ull << 4) | (1ull << 5) | (1ull << 6);
    CardStack mask = BombMask4;
	int index = next_trumping_card_index(highest_card)-3;
    for (int i = std::max(index,0); i < 52; i=i+4) {
        mask = BombMask4 << i;
        if ((hand & mask)==mask) {
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
std::vector<CardStack> generate_moves(const CardStack hand, const CardStack table_face, const Cards highest_card, const MoveType type) {
    std::vector<CardStack> moves = { 0 };
    int active_cards = popcount(table_face);
    int hand_cards = popcount(hand);
    if (hand_cards < active_cards) {
        return moves; // Not enough cards to play
    }
    if (table_face == 0 && (hand & cardBit(Cards::Dog))) {
        moves.push_back(cardBit(Cards::Dog));
    }
    generate_Bombs(hand, highest_card, active_cards, moves, type);

    switch (type) {
    case MoveType::Single:
        generate_Singles(hand, highest_card, moves);
        break;
    case MoveType::Pair:
        generate_Pairs(hand, highest_card, moves);
        break;
    case MoveType::FullHouse:
        generate_FullHouses(hand, highest_card, moves);
        break;
    case MoveType::Straight:
        generate_Straights(hand, highest_card, moves, active_cards);
        break;
    case MoveType::PairSequence:
        generate_Straights_of_Pairs(hand, highest_card, moves, active_cards / 2);
        break;
    case MoveType::Triple:
        generate_Triples(hand, highest_card, moves);
        break;
    case MoveType::Bomb:
        break;
    case MoveType::NotDefined:
    default: {
        generate_Singles(hand, highest_card, moves);
        generate_Pairs(hand, highest_card, moves);
        generate_FullHouses(hand, highest_card, moves);
        generate_Triples(hand, highest_card, moves);
        for (int length = 5; length <= hand_cards; ++length) {
            generate_Straights(hand, highest_card, moves, length);
        }
        for (int length = 2; length * 2 <= hand_cards; ++length) {
            generate_Straights_of_Pairs(hand, highest_card, moves, length);
        }
        break;
    }
    }
    return moves;

}

