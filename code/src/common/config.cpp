
#include "config.hpp"

#include <algorithm>
#include <fstream>
#include <sstream>

/**
 * \class Config
 *
 * Config is a class for parsing configuration files
 */

const std::string Config::m_separator = "=";

/**
 * Constructor
 *
 * \param path is the path of the configuration file
 */
Config::Config(const std::string &path)
{
    // Open configuration file
    std::ifstream configFile(path);
    if (!configFile.is_open()) {
        return;
    }

    // Parse configuration file
    std::string line;
    while (std::getline(configFile, line)) {
        line = strip(line);
        if (line.empty()) {
            continue;
        }

        // Discard comments
        std::size_t commentPosition = line.rfind("//");
        if (commentPosition != std::string::npos) {
            continue;
        }

        // Parse line
        std::size_t separatorPosition = line.rfind(m_separator);
        if (separatorPosition == std::string::npos) {
            continue;
        }

        std::string key   = strip(line.substr(0, separatorPosition));
        std::string value = strip(line.substr(separatorPosition + 1));

        m_values[key] = value;
    }
}

/**
 * Get the requested value.
 *
 * \param key is the key
 * \param defaultValue is a default value to be returned if the key is not
 * found in the configuration
 * \return The requested value.
 */
std::string Config::getString(const std::string &key, const std::string &defaultValue)
{
    auto keyItr = m_values.find(key);
    if (keyItr == m_values.end()) {
        return defaultValue;
    }

    return keyItr->second;
}

/*!
* Strip specified string.
*
* \param string is the inpt string
* \return The stripped string.
*/
std::string Config::strip(const std::string &string)
{
    std::string stripped = string;
    stripped.erase(stripped.begin(), std::find_if(stripped.begin(), stripped.end(), not1(std::ptr_fun<int, int>(std::isspace))));
    stripped.erase(std::find_if(stripped.rbegin(), stripped.rend(), not1(std::ptr_fun<int, int>(isspace))).base(), stripped.end());

    return stripped;
}
