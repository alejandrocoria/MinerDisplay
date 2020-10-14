#include "Settings.h"

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/prettywriter.h>
#include <fstream>

void Settings::loadFromFile() {
    std::ifstream ifs("./settings.json");
    if (!ifs.is_open()) {
        saveToFile();
        return;
    }

    rapidjson::IStreamWrapper isw(ifs);

    rapidjson::Document document;
    document.ParseStream(isw);
    if (document.HasParseError()) {
        return;
    }

    rapidjson::Value& org = document["organizationID"];
    rapidjson::Value& key = document["apiKey"];
    rapidjson::Value& secret = document["apiSecretKey"];

    if (org.IsString() && key.IsString() && secret.IsString()) {
        organizationID = org.GetString();
        apiKey = key.GetString();
        apiSecretKey = secret.GetString();
    }
}

void Settings::saveToFile() {
    std::ofstream ofs("./settings.json");
    rapidjson::OStreamWrapper osw(ofs);

    rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer(osw);

    writer.StartObject();
    writer.Key("organizationID");
    writer.String(organizationID);
    writer.Key("apiKey");
    writer.String(apiKey);
    writer.Key("apiSecretKey");
    writer.String(apiSecretKey);
    writer.EndObject();

    writer.Flush();
}
