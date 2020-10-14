#pragma once

#include <boost/fusion/adapted.hpp>
#include <string>
#include <vector>

struct AlgoJSON {
    std::vector<std::string> columns;
    std::vector<std::vector<double>> data;
};

BOOST_FUSION_ADAPT_STRUCT(
        AlgoJSON,
        columns,
        data)
