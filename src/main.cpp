#include "types.h"
#include "cards.h"
#include "game_state.h"
#include "move_generation.h"

#include <iostream>
#include <bitset>
#include <sstream>
#include <string>

int main() {
    std::string input;
    while (true) {
        int limit = 500;
        while (true) {
            std::cout << "Punktelimit eingeben (Enter = 500): ";
            if (!std::getline(std::cin, input)) {
                return 0;
            }
            if (input.empty()) {
                break;
            }

            std::istringstream input_stream(input);
            int entered_limit;
            char trailing_character;
            if (input_stream >> entered_limit && !(input_stream >> trailing_character) && entered_limit > 0) {
                limit = entered_limit;
                break;
            }

            std::cout << "Bitte eine positive ganze Zahl eingeben.\n";
        }

        Game game = Game();
        game.play_game(limit);
    }
}
