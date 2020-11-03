cmake_minimum_required(VERSION 3.17)
project(MinerDisplay)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")

add_definitions(-DBOOST_UUID_RANDOM_PROVIDER_FORCE_WINCRYPT=1)
add_definitions(-DRAPIDJSON_HAS_STDSTRING=1)

set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_DIR "C:/SFML-2.5.1/lib/cmake/SFML")
find_package(SFML COMPONENTS graphics window system REQUIRED)

set(restc-cpp_DIR "C:/restc-cpp/lib/cmake/restc-cpp")
include_directories("C:/LibreSSL/include")

find_package(ZLIB REQUIRED)
find_package(restc-cpp REQUIRED)

link_directories(C:/LibreSSL/lib)

set(Boost_USE_STATIC_LIBS OFF)
find_package(Boost REQUIRED COMPONENTS
        system
        coroutine
        program_options
        filesystem
        date_time
        context
        chrono
        log
        thread
        log_setup
        regex
        atomic)
include_directories(${Boost_INCLUDE_DIR})


add_executable(${PROJECT_NAME} Button.cpp ChartDisplay.cpp DeviceDisplay.cpp main.cpp MoneyDisplay.cpp
        RigDisplay.cpp SceneDisplays.cpp SceneManager.cpp SceneMenu.cpp SceneSettings.cpp Settings.cpp
        TextBox.cpp)

target_link_options(${PROJECT_NAME} PRIVATE -mwindows)

target_link_libraries(${PROJECT_NAME}
        sfml-graphics
        sfml-window
        sfml-system
        ${RESTC_CPP_LIBRARIES}
        ${ZLIB_LIBRARIES}
        #${OPENSSL_LIBRARIES}
        ssl-48
        crypto-46
        ${Boost_LIBRARIES}
        #${RESTC_CPP_LIBRARIES}
        #${ZLIB_LIBRARIES}
        #${OPENSSL_LIBRARIES}
        #ssl-48
        #crypto-46
        #${Boost_LIBRARIES}
        #pthread
        ws2_32
        )