#include "WeatherApp.h"
#include <iostream>
#include <curl/curl.h>
#include <json.hpp>
#include <ctime>
#include <cstdio>

WeatherApp::WeatherApp()
    : curl_handle(nullptr), latitude(0.0), longitude(0.0)
{
    // Initialize city coordinates
    city_coordinates = {
        {"Miami, FL", {25.7617, -80.1918}},
        {"Orlando, FL", {28.5383, -81.3792}},
        {"Tampa, FL", {27.9506, -82.4572}},
        {"Jacksonville, FL", {30.3322, -81.6557}},
        {"Tallahassee, FL", {30.4383, -84.2807}},
        {"Fort Lauderdale, FL", {26.1224, -80.1373}},
        {"St. Petersburg, FL", {27.7676, -82.6403}},
        {"Cape Coral, FL", {26.5629, -81.9495}},
        {"Fort Myers, FL", {26.6409, -81.8606}},
        {"West Palm Beach, FL", {26.7153, -80.0534}},
        {"Naples, FL", {26.1420, -81.7948}},
        {"Montgomery, AL", {32.3668, -86.3000}},
        {"Juneau, AK", {58.3019, -134.4197}},
        {"Phoenix, AZ", {33.4484, -112.0740}},
        {"Little Rock, AR", {34.7465, -92.2896}},
        {"Sacramento, CA", {38.5816, -121.4944}},
        // Add other cities as needed
    };
}

WeatherApp::~WeatherApp()
{
    cleanupCurl();
}

void WeatherApp::run()
{
    initializeCurl();
    displayCityOptions();
    getUserSelection();
    setupDateTime();
    constructUrl();
    performRequest();
    parseResponse();
    displayData();

    // Pause before exiting
    std::cout << "Press Enter to exit...";
    std::cin.ignore();
    std::cin.get();
}

void WeatherApp::initializeCurl()
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl_handle = curl_easy_init();

    if (curl_handle)
    {
        // Set authentication
        curl_easy_setopt(curl_handle, CURLOPT_USERNAME, "na_smith_adam");
        curl_easy_setopt(curl_handle, CURLOPT_PASSWORD, "5XA6Jg0Xye");

        // Disable SSL verification (not recommended for production)
        curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYHOST, 0L);
    }
    else
    {
        std::cerr << "Failed to initialize cURL handle." << std::endl;
        exit(1);
    }
}

void WeatherApp::cleanupCurl()
{
    if (curl_handle)
    {
        curl_easy_cleanup((CURL*)curl_handle);
        curl_handle = nullptr;
    }
    curl_global_cleanup();
}

void WeatherApp::displayCityOptions()
{
    std::cout << "Select a city:\n";
    int index = 1;
    for (const auto& city : city_coordinates)
    {
        std::cout << index << ". " << city.first << std::endl;
        city_options[index++] = city.first;
    }
}

void WeatherApp::getUserSelection()
{
    int selection;
    std::cout << "Enter the number of your selection: ";
    std::cin >> selection;

    // Check if the selection is valid
    if (city_options.find(selection) == city_options.end())
    {
        std::cerr << "Invalid selection. Exiting program." << std::endl;
        exit(1);
    }

    // Retrieve coordinates for the selected city
    selected_city = city_options[selection];
    std::pair<double, double> coordinates = city_coordinates[selected_city];
    latitude = coordinates.first;
    longitude = coordinates.second;
}

void WeatherApp::setupDateTime()
{
    // Get the current time
    time_t now = time(0);

    // Create start time struct
    struct tm start_tm = *gmtime(&now);

    // Create end time struct by adding 7 days
    time_t future_time = now + (7 * 24 * 60 * 60); // Add 7 days in seconds
    struct tm end_tm = *gmtime(&future_time);

    // Format start_date and end_date as ISO8601 strings
    char start_date_cstr[30];
    char end_date_cstr[30];
    strftime(start_date_cstr, sizeof(start_date_cstr), "%Y-%m-%dT%H:%M:%SZ", &start_tm);
    strftime(end_date_cstr, sizeof(end_date_cstr), "%Y-%m-%dT%H:%M:%SZ", &end_tm);

    start_date = std::string(start_date_cstr);
    end_date = std::string(end_date_cstr);
}

