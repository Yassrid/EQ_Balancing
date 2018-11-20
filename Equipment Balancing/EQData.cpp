//
//  EQData.cpp
//  Equipment Balancing
//
//  Created by Yassine Ridouane on 9/14/18.
//  Copyright © 2018 Yassine Ridouane. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "EQData.h"
#include <math.h>
#include <iostream>

eq_data::eq_data(){
    
}
int eq_data::computeImbalance(){
    //int NLoads = int(_Loads.size());
    int NFacilities = int(_Facilities.size());
    int NEquip = int(_Equipments.size());
    int total_imbalance = 0;
    int residual_imbalance =0;
    for(int f=0;f<NFacilities;f++)
    {
        residual_imbalance += fabs(_NInboundArcs[_Facilities[f].name]-_NOutboundArcs[_Facilities[f].name]);
        for(int e=0;e<NEquip;e++)
        {
            if(_Equipments[e].name=="BT")
                continue;
            total_imbalance += fabs(_InboundEQ[make_pair(_Facilities[f].name,_Equipments[e].name)]-_OutboundEQ[make_pair(_Facilities[f].name,_Equipments[e].name)]);
        }
    }
    cout << " Initial imbalance :"  << total_imbalance << endl;
    cout << " Residual imbalance :"  << residual_imbalance << endl;
    return total_imbalance;
}
void eq_data::mapFacilityEQ(){
    int NLoads = int(_Loads.size());
    int NFacilities = int(_Facilities.size());
    int NEquip = int(_Equipments.size());
    for(int f=0;f<NFacilities;f++)
    {
        for(int e=0;e<NEquip;e++)
        {
            _InboundEQ_new[make_pair(_Facilities[f].name,_Equipments[e].name)]=0;
            _OutboundEQ_new[make_pair(_Facilities[f].name,_Equipments[e].name)]=0;
        }
    }
    
    for(int f=0;f<NFacilities;f++)
    {
        _NInboundArcs[_Facilities[f].name] = 0;
        _NOutboundArcs[_Facilities[f].name] = 0;
        for(int e=0;e<NEquip;e++)
        {
            _InboundEQ[make_pair(_Facilities[f].name,_Equipments[e].name)]=0;
            _OutboundEQ[make_pair(_Facilities[f].name,_Equipments[e].name)]=0;
        }
    }
    
    
    for(int i=0;i<NLoads;i++){
        Load l =_Loads[i];

        _NInboundArcs[l.destination] += 1;
        _NOutboundArcs[l.origin] += 1;
        _InboundArcs[l.destination].push_back(i);
        _OutboundArcs[l.origin].push_back(i);
        _InboundEQ[make_pair(l.destination,l.equipment)]+=1;
        _OutboundEQ[make_pair(l.origin,l.equipment)]+=1;
    }
    return;
}
void eq_data::mapEQ2Int(){
    // building a mapping from category type to integer and vice versa
    map<string, int> EQ2Int;
    map<int, string> Int2EQ;
    
    for(int i=0; i<_EQNames.size();i++)
    {
        //EQ2Int[equipments[i]]=i;
        //Int2EQ[i]=equipments[i];
        EQ2Int[_EQNames[i]]=i;
        Int2EQ[i]=_EQNames[i];
    }
    EQ2Int["BT"]=18;
    Int2EQ[18]="BT";
    _EQ2Int=EQ2Int;
    _Int2EQ = Int2EQ;

    return;
}
void eq_data::mapEQ2PupConv(){
    map<int, double> pupConv;
    for(int i=0;i<_Equipments.size();i++)
    {
        int id = _EQ2Int[_Equipments[i].name];
        pupConv[id] = _Equipments[i].pup_conversion;
    }
    _PupConv = pupConv;

    return;
}
void eq_data::mapEQ2Category(){
    map<int, string> pupCat;
    for(int i=0;i<_Equipments.size();i++)
    {
        int id = _EQ2Int[_Equipments[i].name];
        pupCat[id] = _Equipments[i].category;
    }
    _PupCat = pupCat;
    
    return;
}
void eq_data::mapFac2XLRestriction(){
    map<string,int> FacRes;
    for(int i=0;i<_Facilities.size();i++)
    {
        Facility fac = _Facilities[i];
        FacRes[fac.name] = fac.xlong_restricted;
    }
    _FacXLRes = FacRes;
}
