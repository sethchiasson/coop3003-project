#include <iostream>
#include <curl/curl.h>
#include <string>

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

    // Construct the API endpoint with the desired parameters
    std::string api_url = "https://api.meteomatics.com/now/t_2m:C/47.3769,8.5417/json";

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
            std::cout << "Weather Data: " << response << std::endl;
        }

        // Clean up
        curl_easy_cleanup(curl);
    } else {
        std::cerr << "Failed to initialize cURL" << std::endl;
    }

    return 0;
}

















