#include "types.h"
#include "cards.h"
#include "game_state.h"
#include "move_generation.h"

#include <iostream>
#include <bitset>

int main() {
    CardStack hand = 0;
    addCard(hand, Cards::Mahjong);
    addCard(hand, Cards::Red2);
    addCard(hand, Cards::Green4);
	addCard(hand, Cards::Blue3);
    addCard(hand, Cards::Blue4);
    addCard(hand, Cards::Green5);
    addCard(hand, Cards::Blue5);
    addCard(hand, Cards::Black7);
    addCard(hand, Cards::Red8);
    addCard(hand, Cards::Green9);
    addCard(hand, Cards::RedQueen);
    addCard(hand, Cards::GreenKing);
    addCard(hand, Cards::BlackKing);
    addCard(hand, Cards::BlueAce);
    print(hand);
	std::vector<CardStack> moves = generate_moves(hand, 0, Cards::Dog, MoveType::NotDefined);
    std::cout << "Generated moves: " << moves.size() << '\n';
    for (CardStack move : moves) {
        std::cout << move << '\n';
        print(move);
    }
}
