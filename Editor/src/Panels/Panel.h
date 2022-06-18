#pragma once

namespace Seidon
{
	class Editor;
	class Panel
	{
	public:
		Panel(Editor& editor) : editor(editor) {}

		virtual void Init() {}
		virtual void Draw() {}
		virtual void Destroy() {}
	protected:
		Editor& editor;
	};
}