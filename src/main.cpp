#include "game.h"

#include <iostream>
#include <sstream>
#include <string>

int main(int argc, char* argv[]) {
#ifdef TICHU_PROFILE_BUILD
    bool profiling = true;
#else
    bool profiling = false;
#endif
    if (argc == 2 && std::string(argv[1]) == "--profile") {
        profiling = true;
    }
    else if (argc != 1) {
        std::cerr << "Usage: Tichu [--profile]\n";
        return 1;
    }
    if (profiling) {
        std::cout << "Profiling game: point limit 10000.\n";
        Game game;
        game.play_game<false>(10000);
        return 0;
    }
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
        game.play_game<false>(limit);
    }
}
