#pragma once
#include "../Core/UUID.h"
#include "../Graphics/Texture.h"
#include "../Graphics/HdrCubemap.h"
#include "../Graphics/Mesh.h"
#include "../Graphics/Sprite.h"
#include "../Graphics/Armature.h"
#include "../Graphics/Shader.h"

#include "../Animation/Animation.h"

#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include <string>
#include <vector>
#include <iostream>
#include <fstream>


namespace Seidon
{
	typedef unsigned char Byte;

	enum class Types
	{
		UNKNOWN = 0,
		INT,
		FLOAT,
		FLOAT_NORMALIZED,
		BOOL,
		STRING,
		ID,
		VECTOR2,
		VECTOR3,
		VECTOR3_COLOR,
		VECTOR3_ANGLES,
		VECTOR4,
		VECTOR4_COLOR,
		MESH,
		MESH_VECTOR,
		TEXTURE,
		TEXTURE_VECTOR,
		MATERIAL,
		MATERIAL_VECTOR,
		CUBEMAP,
		ARMATURE,
		ANIMATION,
		SHADER,
		SPRITE
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

		inline bool operator==(const MemberData& other)
		{
			if (name != other.name) return false;
			if (type != other.type) return false;
			if (size != other.size) return false;
			if (offset != other.offset) return false;

			return true;
		}

		inline bool operator!=(const MemberData& other)
		{
			return !(*this == other);
		}
	};

	struct MetaType
	{
		std::string name;
		std::vector<MemberData> members;

		template<typename T, typename U>
		MetaType& AddMember(const std::string& name, U T::* member)
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

			if (typeid(U).hash_code() == typeid(UUID).hash_code())
				data.type = Types::ID;

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

			if (typeid(U).hash_code() == typeid(Animation*).hash_code())
				data.type = Types::ANIMATION;

			if (typeid(U).hash_code() == typeid(Armature*).hash_code())
				data.type = Types::ARMATURE;

			if (typeid(U).hash_code() == typeid(Shader*).hash_code())
				data.type = Types::SHADER;

			if (typeid(U).hash_code() == typeid(Sprite*).hash_code())
				data.type = Types::SPRITE;

			members.push_back(data);

			return *this;
		}

		template<typename T, typename U>
		MetaType& AddMember(const std::string& name, U T::* member, Types typeOverride)
		{
			MemberData data;
			data.name = name;
			data.size = sizeof(U);
			data.offset = OffsetOf(member);
			data.type = typeOverride;

			members.push_back(data);

			return *this;
		}

		void Save(std::ofstream& out, byte* data);
		void Load(std::ifstream& in, byte* data);

		inline bool operator==(const MetaType& other) 
		{ 
			if (members.size() != other.members.size()) return false;

			bool equals = true;
			for (int i = 0; i < members.size(); i++)
				equals &= members[i] == other.members[i];

			return equals;
		}

		inline bool operator!=(const MetaType& other) 
		{ 
			return !(*this == other); 
		}

		static Types StringToType(const std::string& string)
		{
			if (string == "INT")
				return Types::INT;

			if (string == "FLOAT")
				return Types::FLOAT;

			if (string == "FLOAT_NORMALIZED")
				return Types::FLOAT_NORMALIZED;

			if (string == "BOOL")
				return Types::BOOL;

			if (string == "STRING")
				return Types::STRING;

			if (string == "ID")
				return Types::ID;

			if (string == "VECTOR2")
				return Types::VECTOR2;

			if (string == "VECTOR3")
				return Types::VECTOR3;

			if (string == "VECTOR3_COLOR")
				return Types::VECTOR3_COLOR;

			if (string == "VECTOR3_ANGLES")
				return Types::VECTOR3_ANGLES;

			if (string == "MESH_VECTOR")
				return Types::MESH_VECTOR;

			if (string == "MATERIAL_VECTOR")
				return Types::MATERIAL_VECTOR;

			if (string == "TEXTURE_VECTOR")
				return Types::TEXTURE_VECTOR;

			if (string == "TEXTURE")
				return Types::TEXTURE;

			if (string == "CUBEMAP")
				return Types::CUBEMAP;

			if (string == "MESH")
				return Types::MESH;

			if (string == "ANIMATION")
				return Types::ANIMATION;

			if (string == "ARMATURE")
				return Types::ARMATURE;

			if (string == "SHADER")
				return Types::SHADER;

			if (string == "SPRITE")
				return Types::SPRITE;
		}

		static std::string TypeToString(Types type)
		{
			if (type == Types::INT)
				return "Int";

			if (type == Types::FLOAT)
				return "Float";

			if (type == Types::FLOAT_NORMALIZED)
				return "Float normalized";

			if (type == Types::BOOL)
				return "Bool";

			if (type == Types::STRING)
				return "String";

			if (type == Types::ID)
				return "Id";

			if (type == Types::VECTOR2)
				return "Vec2";

			if (type == Types::VECTOR3)
				return "Vec3";

			if (type == Types::VECTOR3_COLOR)
				return "Vec3 Color";

			if (type == Types::VECTOR3_ANGLES)
				return "Vec3 Angles";

			if (type == Types::MESH_VECTOR)
				return "Mesh Vector";

			if (type == Types::MATERIAL_VECTOR)
				return "Material Vector";

			if (type == Types::TEXTURE_VECTOR)
				return "Texture Vector";

			if (type == Types::TEXTURE)
				return "Texture";

			if (type == Types::CUBEMAP)
				return "Cubemap";

			if (type == Types::MESH)
				return "Mesh";

			if (type == Types::ANIMATION)
				return "Animation";

			if (type == Types::ARMATURE)
				return "Armature";

			if (type == Types::SHADER)
				return "Shader";

			if (type == Types::SPRITE)
				return "Sprite";
		}
	};

	class Entity;
	struct ComponentMetaType : public MetaType
	{
		void* (*Add)(Entity entity);
		void (*Remove)(Entity entity);
		void* (*Get)(Entity entity);
		bool  (*Has)(Entity entity);
		void  (*Copy)(Entity src, Entity dst);
	};

	class Scene;
	struct SystemMetaType : public MetaType
	{
		void* (*Add)(Scene& scene);
		void* (*Delete)(Scene& scene);
		void* (*Get)(Scene& scene);
		bool  (*Has)(Scene& scene);
		void  (*Copy)(Scene& src, Scene& dst);
	};
}