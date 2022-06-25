#pragma once
#include <Seidon.h>

#include "Core/EditorWindow.h"
#include "Core/ProjectSelectionWindow.h"

#include "Core/EditorAction.h"
#include "Core/Dockspace.h"

namespace Seidon
{
    using EditorActionCallback = std::function<void(EditorAction&)>;
    class Editor : public Application
    {
    public:
        Project* openProject = nullptr;
        Scene* activeScene;

        ProjectSelectionWindow* projectSelectionWindow;
        EditorWindow* editorWindow;
        SelectedItem selectedItem;
        ResourceManager editorResourceManager;

        EditorActionList actions;
        std::vector<EditorActionCallback> actionCallbacks;

        bool isPlaying = false;

        bool switchProject = false;
        Project* projectToSwitch = nullptr;

    public:
        void Init() override;
        void Update() override;
        void Destroy() override;

        void Play();
        void Stop();

        void SwitchProject(Project* project);
        void SwitchActiveScene(Scene* scene);

        void OnEditorAction(EditorAction* action);
        void AddEditorActionCallback(const EditorActionCallback& callback);

    private:
        void UpdateEditorSystems();
    };
}