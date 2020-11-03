#include "SceneManager.h"

#include "AlgoJSON.h"
#include "Scene.h"
#include "SceneSettings.h"
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <restc-cpp/SerializeJson.h>
#include <restc-cpp/RequestBuilder.h>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Window/Event.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <chrono>
#include <cstdlib>
#include <utility>
#include <mutex>

#include <iostream>

using namespace std::chrono;

namespace {
    constexpr std::array<char, 16> hexDigits = {'0', '1', '2', '3', '4', '5', '6', '7',
                                                '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

    constexpr float requestTimeWait = 300.f;

    std::string apiDomain = "https://api2.nicehash.com";

    std::string rigsPath = "/main/api/v2/mining/rigs2/";
    std::string balancePath = "/main/api/v2/accounting/account2/BTC/";
    std::string algoPath = "/main/api/v2/mining/rigs/stats/algo/";

    std::string exchangeARSPath = "https://api.satoshitango.com/v3/ticker/ARS/BTC";
    std::string exchangeUSDPath = "https://api.satoshitango.com/v3/ticker/USD/BTC";

    std::string hmacSHA256(const std::string &key, const std::string &data) {
        unsigned char *result = nullptr;
        unsigned int resultlen = -1;

        result = HMAC(EVP_sha256(), key.data(), key.size(), reinterpret_cast<const unsigned char *>(data.data()),
                      data.size(), result, &resultlen);

        if (result == nullptr) {
            return "";
        }

        std::string hex;
        for (unsigned int i = 0; i < resultlen; ++i) {
            hex += hexDigits[(result[i] & 0xF0) >> 4];
            hex += hexDigits[result[i] & 0x0F];
        }

        return hex;
    }
}


SceneManager::Visitor::Visitor(SceneVector& scenes):
scenes(scenes) {

}

void SceneManager::Visitor::operator()(const PopBack& /*p*/) {
    scenes.pop_back();
}

void SceneManager::Visitor::operator()(const PopBackTo& p) {
    auto it = std::find_if(scenes.begin(), scenes.end(), [&](const auto& scene){return scene.get() == p.scene;});

    if (it != scenes.end()) {
        ++it;
        scenes.erase(it, scenes.end());
    }
}

void SceneManager::Visitor::operator()(const ClearAll& /*p*/) {
    scenes.clear();
}

void SceneManager::Visitor::operator()(PushBack& p) {
    scenes.push_back(std::move(p.scene));
}


SceneManager::SceneManager():
open              (true),
fullscreen        (false),
windowSize        ({800, 600}) {
    settings.loadFromFile();
    createWindow();
    font.loadFromFile("../data/fonts/Overpass-SemiBold.ttf");
    fontMono.loadFromFile("../data/fonts/Anonymous_Pro_0.ttf");
}

void SceneManager::close() {
    open = false;
}

void SceneManager::changeScene(std::unique_ptr<Scene> scene, bool pushBack) {
    if (!pushBack)
        nextActions.push_back(ClearAll{});

    nextActions.push_back(PushBack{std::move(scene)});
}

void SceneManager::popBack() {
    nextActions.push_back(PopBack{});
}

void SceneManager::popBackTo(const Scene* scene) {
    nextActions.push_back(PopBackTo{scene});
}

const sf::View& SceneManager::getView() const {
    return view;
}

void SceneManager::run(std::unique_ptr<Scene> startScene) {
    scenes.emplace_back(std::move(startScene));

    if (settings.organizationID.empty() || settings.apiKey.empty() || settings.apiSecretKey.empty()) {
        scenes.emplace_back(std::make_unique<SceneSettings>(this));
    }

    activeFrom = scenes.begin();

    sf::Clock clock;
    sf::Time timeElapsed;
    requestTime = sf::seconds(requestTimeWait);
    sf::Event event;

    while(open) {
        if (rigsDirty && rigPromise.valid() && rigPromise.wait_for(seconds(0)) == std::future_status::ready &&
                exchangeARSPromise.valid() && exchangeARSPromise.wait_for(seconds(0)) == std::future_status::ready &&
                exchangeUSDPromise.valid() && exchangeUSDPromise.wait_for(seconds(0)) == std::future_status::ready &&
                balancePromise.valid() && balancePromise.wait_for(seconds(0)) == std::future_status::ready &&
                algoPromise.valid() && algoPromise.wait_for(seconds(0)) == std::future_status::ready) {
            rigsDirtyThisFrame = rigsDirty;
        }

        if (!nextActions.empty()) {
            (*activeFrom)->lostFocus();
            for (auto& nextAction : nextActions) {
                Visitor visitor{scenes};
                std::visit(visitor, nextAction);
            }
            nextActions.clear();
            if (scenes.empty()) {
                close();
                break;
            }
            updateActiveFrom();
            window.setKeyRepeatEnabled(scenes.back()->isKeyRepeat());
        }

        timeElapsed = clock.restart();

        while (window.pollEvent(event)) {
            processEvents(event);
            if (event.type == sf::Event::Resized) {
                for (auto& scene : scenes) {
                    scene->viewUpdated();
                }
            } else {
                scenes.back()->processEvents(event);
            }
            if (!nextActions.empty())
                break;
        }

        for (auto it = activeFrom; it != scenes.end(); ++it) {
            (*it)->update(timeElapsed);
        }

        window.clear();

        for (auto it = activeFrom; it != scenes.end(); ++it) {
            (*it)->draw(window, sf::RenderStates());
        }

        window.display();

        if (rigsDirtyThisFrame) {
            rigsDirty = false;
            rigsDirtyThisFrame = false;
        }

        if (!settings.organizationID.empty() && !settings.apiKey.empty() && !settings.apiSecretKey.empty()) {
            requestTime += timeElapsed;
            if (requestTime.asSeconds() > requestTimeWait) {
                requestTime = sf::Time::Zero;
                restClient = restc_cpp::RestClient::Create();
                rigPromise = restClient->ProcessWithPromise([this](restc_cpp::Context &ctx) { getRigData(ctx); });
                exchangeARSPromise = restClient->ProcessWithPromise(
                        [this](restc_cpp::Context &ctx) { getExchangeARSData(ctx); });
                exchangeUSDPromise = restClient->ProcessWithPromise(
                        [this](restc_cpp::Context &ctx) { getExchangeUSDData(ctx); });
                balancePromise = restClient->ProcessWithPromise(
                        [this](restc_cpp::Context &ctx) { getBalanceData(ctx); });
                algoPromise = restClient->ProcessWithPromise([this](restc_cpp::Context &ctx) { getAlgoData(ctx); });
                restClient->CloseWhenReady(false);
            }
        }
    }
}

const Settings& SceneManager::getSettings() const {
    return settings;
}

void SceneManager::setSettings(const Settings& newSettings) {
    std::scoped_lock<std::shared_mutex> lk(restClientMutex);

    settings = newSettings;
    settings.saveToFile();
    requestTime = sf::seconds(requestTimeWait);
}

const sf::Font& SceneManager::getFont() const {
    return font;
}

const sf::Font& SceneManager::getFontMono() const {
    return fontMono;
}

bool SceneManager::changesInRigData() const {
    return rigsDirtyThisFrame;
}

const RigsJSON& SceneManager::getRigData() const {
    return rigsJSON;
}

const std::vector<DeviceJSON>& SceneManager::getDevicesData() const {
    return devicesJSON;
}

double SceneManager::getBTCBalance() const {
    return std::stod(balanceJSON.totalBalance);
}

double SceneManager::BTCtoUSD(double btc) const {
    if (USDBTC == 0.0) {
        return btc * 11000.0;
    }

    return btc * USDBTC;
}

double SceneManager::BTCtoARS(double btc) const {
    if (ARSBTC == 0.0) {
        return btc * 2000000.0;
    }

    return btc * ARSBTC;
}

const AlgoData& SceneManager::getAlgoData() const {
    return algoData;
}

void SceneManager::updateSize() {
    sf::Vector2u size;
    if (fullscreen) {
        size = window.getSize();
    } else {
        size = windowSize;
    }

    view = window.getView();
    view.setSize(size.x, size.y);
    view.setCenter(size.x / 2, size.y / 2);
    window.setView(view);
}

void SceneManager::processEvents(const sf::Event& event) {
    switch (event.type) {
        case sf::Event::Closed:
            close();
            break;
        case sf::Event::Resized:
            windowSize = {std::max(event.size.width, 480u), std::max(event.size.height, 360u)};
            if ((event.size.width < 480u) || (event.size.height < 360u)) {
                window.setSize(windowSize);
            }
            updateSize();
            break;
        case sf::Event::KeyPressed:
            if (event.key.code == sf::Keyboard::F4 && event.key.alt) {
                close();
            } else if ((event.key.code == sf::Keyboard::F11) || (event.key.code == sf::Keyboard::Return && event.key.alt)) {
                fullscreen = !fullscreen;
                createWindow();
                for (auto& scene : scenes) {
                    scene->viewUpdated();
                }
            } else if (event.key.code == sf::Keyboard::F1) {
                changeScene(std::make_unique<SceneSettings>(this), true);
            }
            break;
        default:
            break;

    }
}

void SceneManager::updateActiveFrom() {
    auto rIterator = std::find_if_not(scenes.rbegin(), scenes.rend(), [](const auto& scene){return scene->isPopup();});

    if (rIterator != scenes.rend())
        ++rIterator;

    activeFrom = rIterator.base();
    (*activeFrom)->gainFocus();
}

void SceneManager::createWindow() {
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    sf::String title = "MinerDisplay - 0.1.0";

    if (fullscreen) {
        window.create(sf::VideoMode::getDesktopMode(), title, sf::Style::None, settings);
    } else {
        window.create({windowSize.x, windowSize.y, 32}, title, sf::Style::Default, settings);
    }

    updateSize();
    window.setKeyRepeatEnabled(true);
    window.setFramerateLimit(60);
}

void SceneManager::getRigData(restc_cpp::Context& ctx) {
    std::shared_lock<std::shared_mutex> lk(restClientMutex);

    long long currentTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    std::string xTime = std::to_string(currentTime);
    std::string XNonce =  boost::uuids::to_string(boost::uuids::random_generator()());
    std::string XRequestId = boost::uuids::to_string(boost::uuids::random_generator()());
    std::string auth = settings.apiKey + ":" + hmacSHA256(settings.apiSecretKey, settings.apiKey + '\0' + xTime + '\0' + XNonce + '\0' + '\0' + settings.organizationID + '\0' + '\0' + "GET" + '\0' + rigsPath + '\0');

    rigsJSON = RigsJSON();
    std::unique_ptr<restc_cpp::Reply> reply;

    try {
        reply = restc_cpp::RequestBuilder(ctx)
                .Get(apiDomain + rigsPath)
                .Header("X-Time", xTime)
                .Header("X-Nonce", XNonce)
                .Header("X-Organization-Id", settings.organizationID)
                .Header("X-Request-Id", XRequestId)
                .Header("X-Auth", auth)
                .Execute();

        restc_cpp::SerializeFromJson(rigsJSON, *reply);

        //std::string body = reply->GetBodyAsString();
        //std::cout << body;
        //const restc_cpp::Reply::HttpResponse& httpResponse = reply->GetHttpResponse();
    } catch (const std::exception& e) {
        std::string what = e.what();
        rigsDirty = true;
        return;
    }

    devicesJSON.clear();
    for (const auto& rig : rigsJSON.miningRigs) {
        for (unsigned int i = 0; i < rig.devices.size(); ++i) {
            devicesJSON.push_back(rig.devices[i]);
        }
    }

    rigsDirty = true;
}

void SceneManager::getExchangeARSData(restc_cpp::Context& ctx) {
#ifndef NDEBUG
    return;
#endif
    ExchangeJSON exchangeJSON;
    std::unique_ptr<restc_cpp::Reply> reply;

    try {
        reply = restc_cpp::RequestBuilder(ctx)
                .Get(exchangeARSPath)
                .Execute();

        restc_cpp::SerializeFromJson(exchangeJSON, *reply);

        const restc_cpp::Reply::HttpResponse& httpResponse = reply->GetHttpResponse();

        ARSBTC = exchangeJSON.data.ticker.BTC.ask;
    } catch (const std::exception& e) {
        std::string what = e.what();
        std::cout << what << std::endl;
        ARSBTC = 0;
        return;
    }
}

void SceneManager::getExchangeUSDData(restc_cpp::Context& ctx) {
#ifndef NDEBUG
    return;
#endif
    ExchangeJSON exchangeJSON;
    std::unique_ptr<restc_cpp::Reply> reply;

    try {
        reply = restc_cpp::RequestBuilder(ctx)
                .Get(exchangeUSDPath)
                .Execute();

        restc_cpp::SerializeFromJson(exchangeJSON, *reply);

        //const restc_cpp::Reply::HttpResponse& httpResponse = reply->GetHttpResponse();

        USDBTC = exchangeJSON.data.ticker.BTC.ask;
    } catch (const std::exception& e) {
        std::string what = e.what();
        USDBTC = 0;
        return;
    }
}

void SceneManager::getBalanceData(restc_cpp::Context& ctx) {
    std::shared_lock<std::shared_mutex> lk(restClientMutex);

    long long currentTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    std::string xTime = std::to_string(currentTime);
    std::string XNonce =  boost::uuids::to_string(boost::uuids::random_generator()());
    std::string XRequestId = boost::uuids::to_string(boost::uuids::random_generator()());
    std::string auth = settings.apiKey + ":" + hmacSHA256(settings.apiSecretKey, settings.apiKey + '\0' + xTime + '\0' + XNonce + '\0' + '\0' + settings.organizationID + '\0' + '\0' + "GET" + '\0' + balancePath + '\0');

    balanceJSON = BalanceJSON();
    std::unique_ptr<restc_cpp::Reply> reply;

    try {
        reply = restc_cpp::RequestBuilder(ctx)
                .Get(apiDomain + balancePath)
                .Header("X-Time", xTime)
                .Header("X-Nonce", XNonce)
                .Header("X-Organization-Id", settings.organizationID)
                .Header("X-Request-Id", XRequestId)
                .Header("X-Auth", auth)
                .Execute();

        restc_cpp::SerializeFromJson(balanceJSON, *reply);

        //const restc_cpp::Reply::HttpResponse& httpResponse = reply->GetHttpResponse();
    } catch (const std::exception& e) {
        std::string what = e.what();
        return;
    }
}

void SceneManager::getAlgoData(restc_cpp::Context& ctx) {
    std::shared_lock<std::shared_mutex> lk(restClientMutex);

    long long currentTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    std::string xTime = std::to_string(currentTime);
    std::string XNonce =  boost::uuids::to_string(boost::uuids::random_generator()());
    std::string XRequestId = boost::uuids::to_string(boost::uuids::random_generator()());
    std::string auth = settings.apiKey + ":" + hmacSHA256(settings.apiSecretKey, settings.apiKey + '\0' + xTime + '\0' + XNonce + '\0' + '\0' + settings.organizationID + '\0' + '\0' + "GET" + '\0' + algoPath + '\0' + "algorithm=20");

    AlgoJSON algoJSON;
    std::unique_ptr<restc_cpp::Reply> reply;

    try {
        reply = restc_cpp::RequestBuilder(ctx)
                .Get(apiDomain + algoPath)
                .Argument("algorithm", "20")
                .Header("X-Time", xTime)
                .Header("X-Nonce", XNonce)
                .Header("X-Organization-Id", settings.organizationID)
                .Header("X-Request-Id", XRequestId)
                .Header("X-Auth", auth)
                .Execute();

        restc_cpp::SerializeFromJson(algoJSON, *reply);

        /*std::string body = reply->GetBodyAsString();
        std::cout << body;
        const restc_cpp::Reply::HttpResponse& httpResponse = reply->GetHttpResponse();*/

        int k = 1;
    } catch (const std::exception& e) {
        std::string what = e.what();
        return;
    }

    algoData = AlgoData();

    unsigned int timeIndex = -1;
    unsigned int profitabilityIndex = -1;
    for (unsigned int i = 0; i < algoJSON.columns.size(); ++i) {
        if (algoJSON.columns[i] == "time") {
            timeIndex = i;
        } else if (algoJSON.columns[i] == "profitability") {
            profitabilityIndex = i;
        }
    }

    if (timeIndex == -1 || profitabilityIndex == -1) {
        return;
    }

    for (const auto& data : algoJSON.data) {
        AlgoData::AlgoDataPoint dataPoint {static_cast<unsigned long long>(data[timeIndex]), data[profitabilityIndex]};
        algoData.dataPoints.push_back(dataPoint);
    }

    int k = 1;
}
