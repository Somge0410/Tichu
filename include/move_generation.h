#pragma once

#include "types.h"
#include "cards.h"
#include <vector>
constexpr CardStack notPhoenixMask = ~(cardBit(Cards::Phoenix) | LAST_FOUR_BITS_MASK);
bool beats(const MoveInfo& move, const MoveInfo& table_move);
void generate_Singles(const CardStack& hand, std::vector<MoveInfo>& moves);
void generate_Pairs(const CardStack& hand,std::vector<MoveInfo>& moves);
void generate_Triples(const CardStack& hand, std::vector<MoveInfo>& moves);
void generate_FullHouses(const CardStack hand, std::vector<MoveInfo>& moves);

std::vector<CardStack> append_Straight_from_vector(const std::vector<MoveInfo>& vector, const Cards highest_card, int length, bool phoenix_available);

void generate_Straights(const CardStack hand, std::vector<MoveInfo>& moves, int length, bool same_color=false);
void generate_Straights_of_Pairs(const CardStack hand, std::vector<MoveInfo>& moves, int length);
void generate_4Bombs(const CardStack hand, std::vector<MoveInfo>& moves);
void generate_straight_Bombs(const CardStack hand, std::vector<MoveInfo>& moves, int length);
void generate_Bombs(const CardStack hand, std::vector<MoveInfo>& moves);
std::vector<MoveInfo> generate_moves(const CardStack hand);
