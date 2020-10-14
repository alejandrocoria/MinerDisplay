#pragma once

#include "AlgoData.h"
#include "BalanceJSON.h"
#include "ExchangeJSON.h"
#include "RigsJSON.h"
#include "Settings.h"
#include <restc-cpp/restc-cpp.h>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <functional>
#include <future>
#include <memory>
#include <shared_mutex>
#include <variant>
#include <vector>

class Scene;

class SceneManager {
public:
    SceneManager();

    void close();
    void changeScene(std::unique_ptr<Scene> scene, bool pushBack = false);
    void popBack();
    void popBackTo(const Scene* scene);
    const sf::View& getView() const;
    void run(std::unique_ptr<Scene> startScene);
    const Settings& getSettings() const;
    void setSettings(const Settings& newSettings);
    const sf::Font& getFont() const;
    const sf::Font& getFontMono() const;
    bool changesInRigData() const;
    const RigsJSON& getRigData() const;
    const std::vector<DeviceJSON>& getDevicesData() const;
    double getBTCBalance() const;
    double BTCtoUSD(double btc) const;
    double BTCtoARS(double btc) const;

private:
    struct PopBack {};
    struct PopBackTo {const Scene* scene;};
    struct ClearAll {};
    struct PushBack {std::unique_ptr<Scene> scene;};

    struct Visitor {
        Visitor(std::vector<std::unique_ptr<Scene>>& scenes);
        void operator()(const PopBack& p);
        void operator()(const PopBackTo& p);
        void operator()(const ClearAll& p);
        void operator()(PushBack& p);
        std::vector<std::unique_ptr<Scene>>& scenes;
    };

    using NextAction = std::variant<PopBack, PopBackTo, ClearAll, PushBack>;
    using SceneVector = std::vector<std::unique_ptr<Scene>>;

    void updateSize();
    void processEvents(const sf::Event& event);
    void updateActiveFrom();
    void createWindow();
    void getRigData(restc_cpp::Context& ctx);
    void getExchangeARSData(restc_cpp::Context& ctx);
    void getExchangeUSDData(restc_cpp::Context& ctx);
    void getBalanceData(restc_cpp::Context& ctx);
    void getAlgoData(restc_cpp::Context& ctx);

    SceneVector scenes;
    SceneVector::iterator activeFrom;
    std::vector<NextAction> nextActions;
    bool open;
    bool fullscreen;
    Settings settings;
    sf::Font font;
    sf::Font fontMono;
    sf::Time requestTime;
    sf::Vector2u windowSize;
    sf::RenderWindow window;
    sf::View view;
    std::unique_ptr<restc_cpp::RestClient> restClient;
    std::future<void> rigPromise;
    std::future<void> exchangeARSPromise;
    std::future<void> exchangeUSDPromise;
    std::future<void> balancePromise;
    std::future<void> algoPromise;
    std::shared_mutex restClientMutex;
    bool rigsDirty = false;
    bool rigsDirtyThisFrame = false;
    double ARSBTC;
    double USDBTC;
    BalanceJSON balanceJSON;
    RigsJSON rigsJSON;
    std::vector<DeviceJSON> devicesJSON;
    AlgoData algoData;
};
