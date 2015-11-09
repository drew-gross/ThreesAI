//
//  Tile.cpp
//  ThreesAI
//
//  Created by Drew Gross on 9/20/15.
//  Copyright © 2015 DrewGross. All rights reserved.
//

#include "Tile.hpp"

#include <boost/algorithm/string/trim.hpp>

#include "Debug.h"

using namespace std;
using namespace boost;

Tile pred(Tile t) {
    switch (t) {
        case Tile::EMPTY: return Tile::EMPTY;
        case Tile::TILE_1: return Tile::EMPTY;
        case Tile::TILE_2: return Tile::EMPTY;
        case Tile::TILE_3: return Tile::EMPTY;
        case Tile::TILE_6: return Tile::TILE_3;
        case Tile::TILE_12: return Tile::TILE_6;
        case Tile::TILE_24: return Tile::TILE_12;
        case Tile::TILE_48: return Tile::TILE_24;
        case Tile::TILE_96: return Tile::TILE_48;
        case Tile::TILE_192: return Tile::TILE_96;
        case Tile::TILE_384: return Tile::TILE_192;
        case Tile::TILE_768: return Tile::TILE_384;
        case Tile::TILE_1536: return Tile::TILE_768;
        case Tile::TILE_3072: return Tile::TILE_1536;
        case Tile::TILE_6144: return Tile::TILE_3072;
        default: debug();
    }
}

Tile succ(Tile t) {
    switch (t) {
        case Tile::EMPTY: return Tile::EMPTY;
        case Tile::TILE_1: return Tile::TILE_3;
        case Tile::TILE_2: return Tile::TILE_3;
        case Tile::TILE_3: return Tile::TILE_6;
        case Tile::TILE_6: return Tile::TILE_12;
        case Tile::TILE_12: return Tile::TILE_24;
        case Tile::TILE_24: return Tile::TILE_48;
        case Tile::TILE_48: return Tile::TILE_96;
        case Tile::TILE_96: return Tile::TILE_192;
        case Tile::TILE_192: return Tile::TILE_384;
        case Tile::TILE_384: return Tile::TILE_768;
        case Tile::TILE_768: return Tile::TILE_1536;
        case Tile::TILE_1536: return Tile::TILE_3072;
        case Tile::TILE_3072: return Tile::TILE_6144;
        case Tile::TILE_6144: return Tile::TILE_6144;
        default: debug();
    }
}

bool canMerge(Tile t1, Tile t2) {
    switch (t1) {
        case Tile::EMPTY: return t2 != Tile::EMPTY;
        case Tile::TILE_1: return t2 == Tile::TILE_2;
        case Tile::TILE_2: return t2 == Tile::TILE_1;
        case Tile::TILE_3: return t2 == Tile::TILE_3;
        case Tile::TILE_6: return t2 == Tile::TILE_6;
        case Tile::TILE_12: return t2 == Tile::TILE_12;
        case Tile::TILE_24: return t2 == Tile::TILE_24;
        case Tile::TILE_48: return t2 == Tile::TILE_48;
        case Tile::TILE_96: return t2 == Tile::TILE_96;
        case Tile::TILE_192: return t2 == Tile::TILE_192;
        case Tile::TILE_384: return t2 == Tile::TILE_384;
        case Tile::TILE_768: return t2 == Tile::TILE_768;
        case Tile::TILE_1536: return t2 == Tile::TILE_1536;
        case Tile::TILE_3072: return t2 == Tile::TILE_3072;
        case Tile::TILE_6144: return t2 == Tile::TILE_6144;
    }
}

