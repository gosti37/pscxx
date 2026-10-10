#ifndef PSCXX_HPP
#define PSCXX_HPP

#include <string>
#include <vector>

namespace PSCXX

{

enum LABELS
{
    SUCCESS,
    NOTFOUND,
    PERDENIED,
    INVALARG,
    PIPERR,
    OOMERR,
    TIMEOUT,
    UNKNOWN
};

struct PRESULT
{
    LABELS errorLabel;
    int exitCode;
    std::string outStd;
    std::string errStd;
};

PRESULT execute(std::vector<std::string>);

}

#endif //PSCXX_HPP