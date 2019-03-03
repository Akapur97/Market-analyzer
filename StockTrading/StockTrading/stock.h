#pragma once
#ifndef STOCK_H
#define STOCK_H

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <unordered_map>

class Stock {
	private:
		std::map<std::string, std::unordered_map<std::string, double> > data_;
	public:
		Stock(const std::string &DATA_FILE_NAME);
		void ImportCsvData(const std::string &DATA_FILE_NAME);
		double GetPrice(std::string &date, const std::string type);
		std::map<std::string, double> GetPrice(std::string &startDate, std::string &endDate, const std::string type);
		void ExportToCsv(std::string CSV_FILE_NAME);
		void CalculateDailyPriceChange();
		~Stock();
};

#endif	// ! STOCK_H