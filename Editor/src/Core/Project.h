#pragma once
#include <Seidon.h>

namespace Seidon
{
	struct Project : public Asset
	{
		std::string rootDirectory;
		std::string assetsDirectory;

		std::vector<Extension*> loadedExtensions;

		std::unordered_map<std::string, System*> editorSystems;
		Scene* loadedScene;

		void Save(const std::string& path);
		void Load(const std::string& path);

		void Save(std::ofstream& out) {}
		void Load(std::ifstream& in) {}

        template <typename T>
        void AddEditorSystem()
        {
            T* system = new T();

            system->scene = loadedScene;
            system->SysInit();

            editorSystems[typeid(T).name()] = system;
        }

        template <typename T>
        T* GetEditorSystem()
        {
            return (T*)editorSystems[typeid(T).name()];
        }

        template <typename T>
        bool HasEditorSystem()
        {
            return editorSystems.count(typeid(T).name()) > 0;
        }

        template <typename T>
        void RemoveEditorSystem()
        {
            T* system = editorSystems[typeid(T).name()];

            system->SysDestroy();

            delete system;

            editorSystems.erase(typeid(T).name());
        }

        void AddEditorSystem(SystemMetaType& metatype);
        void RemoveEditorSystem(SystemMetaType& metatype);
	};
}