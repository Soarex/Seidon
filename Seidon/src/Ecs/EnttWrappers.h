#pragma once
#include <entt/entt.hpp>

namespace Seidon
{
	template<typename... Types>
	using ComponentGroup = entt::group<Types...>;

	template<typename Type>
	using ComponentView = entt::view<Type>;

	template<typename... Types>
	using TypeList = entt::get_t<Types...>;

	template<typename... Types>
	inline constexpr TypeList<Types...> GetTypeList{};

	typedef entt::entity EntityId;
	inline constexpr entt::null_t NullEntityId{};

}