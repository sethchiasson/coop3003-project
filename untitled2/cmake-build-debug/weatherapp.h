#ifndef WEATHERAPP_H
#define WEATHERAPP_H

#include <string>
#include <map>
#include <vector>
#include <utility>

class WeatherApp
{
public:
    WeatherApp();
    ~WeatherApp();
    void run();

private:
    // Member functions
    void initializeCurl();
    void cleanupCurl();
    void displayCityOptions();
    void getUserSelection();
    void setupDateTime();
    void constructUrl();
    void performRequest();
    void parseResponse();
    void displayData();
    static size_t writeCallBack(void* contents, size_t size, size_t nmemb, void* userp);

    // Member variables
    std::map<std::string, std::pair<double, double>> city_coordinates;
    std::map<int, std::string> city_options;
    std::string selected_city;
    double latitude;
    double longitude;
    std::string start_date;
    std::string end_date;
    std::string url;
    std::string readBuffer;
    std::vector<std::pair<std::string, double>> tempDataArray;

    // cURL handle
    void* curl_handle; // We'll cast this to CURL* in the implementation
};

#endif // WEATHERAPP_H
