#include "EditorAction.h"
#include "Utils/DrawFunctions.h"

namespace Seidon
{
	EditorActionList::EditorActionList(unsigned int size)
		: actions(size), tail(0), head(0) { }

	void EditorActionList::Push(EditorAction* action)
	{
		if (IsFull())
		{
			delete actions[tail];
			tail = (tail + 1) % GetCapacity();
		}

		head = (head + 1) % GetCapacity();

		actions[head] = action;
	}

	EditorAction* EditorActionList::Pop()
	{
		if (IsEmpty()) return nullptr;

		EditorAction* res = actions[head];
		head = (head - 1) % GetCapacity();

		return res;
	}

	bool EditorActionList::IsFull()
	{
		return (head + 1) % GetCapacity() == tail;
	}

	bool EditorActionList::IsEmpty()
	{
		return head == tail;
	}

	unsigned int EditorActionList::GetCapacity()
	{
		return actions.size();
	}
}