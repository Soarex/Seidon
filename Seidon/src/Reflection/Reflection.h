#pragma once
#include "../Core/UUID.h"
#include "../Graphics/Texture.h"
#include "../Graphics/HdrCubemap.h"
#include "../Graphics/Mesh.h"

#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include <string>
#include <vector>
#include <iostream>

namespace Seidon
{
	enum class Types
	{
		UNKNOWN = 0,
		INT,
		FLOAT,
		BOOL,
		STRING,
		VECTOR2,
		VECTOR3,
		VECTOR3_COLOR,
		VECTOR3_ANGLES,
		VECTOR4,
		VECTOR4_COLOR,
		MATERIAL_VECTOR,
		MESH_VECTOR,
		TEXTURE_VECTOR,
		TEXTURE,
		CUBEMAP,
		MESH
	};

	template<typename T, typename U> 
	constexpr size_t OffsetOf(U T::* member)
	{
		return (char*)&((T*)nullptr->*member) - (char*)nullptr;
	}

	struct MemberData
	{
		std::string name;
		Types type;
		unsigned int size;
		unsigned int offset;
	};

	class Entity;
	struct ComponentMetaType
	{
		std::string name;
		std::vector<MemberData> members;

		void* (*Add)(Entity& entity);
		void* (*Get)(Entity& entity);
		bool  (*Has)(Entity& entity);
		void  (*Copy)(entt::registry& src, entt::registry& dst, const std::unordered_map<UUID, entt::entity>& enttMap);

		template<typename T, typename U> 
		ComponentMetaType& AddMember(const std::string& name, U T::* member)
		{
			MemberData data;
			data.name = name;
			data.size = sizeof(U);
			data.offset = OffsetOf(member);
			data.type = Types::UNKNOWN;

			if(typeid(U).hash_code() == typeid(int).hash_code())
				data.type = Types::INT;

			if (typeid(U).hash_code() == typeid(float).hash_code())
				data.type = Types::FLOAT;

			if (typeid(U).hash_code() == typeid(bool).hash_code())
				data.type = Types::BOOL;

			if (typeid(U).hash_code() == typeid(std::string).hash_code())
				data.type = Types::STRING;

			if (typeid(U).hash_code() == typeid(glm::vec2).hash_code())
				data.type = Types::VECTOR2;

			if (typeid(U).hash_code() == typeid(glm::vec3).hash_code())
				data.type = Types::VECTOR3;

			if (typeid(U).hash_code() == typeid(std::vector<Mesh*>).hash_code())
				data.type = Types::MESH_VECTOR;

			if (typeid(U).hash_code() == typeid(std::vector<Material*>).hash_code())
				data.type = Types::MATERIAL_VECTOR;

			if (typeid(U).hash_code() == typeid(std::vector<Texture*>).hash_code())
				data.type = Types::TEXTURE_VECTOR;

			if (typeid(U).hash_code() == typeid(Texture*).hash_code())
				data.type = Types::TEXTURE;

			if (typeid(U).hash_code() == typeid(HdrCubemap*).hash_code())
				data.type = Types::CUBEMAP;

			if (typeid(U).hash_code() == typeid(Mesh*).hash_code())
				data.type = Types::MESH;

			members.push_back(data);

			return *this;
		}

		template<typename T, typename U>
		ComponentMetaType& AddMember(const std::string& name, U T::* member, Types typeOverride)
		{
			MemberData data;
			data.name = name;
			data.size = sizeof(U);
			data.offset = OffsetOf(member);
			data.type = typeOverride;

			members.push_back(data);

			return *this;
		}
	};

	class Scene;
	struct SystemMetaType
	{
		std::string name;
		std::vector<MemberData> members;

		void* (*Add)(Scene& scene);
		void* (*Delete)(Scene& scene);
		void* (*Get)(Scene& scene);
		bool  (*Has)(Scene& scene);
		void  (*Copy)(Scene& src, Scene& dst);

		template<typename T, typename U>
		SystemMetaType& AddMember(const std::string& name, U T::* member)
		{
			MemberData data;
			data.name = name;
			data.size = sizeof(U);
			data.offset = OffsetOf(member);
			data.type = Types::UNKNOWN;

			if (typeid(U).hash_code() == typeid(int).hash_code())
				data.type = Types::INT;

			if (typeid(U).hash_code() == typeid(float).hash_code())
				data.type = Types::FLOAT;

			if (typeid(U).hash_code() == typeid(bool).hash_code())
				data.type = Types::BOOL;

			if (typeid(U).hash_code() == typeid(std::string).hash_code())
				data.type = Types::STRING;

			if (typeid(U).hash_code() == typeid(glm::vec2).hash_code())
				data.type = Types::VECTOR2;

			if (typeid(U).hash_code() == typeid(glm::vec3).hash_code())
				data.type = Types::VECTOR3;

			if (typeid(U).hash_code() == typeid(std::vector<Mesh*>).hash_code())
				data.type = Types::MESH_VECTOR;

			if (typeid(U).hash_code() == typeid(std::vector<Material*>).hash_code())
				data.type = Types::MATERIAL_VECTOR;

			if (typeid(U).hash_code() == typeid(std::vector<Texture*>).hash_code())
				data.type = Types::TEXTURE_VECTOR;

			if (typeid(U).hash_code() == typeid(Texture*).hash_code())
				data.type = Types::TEXTURE;

			if (typeid(U).hash_code() == typeid(HdrCubemap*).hash_code())
				data.type = Types::CUBEMAP;

			if (typeid(U).hash_code() == typeid(Mesh*).hash_code())
				data.type = Types::MESH;

			members.push_back(data);

			return *this;
		}

		template<typename T, typename U>
		SystemMetaType& AddMember(const std::string& name, U T::* member, Types typeOverride)
		{
			MemberData data;
			data.name = name;
			data.size = sizeof(U);
			data.offset = OffsetOf(member);
			data.type = typeOverride;

			members.push_back(data);

			return *this;
		}
	};
}