#pragma once
#include <Seidon.h>
#include <EntryPoint.h>

#include "Panels/InspectorPanel.h"
#include "Panels/HierarchyPanel.h"
#include "Panels/AssetBrowserPanel.h"
#include "Dockspace.h"

#include <ImGuizmo/ImGuizmo.h>

namespace Seidon
{
    class Editor : public Application
    {
    public:
        Scene* scene;
        Scene* runtimeScene;
        Entity selectedEntity;

        HierarchyPanel hierarchyPanel;
        InspectorPanel inspectorPanel;
        AssetBrowserPanel assetBrowserPanel;
        Dockspace dockspace;
        Extension e;

        int guizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
        bool local = false;

        bool isPlaying = false;
    public:
        void Init() override;
        void Update() override;
        void Destroy() override;
    };
}