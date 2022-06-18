#include "ProjectSelectionWindow.h"
#include "Editor.h"
#include "Utils/Dialogs.h"

#include <filesystem>

namespace Seidon
{
	ProjectSelectionWindow::ProjectSelectionWindow(Editor& editor) : editor(editor) {}

	void ProjectSelectionWindow::Init()
	{
		if (std::filesystem::exists("Preferences.sdreg"))
			preferences.Load("Preferences.sdreg");

		ImGuiIO& io = ImGui::GetIO();
		textFont = io.Fonts->AddFontFromFileTTF("Resources/Roboto-Regular.ttf", 24);
		titleFont = io.Fonts->AddFontFromFileTTF("Resources/Roboto-Regular.ttf", 40);
	}

	Project* ProjectSelectionWindow::Draw()
	{
		Project* res = nullptr;

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);

		ImGui::Begin("Project Selection", nullptr, flags);

		ImGui::BeginChild("Project History", ImVec2(ImGui::GetContentRegionAvail().x * 0.60f, ImGui::GetContentRegionAvail().y), false, flags);

		ImGui::PushFont(titleFont);

		float size = ImGui::CalcTextSize("Recent Projects").x;

		float offset = (ImGui::GetContentRegionAvail().x - size) * 0.5;
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);

		ImGui::Text("Recent Projects");

		ImGui::PopFont();

		ImGui::PushFont(textFont);
		
		if (preferences.projectHistory.size() == 0)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_WindowBg));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyleColorVec4(ImGuiCol_WindowBg));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGui::GetStyleColorVec4(ImGuiCol_WindowBg));
			ImGui::Button("No Recent Projects", ImVec2(ImGui::GetContentRegionAvail().x, 128));
			ImGui::PopStyleColor(3);
		}

		int i = 0;
		bool toRemove = false;
		static int toRemoveIndex = -1;
		for (ProjectRecord& r : preferences.projectHistory)
		{
			if (ImGui::Button((r.name + "\n" + r.path).c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 128)))
			{
				res = LoadProject(r.path);

				if (!res)
				{
					toRemoveIndex = i;
					ImGui::OpenPopup("Error");
				}
			}

			i++;
		}

		if (ImGui::BeginPopupModal("Error"))
		{
			ImGui::Text("Could not open project file, remove it from the list?");

			if (ImGui::Button("Cancel"))
				ImGui::CloseCurrentPopup();

			ImGui::SameLine();

			if (ImGui::Button("Ok"))
			{
				toRemove = true;
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		if (toRemove)
		{
			if (preferences.projectHistory.size() > 1)
				preferences.projectHistory.erase(preferences.projectHistory.begin() + toRemoveIndex);
			else
				preferences.projectHistory.clear();
		}

		ImGui::PopFont();

		ImGui::EndChild();
		ImGui::SameLine();

		ImGui::BeginChild("Buttons", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), false, flags);

		ImGui::PushFont(titleFont);

		size = ImGui::CalcTextSize("Actions").x;

		offset = (ImGui::GetContentRegionAvail().x - size) * 0.5;
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);

		ImGui::Text("Actions");
		ImGui::PopFont();


		ImGui::PushFont(textFont);

		if (ImGui::Button("New Project", ImVec2(ImGui::GetContentRegionAvail().x, 128)))
			res = CreateNewProject();

		if (ImGui::Button("Open Project", ImVec2(ImGui::GetContentRegionAvail().x, 128)))
			res = OpenProject();

		ImGui::PopFont();

		ImGui::EndChild();

		ImGui::Columns(1);
		ImGui::End();

		return res;
	}

	void ProjectSelectionWindow::Destroy()
	{
		preferences.Save("Preferences.sdreg");
	}

	Project* ProjectSelectionWindow::CreateNewProject()
	{
		std::string filepath = SaveFile("");

		if (filepath == "") return nullptr;

		std::filesystem::create_directory(filepath);
		std::filesystem::create_directory(filepath + "\\Assets");
		std::filesystem::create_directory(filepath + "\\Assets\\Scenes");

		Project* p = new Project();
		p->id = UUID();

		size_t separatorIndex = filepath.find_last_of('\\');
		p->name = filepath.substr(separatorIndex + 1, filepath.size() - separatorIndex);
		
		p->path = filepath + "\\" + p->name + ".sdproject";
		p->rootDirectory = filepath;
		p->assetsDirectory = filepath + "\\Assets";

		Scene* scene = new Scene("Default Scene");
		scene->AddSystem<RenderSystem>();

		scene->Save(filepath + "\\Assets\\Scenes\\Default Scene.sdscene");

		Application::Get()->GetResourceManager()->RegisterAsset(scene, filepath + "\\Assets\\Scenes\\Default Scene.sdscene");

		p->currentScene = scene;

		p->Save(p->path);

		ProjectRecord record;
		record.name = p->name;
		record.path = p->path;

		preferences.projectHistory.push_back(record);

		return p;
	}

	Project* ProjectSelectionWindow::OpenProject()
	{
		std::string filepath = LoadFile("Seidon Project (*.sdproject)\0*.sdproject\0");

		if (filepath == "") return nullptr;

		return LoadProject(filepath);
	}

	Project* ProjectSelectionWindow::LoadProject(const std::string& path)
	{
		if (!std::filesystem::exists(path)) return nullptr;

		Project* p = new Project();
		p->Load(path);

		ProjectRecord record;
		record.name = p->name;
		record.path = p->path;

		if(std::find(preferences.projectHistory.begin(), preferences.projectHistory.end(), record) == preferences.projectHistory.end())
			preferences.projectHistory.push_back(record);

		return p;
	}
}
