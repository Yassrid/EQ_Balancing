//
//  MIPSolver.cpp
//  Equipment Balancing
//
//  Created by Yassine Ridouane on 9/14/18.
//  Copyright © 2018 Yassine Ridouane. All rights reserved.
//

#include "MIPSolver.h"
#include <iostream>
#include <fstream>
MIPSolver::MIPSolver(){
    
    _env = new GRBEnv();
    _model = new GRBModel(*_env);
}
MIPSolver::MIPSolver(eq_data eqdata){
    _datafolder = eqdata._datafolder;
    _env = new GRBEnv();
    _eqData = eqdata;
    _model = new GRBModel(*_env);
    _NLoads = int(eqdata._Loads.size());
    _NFacilities = int(eqdata._Facilities.size());
    _NEquip = int(eqdata._Equipments.size());
}

void MIPSolver::solveModel(int version){
    GRBModel* model= this->_model;
    if(version==0)
        model->set(GRB_DoubleParam_MIPGap,0.01);
    else
        model->set(GRB_DoubleParam_MIPGap,0.01);
    model->optimize();
    
}
void MIPSolver::solveModel1(MIPSolver model0){
    GRBModel* model= this->_model;
    GRBModel* Model0 = model0._model;

    model->set(GRB_DoubleParam_MIPGap,0.05);
    
    
    string var_name;
    for (int arc = 0; arc < _NLoads; ++arc)
    {
        
        int sj = int(_S_js[arc].size());
        for (int eq = 0; eq < sj; ++eq)
        {
            Load l = this->_eqData._Loads[arc];
            int pp=_S_js[arc][eq];
            var_name="Y"+to_string(arc)+"."+to_string(pp);
            GRBVar y = model->getVarByName(var_name);
            GRBVar y0 = Model0->getVarByName(var_name);
            //int error = model->GRBsetdblattrelement(model, "Start", y, 1.0);
            if (y.get(GRB_DoubleAttr_X)>0.99)
            {
                //cout << var_name << " : " << y.get(GRB_DoubleAttr_X) <<endl;
                //num_Subs+=1;
                //if(l.volume==0.0)
                //    num_Empties+=1;
                
                string new_equip =this->_eqData._Int2EQ[pp];
                this->_eqData._Loads[arc].equipment_new = new_equip;
                
                
            }
        }
        
    }
    
    
    
    model->optimize();
    
}


