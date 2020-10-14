#pragma once

#include <vector>

struct AlgoData {
    struct AlgoDataPoint {
        unsigned long long time;
        double profitability;
    };

    std::vector<AlgoDataPoint> dataPoints;
    unsigned long long minTime = -1;
    unsigned long long maxTime = 0;
    double maxProfitability = 0.0;
};
