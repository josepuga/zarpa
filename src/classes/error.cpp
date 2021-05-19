#include <iomanip>
#include <iostream>
#include "error.hpp"

void Error::Show(const std::string& msg, zarpa::errorType error, const std::string& fileName, unsigned int lineNumber)
{
    counts[error]++;
    totalCount++;
    if (lineNumber) {
        std::cerr << "[" << std::setw(4) << lineNumber << "] ";
    }
    if (fileName != "") {
        std::cerr << fileName << ": ";
    }
    switch (error) {
        case zarpa::errorType::warning : std::cerr << "WARNING: "; break;
        case zarpa::errorType::error : std::cerr << "ERROR: "; break;
        case zarpa::errorType::fatal_error : std::cerr << "FATAL ERROR: "; break;
        case zarpa::errorType::dev_error : std::cerr << "DEV. ERROR: "; break;
        default: break;
    }
    std::cerr << msg << std::endl;   
}

void Error::Summary()
{
    if (counts[zarpa::errorType::warning]) {
        std::cerr << std::setw(3) << counts[zarpa::errorType::warning] << " Warnings." << std::endl;
    }
    if (counts[zarpa::errorType::error]) {
        std::cerr << std::setw(3) << counts[zarpa::errorType::error] << " Errors." << std::endl;
    }
    if (counts[zarpa::errorType::fatal_error]) {
        std::cerr << std::setw(3) << counts[zarpa::errorType::fatal_error] << " Fatal errors." << std::endl;
    }
}

void Error::ResetCounts()
{
    for (auto& c : counts) {
        c = 0;
    }
    totalCount = 0;
}