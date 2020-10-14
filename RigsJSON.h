#pragma once

#include <boost/fusion/adapted.hpp>
#include <string>
#include <vector>

struct AlgorithmJSON {
    std::string description;
    std::string enumName;
};

BOOST_FUSION_ADAPT_STRUCT(
    AlgorithmJSON,
    description,
    enumName)

struct StatsJSON {
    AlgorithmJSON algorithm;
};

BOOST_FUSION_ADAPT_STRUCT(
    StatsJSON,
    algorithm)

struct SpeedJSON {
    std::string displaySuffix;
    std::string speed;
    std::string title;
    std::string algorithm;
};

BOOST_FUSION_ADAPT_STRUCT(
    SpeedJSON,
    displaySuffix,
    speed,
    title,
    algorithm)

struct StatusJSON {
    std::string description;
    std::string enumName;
};

BOOST_FUSION_ADAPT_STRUCT(
    StatusJSON,
    description,
    enumName)

struct DeviceTypeJSON {
    std::string description;
    std::string enumName;
};

BOOST_FUSION_ADAPT_STRUCT(
    DeviceTypeJSON,
    description,
    enumName)

struct DeviceJSON {
    std::vector<SpeedJSON> speeds;
    int revolutionsPerMinute = 0;
    int load = 0;
    int temperature = 0;
    StatusJSON status;
    DeviceTypeJSON deviceType;
    std::string name;
    std::string id;
};

BOOST_FUSION_ADAPT_STRUCT(
    DeviceJSON,
    speeds,
    revolutionsPerMinute,
    load,
    temperature,
    status,
    deviceType,
    name,
    id)

struct RigJSON {
    std::vector<StatsJSON> stats;
    std::vector<DeviceJSON> devices;
    std::string name;
    std::string rigId;
};

BOOST_FUSION_ADAPT_STRUCT(
    RigJSON,
    stats,
    devices,
    name,
    rigId)

struct RigsJSON {
    std::string unpaidAmount;
    int totalDevices = -1;
    double totalProfitability = -1;
    std::vector<RigJSON> miningRigs;
};

BOOST_FUSION_ADAPT_STRUCT(
    RigsJSON,
    unpaidAmount,
    totalDevices,
    totalProfitability,
    miningRigs)
