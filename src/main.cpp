#include "types.h"
#include "cards.h"
#include "game_state.h"
#include "move_generation.h"

#include <iostream>
#include <bitset>

int main() {
    CardStack hand = 0;
    addCard(hand, Cards::Phoenix);
    addCard(hand, Cards::Blue3);
    addCard(hand, Cards::Black4);
    addCard(hand, Cards::Blue5);
	addCard(hand, Cards::Black6);
    addCard(hand, Cards::Blue6);/*
    addCard(hand, Cards::Black10);
    addCard(hand, Cards::Green10);
    addCard(hand, Cards::Black7);
    addCard(hand, Cards::Red10);
    addCard(hand, Cards::Green10);
    addCard(hand, Cards::GreenAce);
    addCard(hand, Cards::BlackAce);
    addCard(hand, Cards::RedAce);
    addCard(hand, Cards::Dragon);*/
    print(hand);
    std::vector<CardStack> moves;
	generate_Pairs(hand, Cards::Red2, moves);
    std::cout << "Generated moves: " << moves.size() << '\n';
    for (CardStack move : moves) {
        std::cout << move << '\n';
        print(move);
    }
}
