//
//  MIPSolver.cpp
//  Equipment Balancing
//
//  Created by Yassine Ridouane on 9/14/18.
//  Copyright © 2018 Yassine Ridouane. All rights reserved.
//

#include "MIPSolver.h"

MIPSolver::MIPSolver(){
    
    _env = new GRBEnv();
    _model = new GRBModel(*_env);
}
MIPSolver::MIPSolver(eq_data eqdata){
    _datafolder = eqdata._datafolder;
    _env = new GRBEnv();
    _eqData = eqdata;
    _model = new GRBModel(*_env);
}
void MIPSolver::createModel1(int param_N1){
    int maxSIndex= 7;
    int maxLIndex= 12;
    int maxXLIndex = 16;
    GRBModel model = *_model;
    model.set(GRB_StringAttr_ModelName, "Model1");
    
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
    z = model.addVars(m, GRB_BINARY);
    for (int p = 0; p < m; ++p)
    {
        ostringstream vname;
        vname << "Z" << p;
        z[p].set(GRB_DoubleAttr_Obj, Objective1[p]);
        z[p].set(GRB_StringAttr_VarName, vname.str());
    }
    
    //define variable y_jk  j is an arc and k is an equipent type
    GRBVar** y = 0;
    map<int,vector<int>> S_js;

    for (int w = 0; w < m; ++w)
    {
        Load ld = _eqData._Loads[w];
        string eq_type =ld.equipment;
        int eq_id = _eqData._EQ2Int[eq_type];
        vector<int> S_j;
        if(eq_id<=maxSIndex){
            for(int i=0;i<=maxSIndex;i++){
                if(eq_id!=i){
                    S_j.push_back(i);
                }
                S_js[w]=S_j;
            }
        }
        else if(eq_id>maxSIndex && eq_id<=maxLIndex){
            for(int i=maxSIndex+1;i<=maxLIndex;i++){
                if(eq_id!=i){
                    S_j.push_back(i);
                }
                S_js[w]=S_j;
            }
        }
        else{
            for(int i=maxLIndex+1;i<=maxXLIndex;i++){
                if(eq_id!=i){
                    S_j.push_back(i);
                }
                S_js[w]=S_j;
            }
        }
    }
    y = new GRBVar* [m];
    int y_counter=0;
    for (int w = 0; w < m; ++w)
    {
        int sj = int(S_js[w].size());
        y[w] = model.addVars(sj,GRB_BINARY);
        for (int p = 0; p < sj; ++p)
        {
            int pp=S_js[w][p];
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
        v[i] = model.addVars(EQMaxInd+1,GRB_INTEGER);
        for (int k = 0; k<=EQMaxInd;k++){
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
        int sj = int(S_js[j].size());
        for (int kk = 0; kk < sj; ++kk)
        {
            int k=S_js[j][kk];
            ntot += -y[j][kk];
        }
        stringstream cname1;
        cname1 << "Arc EQ change_"  << j;
        model.addConstr(ntot == 0, cname1.str());
    }
    GRBLinExpr ntot3 = 0;
    // balance constraints
    for (int i = 0; i < n-2; ++i)
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
        for (int k = 0; k<=EQMaxInd;k++){
            ntot3-=v[i][k];
            GRBLinExpr ntot1 = 0;
            GRBLinExpr ntot2 = 0;
            int inbound = _eqData._InboundEQ[make_pair(fac.name,Int2EQ[k])];
            int outbound = _eqData._OutboundEQ[make_pair(fac.name,Int2EQ[k])];
            
            ntot1+=inbound-outbound;
            ntot2+=outbound-inbound;
            for (int jj = 0; jj < inboundArcs.size(); ++jj)
            {
                int j = inboundArcs[jj];
                int dj = initialEq[j];
                if(k==dj)
                {
                    ntot1+= z[j];
                    ntot2-= z[j];
                }
                int sj = int(S_js[j].size());
                for (int kk = 0; kk < sj; ++kk)
                {
                    
                    int k=S_js[j][kk];
                    ntot1-= y[j][kk];
                    ntot2+= y[j][kk];
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
                int sj = int(S_js[j].size());
                for (int kk = 0; kk < sj; ++kk)
                {
                    
                    int k=S_js[j][kk];
                    ntot1+= y[j][kk];
                    ntot2-= y[j][kk];
                }
            }
            stringstream cname2,cname3;
            cname2 << "Balance 1_" << i << k;
            cname3 << "Balance 2_" << i << k;
            model.addConstr(ntot1 >= -v[i][k], cname2.str());
            model.addConstr(ntot2 >= -v[i][k], cname3.str());
        }
        
        
    }
    // number of changes cap constraint
    stringstream cname4;
    cname4 << "Maximum Changes";
    model.addConstr(ntot3 >= -param_N1, cname4.str());
    
    
    
    // The objective is to minimize the total fixed and variable costs
    model.set(GRB_IntAttr_ModelSense, GRB_MINIMIZE);
    // write model
    //model.write(this->_datafolder);
    string datafolder = this->_datafolder+"/model1.lp";
    model.write(datafolder);
    model.optimize();
    _model = &model;
    
}
void MIPSolver::solveModel(){
    //GRBModel model = *_model;
    //_model->optimize();
}

void MIPSolver::exportModel(){
    GRBModel model = *_model;
    //_model->optimize();
}
