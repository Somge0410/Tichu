#pragma once
#include "types.h"

#if defined(_MSC_VER) && defined(_M_X64)
#include <intrin.h>
#endif

constexpr uint64_t LAST_FOUR_BITS_MASK = uint64_t{ 0xF } << 60;
constexpr CardStack CARD_BITS_MASK = (CardStack{ 1 } << static_cast<int>(Cards::Count)) - 1;

namespace card_bits {
constexpr int popcount_fallback(CardStack stack) {
    stack -= (stack >> 1) & CardStack{0x5555555555555555ULL};
    stack = (stack & CardStack{0x3333333333333333ULL}) +
        ((stack >> 2) & CardStack{0x3333333333333333ULL});
    stack = (stack + (stack >> 4)) & CardStack{0x0F0F0F0F0F0F0F0FULL};
    return static_cast<int>((stack * CardStack{0x0101010101010101ULL}) >> 56);
}

#if defined(_MSC_VER) && !defined(__clang__) && defined(_M_X64)
int popcount_runtime(CardStack stack) noexcept;
#endif
}

constexpr int lsb(const CardStack& stack) {
    if (stack == 0) return -1;
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_ctzll(stack);
#else
#if defined(_MSC_VER) && defined(_M_X64) && _MSC_VER >= 1925
    if (!__builtin_is_constant_evaluated()) {
        unsigned long index = 0;
        _BitScanForward64(&index, stack);
        return static_cast<int>(index);
    }
#endif
    int lsb = 0;
    CardStack bit = 1;
    while ((stack & bit) == 0) {
        bit <<= 1;
        ++lsb;
    }
    return lsb;
#endif
}
constexpr int pop_lsb(CardStack& stack) {
    const int index = lsb(stack);
    stack &= stack - 1;
    return index;
}
constexpr int popcount(CardStack stack) {
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_popcountll(stack);
#else
#if defined(_MSC_VER) && defined(_M_X64) && _MSC_VER >= 1925
    if (!__builtin_is_constant_evaluated()) {
        return card_bits::popcount_runtime(stack);
    }
#endif
    return card_bits::popcount_fallback(stack);
#endif
}

Color color(Cards  card);

constexpr CardSet cardBit(unsigned index) {
    return CardSet{ 1 } << index;
}
constexpr CardSet cardBit(Cards card) {
    return cardBit(static_cast<int>(card));
}
CardType get_CardType(Cards card);
CardType get_CardType(CardStack stack);
CardStack push_CardType_To_last_bits(CardStack stack);
CardStack push_CardType_To_last_bits(Cards card);
Cards get_card_from_stack(CardStack stack);
int get_Points(Cards card);
int get_Points(CardStack stack);
int Index(Cards card);
int next_trumping_card_index(Cards card);
int second_next_trump_index(Cards card);

constexpr bool operator<=(CardType lhs, CardType rhs) {
    return static_cast<int>(lhs) <= static_cast<int>(rhs);
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
bool All_Same_color(CardSet card_set);
bool contains_rank(CardStack cards, CardType rank);
void addCard(CardStack& stack, Cards card);
void removeCard(CardStack& stack, Cards card);


void print(Cards card, uint64_t last_four_bits=0);
void print(CardStack stack);
void print(const MoveInfo& move);

Beat_Value get_beat_value(Cards card);
Beat_Value get_beat_value(CardStack stack);
