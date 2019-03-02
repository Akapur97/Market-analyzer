#pragma once
#ifndef  MARKET_DATA_FETCHER_H
#define MARKET_DATA_FETCHER_H

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <curl/curl.h>
#include "curl_easy.h"
#include "curl_form.h"
#include "curl_ios.h"
#include "curl_exception.h"

class MarketDataFetcher {
	private:
		const char* CRUMB_FILE_NAME_;
		const char* COOKIE_FILE_NAME_;
		const char* DATA_FILE_NAME_;

		std::string DateToEpochConverter(const std::string &date);
		void StoreCookieAndCrumb(const std::string &stockName);
		std::string CrumbExtraction();

	public:
		MarketDataFetcher();
		void FetchData(const std::string &stockName, const std::string &startDate, const std::string &endDate);
		~MarketDataFetcher();
};

#endif // ! MARKET_DATA_FETCHER_H
