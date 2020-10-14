#pragma once

#include <boost/fusion/adapted.hpp>
#include <string>
#include <vector>

struct BalanceJSON {
    std::string totalBalance;
};

BOOST_FUSION_ADAPT_STRUCT(
    BalanceJSON,
    totalBalance)
