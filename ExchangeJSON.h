#pragma once

#include <boost/fusion/adapted.hpp>
#include <string>

struct TickerBTCJSON {
    double ask;
};

BOOST_FUSION_ADAPT_STRUCT(
        TickerBTCJSON,
        ask)

struct TickerJSON {
    TickerBTCJSON BTC;
};

BOOST_FUSION_ADAPT_STRUCT(
        TickerJSON,
        BTC)

struct ExchangeDataJSON {
    TickerJSON ticker;
};

BOOST_FUSION_ADAPT_STRUCT(
        ExchangeDataJSON,
        ticker)

struct ExchangeJSON {
    ExchangeDataJSON data;
};

BOOST_FUSION_ADAPT_STRUCT(
        ExchangeJSON,
        data)
