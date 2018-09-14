
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
    CSVReader csv_reader= CSVReader(inputFolder);
    eq_data EQ_data;
    csv_reader.readEQData(EQ_data);
    
    
    // building a mapping from category type to integer
    map<string, int> EQ2Int;
    EQ2Int["W"]=0;
    EQ2Int["WWW"]=1;
    EQ2Int["WWS"]=2;
    EQ2Int["L"]=3;
    EQ2Int["H"]=4;
    EQ2Int["MS"]=5;
    EQ2Int["F"]=6;
    EQ2Int["Z"]=7;
    EQ2Int["ZZ"]=8;
    EQ2Int["ZZS"]=9;
    EQ2Int["TMB"]=10;
    EQ2Int["TMF"]=11;
    EQ2Int["ZZZ"]=12;
    EQ2Int["RBL"]=13;
    EQ2Int["E"]=14;
    EQ2Int["N"]=15;
    
    EQ_data._EQ2Int=EQ2Int;
	
    // Creating the MIP Model on Gurobi
    MIPSolver mip_solver = MIPSolver(EQ_data);
    mip_solver.createModel1();
    
    
    
    
    
    
    return 0;
}