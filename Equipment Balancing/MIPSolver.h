//
//  MIPSolver.h
//  Equipment Balancing
//
//  Created by Yassine Ridouane on 9/14/18.
//  Copyright © 2018 Yassine Ridouane. All rights reserved.
//

#ifndef MIPSolver_h
#define MIPSolver_h

//#include <stdio.h>
#include <sstream>
#include "gurobi_c++.h"
#include "EQData.h"
#define EPSILON 0.0001
//#include <boost/bind.hpp>

class MIPSolver {
public:
    GRBEnv* _env;
    GRBModel* _model;
    eq_data _eqData;
    MIPSolver();
    MIPSolver(eq_data $eqdata);
    void createModel1();
};


#endif /* MIPSolver_h */
