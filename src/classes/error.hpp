#ifndef DC73511E_A6A2_452F_B3FD_2E1C3A136578
#define DC73511E_A6A2_452F_B3FD_2E1C3A136578
#include "zarpa_types.hpp"
#include <array>
#include <string>

struct Error
{
    std::array<uint_t, zarpa::errorType::errorType_count> counts;
    uint_t totalCount;

    void Show(const std::string& msg, zarpa::errorType error, 
            const std::string& fileName = "", unsigned int lineNumber = 0);
    void Summary();
    void ResetCounts();
    
};


#endif /* DC73511E_A6A2_452F_B3FD_2E1C3A136578 */
