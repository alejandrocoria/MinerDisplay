#include "SceneManager.h"
#include "SceneDisplays.h"
#include <memory>

int main(int argc, char **argv) {
    SceneManager sceneManager;
    sceneManager.run(std::make_unique<SceneDisplays>(&sceneManager));
}
