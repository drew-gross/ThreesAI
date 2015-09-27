//
//  RandomHint.hpp
//  ThreesAI
//
//  Created by Drew Gross on 9/26/15.
//  Copyright © 2015 DrewGross. All rights reserved.
//

#ifndef RandomHint_hpp
#define RandomHint_hpp

#include "Hint.h"

class RandomHint : public Hint {    
public:
    RandomHint(Tile upcomingTile, Tile maxBonusTile, std::default_random_engine rng);
    std::ostream& print(std::ostream&) const;
    bool isAnyBonus() const;
};

#endif /* RandomHint_hpp */
