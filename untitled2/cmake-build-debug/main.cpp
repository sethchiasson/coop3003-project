#include <iostream>
#include <curl/curl.h>
#include <json.hpp>
#include <map>
#include <string>

size_t writeCallBack(void *contents, size_t size, size_t numMembers, std::string *readBuffer);

int main()
{
    curl_global_init(CURL_GLOBAL_DEFAULT);

    // Validate API key
    const char* username = "na_smith_adam";
    const char* password = "5XA6Jg0Xye";

    // Dictionary with popular Florida cities and U.S. state capitals and their coordinates
    std::map<std::string, std::pair<double, double> > city_coordinates = {
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
        // Add other state capitals similarly
    };

    // Map to store index and city names for selection
    std::map<int, std::string> city_options;
    int index = 1;
    for (const auto &city : city_coordinates)
    {
        city_options[index++] = city.first;
    }

    while (true)
    {
        CURL *handle = curl_easy_init();

        // Set authentication
        curl_easy_setopt(handle, CURLOPT_USERNAME, username);
        curl_easy_setopt(handle, CURLOPT_PASSWORD, password);

        // Display options to the user
        std::cout << "\nSelect a city (or enter 0 to exit):\n";
        for (const auto &option : city_options)
        {
            std::cout << option.first << ". " << option.second << std::endl;
        }

        // Get the user selection
        int selection;
        std::cout << "Enter the number of your selection: ";
        std::cin >> selection;

        if (selection == 0)
        {
            std::cout << "Exiting program." << std::endl;
            curl_easy_cleanup(handle);
            break;
        }

        // Check if the selection is valid
        if (city_options.find(selection) == city_options.end())
        {
            std::cerr << "Invalid selection. Please try again." << std::endl;
            curl_easy_cleanup(handle);
            continue;
        }

        // Retrieve coordinates for the selected city
        std::string selected_city = city_options[selection];
        std::pair<double, double> coordinates = city_coordinates[selected_city];
        double latitude = coordinates.first;
        double longitude = coordinates.second;

        // Construct the URL with the selected coordinates
        std::string url = "https://api.meteomatics.com/now/t_2m:F/"
                          + std::to_string(latitude) + "," + std::to_string(longitude) + "/json";

        // Set the constructed URL
        CURLcode result = curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
        if (result != CURLE_OK)
        {
            std::cout << "Failed to set URL: " << curl_easy_strerror(result) << std::endl;
            curl_easy_cleanup(handle);
            continue;
        }

        // Disable SSL verification due to errors (not safe for production)
        curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0L);

        std::string readBuffer; // Stores JSON response as a string.

        // Configure libcurl to use the writeCallBack function and where to store it.
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, writeCallBack);
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, &readBuffer);

        result = curl_easy_perform(handle); // Executes the request.

        if (result == CURLE_OK)
        {
            try
            {
                // Parse the JSON response using nlohmann::json
                nlohmann::json jsonResponse = nlohmann::json::parse(readBuffer);

                // Extracting temperature
                double temperature = jsonResponse["data"][0]["coordinates"][0]["dates"][0]["value"];

                // Print extracted temperature
                std::cout << "\nTemperature in " << selected_city << ": " << temperature << "F" << std::endl;
            }
            catch (const nlohmann::json::exception &e)
            {
                std::cerr << "Error parsing JSON: " << e.what() << std::endl;
            }
        }
        else
        {
            std::cout << "Request Failed: " << curl_easy_strerror(result) << std::endl;
        }

        curl_easy_cleanup(handle); // Clears handle.
    }

    curl_global_cleanup(); // Close all resources initialized by the library.

    return 0;
}

size_t writeCallBack(void *contents, size_t size, size_t numMembers, std::string *readBuffer)
{
    size_t totalSize = size * numMembers;
    readBuffer->append((char *)contents, totalSize);
    return totalSize;
}
