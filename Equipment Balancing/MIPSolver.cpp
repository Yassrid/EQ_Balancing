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
    _env = new GRBEnv();
    _eqData = eqdata;
    _model = new GRBModel(*_env);
}
void MIPSolver::createModel1(){
    GRBModel model = *_model;
    model.set(GRB_StringAttr_ModelName, "Model1");
    
    vector<Load> loads = _eqData._Loads;
    int m = loads.size();
    //data
    vector<int> Objective1(m, 1);
    
    
    //decison variables
    //z
    GRBVar* z=0;
    z = model.addVars(m, GRB_BINARY);
    for (int p = 0; p < m; ++p)
    {
        ostringstream vname;
        vname << "Z" << p;
        z[p].set(GRB_DoubleAttr_Obj, Objective1[p]);
        z[p].set(GRB_StringAttr_VarName, vname.str());
    }
    
    //define variable y
    GRBVar** y = 0;
    map<int,vector<int>> S_js;

    for (int w = 0; w < m; ++w)
    {
        Load ld = _eqData._Loads[w];
        string eq_type =ld.equipment;
        int eq_id = _eqData._EQ2Int[eq_type];
        vector<int> S_j;
        if(eq_id<=6){
            for(int i=0;i<=6;i++){
                if(eq_id!=i){
                    S_j.push_back(i);
                }
                S_js[w]=S_j;
            }
        }
        else if(eq_id>6 && eq_id<=11){
            for(int i=7;i<=11;i++){
                if(eq_id!=i){
                    S_j.push_back(i);
                }
                S_js[w]=S_j;
            }
        }
        else{
            for(int i=12;i<=15;i++){
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
        int sj = S_js[w].size();
        y[w] = model.addVars(sj);
        for (int p = 0; p < sj; ++p)
        {
            y_counter++;
            ostringstream vname;
            vname << "Y" << p << "." << w;
            y[w][p].set(GRB_StringAttr_VarName, vname.str());
        }
    }
    
    
    
    // The objective is to minimize the total fixed and variable costs
    model.set(GRB_IntAttr_ModelSense, GRB_MINIMIZE);
}
