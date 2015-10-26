//
//  ForcedHint.hpp
//  ThreesAI
//
//  Created by Drew Gross on 9/26/15.
//  Copyright © 2015 DrewGross. All rights reserved.
//

#ifndef ForcedHint_hpp
#define ForcedHint_hpp

#include "Hint.h"

class ForcedHint : public Hint {
public:
    std::ostream& print(std::ostream&) const;
    
    static std::shared_ptr<ForcedHint const>  unknownBonus();
    explicit ForcedHint(Tile hint1);
    ForcedHint(Tile hint1, Tile hint2);
    ForcedHint(Tile hint1, Tile hint2, Tile hint3);
};

#endif /* ForcedHint_hpp */
