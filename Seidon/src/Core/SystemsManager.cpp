#include "SystemsManager.h"

namespace Seidon
{
    std::map<std::string, System*> SystemsManager::systems;

    void SystemsManager::Init()
    {

    }

    void SystemsManager::Update(float deltaTime)
    {
        for (auto& [typeName, system] : systems)
            system->Update(deltaTime);
    }
}