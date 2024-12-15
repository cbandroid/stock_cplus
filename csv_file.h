#pragma once
#include <string>
#include <vector>
#include "include/StructKlineView.h"


class CsvFile {
public:
	CsvFile(const std::string& filename) : m_filename(filename){}

	~CsvFile() {
	}

    bool read(CL::Kline*& pData, int& outSize);
    bool read(std::vector<CL::Kline> &klines);

private:
	std::string m_filename;
	int parseDateToTime(const std::string& dateStr);
};
