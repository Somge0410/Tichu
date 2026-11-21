#include "types.h"

#include <stdexcept>

std::string get_location_name(Locations location) {
    switch (location) {
        case Locations::P0Hand: return "P0Hand";
        case Locations::P1Hand: return "P1Hand";
        case Locations::P2Hand: return "P2Hand";
        case Locations::P3Hand: return "P3Hand";
        case Locations::Table: return "Table";
        case Locations::P0Bank: return "P0Bank";
        case Locations::P1Bank: return "P1Bank";
        case Locations::P2Bank: return "P2Bank";
        case Locations::P3Bank: return "P3Bank";
        case Locations::Unkown: return "Unkown";
        default: throw std::invalid_argument("Invalid location value.");
    }
}
