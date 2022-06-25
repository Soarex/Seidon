#pragma once
#include "EditorPreferences.h"
#include "Project.h"

#include <Seidon.h>

namespace Seidon
{
	class Editor;
	class ProjectSelectionWindow
	{
	public:
		ProjectSelectionWindow(Editor& editor);

		void Init();
		Project* Draw();
		void Destroy();

	private:
		Editor& editor;
		EditorPreferences preferences;
		ImFont* titleFont;
		ImFont* textFont;

	private:
		Project* CreateNewProject();
		Project* OpenProject();
		Project* LoadProject(const std::string& path);
	};
}