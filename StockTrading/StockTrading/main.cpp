// LibCurl.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#include "marketdatafetcher.h"
#include "stock.h"

int main()
{
	std::string stockName = "BTC-USD";
	std::string startDate = "2018-03-02";
	std::string endDate = "2019-03-03";

	// Instantiate MarketDataFetcher object to start
	// transfer data from Yahoo Finance API
	MarketDataFetcher *yahooFinanceApi = new MarketDataFetcher();
	yahooFinanceApi->FetchData(stockName, startDate, endDate);

	// stockFactory holds a list of stock pointer
	// objects by name
	std::map<std::string, Stock*> stockFactory;
	stockFactory[stockName] = new Stock(yahooFinanceApi->GetDataFileName());
	Stock *bitcoin = stockFactory[stockName];

	// closeData holds the "Close" data for bitcoin
	// stock for the given date range
	std::map<std::string, double> closeData = bitcoin->GetPrice(startDate, endDate, std::string("Close"));

	// Print the closeData calculated above
	for (auto &val : closeData) {
		std::cout << "Date: " << val.first << "		Close Price: " << val.second << std::endl;
	}
	bitcoin->CalculateDailyPriceChange();
	bitcoin->ExportToCsv("BTC_USD.csv");

	// Cleanup
	delete yahooFinanceApi;
	delete bitcoin;
	bitcoin = nullptr;
	yahooFinanceApi = nullptr;
	stockFactory[stockName] = nullptr;

	// Paus terminal window
	system("PAUSE");
	return 0;
}