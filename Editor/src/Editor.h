#pragma once
#include <Seidon.h>

#include "Panels/InspectorPanel.h"
#include "Panels/HierarchyPanel.h"
#include "Panels/SystemsPanel.h"
#include "Panels/FileBrowserPanel.h"
#include "SelectedItem.h"
#include "Dockspace.h"

#include <ImGuizmo/ImGuizmo.h>

namespace Seidon
{
    class Editor : public Application
    {
    public:
        ResourceManager editorResourceManager;
        Scene* scene;
        Scene* runtimeScene;

        SelectedItem selectedItem;

        Scene editorSystems;
        Scene runtimeSystems;

        HierarchyPanel hierarchyPanel;
        InspectorPanel inspectorPanel;
        FileBrowserPanel fileBrowserPanel;
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


        void SaveCurrentScene();
        void LoadScene();

    private:
        void DrawCubeColliders(Renderer& renderer);
        void DrawMeshColliders(Renderer& renderer);
        void DrawCharacterControllers(Renderer& renderer);
    };
}