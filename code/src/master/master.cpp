
#include "master_version.hpp"
#include "config.hpp"

#include <fstream>
#include <iostream>

static int ERROR_OFFSET = 0;

static std::string DATA_FILENAME = "slave_data.dat";

int main(int argc, char *argv[])
{
    //
    // Initialization
    //

    std::cout << "master application - v" << MASTER_VERSION << std::endl;

    // Read configuration file
    Config config("woc.in");

    //
    // Retreiving data
    //
    std::cout << "Retreiving data through slave application..." << std::endl;

    // Get slave executable path
    std::string slaveExecPath = config.getString("slave_exec_path", "slave");

    // Get slave executable arguments
    std::string slaveExecArgs = config.getString("slave_exec_args", "");

    // Call slave process
    std::string slaveCommand = slaveExecPath;
    if (!slaveExecArgs.empty()) {
        slaveCommand += " " + slaveExecArgs;
    }

    int slaveExitStatus = std::system(slaveCommand.c_str());
    if (slaveExitStatus != EXIT_SUCCESS) {
        std::cout << "An error occured during the execution of the slave application";
        exit(slaveExitStatus);
    }

    std::cout << "  Slave application complete succesfully";

    // Read data
    std::cout << "Read data..." << std::endl;

    std::ifstream dataFile;
    dataFile.open(DATA_FILENAME);
    if (!dataFile.is_open()) {
        std::cout << "  Unable to read the data!";
        return ERROR_OFFSET + 30;
    }

    std::string address;
    getline(dataFile, address);
    if (dataFile.bad() || address.length() == 0) {
        std::cout << "  Unable to read the data!";
        return ERROR_OFFSET + 10;
    }
    std::cout << "  Address: " << address << std::endl;

    std::string agent;
    getline(dataFile, agent);
    if (dataFile.bad() || agent.length() == 0) {
        std::cout << "  Unable to read the data!";
        return ERROR_OFFSET + 10;
    }
    std::cout << "  Agent: " << agent << std::endl;

    dataFile.close();

    return EXIT_SUCCESS;
}
