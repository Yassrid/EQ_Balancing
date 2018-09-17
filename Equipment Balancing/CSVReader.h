#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "EQData.h"

class CSVReader {
public:
    string _inputfolder;
    string _district;
    CSVReader();
	CSVReader(string inputFile);
    CSVReader(string inputFile, string district);
    void readLoads(vector<Load> &loads);
    void readFacilities(vector<Facility> &facilties);
    void readEquipments(vector<Equipment> &equipments);
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
