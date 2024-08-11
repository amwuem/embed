#ifndef WMFACTORY_H
#define WMFACTORY_H

#include "wmproc.h"


enum WM_ALGO_TYPE {
    BLIND,
    NDSL,
    NDSL_V2,
    DFT_NDSL,

    MOD
};


WMProcPtr CreateWmInstance(const std::string& secret, WM_ALGO_TYPE type = NDSL_V2);

WMProcPtr CreateWmInstance(const std::vector<uint8_t>& wmbits, WM_ALGO_TYPE type = NDSL_V2);


#endif // WMFACTORY_H
