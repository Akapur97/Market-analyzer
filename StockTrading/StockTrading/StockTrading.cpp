// LibCurl.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <curl/curl.h>
#include "curl_easy.h"
#include "curl_form.h"
#include "curl_ios.h"
#include "curl_exception.h"

#pragma warning(disable : 4996)

size_t write(void *ptr, size_t size, size_t nmemb, FILE *file) {
	size_t memRead = fwrite(ptr, size, nmemb, file);
	return memRead;
}
int main()
{
	// Yahoo Finance Api data retrieval
	std::string stock;
	CURL *cookieCurl;
	char *fileName = "cookie_test.txt";
	char *crumbFileName = "crumb_test.txt";
	FILE *crumbFile;
	std::string crumb;

	// Initialize handle
	cookieCurl = curl_easy_init();

	// Get the stock name
	std::cout << "Please enter the name of the stock for information.\n";
	std::cin >> stock;

	// Create URL for the Yahoo Finance history for that stock
	std::string cookie_url = std::string("http://finance.yahoo.com/quote/") + stock + std::string("/history?p=") + stock;

	// Part 1: Retrieve cookie from URL and store in a file
	// Set URL to go to finance.yahoo.com and prepare cookies file
	if (cookieCurl) {
		curl_easy_setopt(cookieCurl, CURLOPT_URL, cookie_url.c_str());
		curl_easy_setopt(cookieCurl, CURLOPT_COOKIEFILE, fileName);
		curl_easy_setopt(cookieCurl, CURLOPT_COOKIELIST, "ALL");
	}
	
	// Set file as cookie jar
	curl_easy_setopt(cookieCurl, CURLOPT_COOKIESESSION, 1L);		// start a new cookie session
	curl_easy_setopt(cookieCurl, CURLOPT_COOKIEJAR, fileName);	// store cookies in the file
	curl_easy_setopt(cookieCurl, CURLOPT_FOLLOWLOCATION, 1L);	// follow redirection
	curl_easy_setopt(cookieCurl, CURLOPT_HEADER, 0);			// exclude headers in write callback function option

	// Part 2: Extract 'Crumb' from webpage
	crumbFile = fopen(crumbFileName, "wb");
	
	if (crumbFile) {

		// Write data to file
		curl_easy_setopt(cookieCurl, CURLOPT_WRITEDATA, crumbFile);

		// Perform blocking file transfer
		curl_easy_perform(cookieCurl);

		// Close header file
		fclose(crumbFile);
	}

	// End session -> store all retreived cookies in file
	curl_easy_cleanup(cookieCurl);

	// Search the crumbFile for the crumb value
	std::ifstream crumbSearch(crumbFileName);
	std::string str;

	while (std::getline(crumbSearch, str)) {
		if (str.find("CrumbStore\":{\"crumb\":\"") != str.npos) {

			size_t pos = str.find("CrumbStore\":{\"crumb\":\"") + 22;
			size_t pos_end = pos;
			while (str.substr(pos_end, 1) != "\"") {
				++pos_end;
			}
			crumb = str.substr(pos, pos_end - pos);
			break;
		}
	}

	// Use the crumb to create download URL
	CURL *curl = curl_easy_init();	// new handle for download URL
	FILE *stockData;
	CURLcode res;
	char* dataFileName = "stockData.csv";
	// Start date (Epoch)
	struct tm t = { 0 };
	t.tm_year = 117;	// This is year-1900, so 112 = 2012
	t.tm_mon = 8;
	t.tm_mday = 14;
	t.tm_hour = 21;
	t.tm_min = 54;
	t.tm_sec = 13;
	time_t timeSinceEpoch = mktime(&t);
	std::stringstream ss;
	ss << timeSinceEpoch;
	std::string startDate = ss.str();

	// End date (Epoch)
	t.tm_year = 119;	// This is year-1900, so 112 = 2012
	t.tm_mon = 2;
	t.tm_mday = 15;
	t.tm_hour = 16;
	t.tm_min = 12;
	t.tm_sec = 13;
	timeSinceEpoch = mktime(&t);
	std::stringstream sk;
	sk << timeSinceEpoch;
	std::string endDate = sk.str();
	std::string down_url = std::string("https://query1.finance.yahoo.com/v7/finance/download/") + std::string(stock) 
					  + std::string("?period1=") + startDate + std::string("&period2=") + endDate 
					  + std::string("&interval=") + std::string("1d") + std::string("&events=history&crumb=") 
					  + crumb.c_str();

	// Update the cookies
	res = curl_easy_setopt(curl, CURLOPT_COOKIEFILE, fileName);

	// Set URL 
	res = curl_easy_setopt(curl, CURLOPT_URL, down_url.c_str());
	res = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);		// allows redirection

	// Send data to this function
	res = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write);

	// Open the file
	stockData = fopen(dataFileName, "wb");
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

	system("PAUSE");
	return 0;
}