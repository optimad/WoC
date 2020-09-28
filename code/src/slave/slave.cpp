
#include "slave_version.hpp"

#include <curl/curl.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#if __linux__
#include <sys/utsname.h>
#endif

static int ERROR_OFFSET = 100;

static std::string HTTPBIN_URL_ADDRESS = "http://httpbin.org/ip";
static std::string HTTPBIN_URL_AGENT   = "http://httpbin.org/user-agent";

static std::string DATA_FILENAME = "slave_data.dat";

/*!
 * Callback function to write retreived chunks to user defined string.
 *
 * \param rawChunk points to the delivered data chunk
 * \param chunkDummySize according to libcurl documentation this argument is
 * always 1
 * \param chunkMemorySize is the size, expressed in bytes, ofthe delivered
 * data chunk
 * \param rawString points to the used defined string
 * \return The total number of bytes read.
 */
static std::size_t writeStringCallback(void *rawChunk, std::size_t chunkDummySize, std::size_t chunkMemorySize, void *rawString)
{
    std::size_t chunkTotalSize = chunkDummySize * chunkMemorySize;
    std::size_t nChunkCharacters = chunkTotalSize / sizeof(char);
    std::string chunkString(static_cast<char *>(rawChunk), nChunkCharacters);

    std::string &string = *(static_cast<std::string *>(rawString));
    string += chunkString;

    return chunkTotalSize;
}

/*!
 * Retreive string data from the specified URL.
 *
 * \param url is the URL from which data will be retrieved
 * \param[out] data on output will contain the reteived data
 * \return On success retruns 0, on failure returns the error code.
 */
int retreiveStringData(const std::string &url, std::string *data)
{
    // Initialize curl session
    CURL *curlHandle = curl_easy_init();

    // Specify URL to get
    curl_easy_setopt(curlHandle, CURLOPT_URL, url.c_str());

    // Send all data to this function
    curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, writeStringCallback);

    // Pass our 'chunk' struct to the callback function
    curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, (void *) data);

    // Provide a user-agent
    std::string dummyAgent = "slave-agent/" + std::string(SLAVE_VERSION);
    curl_easy_setopt(curlHandle, CURLOPT_USERAGENT, dummyAgent.c_str());

    // Retreive the data
    CURLcode requestStatus = curl_easy_perform(curlHandle);
    if (requestStatus != CURLE_OK) {
        return ERROR_OFFSET + 1;
    }

    // Cleanup
    curl_easy_cleanup(curlHandle);

    return 0;
}

/*!
 * Parse httpbin data.
 *
 * \param data is the data retrieved from httpbin
 * \param[out] value on output will contain the parsed value
 * \return On success retruns 0, on failure returns the error code.
 */
int getHttpbinValue(const std::string &data, std::string *value)
{
    // Extract value
    std::size_t valueStart = data.find(":");
    if (valueStart == std::string::npos) {
        return ERROR_OFFSET + 1;
    }

    std::size_t valueEnd = data.find("\n", valueStart + 1);
    if (valueEnd == std::string::npos) {
        return ERROR_OFFSET + 1;
    }

    *value = data.substr(valueStart + 1, valueEnd - (valueStart + 1));
    std::replace(value->begin(), value->end(), '"', ' ');

    // Trim string
    value->erase(value->begin(), std::find_if(value->begin(), value->end(), [](int ch) {
        return !std::isspace(ch);
    }));

    value->erase(std::find_if(value->rbegin(), value->rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), value->end());

    return 0;
}

std::string getOperatingSystemName()
{
    std::string name;

#if __linux__
    struct utsname uts;
    uname(&uts);
    name = uts.sysname;
#elif _WIN32
    OSVERSIONINFO osvi;
    ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    ::GetVersionEx(&osvi);

    std::ostringstream nameStream;
    nameStream << "Windows v" << osvi.dwMajorVersion << "." << osvi.dwMinorVersion;

    name = nameStream.str();
#else
    name = "Undefined";
#endif

    return name;
}

int main(int argc, char *argv[])
{
    int errorStatus;

    //
    // Initialization
    //

    std::cout << "slave application - v" << SLAVE_VERSION << std::endl;

    // Print operating system name
    std::string operatingSystemName = getOperatingSystemName();
    std::cout << "Operating system:  " << operatingSystemName << std::endl;

    // Initialize curl library
    curl_global_init(CURL_GLOBAL_ALL);

    //
    // Retreiving data
    //
    std::cout << "Retreiving data..." << std::endl;

    // Address
    std::cout << "  Retreiving address: ";
    std::string addressJSON;
    errorStatus = retreiveStringData(HTTPBIN_URL_ADDRESS, &addressJSON);
    if (errorStatus != 0) {
        std::cout << "FAILED" << std::endl;
        return ERROR_OFFSET + 10;
    }

    std::string address;
    errorStatus = getHttpbinValue(addressJSON, &address);
    if (errorStatus != 0) {
        std::cout << "FAILED" << std::endl;
        return ERROR_OFFSET + 11;
    }

    std::cout << address << std::endl;

    // Agent
    std::cout << "  Retreiving agent: ";

    std::string agentJSON;
    errorStatus = retreiveStringData(HTTPBIN_URL_AGENT, &agentJSON);
    if (errorStatus != 0) {
        std::cout << "FAILED" << std::endl;
        return ERROR_OFFSET + 20;
    }

    std::string agent;
    errorStatus = getHttpbinValue(agentJSON, &agent);
    if (errorStatus != 0) {
        std::cout << "FAILED" << std::endl;
        return ERROR_OFFSET + 21;
    }

    std::cout << agent << std::endl;

    //
    // Write data to file
    //
    std::cout << "Writing data..." << std::endl;

    std::ofstream dataFile;
    dataFile.open(DATA_FILENAME);
    if (!dataFile.is_open()) {
        std::cout << "  Unable to write the data!";
        return ERROR_OFFSET + 30;
    }

    dataFile << address << "\n";
    dataFile << agent << "\n";

    dataFile.close();

    std::cout << "  Data written to \"" << DATA_FILENAME << "\"" << std::endl;

    //
    // Cleanup
    //

    // Cleanup curl library
    curl_global_init(CURL_GLOBAL_ALL);

    return EXIT_SUCCESS;
}
