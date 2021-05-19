#include <algorithm>
#include "instructioninfo.hpp"

InstructionInfo::InstructionInfo()
{
    std::fill_n(paramsType.begin(), paramsType.size(), zarpa::paramType::no_param);
}


void InstructionInfo::AddParamType(zarpa::paramType type, const size_t index) 
{
    paramsType[index] = paramsType[index] | type;
    vecParamsType[index].push_back(type);
}

/*
std::vector<zarpa::paramType> InstructionInfo::GetVectorParamTypes(unsigned index) 
{
        std::vector<zarpa::paramType> result {};
        //for (zarpa::paramType i = zarpa::paramType::no_param; i < zarpa::paramType::_lastone; i ) {

        for (size_t i = 1; i < static_cast<uint32_t>(zarpa::paramType::_lastone); i++ ) {
            if ((paramsType[index] & static_cast<zarpa::paramType>(i)) == static_cast<zarpa::paramType>(i)) {
                        result.push_back(static_cast<zarpa::paramType>(i));
            }
        }
        return result;
    }    
}
*/