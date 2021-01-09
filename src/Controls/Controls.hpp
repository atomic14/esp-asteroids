//
//  Controls.hpp
//  AsteroidsTestBed
//
//  Created by Chris Greening on 08/01/2021.
//

#ifndef Controls_hpp
#define Controls_hpp

class Controls {
public:
    virtual bool is_firing() = 0;
    virtual bool is_thrusting() = 0;
    virtual float get_direction() = 0;
};

#endif /* Controls_hpp */
