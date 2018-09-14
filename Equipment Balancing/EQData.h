//
//  eq_data.h
//  Equipment Balancing
//
//  Created by Yassine Ridouane on 9/13/18.
//  Copyright © 2018 Yassine Ridouane. All rights reserved.
//

#ifndef eq_data_h
#define eq_data_h


#include "DataStructures.h"
#include <boost/bind.hpp>
#include <map>

class eq_data {
public:
    vector<Load> _Loads;
    vector<Facility> _Facilities;
    vector<Equipment> _Equipments;
    map<string,int> _EQ2Int;
    vector<Imbalance> _Imbalance;
    void computeImbalance();
private:
    

};

#endif /* eq_data_h */
