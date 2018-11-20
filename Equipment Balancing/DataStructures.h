// Equipment Balancing



#include <vector>
#include <string>


using namespace std;
struct Load
{
	string loadId;
	string origin;
    string origin_dis;
    string origin_reg;
	string destination;
	string origin_day;
    string destination_dis;
    string destination_reg;
	string due_day;
	char origin_sort;
	char destination_sort;

    int trailer_max_number;
	double volume;
	double weight;
    
	string equipment;
    string equipment_new;
};

struct Equipment
{
	string name;
	string category;
    
    double outside_length;
    double inside_length;
    double outside_height;
    double inside_height;
    double outside_width;
    double inside_width;
    double capacity;
    double weight;
    double pup_conversion;
    
};

struct Facility
{
	string name;
    string slic;
	string district;
	string region;
	string city;
    string state;
	string country;
    string location_type;
    string operations_type;
	double longitude;
	double latitude;

	bool long_restricted;
    bool xlong_restricted;
};
struct Imbalance{
    string facility;
    string category;
    
    int inbound=0;
    int outbound=0;
};
