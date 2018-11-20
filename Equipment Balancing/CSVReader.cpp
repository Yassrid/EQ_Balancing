#include "CSVReader.h"
#include <iterator>
#include <iostream>
#include <cstdlib>



CSVReader::CSVReader(){
    _inputfolder="";
    _district ="ALL";
}
/*CSVReader::CSVReader(string inputFile, int esm){
    _inputfolder=inputFile;
    _region ="ALL";
    _district="ALL";
    _esm = esm;
}*/
CSVReader::CSVReader(string inputFile, int esm, string region, string district){
    _inputfolder=inputFile;
    _district = district;
    _region = region;
    _esm = esm;
}
void CSVReader::readEQData(eq_data &eqdata){
    cout << "|-Reading Data:\n";
    eqdata._datafolder = this->_inputfolder;
    readFacilities(eqdata._Facilities);
    readLoadChains(eqdata._Loads);
    readEquipments(eqdata._Equipments);
    readESM(eqdata._ESM);
    readESMEQ(eqdata._EQNames);
    eqdata._NEquipments =int(eqdata._EQNames.size());
    cout << "|-Data collection done!:\n";
}

void CSVReader::readFacilities(vector<Facility> &facilities){
    
    // Read Facilities-------------
    cout << "  -Facilities...";
    vector<Facility> Facilities;
    string filename = _inputfolder+"all_facilities_Current_1113.csv";
    ifstream file(filename);
    if (!file.is_open()) {
        
        cout << "File " << filename << " not found." << endl;
        exit(-1);
    }
    
    CSVRow row;
    int iter = 0;
    while (file >> row)
    {
        
        iter++;
        if (iter == 1)
            continue;
        if(_district!="ALL")
        {
            if (_district!=row[3])
                continue;
        }
        else if(_region!="ALL")
        {
            if (_region!=row[2])
                continue;
        }
        
        
        Facility facility;
        facility.slic = row[0];
        facility.name = removeSpaces(row[1]);
        facility.region = row[2];
        facility.district = row[3];
        facility.state = row[4];
        facility.country = row[5];
        facility.operations_type = row[6];
        if(row[12]!="")
        {
            // deleting the "\r" at the end
            //string s=row[13];
            //s.erase(s.end()-1);
            //facility.latitude = stod(row[12]);
            //facility.longitude = stod(s);
        }
        facility.long_restricted=stoi(row[7]);
        string XLRes=row[8];
        facility.xlong_restricted=stoi(string(1,XLRes[0]));;//(stoi(row[5])==1);
        
        Facilities.push_back(facility);
    }
    // Add the two dummy facilities
    if(_district!="ALL"){
        Facility dummySender;
        dummySender.name = "dummySender";
        dummySender.district = "-1";
        Facility dummyReceiver;
        dummyReceiver.name = "dummyReceiver";
        dummyReceiver.district = "-1";
        Facilities.push_back(dummySender);
        Facilities.push_back(dummyReceiver);
    }
    else if(_region!="ALL"){
        Facility dummySender;
        dummySender.name = "dummySender";
        dummySender.district = "-1";
        Facility dummyReceiver;
        dummyReceiver.name = "dummyReceiver";
        dummyReceiver.district = "-1";
        Facilities.push_back(dummySender);
        Facilities.push_back(dummyReceiver);
    }
    cout << Facilities.size() << " facilities found.\n";
    facilities = Facilities;
}
void CSVReader::readEquipments(vector<Equipment> &equipments){
    
    cout << "  -Equipments...";
    vector<Equipment> Equipments;
    string filename = _inputfolder+"equipments_vf.csv";
    ifstream file(filename);
    if (!file.is_open()) {
        
        cout << "File " << filename << " not found." << endl;
        exit(-1);
    }
    
    CSVRow row;
    int iter = 0;
    while (file >> row)
    {
        iter++;
        if (iter == 1)
            continue;
        // deleting the "\r" at the end
        string s=row[10];
        s.erase(s.end()-1);
        Equipment equipment;
        equipment.name = removeSpaces(row[0]);
        equipment.category = removeSpaces(s);
        equipment.capacity = stod(row[7]);
        equipment.weight = stod(row[8]);
        equipment.inside_length = stod(row[2]);
        equipment.outside_length = stod(row[1]);
        equipment.inside_height = stod(row[4]);
        equipment.outside_height = stod(row[3]);
        equipment.inside_width = stod(row[6]);
        equipment.outside_width = stod(row[5]);
        equipment.pup_conversion = stod(row[9]);
        if(equipment.name=="YY" || equipment.name=="Y"  || //equipment.name=="TMF" || equipment.name=="TMB" ||
           equipment.name=="S" || equipment.name=="OO" || equipment.name=="O")// || equipment.name=="LC")
            continue;
        Equipments.push_back(equipment);
    }
    cout << Equipments.size() << " equipment types found.\n";
    equipments = Equipments;
}
void CSVReader::readLoadChains(vector<Load> &loads){
    
    cout << "  -Load Chains...";
    vector<Load> Loads;
    string filename = _inputfolder+"load_chains_Current_1119_REGDIS.csv";
    ifstream file(filename);
    if (!file.is_open()) {
        
        cout << "File " << filename << " not found." << endl;
        exit(-1);
    }
    
    CSVRow row;
    int iter = 0;
    while (file >> row)
    {
        iter++;
        if (iter == 1)
            continue;
        // deleting the "\r" at the end
        string destDis=row[15];
        destDis.erase(destDis.end()-1);
        if(_region!="ALL")
            if (_region!=(row[12]) && _region!=row[14])
                continue;
        if(_district!="ALL")
            if (_district!=row[13] && _district!=destDis)
                continue;
        Load load;
        
        
        load.loadId = row[0];
        load.origin = removeSpaces(row[10]);
        load.destination = removeSpaces(row[11]);
        load.origin_day = row[1];
        load.volume = stod(row[3]);
        load.trailer_max_number = stoi(row[4]);
        load.equipment=removeSpaces(row[2]);
        load.equipment_new=removeSpaces(row[2]);
        if (load.equipment == "BT")
            continue;
        if(_district!="ALL"){
            if (_district!=row[13])
                load.origin = "DummySender";
            if (_district!=destDis)
                load.destination = "DummyReceiver";
        }
        else{
            if(_region!="ALL"){
                if (_region!=row[12])
                    load.origin = "DummySender";
                if (_region!=row[14])
                    load.destination = "DummyReceiver";
            }
            }
        
        Loads.push_back(load);
    }
    cout << Loads.size() << " chains found.\n";
    loads = Loads;
}
void CSVReader::readESM(map<pair<int,int>,int> &ESM){
    
    cout << "  -EQ Substitution Matrix...";
    map<pair<int,int>,int> Esm;
    string filename = _inputfolder+"esm"+to_string(_esm)+".csv";
    ifstream file(filename);
    if (!file.is_open()) {
        
        cout << "File " << filename << " not found." << endl;
        exit(-1);
    }
    
    CSVRow row;
    int iter = 0;
    while (file >> row)
    {
        iter++;
        if (iter == 1)
            continue;
        int NRow = int(row.size());
        for (int i=0;i<NRow-1;i++)
        {
            string s=row[i+1];
            Esm[make_pair(iter-2,i)]=stoi(string(1,s[0]));
        }
        
    }
    cout << Esm.size() << " equipment substitutions found.\n";
    ESM = Esm;
}
void CSVReader::readESMEQ(vector<string> &EQNames){
    
    //cout << "  -EQ Substitution Matrix Equipment...";
    vector<string> names;
    string filename = _inputfolder+"esm"+to_string(_esm)+".csv";
    ifstream file(filename);
    if (!file.is_open()) {
        
        cout << "File " << filename << " not found." << endl;
        exit(-1);
    }
    
    CSVRow row;
    file >> row;
    
    int NRow = int(row.size());
    string s=row[NRow-1];
    s.erase(s.end()-1);
    for(int i=0;i<NRow-2;i++)
    {
        names.push_back(row[i+1]);
    }
    names.push_back(s);
    EQNames = names;
}
void CSVRow::readNextRow(std::istream& str){
    std::string         line;
    std::getline(str, line);
    
    std::stringstream   lineStream(line);
    std::string         cell;
    
    m_data.clear();
    while (getline(lineStream, cell, ','))
    {
        m_data.push_back(cell);
    }
    // This checks for a trailing comma with no data after it.
    if (!lineStream && cell.empty())
    {
        // If there was a trailing comma then add an empty element.
        m_data.push_back("");
    }
}
std::istream& operator>>(std::istream& str, CSVRow& data)
{
    data.readNextRow(str);
    return str;
}
string removeSpaces(string str)
{
    str.erase(remove(str.begin(), str.end(), ' '), str.end());
    return str;
}

