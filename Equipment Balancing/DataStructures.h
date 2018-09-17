// Equipment Balancing



#include <vector>
#include <string>
#include <stdio.h>

using namespace std;
struct Load
{
	string loadId;
	string origin;
	string destination;
	string origin_date;
	string due_date;
	char origin_sort;
	char destination_sort;


	double volume;
	double weight;
    
	string equipment;
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

	double longitude;
	double latitude;

	bool long_restricted;
    bool xlong_restricted;
};
struct Imbalance{
    string facility;
    string category;
    
    int outbound=0;
    int inbound=0;
};
