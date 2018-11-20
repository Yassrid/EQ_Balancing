#include <string>
#include <set>
#include <vector>
#include <fstream>
#include <sstream>
#include "EQData.h"

class CSVReader {
public:
    string _inputfolder;
    string _district;
    string _region;
    int _esm;
    CSVReader();
	//CSVReader(string inputFile, int esm=1);
    CSVReader(string inputFile, int esm=1,string region="ALL", string district="ALL");
    void readLoads(vector<Load> &loads);
    void readLoadChains(vector<Load> &loads);
    void readFacilities(vector<Facility> &facilties);
    void readEquipments(vector<Equipment> &equipments);
    void readESM (map<pair<int,int>,int> &ESM);
    void readESMEQ (vector<string> &EQNames);
    void readEQData(eq_data &eq_data );
private:
	/* secrets */
};
class CSVWriter {
public:
	CSVWriter(const std::string &outputFile);
	void writeNextLine(const std::vector<std::string> &fields);
private:
	/* more secrets */
};
class CSVRow
{
public:
    std::string const& operator[](std::size_t index) const
    {
        return m_data[index];
    }
    std::size_t size() const
    {
        return m_data.size();
    }
    void readNextRow(std::istream& str);
    
private:
    std::vector<std::string>    m_data;
};

std::istream& operator>>(std::istream& str, CSVRow& data);
string removeSpaces(string world);
