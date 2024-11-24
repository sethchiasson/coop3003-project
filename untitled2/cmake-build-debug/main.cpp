#include <iostream>
#include <curl/curl.h>
#include <string>
#include <map>
#include <nlohmann/json.hpp>
#include <chrono>      // For date and time manipulation
#include <iomanip>     // For std::put_time
#include <sstream>     // For std::stringstream
#include <ctime>       // For time functions

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
            {"Naples, FL", {26.1420, -81.7948}},
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

    // Get current date and date 6 days ahead
    auto now = std::chrono::system_clock::now();
    auto in_six_days = now + std::chrono::hours(24 * 6);

    // Format dates in ISO 8601 format
    std::stringstream start_date_ss, end_date_ss;
    auto start_time_t = std::chrono::system_clock::to_time_t(now);
    auto end_time_t = std::chrono::system_clock::to_time_t(in_six_days);

    start_date_ss << std::put_time(std::gmtime(&start_time_t), "%Y-%m-%dT%H:%M:%SZ");
    end_date_ss << std::put_time(std::gmtime(&end_time_t), "%Y-%m-%dT%H:%M:%SZ");

    std::string start_date = start_date_ss.str();
    std::string end_date = end_date_ss.str();

    // Construct the API endpoint with the selected city's coordinates, date range, and parameters
    std::string api_url = "https://api.meteomatics.com/" + start_date + "--" + end_date + ":P1D/"
                        + "t_2m:C,prob_precip_24h:p/"
                        + std::to_string(latitude) + "," + std::to_string(longitude) + "/json";

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
            try {
                // Parse the JSON response using nlohmann/json
                nlohmann::json jsonResponse = nlohmann::json::parse(response);

                // Extract temperature and precipitation chance for each date
                std::cout << "\nWeather Forecast for " << selected_city << " (EST):\n";
                const auto& data = jsonResponse["data"];

                // Create a map to hold parameter names and their units
                std::map<std::string, std::string> parameter_units;
                for (const auto& parameter : data) {
                    parameter_units[parameter["parameter"]] = parameter["units"];
                }

                // Assuming the coordinates array has only one element
                const auto& dates = data[0]["coordinates"][0]["dates"];

                for (size_t i = 0; i < dates.size(); ++i) {
                    // Get date string in UTC
                    std::string date_str = dates[i]["date"];

                    // Parse the date string into std::tm structure (UTC)
                    std::istringstream ss(date_str);
                    std::tm tm_utc = {};
                    ss >> std::get_time(&tm_utc, "%Y-%m-%dT%H:%M:%SZ");
                    if (ss.fail()) {
                        std::cerr << "Error parsing date string: " << date_str << std::endl;
                        continue;
                    }

                    // Convert std::tm to time_t (UTC)
                    std::time_t time_utc = timegm(&tm_utc);  // timegm converts tm in UTC to time_t

                    // Adjust time to EST by subtracting 5 hours (in seconds)
                    const int est_offset = -5 * 3600;  // EST is UTC-5
                    std::time_t time_est = time_utc + est_offset;

                    // Convert adjusted time_t back to std::tm structure (local time)
                    std::tm* tm_est = std::localtime(&time_est);

                    // Format the date in EST
                    char date_buffer[50];
                    std::strftime(date_buffer, sizeof(date_buffer), "%A, %B %d, %Y", tm_est);

                    // Get temperature
                    double temperature = data[0]["coordinates"][0]["dates"][i]["value"];

                    // Get precipitation chance
                    double precip_chance = data[1]["coordinates"][0]["dates"][i]["value"];

                    // Print the data
                    std::cout << "\nDate: " << date_buffer << std::endl;
                    std::cout << "Temperature: " << temperature << " " << parameter_units["t_2m:C"] << std::endl;
                    std::cout << "Chance of Rain: " << precip_chance << " " << parameter_units["prob_precip_24h:p"] << std::endl;
                }

            } catch (const nlohmann::json::exception& e) {
                std::cerr << "Error parsing JSON: " << e.what() << std::endl;
            }
        }

        // Clean up
        curl_easy_cleanup(curl);
    } else {
        std::cerr << "Failed to initialize cURL" << std::endl;
    }

    return 0;
}
