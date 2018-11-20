//#define _MULTOBJ_GRB
#include <iterator>
#include <iostream>
#include <cstdlib>
#include "CSVReader.h"
#include "gurobi_c++.h"
#include "MIPSolver.h"
#include <ctime>
using namespace std;


int main()
{
    cout << "<----------UPS EQUIPMENT BALANCING---------->\n";
    
    // set three timers : timer1 for data reading, timer2 for first optimization model. timer3 for second optimization model
    clock_t start1, start2,start3;
    double duration1,duration2,duration3;
    
    
    
    // Reading input data
    start1 = clock();
    // Choose folder where the input/output files will be saved
    string inputFolder = "/Users/yridouane/Desktop/Eqi/EquipmentBalancing/data/";
    string district = "ALL";
    string region = "ALL";
    int esm = 2;
    
    CSVReader csv_reader= CSVReader(inputFolder,esm,region,district);
    //CSVReader csv_reader= CSVReader(inputFolder,district);
    
    //Initialize eq_data object
    eq_data EQ_data;
    csv_reader.readEQData(EQ_data);
    
    //Generate static mappings needed in the optimzation model
    EQ_data.mapEQ2Int();
    EQ_data.mapEQ2PupConv();
    EQ_data.mapEQ2Category();
    EQ_data.mapFac2XLRestriction();
    EQ_data.mapFacilityEQ();
    int _NLoads = int(EQ_data._Loads.size());
    int initial_imbalance = EQ_data.computeImbalance();
    duration1 = ( clock() - start1 ) / (double) CLOCKS_PER_SEC;
    
#ifdef _MULTOBJ_GRB
    MIPSolver mip_solverH = MIPSolver(EQ_data);
    mip_solverH.createModelH();//(param_N1);
    mip_solverH.solveModel();
	int min_substititution2= mip_solverH.printResults(1);

#else
    // Solve model 1 : minimize imbalance
    start2 = clock();
    MIPSolver mip_solver1 = MIPSolver(EQ_data);
    mip_solver1.createModel1();
    mip_solver1.solveModel();
    int min_imbalance= mip_solver1.printResults();
    duration2 = ( clock() - start2 ) / (double) CLOCKS_PER_SEC;

    // Solve model 2 : minimize number of subsitutions
    start3 = clock();
    MIPSolver mip_solver2 = MIPSolver(EQ_data);
    mip_solver2.createModel2(min_imbalance);
    mip_solver2.solveModel(1);
    int min_substititution= mip_solver2.printResults(1);
    duration3 = ( clock() - start3 ) / (double) CLOCKS_PER_SEC;
    
    cout<< "Imbalance reduction : " << initial_imbalance-min_imbalance << "(" << double(initial_imbalance-min_imbalance)/double(initial_imbalance)*100 << ")" << endl;
    cout<< "Min # substitions : " << min_substititution << "(" << double(min_substititution)/double(_NLoads)*100 << ")" << endl;
    cout << "runtime data : " << duration1 << endl;
    cout << "runtime Opt1 : " << duration2 << endl;
    cout << "runtime Opt2 : " << duration3 << endl;
#endif
    
    
    
    
    
    
    return 0;
}


