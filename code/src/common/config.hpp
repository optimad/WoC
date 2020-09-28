
#ifndef __WOC_CONFIG_HPP__
#define __WOC_CONFIG_HPP__

#include <string>
#include <unordered_map>

class Config {

public:
    Config(const std::string &filename);

    std::string getString(const std::string &key, const std::string &defaultValue = "");

private:
    static const std::string m_separator;

    std::unordered_map<std::string, std::string> m_values;

    std::string strip(const std::string &string);

};

#endif
