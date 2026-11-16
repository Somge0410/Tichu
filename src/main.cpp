#include "types.h"
#include "cards.h"
#include "game_state.h"
#include "move_generation.h"

#include <iostream>
#include <bitset>

int main() {
    Game game=Game();
    game.play_game(500);
    return 0;
}