optional<Tile> mergeResult(Tile t1, Tile t2) {
    switch (t1) {
        case Tile::EMPTY: return t2 == Tile::EMPTY ? none : make_optional(t2);
        case Tile::TILE_1: return t2 == Tile::TILE_2 ? make_optional(Tile::TILE_3) : none;
        case Tile::TILE_2: return t2 == Tile::TILE_1 ? make_optional(Tile::TILE_3) : none;
        case Tile::TILE_3: return t2 == Tile::TILE_3 ? make_optional(Tile::TILE_6) : none;
        case Tile::TILE_6: return t2 == Tile::TILE_6 ? make_optional(Tile::TILE_12) : none;
        case Tile::TILE_12: return t2 == Tile::TILE_12 ? make_optional(Tile::TILE_24) : none;
        case Tile::TILE_24: return t2 == Tile::TILE_24 ? make_optional(Tile::TILE_48) : none;
        case Tile::TILE_48: return t2 == Tile::TILE_48 ? make_optional(Tile::TILE_96) : none;
        case Tile::TILE_96: return t2 == Tile::TILE_96 ? make_optional(Tile::TILE_192) : none;
        case Tile::TILE_192: return t2 == Tile::TILE_192 ? make_optional(Tile::TILE_384) : none;
        case Tile::TILE_384: return t2 == Tile::TILE_384 ? make_optional(Tile::TILE_768) : none;
        case Tile::TILE_768: return t2 == Tile::TILE_768 ? make_optional(Tile::TILE_1536) : none;
        case Tile::TILE_1536: return t2 == Tile::TILE_1536 ? make_optional(Tile::TILE_3072) : none;
        case Tile::TILE_3072: return t2 == Tile::TILE_3072 ? make_optional(Tile::TILE_6144) : none;
        case Tile::TILE_6144: return t2 == Tile::TILE_6144 ? make_optional(Tile::TILE_6144) : none;
    }
}

Tile tileFromString(string s) {
    trim(s);
    if (s == "") return Tile::EMPTY;
    if (s == "0") return Tile::EMPTY;
    if (s == "1") return Tile::TILE_1;
    if (s == "2") return Tile::TILE_2;
    if (s == "3") return Tile::TILE_3;
    if (s == "6") return Tile::TILE_6;
    if (s == "12") return Tile::TILE_12;
    if (s == "24") return Tile::TILE_24;
    if (s == "48") return Tile::TILE_48;
    if (s == "96") return Tile::TILE_96;
    if (s == "192") return Tile::TILE_192;
    if (s == "384") return Tile::TILE_384;
    if (s == "768") return Tile::TILE_768;
    if (s == "1536") return Tile::TILE_1536;
    if (s == "3072") return Tile::TILE_3072;
    if (s == "6144") return Tile::TILE_6144;
    debug();
    return Tile::EMPTY;
}

unsigned int tileScore(Tile tileValue) {
    switch (tileValue) {
        case Tile::EMPTY: return 0;
        case Tile::TILE_1: return 0;
        case Tile::TILE_2: return 0;
        case Tile::TILE_3: return 3;
        case Tile::TILE_6: return 9;
        case Tile::TILE_12: return 27;
        case Tile::TILE_24: return 81;
        case Tile::TILE_48: return 243;
        case Tile::TILE_96: return 729;
        case Tile::TILE_192: return 2187;
        case Tile::TILE_384: return 6561;
        case Tile::TILE_768: return 19683;
        case Tile::TILE_1536: return 59049;
        case Tile::TILE_3072: return 177147;
        case Tile::TILE_6144: return 531441;
    }
}

ostream& operator<<(ostream &os, Tile t){
    switch (t) {
        case Tile::EMPTY:
            os << "    " ;
            return os;
        case Tile::TILE_1:
            os << "   1";
            return os;
        case Tile::TILE_2:
            os << "   2";
            return os;
        case Tile::TILE_3:
            os << "   3";
            return os;
        case Tile::TILE_6:
            os << "   6";
            return os;
        case Tile::TILE_12:
            os << "  12";
            return os;
        case Tile::TILE_24:
            os << "  24";
            return os;
        case Tile::TILE_48:
            os << "  48";
            return os;
        case Tile::TILE_96:
            os << "  96";
            return os;
        case Tile::TILE_192:
            os << " 192";
            return os;
        case Tile::TILE_384:
            os << " 384";
            return os;
        case Tile::TILE_768: 
            os << " 768";
            return os;
        case Tile::TILE_1536: 
            os << "1536";
            return os;
        case Tile::TILE_3072: 
            os << "3072";
            return os;
        case Tile::TILE_6144: 
            os << "6144";
            return os;
        default: debug();
    }
    return os;
}