void MIPSolver::createModel1(){
    int maxSIndex= 9;
    int maxLIndex= 14;
    int maxXLIndex = 18;
    GRBModel* model = this->_model;
    model->set(GRB_StringAttr_ModelName, "Model1");
    
    vector<Load> loads = _eqData._Loads;
    vector<Facility> facilities = _eqData._Facilities;
    map<int,string> Int2EQ = _eqData._Int2EQ;
    map<int,int> initialEq;
    
    int m = int(loads.size());
    int n = int(facilities.size());
    
    for(int j=0;j<m;j++){
        initialEq[j]=_eqData._EQ2Int[loads[j].equipment];
    }
    //data
    vector<int> Objective1(m, 1);
    
    
    //decison variables
    //z_j where j is an arc
    GRBVar* z=0;
    z = model->addVars(m, GRB_BINARY);
    for (int p = 0; p < m; ++p)
    {
        ostringstream vname;
        vname << "Z" << p;
        //z[p].set(GRB_DoubleAttr_Obj, Objective1[p]);
        z[p].set(GRB_StringAttr_VarName, vname.str());
    }
    
    //define variable y_jk  j is an arc and k is an equipent type
    GRBVar** y = 0;
    //map<int,vector<int>> S_js;
    for (int w = 0; w < m; ++w)
    {
        Load ld = _eqData._Loads[w];
        string eq_type =ld.equipment;
        bool xlRes = (_eqData._FacXLRes[ld.origin]*_eqData._FacXLRes[ld.destination])>0;
        int eq_id = _eqData._EQ2Int[eq_type];
        double volPer = ld.volume*_eqData._PupConv[eq_id]/100.0;
        vector<int> S_j;
        for(int i=0;i<_eqData._EQNames.size();i++)
        {
            int id=_eqData._EQ2Int[_eqData._EQNames[i]];
            int esm = _eqData._ESM[make_pair(eq_id, i)];
            if(eq_id!=id && esm==1 && volPer<=_eqData._PupConv[id]){
                if(_eqData._PupCat[id]=="XL"  && xlRes)
                    continue;
                if(ld.trailer_max_number>1 && ((_eqData._PupCat[eq_id]=="S" && _eqData._PupCat[id]!="S")||
                                               (_eqData._PupCat[eq_id]=="L" && _eqData._PupCat[id]=="XL")))
                    continue;
                S_j.push_back(i);
            }
        }
        _S_js[w]=S_j;
    }
   
    y = new GRBVar* [m];
    int y_counter=0;
    for (int w = 0; w < m; ++w)
    {
        int sj = int(_S_js[w].size());
        y[w] = model->addVars(sj,GRB_BINARY);
        for (int p = 0; p < sj; ++p)
        {
            int pp=_S_js[w][p];
            y_counter++;
            ostringstream vname;
            vname << "Y" << w << "." << pp;
            y[w][p].set(GRB_StringAttr_VarName, vname.str());
        }
    }
    //define variable v_ik i is a facility and k is an equipment
    GRBVar** v = 0;
    v = new GRBVar* [n];
    int v_counter=0;
    bool noLong = false;
    bool noXLong = false;
    int EQMaxInd = 0;
    for (int i = 0; i < n; ++i)
    {
        Facility fac = facilities[i];
        noLong = fac.long_restricted;
        noXLong = fac.xlong_restricted;
        if(noLong)
            EQMaxInd = maxSIndex;
        else if(noXLong)
            EQMaxInd = maxLIndex;
        else
            EQMaxInd = maxXLIndex;
        EQMaxInd = maxXLIndex-1;
        v[i] = model->addVars(EQMaxInd,GRB_INTEGER);
        for (int k = 0; k<EQMaxInd;k++){
            v_counter++;
            ostringstream vname;
            vname << "V" << i << "." << k;
            v[i][k].set(GRB_StringAttr_VarName, vname.str());
        }
    }
    
    // arc eq substituion constraint
    for (int j = 0; j < m; ++j)
    {
        GRBLinExpr ntot = 0;
        ntot += z[j];
        int sj = int(_S_js[j].size());
        for (int kk = 0; kk < sj; ++kk)
        {
            //int k=_S_js[j][kk];
            ntot += -y[j][kk];
        }
        stringstream cname1;
        cname1 << "Arc_EQ_change_"  << j;
        model->addConstr(ntot == 0, cname1.str());
    }
    GRBLinExpr ntot3 = 0;
    // balance constraints
    for (int i = 0; i < n; ++i)
    {
        
        Facility fac = facilities[i];
        vector<int> inboundArcs = _eqData._InboundArcs[fac.name];
        vector<int> outboundArcs = _eqData._OutboundArcs[fac.name];
        noLong = fac.long_restricted;
        noXLong = fac.xlong_restricted;
        if(noLong)
            EQMaxInd = maxSIndex;
        else if(noXLong)
            EQMaxInd = maxLIndex;
        else
            EQMaxInd = maxXLIndex;
        EQMaxInd = maxXLIndex-1;
        for (int k = 0; k<EQMaxInd;k++){
            ntot3+=v[i][k];
            GRBLinExpr ntot1 = 0;
            GRBLinExpr ntot2 = 0;
            int inbound = _eqData._InboundEQ[make_pair(fac.name,Int2EQ[k])];
            int outbound = _eqData._OutboundEQ[make_pair(fac.name,Int2EQ[k])];
            
            ntot1+=outbound-inbound;
            ntot2+=inbound-outbound;
            for (int jj = 0; jj < inboundArcs.size(); ++jj)
            {
                int j = inboundArcs[jj];
                int dj = initialEq[j];
                if(k==dj)
                {
                    ntot1+= z[j];
                    ntot2-= z[j];
                }
                int sj = int(_S_js[j].size());
                for (int kk = 0; kk < sj; ++kk)
                {
                    int k2=_S_js[j][kk];
                    if(k2!=dj){
                        if(k==k2)
                        {
                            ntot1-= y[j][kk];
                            ntot2+= y[j][kk];
                        }
                        if(k==1 & k2==EQMaxInd)
                        {
                            ntot1-= 2*y[j][kk];
                            ntot2+= 2*y[j][kk];
                        }
                    }
                }
                
            }
            for (int jj = 0; jj < outboundArcs.size(); ++jj)
            {
                int j = outboundArcs[jj];
                int dj = initialEq[j];
                if(k==dj)
                {
                    ntot1-= z[j];
                    ntot2+= z[j];
                }
                int sj = int(_S_js[j].size());
                for (int kk = 0; kk < sj; ++kk)
                {
                    
                    int k2=_S_js[j][kk];
                    if(k2!=dj){
                        if(k==k2)
                        {
                            ntot1+= y[j][kk];
                            ntot2-= y[j][kk];
                        }
                        if(k==1 & k2==EQMaxInd)
                        {
                            ntot1+= 2*y[j][kk];
                            ntot2-= 2*y[j][kk];
                        }
                    }
                    
                    
                }
            }
            stringstream cname2,cname3;
            cname2 << "Balance1_" << i << "." <<k;
            cname3 << "Balance2_" << i << "." <<k;
            model->addConstr(ntot1 <= v[i][k], cname2.str());
            model->addConstr(ntot1 >= -v[i][k], cname3.str());
        }
        
        
    }
    // number of changes cap constraint
    stringstream cname4;
    //cname4 << "Maximum Changes";
    //model->addConstr(ntot3 <= param_N1, cname4.str());
    model->setObjective(ntot3);
    
    
    
    // The objective is to minimize the total fixed and variable costs
    model->set(GRB_IntAttr_ModelSense, GRB_MINIMIZE);
    // write model
    //model.write(this->_datafolder);
    string datafolder = this->_datafolder+"/model1.lp";
    model->write(datafolder);
    //model.optimize();
    //this->_model = model;
    
}
void MIPSolver::createModel3(int param_N1){
    
}
void MIPSolver::createModel2(int param_N1){
    int maxSIndex= 9;
    int maxLIndex= 14;
    int maxXLIndex = 18;
    GRBModel* model = this->_model;
    model->set(GRB_StringAttr_ModelName, "Model1");
    
    vector<Load> loads = _eqData._Loads;
    vector<Facility> facilities = _eqData._Facilities;
    map<int,string> Int2EQ = _eqData._Int2EQ;
    map<int,int> initialEq;
    
    int m = int(loads.size());
    int n = int(facilities.size());
    
    for(int j=0;j<m;j++){
        initialEq[j]=_eqData._EQ2Int[loads[j].equipment];
    }
    //data
    vector<int> Objective1(m, 1);
    
    
    //decison variables
    //z_j where j is an arc
    GRBVar* z=0;
    z = model->addVars(m, GRB_BINARY);
    for (int p = 0; p < m; ++p)
    {
        ostringstream vname;
        vname << "Z" << p;
        z[p].set(GRB_DoubleAttr_Obj, Objective1[p]);
        z[p].set(GRB_StringAttr_VarName, vname.str());
    }
    
    //define variable y_jk  j is an arc and k is an equipent type
    GRBVar** y = 0;
    //map<int,vector<int>> S_js;
    for (int w = 0; w < m; ++w)
    {
        Load ld = _eqData._Loads[w];
        string eq_type =ld.equipment;
        bool xlRes = (_eqData._FacXLRes[ld.origin]*_eqData._FacXLRes[ld.destination])>0;
        int eq_id = _eqData._EQ2Int[eq_type];
        double volPer = ld.volume*_eqData._PupConv[eq_id]/100.0;
        if(ld.equipment=="BT")
            volPer=0;
        vector<int> S_j;
        for(int i=0;i<_eqData._EQNames.size();i++)
        {
            int id=_eqData._EQ2Int[_eqData._EQNames[i]];
            int esm = _eqData._ESM[make_pair(eq_id, i)];
            if(ld.equipment=="BT")
                esm=1;
            if(eq_id!=id && esm==1 && volPer<=_eqData._PupConv[id]){
                if(_eqData._PupCat[id]=="XL"  && xlRes)
                    continue;
                if(ld.trailer_max_number>1 && ((_eqData._PupCat[eq_id]=="S" && _eqData._PupCat[id]!="S")||
                                               (_eqData._PupCat[eq_id]=="L" && _eqData._PupCat[id]=="XL")))
                    continue;
                S_j.push_back(i);
            }
        }
        if(ld.equipment=="BT")
            eq_id=eq_id;
        _S_js[w]=S_j;
    }
    
    y = new GRBVar* [m];
    int y_counter=0;
    for (int w = 0; w < m; ++w)
    {
        int sj = int(_S_js[w].size());
        y[w] = model->addVars(sj,GRB_BINARY);
        for (int p = 0; p < sj; ++p)
        {
            int pp=_S_js[w][p];
            y_counter++;
            ostringstream vname;
            vname << "Y" << w << "." << pp;
            y[w][p].set(GRB_StringAttr_VarName, vname.str());
        }
    }
    //define variable v_ik i is a facility and k is an equipment
    GRBVar** v = 0;
    v = new GRBVar* [n];
    int v_counter=0;
    bool noLong = false;
    bool noXLong = false;
    int EQMaxInd = 0;
    for (int i = 0; i < n; ++i)
    {
        Facility fac = facilities[i];
        noLong = fac.long_restricted;
        noXLong = fac.xlong_restricted;
        if(noLong)
            EQMaxInd = maxSIndex;
        else if(noXLong)
            EQMaxInd = maxLIndex;
        else
            EQMaxInd = maxXLIndex;
        EQMaxInd = maxXLIndex-1;
        v[i] = model->addVars(EQMaxInd,GRB_INTEGER);
        for (int k = 0; k<EQMaxInd;k++){
            v_counter++;
            ostringstream vname;
            vname << "V" << i << "." << k;
            v[i][k].set(GRB_StringAttr_VarName, vname.str());
        }
    }
    
    // arc eq substituion constraint
    for (int j = 0; j < m; ++j)
    {
        GRBLinExpr ntot = 0;
        ntot += z[j];
        int sj = int(_S_js[j].size());
        for (int kk = 0; kk < sj; ++kk)
        {
            //int k=_S_js[j][kk];
            ntot += -y[j][kk];
        }
        stringstream cname1;
        cname1 << "Arc_EQ_change_"  << j;
        model->addConstr(ntot == 0, cname1.str());
    }
    GRBLinExpr ntot3 = 0;
    // balance constraints
    for (int i = 0; i < n; ++i)
    {
        
        Facility fac = facilities[i];
        vector<int> inboundArcs = _eqData._InboundArcs[fac.name];
        vector<int> outboundArcs = _eqData._OutboundArcs[fac.name];
        noLong = fac.long_restricted;
        noXLong = fac.xlong_restricted;
        if(noLong)
            EQMaxInd = maxSIndex;
        else if(noXLong)
            EQMaxInd = maxLIndex;
        else
            EQMaxInd = maxXLIndex;
        EQMaxInd = maxXLIndex-1;
        for (int k = 0; k<EQMaxInd;k++){
            ntot3+=v[i][k];
            GRBLinExpr ntot1 = 0;
            GRBLinExpr ntot2 = 0;
            int inbound = _eqData._InboundEQ[make_pair(fac.name,Int2EQ[k])];
            int outbound = _eqData._OutboundEQ[make_pair(fac.name,Int2EQ[k])];
            
            ntot1+=outbound-inbound;
            ntot2+=inbound-outbound;
            for (int jj = 0; jj < inboundArcs.size(); ++jj)
            {
                int j = inboundArcs[jj];
                int dj = initialEq[j];
                if(k==dj)
                {
                    ntot1+= z[j];
                    ntot2-= z[j];
                }
                int sj = int(_S_js[j].size());
                for (int kk = 0; kk < sj; ++kk)
                {
                    int k2=_S_js[j][kk];
                    if(k2!=dj){
                        if(k==k2)
                        {
                            ntot1-= y[j][kk];
                            ntot2+= y[j][kk];
                        }
                        if(k==1 & k2==EQMaxInd)
                        {
                            ntot1-= 2*y[j][kk];
                            ntot2+= 2*y[j][kk];
                        }
                    }
                }
                
            }
            for (int jj = 0; jj < outboundArcs.size(); ++jj)
            {
                int j = outboundArcs[jj];
                int dj = initialEq[j];
                if(k==dj)
                {
                    ntot1-= z[j];
                    ntot2+= z[j];
                }
                int sj = int(_S_js[j].size());
                for (int kk = 0; kk < sj; ++kk)
                {
                    
                    int k2=_S_js[j][kk];
                    if(k2!=dj){
                        if(k==k2)
                        {
                            ntot1+= y[j][kk];
                            ntot2-= y[j][kk];
                        }
                        if(k==1 & k2==EQMaxInd)
                        {
                            ntot1+= 2*y[j][kk];
                            ntot2-= 2*y[j][kk];
                        }
                    }
                    
                    
                }
            }
            stringstream cname2,cname3;
            cname2 << "Balance1_" << i << "." <<k;
            cname3 << "Balance2_" << i << "." <<k;
            model->addConstr(ntot1 <= v[i][k], cname2.str());
            model->addConstr(ntot1 >= -v[i][k], cname3.str());
        }
        
        
    }
    // number of changes cap constraint
    stringstream cname4;
    cname4 << "Maximum Changes";
    model->addConstr(ntot3 <= param_N1, cname4.str());
    
    
    
    // The objective is to minimize the total fixed and variable costs
    model->set(GRB_IntAttr_ModelSense, GRB_MINIMIZE);
    // write model
    //model.write(this->_datafolder);
    string datafolder = this->_datafolder+"/model1.lp";
    model->write(datafolder);
    //model.optimize();
    //this->_model = model;
    
}
int MIPSolver::printResults(int version){
    GRBModel* model= this->_model;
    //Print solutions
    if(version==0){
        cout << "\nTotal Final Imbalance: " << model->get(GRB_DoubleAttr_ObjVal) << endl;
        
        int maxSIndex= 9;
        int maxLIndex= 14;
        int maxXLIndex = 18;
        int  EQMaxInd;
        string filename = _datafolder+"EQ_Changes1.csv";
        ofstream outputFile;
        ofstream file(filename);
        
        
        int num_Empties =0;
        int num_Subs =0;
        // write the file headers
        file << "number" << "," << "loadId" <<  "," << "origin_day" <<  "," << "volume_per" << "," <<"old_equipment" <<  "," << "new_equipment" << endl;
        
        string var_name;
        for (int arc = 0; arc < _NLoads; ++arc)
        {
            
            int sj = int(_S_js[arc].size());
            for (int eq = 0; eq < sj; ++eq)
            {
                Load l = this->_eqData._Loads[arc];
                int pp=_S_js[arc][eq];
                var_name="Y"+to_string(arc)+"."+to_string(pp);
                GRBVar y = model->getVarByName(var_name);
                if (y.get(GRB_DoubleAttr_X)>0.99)
                {
                    //cout << var_name << " : " << y.get(GRB_DoubleAttr_X) <<endl;
                    num_Subs+=1;
                    if(l.volume==0.0)
                        num_Empties+=1;
                    
                    string new_equip =this->_eqData._Int2EQ[pp];
                    this->_eqData._Loads[arc].equipment_new = new_equip;
                    file << arc << "," << l.loadId <<  "," << l.origin_day <<  "," << l.volume <<  "," << l.equipment <<  "," << new_equip << endl;
                    
                }
            }
            
        }
        cout << "#  Subs" << " : " << num_Subs <<endl;
        cout << "# Empty  Subs" << " : " << num_Empties << "(" << double(num_Empties)/double(num_Subs) << ")" << endl;
        file.close();
        
        
        // create and open the .csv file
        for(int i=0;i<_NLoads;i++){
            Load l =this->_eqData._Loads[i];
            this->_eqData._InboundEQ_new[make_pair(l.destination,l.equipment_new)]+=1;
            this->_eqData._OutboundEQ_new[make_pair(l.origin,l.equipment_new)]+=1;
        }
        // imbalance table
        string filename2 = _datafolder+"ImbalanceOld1.csv";
        string filename3 = _datafolder+"ImbalanceNew1.csv";
        ofstream file2(filename2);
        ofstream file3(filename3);
        
        // create and open the .csv file
        
        
        // write the file headers
        file2 << "Facility" << "," << "Operations_Type" <<  "," << "Equipment" <<"," << "Inbound" <<  "," << "Outbound" <<  "," << "Surplus" << "," << "|Surplus|" <<endl;
        file3 << "Facility" << "," << "Operations_Type" << "," << "Equipment" << "," << "Inbound" <<  "," << "Outbound" <<  "," << "Surplus" <<  "," << "|Surplus|"<< endl;
        for(int f=0;f<_NFacilities;f++)
        {
            for(int e=0;e<_NEquip;e++)
            {
                string fac_name = _eqData._Facilities[f].name;
                string eq_name = _eqData._Equipments[e].name;
                int inbound = this->_eqData._InboundEQ[make_pair(fac_name,eq_name)];
                int outbound = this->_eqData._OutboundEQ[make_pair(fac_name,eq_name)];
                file2 << fac_name << "," << _eqData._Facilities[f].operations_type <<"," << eq_name <<  "," << inbound << ","  << outbound  << "," << inbound-outbound  << "," << abs(inbound-outbound) << endl;
                
            }
        }
        file2.close();
        string validEQ[] = {"W","WW","WWS","L","H","F","E","Z","ZZ","ZZS","ZZZ","N","LC"};
        for(int f=0;f<_NFacilities;f++)
        {
            Facility fac = this->_eqData._Facilities[f];
            bool noLong = fac.long_restricted;
            bool noXLong = fac.xlong_restricted;
            
            if(noLong)
                EQMaxInd = maxXLIndex;
            else if(noXLong)
                EQMaxInd = maxXLIndex;
            else
                EQMaxInd = maxXLIndex;
            for(int e=0;e<EQMaxInd-1;e++) //_NEquip
            {
                string fac_name = _eqData._Facilities[f].name;
                //string eq_name = _eqData._Equipments[e].name;
                string eq_name = _eqData._Int2EQ[e];
                int inbound = this->_eqData._InboundEQ_new[make_pair(fac_name,eq_name)];
                int outbound = this->_eqData._OutboundEQ_new[make_pair(fac_name,eq_name)];
                
                string var_name = "V"+ to_string(f)+"." +to_string(e);
                GRBVar v = model->getVarByName(var_name);
                int V =v.get(GRB_DoubleAttr_X);
                
                
                
                file3 << fac_name << "," << _eqData._Facilities[f].operations_type <<"," << eq_name <<  "," << inbound << ","  << outbound  << "," << inbound-outbound  << "," << V << endl;
            }
        }
        file3.close();
        
        
        
        
        
        
        return model->get(GRB_DoubleAttr_ObjVal);
    }
    //cout << "\nTOTAL Equipment changes: " << model->get(GRB_DoubleAttr_ObjVal) << endl;
    
    int maxSIndex= 9;
    int maxLIndex= 14;
    int maxXLIndex = 18;
    int total =0;
    int  EQMaxInd;
    for (int i = 0; i < _NFacilities; ++i)
    {
        
        Facility fac = this->_eqData._Facilities[i];
        
        bool noLong = fac.long_restricted;
        bool noXLong = fac.xlong_restricted;
        
        if(noLong)
            EQMaxInd = maxSIndex;
        else if(noXLong)
            EQMaxInd = maxLIndex;
        else
            EQMaxInd = maxXLIndex;
        EQMaxInd = maxXLIndex-1;
        for (int k = 0; k<EQMaxInd;k++){
            string var_name = "V"+ to_string(i)+"." +to_string(k);
            GRBVar v = model->getVarByName(var_name);
            total += v.get(GRB_DoubleAttr_X);
        }
        
    }
    cout << "\nTOTAL imbalance: " << total << endl;
    
    
    
    string filename = _datafolder+"EQ_Changes.csv";
    ofstream outputFile;
    ofstream file(filename);
    
    
    int num_Empties =0;
    int num_Subs =0;
    // write the file headers
    file << "number" << "," << "loadId" <<  "," << "origin_day" <<  "," << "volume_per" << "," << "LC_Origin" << "," << "LC_Destination" << "," << "," <<"old_equipment" <<  "," << "new_equipment" << endl;
    
    
    for (int arc = 0; arc < _NLoads; ++arc)
    {
        
        int sj = int(_S_js[arc].size());
        for (int eq = 0; eq < sj; ++eq)
        {
            Load l = this->_eqData._Loads[arc];
            int pp=_S_js[arc][eq];
            string var_name="Y"+to_string(arc)+"."+to_string(pp);
            GRBVar y = model->getVarByName(var_name);
            if (y.get(GRB_DoubleAttr_X)>0.99)
            {
                //cout << var_name << " : " << y.get(GRB_DoubleAttr_X) <<endl;
                num_Subs+=1;
                if(l.volume==0.0)
                    num_Empties+=1;
                
                string new_equip =this->_eqData._Int2EQ[pp];
                this->_eqData._Loads[arc].equipment_new = new_equip;
                file << arc << "," << l.loadId <<  "," << l.origin_day <<  "," << l.volume <<  "," << l.origin <<  "," << l.destination <<  "," << l.equipment <<  "," << new_equip << endl;
                
            }
        }
        
    }
    cout << "#  Subs" << " : " << num_Subs <<endl;
    cout << "# Empty  Subs" << " : " << num_Empties << "(" << double(num_Empties)/double(num_Subs) << ")" << endl;
    file.close();
    
    
    // create and open the .csv file
    for(int i=0;i<_NLoads;i++){
        Load l =this->_eqData._Loads[i];
        this->_eqData._InboundEQ_new[make_pair(l.destination,l.equipment_new)]+=1;
        this->_eqData._OutboundEQ_new[make_pair(l.origin,l.equipment_new)]+=1;
    }
    // imbalance table
    string filename2 = _datafolder+"ImbalanceOld.csv";
    string filename3 = _datafolder+"ImbalanceNew.csv";
    ofstream file2(filename2);
    ofstream file3(filename3);
    
    // create and open the .csv file
    
    
    // write the file headers
    file2 << "Facility" << "," << "Operations_Type" <<  "," << "Equipment" <<"," << "Inbound" <<  "," << "Outbound" <<  "," << "Surplus" << "," << "|Surplus|" <<endl;
    file3 << "Facility" << "," << "Operations_Type" << "," << "Equipment" << "," << "Inbound" <<  "," << "Outbound" <<  "," << "Surplus" <<  "," << "|Surplus|"<< endl;
    for(int f=0;f<_NFacilities;f++)
    {
        for(int e=0;e<_NEquip;e++)
        {
            string fac_name = _eqData._Facilities[f].name;
            string eq_name = _eqData._Equipments[e].name;
            int inbound = this->_eqData._InboundEQ[make_pair(fac_name,eq_name)];
            int outbound = this->_eqData._OutboundEQ[make_pair(fac_name,eq_name)];
            file2 << fac_name << "," << _eqData._Facilities[f].operations_type <<"," << eq_name <<  "," << inbound << ","  << outbound  << "," << inbound-outbound  << "," << abs(inbound-outbound) << endl;
            
        }
    }
    file2.close();
    string validEQ[] = {"W","WW","WWS","L","H","F","E","Z","ZZ","ZZS","ZZZ","N","LC"};
    for(int f=0;f<_NFacilities;f++)
    {
        Facility fac = this->_eqData._Facilities[f];
        bool noLong = fac.long_restricted;
        bool noXLong = fac.xlong_restricted;
        
        if(noLong)
            EQMaxInd = maxXLIndex;
        else if(noXLong)
            EQMaxInd = maxXLIndex;
        else
            EQMaxInd = maxXLIndex;
        for(int e=0;e<EQMaxInd-1;e++) //_NEquip
        {
            string fac_name = _eqData._Facilities[f].name;
            //string eq_name = _eqData._Equipments[e].name;
            string eq_name = _eqData._Int2EQ[e];
            int inbound = this->_eqData._InboundEQ_new[make_pair(fac_name,eq_name)];
            int outbound = this->_eqData._OutboundEQ_new[make_pair(fac_name,eq_name)];
            
            string var_name = "V"+ to_string(f)+"." +to_string(e);
            GRBVar v = model->getVarByName(var_name);
            int V =v.get(GRB_DoubleAttr_X);
            
            
            
            file3 << fac_name << "," << _eqData._Facilities[f].operations_type <<"," << eq_name <<  "," << inbound << ","  << outbound  << "," << inbound-outbound  << "," << V << endl;
        }
    }
    file3.close();
    //return total;
    return model->get(GRB_DoubleAttr_ObjVal);
}
void MIPSolver::createModelH(){
    int maxSIndex= 9;
    int maxLIndex= 14;
    int maxXLIndex = 18;
    GRBModel* model = this->_model;
    model->set(GRB_StringAttr_ModelName, "ModelH");
    
    vector<Load> loads = _eqData._Loads;
    vector<Facility> facilities = _eqData._Facilities;
    map<int,string> Int2EQ = _eqData._Int2EQ;
    map<int,int> initialEq;
    
    int m = int(loads.size());
    int n = int(facilities.size());
    
    for(int j=0;j<m;j++){
        initialEq[j]=_eqData._EQ2Int[loads[j].equipment];
    }
    //data
    vector<int> Objective1(m, 1);
    
    
    //decison variables
    //z_j where j is an arc
    GRBVar* z=0;
    z = model->addVars(m, GRB_BINARY);
    for (int p = 0; p < m; ++p)
    {
        ostringstream vname;
        vname << "Z" << p;
        //z[p].set(GRB_DoubleAttr_Obj, Objective1[p]);
        z[p].set(GRB_StringAttr_VarName, vname.str());
    }
    
    //define variable y_jk  j is an arc and k is an equipent type
    GRBVar** y = 0;
    //map<int,vector<int>> S_js;
    for (int w = 0; w < m; ++w)
    {
        Load ld = _eqData._Loads[w];
        string eq_type =ld.equipment;
        bool xlRes = (_eqData._FacXLRes[ld.origin]*_eqData._FacXLRes[ld.destination])>0;
        int eq_id = _eqData._EQ2Int[eq_type];
        double volPer = ld.volume*_eqData._PupConv[eq_id]/100.0;
        vector<int> S_j;
        for(int i=0;i<_eqData._EQNames.size();i++)
        {
            int id=_eqData._EQ2Int[_eqData._EQNames[i]];
            int esm = _eqData._ESM[make_pair(eq_id, i)];
            if(eq_id!=id && esm==1 && volPer<=_eqData._PupConv[id]){
                if(_eqData._PupCat[id]=="XL"  && xlRes)
                    continue;
                if(ld.trailer_max_number>1 && ((_eqData._PupCat[eq_id]=="S" && _eqData._PupCat[id]!="S")||
                                               (_eqData._PupCat[eq_id]=="L" && _eqData._PupCat[id]=="XL")))
                    continue;
                S_j.push_back(i);
            }
        }
        _S_js[w]=S_j;
    }
    
    y = new GRBVar* [m];
    int y_counter=0;
    for (int w = 0; w < m; ++w)
    {
        int sj = int(_S_js[w].size());
        y[w] = model->addVars(sj,GRB_BINARY);
        for (int p = 0; p < sj; ++p)
        {
            int pp=_S_js[w][p];
            y_counter++;
            ostringstream vname;
            vname << "Y" << w << "." << pp;
            y[w][p].set(GRB_StringAttr_VarName, vname.str());
        }
    }
    //define variable v_ik i is a facility and k is an equipment
    GRBVar** v = 0;
    v = new GRBVar* [n];
    int v_counter=0;
    bool noLong = false;
    bool noXLong = false;
    int EQMaxInd = 0;
    for (int i = 0; i < n; ++i)
    {
        Facility fac = facilities[i];
        noLong = fac.long_restricted;
        noXLong = fac.xlong_restricted;
        if(noLong)
            EQMaxInd = maxSIndex;
        else if(noXLong)
            EQMaxInd = maxLIndex;
        else
            EQMaxInd = maxXLIndex;
        EQMaxInd = maxXLIndex-1;
        v[i] = model->addVars(EQMaxInd,GRB_INTEGER);
        for (int k = 0; k<EQMaxInd;k++){
            v_counter++;
            ostringstream vname;
            vname << "V" << i << "." << k;
            v[i][k].set(GRB_StringAttr_VarName, vname.str());
        }
    }
    GRBLinExpr ntot4 = 0;
    // arc eq substituion constraint
    for (int j = 0; j < m; ++j)
    {
        GRBLinExpr ntot = 0;
        ntot += z[j];
        ntot4 += z[j];
        int sj = int(_S_js[j].size());
        for (int kk = 0; kk < sj; ++kk)
        {
            //int k=_S_js[j][kk];
            ntot += -y[j][kk];
        }
        stringstream cname1;
        cname1 << "Arc_EQ_change_"  << j;
        model->addConstr(ntot == 0, cname1.str());
    }
    GRBLinExpr ntot3 = 0;

    // balance constraints
    for (int i = 0; i < n; ++i)
    {
        
        Facility fac = facilities[i];
        vector<int> inboundArcs = _eqData._InboundArcs[fac.name];
        vector<int> outboundArcs = _eqData._OutboundArcs[fac.name];
        noLong = fac.long_restricted;
        noXLong = fac.xlong_restricted;
        if(noLong)
            EQMaxInd = maxSIndex;
        else if(noXLong)
            EQMaxInd = maxLIndex;
        else
            EQMaxInd = maxXLIndex;
        EQMaxInd = maxXLIndex-1;
        for (int k = 0; k<EQMaxInd;k++){
            ntot3+=v[i][k];
            GRBLinExpr ntot1 = 0;
            GRBLinExpr ntot2 = 0;
            int inbound = _eqData._InboundEQ[make_pair(fac.name,Int2EQ[k])];
            int outbound = _eqData._OutboundEQ[make_pair(fac.name,Int2EQ[k])];
            
            ntot1+=outbound-inbound;
            ntot2+=inbound-outbound;
            for (int jj = 0; jj < inboundArcs.size(); ++jj)
            {
                int j = inboundArcs[jj];
                int dj = initialEq[j];
                if(k==dj)
                {
                    ntot1+= z[j];
                    ntot2-= z[j];
                }
                int sj = int(_S_js[j].size());
                for (int kk = 0; kk < sj; ++kk)
                {
                    int k2=_S_js[j][kk];
                    if(k2!=dj){
                        if(k==k2)
                        {
                            ntot1-= y[j][kk];
                            ntot2+= y[j][kk];
                        }
                        if(k==1 & k2==EQMaxInd)
                        {
                            ntot1-= 2*y[j][kk];
                            ntot2+= 2*y[j][kk];
                        }
                    }
                }
                
            }
            for (int jj = 0; jj < outboundArcs.size(); ++jj)
            {
                int j = outboundArcs[jj];
                int dj = initialEq[j];
                if(k==dj)
                {
                    ntot1-= z[j];
                    ntot2+= z[j];
                }
                int sj = int(_S_js[j].size());
                for (int kk = 0; kk < sj; ++kk)
                {
                    
                    int k2=_S_js[j][kk];
                    if(k2!=dj){
                        if(k==k2)
                        {
                            ntot1+= y[j][kk];
                            ntot2-= y[j][kk];
                        }
                        if(k==1 & k2==EQMaxInd)
                        {
                            ntot1+= 2*y[j][kk];
                            ntot2-= 2*y[j][kk];
                        }
                    }
                    
                    
                }
            }
            stringstream cname2,cname3;
            cname2 << "Balance1_" << i << "." <<k;
            cname3 << "Balance2_" << i << "." <<k;
            model->addConstr(ntot1 <= v[i][k], cname2.str());
            model->addConstr(ntot1 >= -v[i][k], cname3.str());
        }
        
        
    }
    // number of changes cap constraint
    stringstream cname4;
    //cname4 << "Maximum Changes";
    //model->addConstr(ntot3 <= param_N1, cname4.str());
    // Set number of objectives
    //model->set(GRB_IntAttr_NumObj, 2);
    
    // Set and configure objective 0
    //model->set(GRB_IntParam_ObjNumber, 0);
    //model->set(GRB_IntAttr_ObjNPriority, 10);
    //model->setObjective(ntot3);
    //model->set(GRB_IntParam_ObjNumber, 1);
    //model->set(GRB_IntAttr_ObjNPriority , 5);
    model->setObjectiveN(ntot3,0,1);
    model->setObjectiveN(ntot4,1,0);
    
    
    
    // The objective is to minimize the total fixed and variable costs
    model->set(GRB_IntAttr_ModelSense, GRB_MINIMIZE);
    // write model
    //model.write(this->_datafolder);
    string datafolder = this->_datafolder+"/modelH.lp";
    model->write(datafolder);
    //model.optimize();
    //this->_model = model;
    
}
