#pragma once
#include <xhash>

namespace Seidon
{
	class UUID
	{
	private:
		uint64_t uuid;

	public:
		UUID();
		UUID(uint64_t uuid);
		UUID(const UUID&) = default;

		operator uint64_t() const { return uuid; }
	};
}

namespace std 
{
	template<>
	struct hash<Seidon::UUID>
	{
		std::size_t operator()(const Seidon::UUID& uuid) const
		{
			return hash<uint64_t>()((uint64_t)uuid);
		}
	};

}