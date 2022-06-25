#pragma once
#include <Seidon.h>
#include "Project.h"
#include "Panels/Panel.h"
#include "Dockspace.h"
#include "SelectedItem.h"

namespace Seidon
{
	class Editor;
	class EditorWindow
	{
	public:
		EditorWindow(Editor& editor);

		void Init();
		void Draw();
		void Destroy();
	private:
		Editor& editor;

		Dockspace dockspace;
		std::vector<Panel*> panels;
	private:
		void ProcessInput();
		void DrawMenuBar();
		void NewScene();
		void LoadScene(const std::string& path);
		void OpenScene();
		void SaveSceneAs();
		void SaveSceneAndProject();

		void ExtensionRegistryPopup(bool& open);
	};
}