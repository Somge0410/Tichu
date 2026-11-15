#pragma once
#include "types.h"

Locations convert_global_to_local(Locations global_location, PlayerID id) {
    if(global_location == Locations::P1Hand) {
        return static_cast<Locations>((static_cast<int>(Locations::P1Hand) + id) % 4);
    } else if(global_location == Locations::P2Hand) {
        return static_cast<Locations>((static_cast<int>(Locations::P2Hand) + id) % 4);
    } else if(global_location == Locations::P3Hand) {
        return static_cast<Locations>((static_cast<int>(Locations::P3Hand) + id) % 4);
    } else if(global_location == Locations::P4Hand) {
        return static_cast<Locations>((static_cast<int>(Locations::P4Hand) + id) % 4);
    } else if(global_location == Locations::Table) {
        return Locations::Table;
    } else if(global_location == Locations::P1Bank) {
        return static_cast<Locations>((static_cast<int>(Locations::P1Bank) + id) % 4);
    } else if(global_location == Locations::P2Bank) {
        return static_cast<Locations>((static_cast<int>(Locations::P2Bank) + id) % 4);
    } else if(global_location == Locations::P3Bank) {
        return static_cast<Locations>((static_cast<int>(Locations::P3Bank) + id) % 4);
    } else if(global_location == Locations::P4Bank) {
        return static_cast<Locations>((static_cast<int>(Locations::P4Bank) + id) % 4);
    }
}
Locations convert_local_to_global(Locations local_location, PlayerID id) {
    if(local_location == Locations::P1Hand) {
        return static_cast<Locations>((static_cast<int>(Locations::P1Hand) - id + 4) % 4);
    } else if(local_location == Locations::P2Hand) {
        return static_cast<Locations>((static_cast<int>(Locations::P2Hand) - id + 4) % 4);
    } else if(local_location == Locations::P3Hand) {
        return static_cast<Locations>((static_cast<int>(Locations::P3Hand) - id + 4) % 4);
    } else if(local_location == Locations::P4Hand) {
        return static_cast<Locations>((static_cast<int>(Locations::P4Hand) - id + 4) % 4);
    } else if(local_location == Locations::Table) {
        return Locations::Table;
    } else if(local_location == Locations::P1Bank) {
        return static_cast<Locations>((static_cast<int>(Locations::P1Bank) - id + 4) % 4);
    } else if(local_location == Locations::P2Bank) {
        return static_cast<Locations>((static_cast<int>(Locations::P2Bank) - id + 4) % 4);
    } else if(local_location == Locations::P3Bank) {
        return static_cast<Locations>((static_cast<int>(Locations::P3Bank) - id + 4) % 4);
    } else if(local_location == Locations::P4Bank) {
        return static_cast<Locations>((static_cast<int>(Locations::P4Bank) - id + 4) % 4);
    }
}
Standings convert_global_to_local(Standings global_standings, PlayerID id) {
    Standings local_standings;
    for (int i = 0; i < 2; ++i) {
        local_standings[i] = global_standings[(i + id) % 2];
    }
    return local_standings;
}

class GameKnowlege {
public:
    GameKnowlege() {
        clear();
    }
    void give_cards_to_Location(CardStack stack, Locations global_location, PlayerID id) {
        Locations local_location = convert_global_to_local(global_location, id);
        while (stack) {
            int cardIndex = pop_lsb(stack);
            cardLocations[cardIndex] = local_location;
            points[static_cast<int>(local_location)] += get_Points(static_cast<Cards>(cardIndex));
        }
    }
    void give_new_standings(const Standings new_standings, PlayerID id) {
        Standings local_standings = convert_global_to_local(new_standings,id);
        standings = local_standings;
    }
    void clear() {
        cardLocations.fill(Locations::Unkown);
        standings = { 0, 0 };
        points.fill(0);
        tichu.fill(false);
        grandTichu.fill(false);
    }
    void set_tichu(const Tichu new_tichu, PlayerID id) {
        for (size_t i = 0; i < 4; ++i) {
            tichu[i] = new_tichu[(i + id) % 4];
        }
    }
    void set_grand_tichu(const GrandTichu new_grand_tichu, PlayerID id) {
        for (size_t i = 0; i < 4; ++i) {
            grandTichu[i] = new_grand_tichu[(i + id) % 4];
        }
    }
private:
    CardLocations cardLocations={};
    Standings standings = { 0, 0 };
    Points points = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    Tichu tichu = { false, false, false, false };
    GrandTichu  grandTichu = { false, false, false, false };

};
class Player {
public:
    Player(PlayerID id) : currentPlayer(id) {}
    void set_current_player(PlayerID id) {
        currentPlayer = id;
        gameKnowlege.clear();
        CardStack hand = 0;
    }
    void ReceiveCardLocationChangeInfo(const CardStack stack, Locations global_location) {
        gameKnowlege.give_cards_to_Location(stack, global_location, currentPlayer);
        if (Locations::P1Hand == convert_global_to_local(global_location, currentPlayer)) {
            add_cards_to_hand(stack);
        }

    }
    void ReceiveTichuInfo(const Tichu tichu, const GrandTichu grandTichu) {

        gameKnowlege.set_tichu(tichu, currentPlayer);
        gameKnowlege.set_grand_tichu(grandTichu, currentPlayer);
    }
    CardStack announce_move_to_table() {
        remove_cards_from_hand(0); //TODO
        return 0; //TODO
    }
    std::array<CardStack,3> announce_change() {
        remove_cards_from_hand(0); //TODO
        return {0,0,0}; //TODO
    }
    bool announce_tichu() {
        return false; //TODO
    }
    bool announce_grand_tichu() {
        return false; //TODO
    }

private:
    PlayerID currentPlayer = 0;
    GameKnowlege gameKnowlege;
    CardStack hand = 0;
    void remove_cards_from_hand(const CardStack stack) {
        hand &= ~stack;
    }
    void add_cards_to_hand(const CardStack stack) {
        hand |= stack;
    }
};
