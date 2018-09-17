//
//  EQData.cpp
//  Equipment Balancing
//
//  Created by Yassine Ridouane on 9/14/18.
//  Copyright © 2018 Yassine Ridouane. All rights reserved.
//

#include <stdio.h>
#include "EQData.h"

void eq_data::computeImbalance(){
    int NLoads = int(_Loads.size());
    int NFacilities = int(_Facilities.size());
    int NEquip = int(_Equipments.size());
    Imbalance imb;
    for(int f=0;f<NFacilities;f++)
    {
        for(int e=0;e<NEquip;e++)
        {
            _InboundEQ[make_pair(_Facilities[f].name,_Equipments[e].name)]=0;
            _OutboundEQ[make_pair(_Facilities[f].name,_Equipments[e].name)]=0;
        }
    }
    for(int i=0;i<NLoads;i++){
        Load l =_Loads[i];
        _InboundEQ[make_pair(l.destination,l.equipment)]+=1;
        _OutboundEQ[make_pair(l.origin,l.equipment)]+=1;
    }
    return;
}
void eq_data::mapFacilityEQ(){
    int NLoads = int(_Loads.size());
    int NFacilities = int(_Facilities.size());
    int NEquip = int(_Equipments.size());
    for(int f=0;f<NFacilities;f++)
    {
        for(int e=0;e<NEquip;e++)
        {
            _InboundEQ[make_pair(_Facilities[f].name,_Equipments[e].name)]=0;
            _OutboundEQ[make_pair(_Facilities[f].name,_Equipments[e].name)]=0;
        }
    }
    
    
    for(int i=0;i<NLoads;i++){
        Load l =_Loads[i];
        if(l.origin=="SEAIL"){
            l=l;
        }

        _InboundArcs[l.destination].push_back(i);
        _OutboundArcs[l.origin].push_back(i);
        _InboundEQ[make_pair(l.destination,l.equipment)]+=1;
        _OutboundEQ[make_pair(l.origin,l.equipment)]+=1;
    }
    return;
}
void eq_data::mapEQ2Int(){
    // building a mapping from category type to integer
    map<string, int> EQ2Int;
    EQ2Int["W"]=0;
    EQ2Int["WW"]=1;
    EQ2Int["WWW"]=2;
    EQ2Int["WWS"]=3;
    EQ2Int["L"]=4;
    EQ2Int["H"]=5;
    EQ2Int["MS"]=6;
    EQ2Int["F"]=7;
    EQ2Int["Z"]=8;
    EQ2Int["ZZ"]=9;
    EQ2Int["ZZS"]=10;
    EQ2Int["TMB"]=11;
    EQ2Int["TMF"]=12;
    EQ2Int["ZZZ"]=13;
    EQ2Int["RBL"]=14;
    EQ2Int["E"]=15;
    EQ2Int["N"]=16;
    
    _EQ2Int=EQ2Int;
    
    // building a mapping from integer to equipment type
    map<int, string> Int2EQ;
    Int2EQ[0]="W";
    Int2EQ[1]="WW";
    Int2EQ[2]="WWW";
    Int2EQ[3]="WWS";
    Int2EQ[4]="L";
    Int2EQ[5]="H";
    Int2EQ[6]="MS";
    Int2EQ[7]="F";
    Int2EQ[8]="Z";
    Int2EQ[9]="ZZ";
    Int2EQ[10]="ZZS";
    Int2EQ[11]="TMB";
    Int2EQ[12]="TMF";
    Int2EQ[13]="ZZZ";
    Int2EQ[14]="RBL";
    Int2EQ[15]="E";
    Int2EQ[16]="N";
    _Int2EQ = Int2EQ;
    _EQ2Int=EQ2Int;
    return;
}
