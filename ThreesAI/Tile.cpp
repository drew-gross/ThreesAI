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

Tile Tile::pred() const {
    switch (this->value) {
        case T::EMPTY: return T::EMPTY;
        case T::_1: return T::EMPTY;
        case T::_2: return T::EMPTY;
        case T::_3: return T::EMPTY;
        case T::_6: return T::_3;
        case T::_12: return T::_6;
        case T::_24: return T::_12;
        case T::_48: return T::_24;
        case T::_96: return T::_48;
        case T::_192: return T::_96;
        case T::_384: return T::_192;
        case T::_768: return T::_384;
        case T::_1536: return T::_768;
        case T::_3072: return T::_1536;
        case T::_6144: return T::_3072;
        default: debug();
    }
}

Tile Tile::succ() const {
    switch (this->value) {
        case T::EMPTY: return T::EMPTY;
        case T::_1: return T::_3;
        case T::_2: return T::_3;
        case T::_3: return T::_6;
        case T::_6: return T::_12;
        case T::_12: return T::_24;
        case T::_24: return T::_48;
        case T::_48: return T::_96;
        case T::_96: return T::_192;
        case T::_192: return T::_384;
        case T::_384: return T::_768;
        case T::_768: return T::_1536;
        case T::_1536: return T::_3072;
        case T::_3072: return T::_6144;
        case T::_6144: return T::_6144;
        default: debug();
    }
}

bool Tile::canMergeOrMove(Tile t2) const {
    switch (this->value) {
        case T::EMPTY: return t2 != T::EMPTY;
        case T::_1: return t2 == T::_2;
        case T::_2: return t2 == T::_1;
        case T::_3: return t2 == T::_3;
        case T::_6: return t2 == T::_6;
        case T::_12: return t2 == T::_12;
        case T::_24: return t2 == T::_24;
        case T::_48: return t2 == T::_48;
        case T::_96: return t2 == T::_96;
        case T::_192: return t2 == T::_192;
        case T::_384: return t2 == T::_384;
        case T::_768: return t2 == T::_768;
        case T::_1536: return t2 == T::_1536;
        case T::_3072: return t2 == T::_3072;
        case T::_6144: return false;
    }
}

bool Tile::canMerge(Tile t2) const {
    switch (this->value) {
        case T::EMPTY: return false;
        case T::_1: return t2 == T::_2;
        case T::_2: return t2 == T::_1;
        case T::_3: return t2 == T::_3;
        case T::_6: return t2 == T::_6;
        case T::_12: return t2 == T::_12;
        case T::_24: return t2 == T::_24;
        case T::_48: return t2 == T::_48;
        case T::_96: return t2 == T::_96;
        case T::_192: return t2 == T::_192;
        case T::_384: return t2 == T::_384;
        case T::_768: return t2 == T::_768;
        case T::_1536: return t2 == T::_1536;
        case T::_3072: return t2 == T::_3072;
        case T::_6144: return false;
    }
}

optional<Tile> Tile::mergeResult(Tile t2) const {
    switch (this->value) {
        case T::EMPTY: return t2 == T::EMPTY ? none : make_optional(t2);
        case T::_1: return t2 == T::_2 ? make_optional(Tile(T::_3)) : none;
        case T::_2: return t2 == T::_1 ? make_optional(Tile(T::_3)) : none;
        case T::_3: return t2 == T::_3 ? make_optional(Tile(T::_6)) : none;
        case T::_6: return t2 == T::_6 ? make_optional(Tile(T::_12)) : none;
        case T::_12: return t2 == T::_12 ? make_optional(Tile(T::_24)) : none;
        case T::_24: return t2 == T::_24 ? make_optional(Tile(T::_48)) : none;
        case T::_48: return t2 == T::_48 ? make_optional(Tile(T::_96)) : none;
        case T::_96: return t2 == T::_96 ? make_optional(Tile(T::_192)) : none;
        case T::_192: return t2 == T::_192 ? make_optional(Tile(T::_384)) : none;
        case T::_384: return t2 == T::_384 ? make_optional(Tile(T::_768)) : none;
        case T::_768: return t2 == T::_768 ? make_optional(Tile(T::_1536)) : none;
        case T::_1536: return t2 == T::_1536 ? make_optional(Tile(T::_3072)) : none;
        case T::_3072: return t2 == T::_3072 ? make_optional(Tile(T::_6144)) : none;
        case T::_6144: return none;
    }
}

Tile tileFromString(string s) {
    trim(s);
    if (s == "") return T::EMPTY;
    if (s == "0") return T::EMPTY;
    if (s == "1") return T::_1;
    if (s == "2") return T::_2;
    if (s == "3") return T::_3;
    if (s == "6") return T::_6;
    if (s == "12") return T::_12;
    if (s == "24") return T::_24;
    if (s == "48") return T::_48;
    if (s == "96") return T::_96;
    if (s == "192") return T::_192;
    if (s == "384") return T::_384;
    if (s == "768") return T::_768;
    if (s == "1536") return T::_1536;
    if (s == "3072") return T::_3072;
    if (s == "6144") return T::_6144;
    debug();
    return T::EMPTY;
}

unsigned int Tile::tileScore() const {
    switch (this->value) {
        case T::EMPTY: return 0;
        case T::_1: return 0;
        case T::_2: return 0;
        case T::_3: return 3;
        case T::_6: return 9;
        case T::_12: return 27;
        case T::_24: return 81;
        case T::_48: return 243;
        case T::_96: return 729;
        case T::_192: return 2187;
        case T::_384: return 6561;
        case T::_768: return 19683;
        case T::_1536: return 59049;
        case T::_3072: return 177147;
        case T::_6144: return 531441;
    }
}

ostream& operator<<(ostream &os, Tile t){
    switch (t.value) {
        case T::EMPTY:
            os << "    " ;
            return os;
        case T::_1:
            os << "   1";
            return os;
        case T::_2:
            os << "   2";
            return os;
        case T::_3:
            os << "   3";
            return os;
        case T::_6:
            os << "   6";
            return os;
        case T::_12:
            os << "  12";
            return os;
        case T::_24:
            os << "  24";
            return os;
        case T::_48:
            os << "  48";
            return os;
        case T::_96:
            os << "  96";
            return os;
        case T::_192:
            os << " 192";
            return os;
        case T::_384:
            os << " 384";
            return os;
        case T::_768: 
            os << " 768";
            return os;
        case T::_1536: 
            os << "1536";
            return os;
        case T::_3072: 
            os << "3072";
            return os;
        case T::_6144:
            os << "6144";
            return os;
        default: debug();
    }
    return os;
}