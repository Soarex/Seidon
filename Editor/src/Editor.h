#pragma once
#include <Seidon.h>

#include "Panels/InspectorPanel.h"
#include "Panels/HierarchyPanel.h"
#include "Panels/SystemsPanel.h"
#include "Panels/FileBrowserPanel.h"
#include "Panels/AnimationPanel.h"
#include "EditorAction.h"
#include "SelectedItem.h"
#include "Dockspace.h"

#include <ImGuizmo/ImGuizmo.h>

namespace Seidon
{
    using EditorActionCallback = std::function<void(EditorAction&)>;
    class Editor : public Application
    {
    public:
        EditorActionList actions;
        std::vector<EditorActionCallback> actionCallbacks;

        ResourceManager editorResourceManager;
        Scene* scene;
        Scene* runtimeScene;

        SelectedItem selectedItem;

        Scene editorSystems;
        Scene runtimeSystems;

        HierarchyPanel hierarchyPanel;
        InspectorPanel inspectorPanel;
        FileBrowserPanel fileBrowserPanel;
        AnimationPanel animationPanel;
        SystemsPanel systemsPanel;
        Dockspace dockspace;
        Extension e;

        RenderFunction drawColliders;
        bool colliderRenderingEnabled = false;

        int guizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
        bool local = false;

        bool isPlaying = false;
    public:
        Editor();

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
    };
}