#include "stdafx.h"
#include "marketdatafetcher.h"

#pragma warning(disable : 4996)

// Helper function for CURLOPT_WRITEFUNCTION
size_t write(void *ptr, size_t size, size_t nmemb, FILE *file) {
	size_t memRead = fwrite(ptr, size, nmemb, file);
	return memRead;
}

// Converts YYYY-MM-DD date format to 'Time since Epoch' format
std::string DateToEpochConverter(const std::string &date) {
	// Store string date in tm structure to create time_t format  
	struct tm time = { 0 };
	time.tm_year = std::stoi(date.substr(0, 4)) - 1900;
	time.tm_mon = std::stoi(date.substr(5, 2));
	time.tm_mday = std::stoi(date.substr(8, 2));
	time.tm_hour = 23;
	time.tm_min = 59;
	time.tm_sec = 59;
	time_t epochTime = mktime(&time) - 2720000;	// mktime() has an error of +2720000

	std::stringstream ss;
	ss << epochTime;
	return ss.str();
}

// Constructor
MarketDataFetcher::MarketDataFetcher(): COOKIE_FILE_NAME_("cookie.txt"),
										CRUMB_FILE_NAME_("crumb.txt"), 
										DATA_FILE_NAME_("stockdata.csv") {}

// Helper function for FetchData()
void MarketDataFetcher::StoreCookieAndCrumb(const std::string &stockName) const {
	CURL *cookieCurl = curl_easy_init();	// cookie handle
	FILE *crumbFile;

	// Create URL for the Yahoo Finance history for that stock
	std::string cookie_url = std::string("http://finance.yahoo.com/quote/") + stockName + std::string("/history?p=") + stockName;

	// Part 1: Retrieve cookie from URL and store in a file
	// Set URL to go to finance.yahoo.com and prepare cookies file
	if (cookieCurl) {
		curl_easy_setopt(cookieCurl, CURLOPT_URL, cookie_url.c_str());
		curl_easy_setopt(cookieCurl, CURLOPT_COOKIEFILE, COOKIE_FILE_NAME_);
		curl_easy_setopt(cookieCurl, CURLOPT_COOKIELIST, "ALL");
	}

	// Set file as cookie jar
	curl_easy_setopt(cookieCurl, CURLOPT_COOKIESESSION, 1L);			// start a new cookie session
	curl_easy_setopt(cookieCurl, CURLOPT_COOKIEJAR, COOKIE_FILE_NAME_);	// store cookies in the file
	curl_easy_setopt(cookieCurl, CURLOPT_FOLLOWLOCATION, 1L);			// follow redirection
	curl_easy_setopt(cookieCurl, CURLOPT_HEADER, 0);					// exclude headers in write callback function option

	// Part 2: Extract 'Crumb' from webpage
	crumbFile = fopen(CRUMB_FILE_NAME_, "wb");

	if (crumbFile) {

		// Write data to file
		curl_easy_setopt(cookieCurl, CURLOPT_WRITEDATA, crumbFile);

		// Perform blocking file transfer
		curl_easy_perform(cookieCurl);

		// Close header file
		fclose(crumbFile);
	}

	// End session -> store all retrieved cookies in file
	curl_easy_cleanup(cookieCurl);
}

// Helper function for FetchData()
std::string MarketDataFetcher::CrumbExtraction() const {

	// Search the crumbFile for the crumb value
	std::ifstream crumbSearch(CRUMB_FILE_NAME_);
	std::string str;

	while (std::getline(crumbSearch, str)) {
		if (str.find("CrumbStore\":{\"crumb\":\"") != str.npos) {

			size_t pos = str.find("CrumbStore\":{\"crumb\":\"") + 22;
			size_t pos_end = pos;
			while (str.substr(pos_end, 1) != "\"") {
				++pos_end;
			}
			return str.substr(pos, pos_end - pos);
		}
	}
}

void MarketDataFetcher::FetchData(const std::string &stockName, const std::string &startDate, const std::string &endDate) const{

	CURL *curl = curl_easy_init();	// handle for download URL
	FILE *stockData;
	CURLcode res;

	StoreCookieAndCrumb(stockName);
	std::string crumb = CrumbExtraction();
	std::string downUrl = std::string("https://query1.finance.yahoo.com/v7/finance/download/") + stockName
							+ std::string("?period1=") + DateToEpochConverter(startDate) + std::string("&period2=")
							+ DateToEpochConverter(endDate) + std::string("&interval=") + std::string("1d") 
							+ std::string("&events=history&crumb=") + crumb;

	// Update the cookies
	res = curl_easy_setopt(curl, CURLOPT_COOKIEFILE, COOKIE_FILE_NAME_);

	// Set URL 
	res = curl_easy_setopt(curl, CURLOPT_URL, downUrl.c_str());
	res = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);		// allows redirection

	// Send data to this function
	res = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write);

	// Open the file
	stockData = fopen(DATA_FILE_NAME_, "wb");
	if (stockData) {

		// Write the page body to the csv file
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, stockData);

		// Peform blocking file transfer
		res = curl_easy_perform(curl);

		// Close header file 
		fclose(stockData);
	}

	// CLose the handle and cleanup
	curl_easy_cleanup(curl);
	curl_global_cleanup();
}

const char* MarketDataFetcher::GetDataFileName() const {
	return DATA_FILE_NAME_;
}

MarketDataFetcher::~MarketDataFetcher() {}