#include "stdafx.h"
#include "stock.h"
#include <algorithm>

Stock::Stock(const std::string &DATA_FILE_NAME) {
	ImportCsvData(DATA_FILE_NAME);
}

void Stock::ImportCsvData(const std::string &DATA_FILE_NAME) {
	// Open CSV file
	std::ifstream dataSearch(DATA_FILE_NAME);
	std::string row;

	// Iterate through the CSV file
	while (std::getline(dataSearch, row)) {
		std::string date;
		std::replace(row.begin(), row.end(), ',', ' ');
		std::stringstream ss(row);
		std::unordered_map<std::string, double> temp;

		ss >> date;
		ss >> temp["Open"];
		ss >> temp["High"];
		ss >> temp["Low"];
		ss >> temp["Close"];
		ss >> temp["Adj close"];
		ss >> temp["Volume"];

		data_[date] = temp;
	}
}

double Stock::GetPrice(std::string &date, std::string type) {
	return data_[date][type];
}

std::map<std::string, double> Stock::GetPrice(std::string &startDate, std::string &endDate, const std::string type) {
	std::map<std::string, double> close;
	auto itBegin = data_.find(startDate);
	auto itEnd = data_.find(endDate);
	for (auto it = itBegin; it != itEnd; ++it) {
		close[it->first] = data_[it->first][type];
	}
	close[itEnd->first] = data_[itEnd->first][type];
	return close;
}

// Export stock data to .csv file
void Stock::ExportToCsv(std::string CSV_FILE_NAME) {
	// Create and open a new CSV file
	std::ofstream csvFile;
	csvFile.open(CSV_FILE_NAME);

	// Input headings in CSV file
	csvFile << "Date,";
	auto itInnerMap = (data_[(++data_.begin())->first]).begin();
	for (; itInnerMap != data_[(++data_.begin())->first].end(); ++itInnerMap) {
		csvFile << itInnerMap->first << ",";
	}
	csvFile << "\n";

	// Start storing data
	auto itOuterMap = (++data_.rbegin());
	for (; itOuterMap != data_.rend(); ++itOuterMap) {
		csvFile << itOuterMap->first << ",";
		for (auto &val : data_[itOuterMap->first]) {
			csvFile << val.second << ",";
		}
		csvFile << "\n";
	}
	csvFile.close();
}

// Calculate daily price change in % and store in data_
void Stock::CalculateDailyPriceChange() {
	auto it = (++data_.rbegin());
	for (; it != (--data_.rend()); ++it) {
		auto nextIt = std::next(it, 1);
		data_[it->first]["% Change (24 hours)"] = ((data_[it->first]["Close"] - data_[nextIt->first]["Close"])
												/ (data_[nextIt->first]["Close"])) * 100;
	}
}

Stock::~Stock() {}