void WeatherApp::constructUrl()
{
    // Construct the URL with the date range and hourly interval (PT1H)
    url = "https://api.meteomatics.com/" + start_date + "--" + end_date + ":PT1H/t_2m:F/"
          + std::to_string(latitude) + "," + std::to_string(longitude) + "/json";

    // Set the constructed URL
    CURLcode result = curl_easy_setopt((CURL*)curl_handle, CURLOPT_URL, url.c_str());
    if (result != CURLE_OK)
    {
        std::cerr << "Failed to set URL: " << curl_easy_strerror(result) << std::endl;
        exit(1);
    }
}

void WeatherApp::performRequest()
{
    // Set up the write callback
    curl_easy_setopt((CURL*)curl_handle, CURLOPT_WRITEFUNCTION, writeCallBack);
    curl_easy_setopt((CURL*)curl_handle, CURLOPT_WRITEDATA, &readBuffer);

    // Perform the request
    CURLcode result = curl_easy_perform((CURL*)curl_handle);
    if (result != CURLE_OK)
    {
        std::cerr << "Request failed: " << curl_easy_strerror(result) << std::endl;
        exit(1);
    }
}

void WeatherApp::parseResponse()
{
    // Parse the response into key:value pairs all in string.
    nlohmann::json jsonResponse;
    try
    {
        jsonResponse = nlohmann::json::parse(readBuffer);
    }
    catch (const nlohmann::json::exception& e)
    {
        std::cerr << "Error parsing JSON: " << e.what() << std::endl;
        std::cerr << "Response was: " << readBuffer << std::endl;
        exit(1);
    }

    // Check for API errors
    if (jsonResponse.contains("error"))
    {
        std::cerr << "API Error: " << jsonResponse["error"] << std::endl;
        exit(1);
    }

    // Extract the dates and temperatures
    try
    {
        auto dates = jsonResponse["data"][0]["coordinates"][0]["dates"];

        for (const auto& date_entry : dates)
        {
            std::string date_str = date_entry["date"];
            double temperature = date_entry["value"];

            // Store each temperature reading in the vector
            tempDataArray.push_back({date_str, temperature});
        }
    }
    catch (const nlohmann::json::exception& e)
    {
        std::cerr << "Error extracting data from JSON: " << e.what() << std::endl;
        exit(1);
    }
}

void WeatherApp::displayData()
{
    std::string previous_date = "";

    for (const auto& data : tempDataArray)
    {
        // Parse the date string to get date and time components
        int year, month, day, hour, minute, second;
        if (sscanf(data.first.c_str(), "%4d-%2d-%2dT%2d:%2d:%2dZ", &year, &month, &day, &hour, &minute, &second) != 6)
        {
            std::cerr << "Error parsing date: " << data.first << std::endl;
            continue;
        }

        // Adjust year to two digits
        int year_short = year % 100;

        // Format date as MM/DD/YY
        char formatted_date[9];
        snprintf(formatted_date, sizeof(formatted_date), "%02d/%02d/%02d", month, day, year_short);

        // Format time as HH:MM:SS
        char formatted_time[9];
        snprintf(formatted_time, sizeof(formatted_time), "%02d:%02d:%02d", hour, minute, second);

        // If the date has changed, add a new line
        if (previous_date != std::string(formatted_date))
        {
            if (!previous_date.empty())
            {
                std::cout << std::endl; // Add a new line between dates
            }
            previous_date = formatted_date;
        }

        // Print the data
        std::cout << "Date: " << formatted_date << std::endl;
        std::cout << "Temp: " << data.second << " F" << std::endl;
        std::cout << "Hour: " << formatted_time << std::endl;
    }
}

// Static member function
size_t WeatherApp::writeCallBack(void* contents, size_t size, size_t nmemb, void* userp)
{
    size_t totalSize = size * nmemb;
    std::string* s = static_cast<std::string*>(userp);
    s->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}
