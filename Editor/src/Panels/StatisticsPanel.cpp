#include "StatisticsPanel.h"
#include <Seidon.h>

#include "../Editor.h"

namespace Seidon
{
    StatisticsPanel::StatisticsPanel(Editor& editor)
        : Panel(editor)
    {

    }

	void StatisticsPanel::Init()
	{

	}

	void StatisticsPanel::Draw()
	{
        ImGui::Begin("Stats");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        RenderSystem* renderSystem = nullptr;

        if (!editor.isPlaying && editor.openProject->HasEditorSystem<RenderSystem>())
            renderSystem = editor.openProject->GetEditorSystem<RenderSystem>();

        if (editor.isPlaying && editor.GetSceneManager()->GetActiveScene()->HasSystem<RenderSystem>())
            renderSystem = &editor.GetSceneManager()->GetActiveScene()->GetSystem<RenderSystem>();

        if (renderSystem)
        {
            const RenderStats& stats = renderSystem->GetRenderStats();

            ImGui::Text("Vertex use: %d / %d", stats.vertexCount, stats.vertexBufferSize);
            ImGui::Text("Index use: %d / %d", stats.indexCount, stats.indexBufferSize);

            float memoryUsedInMB = (stats.indexCount * sizeof(int) + stats.vertexCount * sizeof(Vertex)) / 1000000.0f;
            float memoryAllocatedInMB = (stats.indexBufferSize * sizeof(int) + stats.vertexBufferSize * sizeof(Vertex)) / 1000000.0f;
            ImGui::Text("Memory used: %.2f MB / %.2f MB", memoryUsedInMB, memoryAllocatedInMB);
            ImGui::Text("Object count: %d in %d batches", stats.objectCount, stats.batchCount);
        }

        ImGui::End();
	}

	void StatisticsPanel::Destroy()
	{

	}
}