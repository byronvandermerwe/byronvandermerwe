//============================================================================
// Name        : VehPositionApp.cpp
// Author      : Byron
// Version     :
// Copyright   : Your copyright notice
// Description : Find the position id of co-oordinates from a given list.
//============================================================================

#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>
#include <chrono>

using namespace std;
using namespace std::chrono;

struct VehData {
	int32_t PositionId;
	string Registration;
	uint64_t TimeStamp;
};

struct Gps {
	float Latitude;
	float Longitude;
};

struct Result {
	int32_t PositionId;
	float Latitude;
	float Longitude;
	float DistanceBetweenLatitude;
	float DistanceBetweenLongitude;
	float CompareLatitude;
	float CompareLongitude;
};

//Operator to compare Gps struct
bool operator<(const Gps& l, const Gps& r) {
	return ((l.Latitude != r.Latitude) && (l.Longitude != r.Longitude));
}

struct sortclass {
	bool operator() (Gps g, Gps h) {
		return (g.Latitude < h.Latitude);
  	}
} sortobject;

///Read vehicle dat from file and create a map with it containing the co-ordinates as the key
///and PositionId, Registration and TimeStamp as value
vector<pair<Gps, VehData>> ReadVehData(FILE *infile) {
	vector<pair<Gps, VehData>> vehData{};

	while (!feof(infile)) {
		VehData veh;
		//Get PositionId
		fread(&veh.PositionId, 1, sizeof(veh.PositionId), infile);
		//Get Registration
		char i{0};
		stringstream ss;
		do {
			fread(&i, 1, 1, infile);
			ss << i;
		}
		while (i != 0);
		veh.Registration = ss.str();
		//Get Latitude
		Gps gps;
		char *b = (char *)&gps.Latitude;
		for (uint32_t x = 0; x < sizeof(gps.Latitude); x++) {
			fread(&b[x], 1, 1, infile);
		}
		//Get Longitude
		b = (char *)&gps.Longitude;
		for (uint32_t x = 0; x < sizeof(gps.Longitude); x++) {
			fread(&b[x], 1, 1, infile);
		}
		//Get TimeStamp
		for (uint32_t x = 0; x < sizeof(veh.TimeStamp); x++) {
			fread(&i, 1, 1, infile);
			veh.TimeStamp |= (i << x);
		}
		vehData.push_back({gps,veh});
	}
	return vehData;
}

bool compare(pair<Gps, VehData>& a, pair<Gps, VehData>& b) {
	if (a.first.Latitude <= b.first.Latitude) {
		if (a.first.Longitude < b.first.Longitude) {
			return true;
		}
	}
	return false;
}

vector<pair<Gps, VehData>> GetSortedData(vector<pair<Gps, VehData>>& Data) {
    vector<pair<Gps, VehData>> sortedData;

    // Copy key-value pair from Map
    // to vector of pairs
    for (auto& x : Data) {
        sortedData.push_back(x);
    }

    // Sort using comparator function
    sort(sortedData.begin(), sortedData.end(), compare);

    return sortedData;
}

Result FindNearest(vector<pair<Gps, VehData>> &sortedData, const Gps &Compare) {
    auto sdx = 0.0;
    auto sdy = 0.0;
    pair<Gps, VehData> closestPosition;
	for (const auto &val : sortedData) {
		float dtx;
		if (val.first.Latitude > Compare.Latitude)
			dtx = val.first.Latitude - Compare.Latitude;
		else
			dtx = Compare.Latitude - val.first.Latitude;

		if(dtx != 0.0 && sdx == 0.0)
			sdx = dtx;

		float dty;

		if (val.first.Longitude > Compare.Longitude)
			dty = val.first.Longitude > Compare.Longitude;
		else
			dty = Compare.Longitude - val.first.Longitude;

		if (dty != 0.0 && sdy == 0.0)
			sdy = dty;

	   	if (dtx <= sdx) {
			sdx = dtx;
		  	if (dty < sdy) {
			  	sdy = dty;
			  	closestPosition.first.Latitude = val.first.Latitude;
			  	closestPosition.first.Longitude = val.first.Longitude;
			  	closestPosition.second.PositionId = val.second.PositionId;
			}
	   	}
	}
	Result res;
	res.PositionId = closestPosition.second.PositionId;
	res.Latitude = 	closestPosition.first.Latitude;
	res.Longitude =	closestPosition.first.Longitude;
	res.DistanceBetweenLatitude = sdx;
	res.DistanceBetweenLongitude = sdy;
	res.CompareLatitude = Compare.Latitude;
	res.CompareLongitude = Compare.Longitude;
	
	return res;
}

int main(int argc, char **argv) {
	vector<Gps> CompareList;
	Gps g;

	//Position 1
	g.Latitude = 34.544909;
	g.Longitude = -102.100843;
	CompareList.push_back(g);
	//Position 2
	g.Latitude = 32.345544;
	g.Longitude = -99.123124;
	CompareList.push_back(g);
	//Position 3
	g.Latitude = 33.234235;
	g.Longitude = -100.214124;
	CompareList.push_back(g);
	//Position 4
	g.Latitude = 35.195739;
	g.Longitude = -95.348899;
	CompareList.push_back(g);
	//Position 5
	g.Latitude = 31.895839;
	g.Longitude = -97.789573;
	CompareList.push_back(g);
	//Position 6
	g.Latitude = 32.895839;
	g.Longitude = -101.789573;
	CompareList.push_back(g);
	//Position 7
	g.Latitude = 34.115839;
	g.Longitude = -100.225732;
	CompareList.push_back(g);
	//Position 8
	g.Latitude = 32.335839;
	g.Longitude = -99.992232;
	CompareList.push_back(g);
	//Position 9
	g.Latitude = 33.535339;
	g.Longitude = -94.792232;
	CompareList.push_back(g);
	//Position 10
	g.Latitude = 32.234235;
	g.Longitude = -100.222222;
	CompareList.push_back(g);

    if (argc != 2) {
       fprintf(stderr, "usage: %s \"path to file\"\n", argv[0]);
       exit(0);
    }

	auto path = argv[1];
	auto inFile = fopen(path, "rb");
	if (inFile != NULL) {
		auto start = high_resolution_clock::now();
		auto vehData = ReadVehData(inFile);
		auto sortedData = GetSortedData(vehData);
		auto stop = high_resolution_clock::now();
		auto duration = duration_cast<microseconds>(stop - start);
		cout << "Data file read execution time: " << (duration.count() * 0.0001) << " ms" << endl << endl;
		
		vector<Result> results;
		start = high_resolution_clock::now();
		for (const auto &x : CompareList) {
			auto res = FindNearest(sortedData, x);
			results.push_back(res);
		}
		stop = high_resolution_clock::now();
		auto duration2 = duration_cast<microseconds>(stop - start);
		cout << "Closest position calculation execution time: " << (duration2.count() * 0.0001) << " ms" << endl << endl;

		cout << "Total execution time : " << (duration.count() * 0.0001) + (duration2.count() * 0.0001) << " ms" << endl << endl;

		for (const auto &res : results) {
			printf("PositionId %d\t<%f:%f>\tclosest distance calculated <%f:%f> ==> compared position <%f:%f>\n",
					res.PositionId,
					res.Latitude,
					res.Longitude,
					res.DistanceBetweenLatitude,
					res.DistanceBetweenLongitude,
					res.CompareLatitude,
					res.CompareLongitude);
		}
		fclose(inFile);
	}
	else {
		fprintf(stderr, "File not found - %s", path);
	}

	return 0;
}
   
