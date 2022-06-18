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
		Project* project;

		Dockspace dockspace;
		std::vector<Panel*> panels;

	private:
		void DrawMenuBar();
	};
}