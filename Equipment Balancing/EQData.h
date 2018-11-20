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
    eq_data();
    string _datafolder;
    vector<Load> _Loads;
    vector<Facility> _Facilities;
    vector<Equipment> _Equipments;
    map<pair<int,int>,int> _ESM;
    vector<string> _EQNames;
    int _NEquipments;
    map<string,int> _EQ2Int;
    map<string,int> _FacXLRes;
    map<int,string> _Int2EQ;
    map<int,double> _PupConv;
    map<int,string> _PupCat;
    map<string,vector<int>> _InboundArcs;
    map<string,vector<int>> _OutboundArcs;
    map<string,int> _NInboundArcs;
    map<string,int> _NOutboundArcs;
    map<pair<string,string>, int> _InboundEQ;
    map<pair<string,string>, int> _OutboundEQ;
    map<pair<string,string>, int> _InboundEQ_new;
    map<pair<string,string>, int> _OutboundEQ_new;
    int computeImbalance();
    void mapEQ2Int();
    void mapEQ2PupConv();
    void mapEQ2Category();
    void mapFac2XLRestriction();
    void mapFacilityEQ();
private:
    

};
#endif /* eq_data_h */
