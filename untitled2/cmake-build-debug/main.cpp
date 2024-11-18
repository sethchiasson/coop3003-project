#include <iostream>
#include <curl/curl.h>
#include <string>
#include <map>
#include <nlohmann/json.hpp>

// Callback function to write the API response data into a string
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* response) {
    response->append((char*)contents, size * nmemb);
    return size * nmemb;
}

int main() {
    CURL* curl;
    CURLcode res;
    std::string response;

    // Your Meteomatics credentials
    std::string username = "fgcu_chiasson_seth";
    std::string password = "K05dOjA4Lr";

    // Dictionary with popular Florida cities and U.S. state capitals and their coordinates
    std::map<std::string, std::pair<double, double>> city_coordinates = {
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
            {"Montgomery, AL", {32.3668, -86.3000}},
            {"Juneau, AK", {58.3019, -134.4197}},
            {"Phoenix, AZ", {33.4484, -112.0740}},
            {"Little Rock, AR", {34.7465, -92.2896}},
            {"Sacramento, CA", {38.5816, -121.4944}},
            // Add other state capitals similarly
    };

    // Display options to the user
    std::cout << "Select a city:\n";
    int index = 1;
    std::map<int, std::string> city_options;
    for (const auto& city : city_coordinates) {
        std::cout << index << ". " << city.first << std::endl;
        city_options[index++] = city.first;
    }

    // Get the user selection
    int selection;
    std::cout << "Enter the number of your selection: ";
    std::cin >> selection;

    // Retrieve coordinates for the selected city
    std::string selected_city = city_options[selection];
    std::pair<double, double> coordinates = city_coordinates[selected_city];
    double latitude = coordinates.first;
    double longitude = coordinates.second;

    // Construct the API endpoint with the selected city's coordinates
    std::string api_url = "https://api.meteomatics.com/now/t_2m:C/" + std::to_string(latitude) + "," + std::to_string(longitude) + "/json";

    // Initialize cURL
    curl = curl_easy_init();
    if (curl) {
        // Set the URL for the API
        curl_easy_setopt(curl, CURLOPT_URL, api_url.c_str());

        // Provide the username and password for HTTP Basic Authentication
        std::string userpwd = username + ":" + password;
        curl_easy_setopt(curl, CURLOPT_USERPWD, userpwd.c_str());

        // Disable SSL peer verification (Insecure)
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        // Define the callback function to capture the response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // Make the request
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        } else {
            // Print the API response
            std::cout << "Weather Data for " << selected_city << ": " << response << std::endl;
        }

        // Clean up
        curl_easy_cleanup(curl);
    } else {
        std::cerr << "Failed to initialize cURL" << std::endl;
    }

    return 0;
}

















