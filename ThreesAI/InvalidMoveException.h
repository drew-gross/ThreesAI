//
//  InvalidMoveException.h
//  ThreesAI
//
//  Created by Drew Gross on 9/14/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#ifndef ThreesAI_InvalidMoveException_h
#define ThreesAI_InvalidMoveException_h

class InvalidMoveException : public std::logic_error {
public:
    InvalidMoveException() : logic_error("That move cannot be made"){};
};

#endif