void CSVReader::readLoads(vector<Load> &loads){
    
    cout << "  -Loads...";
    vector<Load> Loads;
    //string filename = _inputfolder+"loads_v2.csv";
    string filename = _inputfolder+"load_chains_live0726_6pm_1106.csv";
    ifstream file(filename);
    if (!file.is_open()) {
        
        cout << "File " << filename << " not found." << endl;
        exit(-1);
    }
    
    CSVRow row;
    int iter = 0;
    while (file >> row)
    {
        iter++;
        if (iter == 1)
            continue;
        if(_district!="ALL")
            if (_district!=row[12] && _district!=row[8])
                continue;
        Load load;
        if(iter==1144)
            iter =iter;
        if(load.equipment=="BT")
            continue;
        load.loadId = row[0];
        load.origin = removeSpaces(row[5]);
        load.destination = removeSpaces(row[9]);
        load.origin_sort = row[13][0];
        load.destination_sort = row[14][0];
        load.origin_day = row[17];
        //load.due_date = stoi(row[16]);
        load.volume = stod(string(row[2]));
        load.weight = stod(row[3]);
        load.equipment=removeSpaces(row[4]);
        load.equipment_new=removeSpaces(row[4]);
        if(_district!="ALL"){
            if (_district!=row[8])
                load.origin = "DummySender";
            if (_district!=row[12])
                load.destination = "DummyReceiver";
        }
        if(load.origin=="SEAIL")
            load=load;
        Loads.push_back(load);
    }
    cout << Loads.size() << " loads found.\n";
    loads = Loads;
}
