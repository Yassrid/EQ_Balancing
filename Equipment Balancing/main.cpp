
#include <iterator>
#include <iostream>
#include <cstdlib>
#include "CSVReader.h"
#include "gurobi_c++.h"
#include "MIPSolver.h"
using namespace std;


int main()
{
    cout << "<----------UPS EQUIPMENT BALANCING---------->\n";
    
    // Reading input data
    string inputFolder = "/Users/yridouane/Desktop/Eqi/EquipmentBalancing/data/";
    string district = "46";
    //CSVReader csv_reader= CSVReader(inputFolder);
    CSVReader csv_reader= CSVReader(inputFolder,district);
    eq_data EQ_data;
    csv_reader.readEQData(EQ_data);
    EQ_data.mapEQ2Int();
    EQ_data.mapFacilityEQ();
    //EQ_data.computeImbalance();
    
    
    
    
	
    // Creating the MIP Model on Gurobi
    MIPSolver mip_solver = MIPSolver(EQ_data);
    int param_N1 = 2600;
    mip_solver.createModel1(param_N1);
    mip_solver.solveModel();
    
    
    
    
    
    
    
    return 0;
}

/*try {
    GRBEnv env = GRBEnv();
    
    GRBModel model = GRBModel(env);
    
    // Create variables
    
    GRBVar x = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "x");
    GRBVar y = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "y");
    GRBVar z = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "z");
    
    // Set objective: maximize x + y + 2 z
    
    model.setObjective(x + y + 2 * z, GRB_MAXIMIZE);
    
    // Add constraint: x + 2 y + 3 z <= 4
    
    model.addConstr(x + 2 * y + 3 * z <= 4, "c0");
    
    // Add constraint: x + y >= 1
    
    model.addConstr(x + y >= 1, "c1");
    
    // Optimize model
    
    model.optimize();
    
    cout << x.get(GRB_StringAttr_VarName) << " "
    << x.get(GRB_DoubleAttr_X) << endl;
    cout << y.get(GRB_StringAttr_VarName) << " "
    << y.get(GRB_DoubleAttr_X) << endl;
    cout << z.get(GRB_StringAttr_VarName) << " "
    << z.get(GRB_DoubleAttr_X) << endl;
    
    cout << "Obj: " << model.get(GRB_DoubleAttr_ObjVal) << endl;
    
    } catch(GRBException e) {
        cout << "Error code = " << e.getErrorCode() << endl;
        cout << e.getMessage() << endl;
    } catch(...) {
        cout << "Exception during optimization" << endl;
    }*/
