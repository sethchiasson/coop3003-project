Program Structure
The application is organized into three main files:

WeatherApp.h: Header file containing the declaration of the WeatherApp class, its member functions, and variables.
WeatherApp.cpp: Implementation file defining the member functions of the WeatherApp class.
main.cpp: The entry point of the application, where an instance of WeatherApp is created and executed.

WeatherApp Class

Responsibilities:
User Interaction: Manages user inputs for selecting a city and entering API credentials.
API Communication: Constructs and sends HTTP requests to the Meteomatics API using cURL.
Data Processing: Parses JSON responses and extracts relevant weather data.
Output Display: Formats and displays the weather forecast to the user.

Key Member Functions:

run(): The main function orchestrating the application's workflow.
getCredentials(): Prompts the user for Meteomatics API username and password.
initializeCurl(): Sets up the cURL environment and handles.
displayCityOptions(): Shows a list of available cities to the user.
getUserSelection(): Captures and validates the user's city choice.
setupDateTime(): Prepares the date and time parameters for the API request.
constructUrl(): Builds the API request URL with the appropriate parameters.
performRequest(): Executes the HTTP request using cURL.
parseResponse(): Parses the JSON response to extract temperature data.
displayData(): Formats and presents the weather data to the user.
cleanupCurl(): Cleans up cURL resources after the request is completed.
writeCallBack(): Static function used by cURL to write received data into a buffer.

Member Variables:

city_coordinates: A map of city names to their latitude and longitude coordinates.
city_options: A map assigning numerical options to city names for selection.
selected_city: The city chosen by the user.
latitude, longitude: Coordinates of the selected city.
start_date, end_date: Strings representing the date range for the weather forecast.
url: The constructed API request URL.
readBuffer: Buffer storing the raw JSON response from the API.
tempDataArray: A vector of pairs holding date strings and temperature values.
username, password: The user's API credentials.
curl_handle: Handle for the cURL session.

Included Libraries:

iostream (#include <iostream>): Facilitates input/output operations, such as reading user input and writing to the console.
curl/curl.h (#include <curl/curl.h>): Provides functions and definitions necessary for performing HTTP requests using cURL.
nlohmann/json.hpp (#include <nlohmann/json.hpp>): A header-only library used for parsing and manipulating JSON data.
ctime (#include <ctime>): Offers functions for handling date and time operations, crucial for setting request parameters.
cstdio (#include <cstdio>): Contains functions for input/output operations, used here for string formatting.
string (#include <string>): Supports the use of std::string, essential for handling textual data.
map (#include <map>): Enables the use of std::map, which stores key-value pairs, used for city data and options.
vector (#include <vector>): Allows the use of std::vector, a dynamic array used for storing temperature data.
utility (#include <utility>): Provides utility components like std::pair, used to store pairs of data elements.


How the Program Works

An instance of WeatherApp is created in main.cpp.
The constructor initializes member variables and loads city coordinates.
User Authentication:

getCredentials() prompts the user to enter their Meteomatics API username and password.
Credentials are securely stored in memory for the session.
cURL Setup:

initializeCurl() initializes the cURL environment and configures the session with the user's credentials.
SSL verification is disabled for simplicity (note: should be enabled in production).
City Selection:

displayCityOptions() lists available cities.
getUserSelection() captures the user's choice and retrieves corresponding coordinates.
Date and Time Configuration:

setupDateTime() obtains the current time and calculates a 7-day range.
Dates are formatted into ISO8601 strings required by the API.
API Request Construction:

constructUrl() builds the request URL using the selected city and date range.
Parameters include the temperature variable (t_2m:F) and hourly intervals (PT1H).
Performing the Request:

performRequest() sends the HTTP GET request to the API.
writeCallBack() collects the response data into readBuffer.
Response Parsing:

parseResponse() parses the JSON data using nlohmann::json.
Temperature data and timestamps are extracted and stored in tempDataArray.
Data Display:

displayData() formats the dates and temperatures for readability.
Data is presented to the user, organized by date and time.

Cleanup and Exit:

cleanupCurl() releases cURL resources.
The program waits for user input before closing.

The WeatherApp provides a user-friendly interface to access weather forecasts. By structuring the code with a class, it promotes code reuse and easier maintenance. The use of standard libraries and careful handling of resources ensures efficient and secure operation. The program demonstrates good practices in user interaction, data retrieval, and output formatting, serving as a solid foundation for further development or enhancement.
