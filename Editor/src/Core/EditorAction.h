#pragma once
#include <Seidon.h>

namespace Seidon
{
	struct EditorAction
	{
		virtual void Do() = 0;
		virtual void Undo() = 0;
	};

	struct ChangeData;
	struct EditorComponentAction : public EditorAction
	{
		Entity modifiedEntity;
		ComponentMetaType modifiedMetaType;
		std::string modifiedMemberName;

		byte oldValue[50];
		byte newValue[50];

		void Do()
		{
			void* component = modifiedMetaType.Get(modifiedEntity);
			modifiedMetaType.ModifyMemberV(modifiedMemberName, component, newValue);
		}

		void Undo()
		{
			void* component = modifiedMetaType.Get(modifiedEntity);
			modifiedMetaType.ModifyMemberV(modifiedMemberName, component, oldValue);
		}
	};

	class EditorActionList
	{
	public:
		EditorActionList(unsigned int size = 20);

		void Push(EditorAction* action);
		EditorAction* Pop();

		bool IsEmpty();
		bool IsFull();
		unsigned int GetCapacity();
	private:
		unsigned int tail;
		unsigned int head;
		std::vector<EditorAction*> actions;
	};
}