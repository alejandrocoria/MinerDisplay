#pragma once

#include <string>

struct Settings {
    void loadFromFile();
    void saveToFile();

    std::string organizationID = "";
    std::string apiKey = "";
    std::string apiSecretKey = "";
};
