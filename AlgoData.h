#pragma once

#include <vector>

struct AlgoData {
    struct AlgoDataPoint {
        unsigned long long time;
        double profitability;
    };

    std::vector<AlgoDataPoint> dataPoints;
};
