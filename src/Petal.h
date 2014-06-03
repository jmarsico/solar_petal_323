//
//  Petal.h
//  solar_petals_oF
//
//  Created by Jakob Marsico on 3/27/14.
//
//

#ifndef __solar_petals_oF__Petal__
#define __solar_petals_oF__Petal__

#include <iostream>
#include "ofMain.h"


class Petal{
public:
    Petal();
    
    void init(float width, float height);
    void setWaitTime(int _waiter);
    void update();
    void display(bool changeColor);
    
    float locX;
    float locY;
    float h;
    float w;
    float maxWidth;
	
	int waitCounts;
	int waitVar;
	int waitCoeff;
    
	int counter;
	int closeCounts;
	int openCounts;
	bool state;
	bool prevState;
    
    
    
};

#endif /* defined(__solar_petals_oF__Petal__) */
