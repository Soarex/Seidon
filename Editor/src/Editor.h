#pragma once
#include <Seidon.h>

#include "EditorWindow.h"
#include "ProjectSelectionWindow.h"

#include "EditorAction.h"
#include "Dockspace.h"

#include <ImGuizmo/ImGuizmo.h>

namespace Seidon
{
    using EditorActionCallback = std::function<void(EditorAction&)>;
    class Editor : public Application
    {
    public:
        Project* openProject = nullptr;

        EditorActionList actions;
        std::vector<EditorActionCallback> actionCallbacks;

        ProjectSelectionWindow* projectSelectionWindow;
        EditorWindow* editorWindow;
        SelectedItem selectedItem;
        ResourceManager editorResourceManager;
        Scene* scene;
        Scene* runtimeScene;
        Scene editorSystems;
        Scene runtimeSystems;

        Dockspace dockspace;

        Extension e;

        RenderFunction drawColliders;
        bool colliderRenderingEnabled = false;

        int guizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
        BoundingBox viewBounds;
        bool local = false;

        bool isPlaying = false;
    public:
        void Init() override;
        void Update() override;
        void Destroy() override;

        void OnEditorAction(EditorAction* action);
        void AddEditorActionCallback(const EditorActionCallback& callback);
    private:
        void DrawCubeColliders(Renderer& renderer);
        void DrawMeshColliders(Renderer& renderer);
        void DrawCharacterControllers(Renderer& renderer);
        void DrawGuizmos(Renderer& renderer);
        void DrawTransformGuizmos();

        void ReloadDll();
    };
}