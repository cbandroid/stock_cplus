#include "csv_file.h"
#include "time_util.h"
#include <iostream>

#include <sstream>
#include <fstream>
#include <ctime>

bool CsvFile::read(CL::Kline*& pData, int& outSize) {
	std::ifstream file(m_filename);

	if (!file.is_open()) {
		std::cerr << "Error opening file!" << m_filename << std::endl;
		outSize = 0;
		return false;
	}

	std::string line;
    std::vector<CL::Kline> klines;

	// Skip the header line
	std::getline(file, line);

	while (std::getline(file, line)) {
		std::stringstream ss(line);
		std::string token;
        CL::Kline kline;

		// Parse each field from the CSV line
		std::getline(ss, token, ',');
        kline.date = TimeUtil::parseDateToTime(token);

		std::getline(ss, token, ',');
		kline.open = std::stof(token);

		std::getline(ss, token, ',');
		kline.high = std::stof(token);

		std::getline(ss, token, ',');
		kline.low = std::stof(token);

		std::getline(ss, token, ',');
		kline.close = std::stof(token);

		//std::getline(ss, token, ',');
		//kline.vol = std::stof(token);
		kline.vol = 0;
		// Initialize with default values
		kline.amount = 0.0f;
		kline.position = 0.0f;
		kline.volPositive = 0.0f;

		klines.push_back(kline);
	}

	file.close();
	
	if (klines.empty())
		return false;

	// If pData is not null, release the existing memory
	if (pData != nullptr) {
		delete[] pData;
	}

	// Allocate memory for pData and copy klines data
	outSize = klines.size();


    pData = new CL::Kline[outSize];

	std::copy(klines.begin(), klines.end(), pData);
    return true;
}


bool CsvFile::read(std::vector<CL::Kline>& klines) {
	std::ifstream file(m_filename);

	if (!file.is_open()) {
		std::cerr << "Error opening file!" << m_filename << std::endl;
		//outSize = 0;
		return false;
	}

	std::string line;
	//std::vector<Kline> klines;

	// Skip the header line
	std::getline(file, line);

	while (std::getline(file, line)) {
		std::stringstream ss(line);
		std::string token;
        CL::Kline kline;

		// Parse each field from the CSV line
		std::getline(ss, token, ',');
        kline.date = TimeUtil::parseDateToTime(token);

		std::getline(ss, token, ',');
		kline.open = std::stof(token);

		std::getline(ss, token, ',');
		kline.high = std::stof(token);

		std::getline(ss, token, ',');
		kline.low = std::stof(token);

		std::getline(ss, token, ',');
		kline.close = std::stof(token);

		//std::getline(ss, token, ',');
		//kline.vol = std::stof(token);
		kline.vol = 0;
		// Initialize with default values
		kline.amount = 0.0f;
		kline.position = 0.0f;
		kline.volPositive = 0.0f;

		klines.push_back(kline);
	}

	file.close();

	if (klines.empty())
		return false;

	int outSize = klines.size();

	return true;
}

