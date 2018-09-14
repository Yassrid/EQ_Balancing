#include "CSVReader.h"
#include "pch.h"
#include <iterator>
#include <iostream>
#include <cstdlib>



CSVReader::CSVReader(){
    _inputfolder="";
}
CSVReader::CSVReader(string inputFile){
    _inputfolder=inputFile;
}
void CSVReader::readEQData(eq_data &eqdata){
    cout << "|-Reading Data:\n";
    readLoads(eqdata._Loads);
    readFacilities(eqdata._Facilities);
    readEquipments(eqdata._Equipments);
    cout << "|-Data colection done!:\n";
}
void CSVReader::readLoads(vector<Load> &loads){
    
    cout << "  -Loads...";
    vector<Load> Loads;
    string filename = _inputfolder+"loads_v2.csv";
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
        
        Load load;
        load.loadId = row[0];
        load.origin = row[5];
        load.destination = row[9];
        load.origin_sort = row[13][0];
        load.destination_sort = row[14][0];
        //load.origin_date = row[0];
        //load.due_date = stoi(row[16]);
        load.volume = stod(row[2]);
        load.weight = stod(row[3]);
        Loads.push_back(load);
    }
    cout << Loads.size() << " loads found.\n";
    loads = Loads;
}
void CSVReader::readFacilities(vector<Facility> &facilities){
    
    // Read Facilities-------------
    cout << "  -Facilities...";
    vector<Facility> Facilities;
    string filename = _inputfolder+"all_facilities_v2.csv";
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
        string s=row[12];
        s.erase(s.end()-1);
        
        Facility facility;
        facility.name = row[1];
        facility.slic = row[0];
        facility.region = row[2];
        facility.district = row[3];
        facility.city = row[8];
        facility.state = row[9];
        facility.country = row[10];
        facility.location_type = row[6];
        if(row[11]!="")
        {
            facility.latitude = stod(row[11]);
            facility.longitude = stod(s);
        }
        facility.long_restricted=(stoi(row[4])==1);
        facility.xlong_restricted=(stoi(row[5])==1);
        
        Facilities.push_back(facility);
    }
    cout << Facilities.size() << " facilities found.\n";
    facilities = Facilities;
}
void CSVReader::readEquipments(vector<Equipment> &equipments){
    
    cout << "  -Equipments...";
    vector<Equipment> Equipments;
    string filename = _inputfolder+"equipments_v2.csv";
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
        equipment.name = row[0];
        equipment.category = s;
        equipment.capacity = stod(row[7]);
        equipment.weight = stod(row[8]);
        equipment.inside_length = stod(row[2]);
        equipment.outside_length = stod(row[1]);
        equipment.inside_height = stod(row[4]);
        equipment.outside_height = stod(row[3]);
        equipment.inside_width = stod(row[6]);
        equipment.outside_width = stod(row[5]);
        equipment.pup_conversion = stod(row[9]);
        Equipments.push_back(equipment);
    }
    cout << Equipments.size() << " equipment types found.\n";
    equipments = Equipments;
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


