#pragma once
#include <entt/entt.hpp>

class EcsContext 
{
	EcsContext() = default;

public:
	inline entt::id_type value(const entt::id_type name) 
	{
		if (nameToIndex.find(name) == nameToIndex.cend())
			nameToIndex[name] = entt::id_type(nameToIndex.size());

		return nameToIndex[name] + 1;
	}

	static EcsContext* Instance()
	{
		static EcsContext self{};
		entt::meta_ctx ctx{};
		self.metaContext = ctx;
		return &self;
	}

	void SetMetaContext()
	{
		entt::meta_ctx::bind(metaContext);
	}

private:
	std::unordered_map<entt::id_type, entt::id_type> nameToIndex{};
	entt::meta_ctx metaContext;
};