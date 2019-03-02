// LibCurl.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#include "marketdatafetcher.h"

int main()
{
	std::string stockName = "BTC-USD";
	std::string startDate = "02/03/2018";
	std::string endDate = "02/03/2019";

	MarketDataFetcher yahooFinanceApi;
	yahooFinanceApi.FetchData(stockName, startDate, endDate);

	system("PAUSE");
	return 0;
}