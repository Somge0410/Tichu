#include "cards.h"

#include <iostream>
#include <stdexcept>
#include <string>

#if defined(_MSC_VER) && !defined(__clang__) && defined(_M_X64)
namespace card_bits {
namespace {
const bool has_popcnt = [] {
    int cpu_info[4] = {};
    __cpuid(cpu_info, 1);
    return (cpu_info[2] & (1 << 23)) != 0;
}();
}

int popcount_runtime(CardStack stack) noexcept {
    return has_popcnt ? static_cast<int>(__popcnt64(stack)) : popcount_fallback(stack);
}
}
#endif

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

CardType get_CardType(Cards card) {
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

CardType get_CardType(CardStack stack) {
    if(stack & cardBit(Cards::Phoenix)) {
        return static_cast<CardType>((stack & LAST_FOUR_BITS_MASK)>>60); // Return Phoenix if present in the stack
	}
    int lsb = pop_lsb(stack);
    Cards card = static_cast<Cards>(lsb);

    return get_CardType(card);
}

CardStack push_CardType_To_last_bits(CardStack stack) {
    CardType card_type = get_CardType(stack);
    return (static_cast<CardStack>(card_type) << 60);
}

CardStack push_CardType_To_last_bits(Cards card) {
	return push_CardType_To_last_bits(cardBit(card));
}

Cards get_card_from_stack(CardStack stack) {
    if (popcount(stack) != 1) {
        throw std::invalid_argument("Stack contains more than one card, cannot determine a single card.");
	}
    int lsb = pop_lsb(stack);
	return static_cast<Cards>(lsb);
}

int get_Points(Cards card) {
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
        CardType cardValue = get_CardType(card);
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

int get_Points(CardStack stack) {
    int total_points = 0;
    while (stack) {
        int lsb = pop_lsb(stack);
        Cards card = static_cast<Cards>(lsb);
        total_points += get_Points(card);
    }
    return total_points;
}

int Index(Cards card) {
    return static_cast<int>(card);
}

int next_trumping_card_index(Cards card) {
    if(card == Cards::Dragon) {
        return -2; // No card can trump the Dragon
    }
    else if (card == Cards::Phoenix) {
        return Index(Cards::Mahjong); // The Dragon trumps the Phoenix
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
    Cards new_card = static_cast<Cards>(next_trumping_card_index(card));
    return next_trumping_card_index(new_card);
}

bool All_Same_color(CardSet card_set) {
    if (!card_set) return true;
    int lsb = pop_lsb(card_set);
    Color first_color = color(static_cast<Cards>(lsb));
    while (card_set) {
        lsb = pop_lsb(card_set);
        if (first_color != color(static_cast<Cards>(lsb))) return false;
    }
    return true;
}

bool contains_rank(CardStack cards, CardType rank) {
    const int value = static_cast<int>(rank);
    if (value < 2 || value > 14) return false;
    const CardStack mask = CardStack{ 0xF } << (3 + 4 * (value - 2));
    return (cards & mask) != 0;
}

void addCard(CardStack& stack, Cards card) {
    int cardValue = static_cast<int>(card);
    stack |= (1ULL << cardValue);
}

void removeCard(CardStack& stack, Cards card) {
    int cardValue = static_cast<int>(card);
    stack &= ~(1ULL << cardValue);
}

void print(Cards card, uint64_t last_four_bits) {
    Color color_card = color(card);
	CardType card_type = get_CardType(card);
    std::string color_str;
    switch (color_card) {
        case Color::Blue: color_str = "Blue"; break;
        case Color::Green: color_str = "Green"; break;
        case Color::Red: color_str = "Red"; break;
        case Color::Black: color_str = "Black"; break;
        default: color_str = ""; break;
    }
    std::string type_str;
    switch (card_type) {
        case CardType::Dog: type_str = "Dog"; break;
        case CardType::Phoenix: type_str = "Phoenix=" + std::to_string(last_four_bits >> 60); break;
        case CardType::Mahjong: type_str = "Mahjong"; break;
        case CardType::Dragon: type_str = "Dragon"; break;
		case CardType::Ace: type_str = "Ace"; break;
		case CardType::King: type_str = "King"; break;
		case CardType::Queen: type_str = "Queen"; break;
		case CardType::Jack: type_str = "Jack"; break;
        default: type_str = std::to_string(static_cast<int>(card_type)); break;
    }
	std::cout << "{ " << color_str << " " << type_str << " }" << std::endl;
}

void print(CardStack stack) {
	std::cout << "{ ";
	CardStack without_last_four_bits = stack & ~LAST_FOUR_BITS_MASK;
	CardStack last_four_bits = stack & LAST_FOUR_BITS_MASK;
    while(without_last_four_bits){
        int lsb = pop_lsb(without_last_four_bits);
		print(static_cast<Cards>(lsb),last_four_bits);
	}
	std::cout << " }" << std::endl;
    return;
}

void print(const MoveInfo& move) {
    if (move.type == MoveType::Single && (move.cards & cardBit(Cards::Phoenix))) {
        std::cout << "{ { Phoenix=" << static_cast<int>(move.beat_value) << ".5 } }" << std::endl;
        return;
    }
    print(move.cards);
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
