#include "ipch.h"

#include "SceneSerializer.h"
#include "Scene/Entity.h"
#include "Assets/Script/Script.h"
#include "FileSystem/FileSystem.h"

#include "yaml-cpp/yaml.h"


namespace YAML {
	template<>
	struct convert<glm::vec3> {
		static Node encode(const glm::vec3& vec3) {
			Node node;
			node.push_back(vec3.x);
			node.push_back(vec3.y);
			node.push_back(vec3.z);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& vec3) {
			if (!node.IsSequence() || node.size() != 3)
				return false;
			
			vec3.x = node[0].as<float>();
			vec3.y = node[1].as<float>();
			vec3.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4> {
		static Node encode(const glm::vec4& vec4) {
			Node node;
			node.push_back(vec4.x);
			node.push_back(vec4.y);
			node.push_back(vec4.z);
			node.push_back(vec4.w);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& vec4) {
			if (!node.IsSequence() || node.size() != 4)
				return false;

			vec4.x = node[0].as<float>();
			vec4.y = node[1].as<float>();
			vec4.z = node[2].as<float>();
			vec4.w = node[3].as<float>();
			return true;
		}
	};
}

namespace Rapier {


	static void ParseNativeScriptComponent(Entity& entity, const std::string& scriptName, bool enableOnUpdate) {

		for (auto& script : EntityScriptContainer::s_EntityScriptContainer.m_Scripts)
		{
			if (script->GetName() == scriptName)
			{
				entity.AddComponent<NativeScriptComponent>(script->Clone());
				break;
			}
		}
	}


	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& vec3) {
		out << YAML::Flow;
		out << YAML::BeginSeq << vec3.x << vec3.y << vec3.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& vec4) {
		out << YAML::Flow;
		out << YAML::BeginSeq << vec4.x << vec4.y << vec4.z << vec4.w << YAML::EndSeq;
		return out;
	}



	static void SerializeEntity(YAML::Emitter& out, Entity entity) {
		out << YAML::BeginMap;
		out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();

		if (entity.HasComponent<TagComponent>()) {
			out << YAML::Key << "TagComponent";

			out << YAML::BeginMap;
			auto& tag = entity.GetComponent<TagComponent>().Tag;
			out << YAML::Key << "Tag" << YAML::Value << tag;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<TransformComponent>()) {
			out << YAML::Key << "TransformComponent";

			out << YAML::BeginMap;
			auto& transformComponent = entity.GetComponent<TransformComponent>();
			out << YAML::Key << "Translation" << YAML::Value << transformComponent.Translation;
			out << YAML::Key << "Rotation" << YAML::Value << transformComponent.Rotation;
			out << YAML::Key << "Scale" << YAML::Value << transformComponent.Scale;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<CameraComponent>()) {
			out << YAML::Key << "CameraComponent";

			out << YAML::BeginMap;
			auto& cameraComponent = entity.GetComponent<CameraComponent>();

			out << YAML::Key << "Camera";
			out << YAML::BeginMap;
			out << YAML::Key << "AspectRatio" << YAML::Value << cameraComponent.Camera.GetAspectRatio();
			out << YAML::Key << "NearClip" << YAML::Value << cameraComponent.Camera.GetNearClip();
			out << YAML::Key << "FarClip" << YAML::Value << cameraComponent.Camera.GetFarClip();
			out << YAML::Key << "Height" << YAML::Value << cameraComponent.Camera.GetHeight();
			out << YAML::Key << "Size" << YAML::Value << cameraComponent.Camera.GetSize();
			out << YAML::EndMap;

			out << YAML::Key << "Primary" << YAML::Value << cameraComponent.Primary;
			out << YAML::Key << "FixedAspectRatio" << YAML::Value << cameraComponent.FixedAspectRatio;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<SpriteRendererComponent>()) {
			out << YAML::Key << "SpriteRendererComponent";

			out << YAML::BeginMap;
			auto& spriteRendererComponent = entity.GetComponent<SpriteRendererComponent>();
			out << YAML::Key << "Color" << YAML::Value << spriteRendererComponent.Color;
			out << YAML::Key << "Filename" << YAML::Value << spriteRendererComponent.Texture->GetFilepath();;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<NativeScriptComponent>()) {
			out << YAML::Key << "NativeScriptComponent";

			out << YAML::BeginMap;
			auto& nativeScriptComponent = entity.GetComponent<NativeScriptComponent>();


			
			out << YAML::Key << "ScriptName" << YAML::Value << nativeScriptComponent.Instance->GetName();
			

			out << YAML::Key << "EnableOnUpdate" << YAML::Value << nativeScriptComponent.EnableOnUpdate;
			out << YAML::EndMap;
		}

		out << YAML::EndMap;
	}



	SceneSerializer::SceneSerializer(const Ref<Scene>& scene) 
		: m_Scene(scene) {

	}

	void SceneSerializer::Serialize(const std::string& filename) {
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "SceneTitle";
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

		m_Scene->m_Registry.each([&](auto entityId) {
			Entity entity = { entityId, m_Scene.get() };

			SerializeEntity(out, entity);
			});

		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream stream(filename);
		stream << out.c_str();
	}

	bool SceneSerializer::Deserialize(const std::string& filepath) {


		YAML::Node data = YAML::LoadFile(filepath);
		
		if (!data["Scene"])
			return false;

		std::string sceneName = data["Scene"].as<std::string>();

		auto entities = data["Entities"];
		if (entities) {
			for (auto entity : entities) {
				uint64_t uuid = entity["Entity"].as<uint64_t>();

				std::string name;
				auto tagComponent = entity["TagComponent"];
				if (tagComponent)
					name = tagComponent["Tag"].as<std::string>();

				Entity deserializedEntity = m_Scene->LoadEntity(uuid, name);

				RAPIER_CORE_INFO("Deserializing Entity '{0}' with UUID : {1}", name, deserializedEntity.GetComponent<UUIDComponent>().Id);

				auto transformComponent = entity["TransformComponent"];
				if (transformComponent) {
					auto& eTransformComponent = deserializedEntity.GetComponent<TransformComponent>();
					eTransformComponent.Translation = transformComponent["Translation"].as<glm::vec3>();
					eTransformComponent.Rotation = transformComponent["Rotation"].as<glm::vec3>();
					eTransformComponent.Scale = transformComponent["Scale"].as<glm::vec3>();
				}


				auto cameraComponent = entity["CameraComponent"];
				if (cameraComponent) {
					auto& eCameraComponent = deserializedEntity.AddComponent<CameraComponent>();
					eCameraComponent.Primary = cameraComponent["Primary"].as<bool>();
					eCameraComponent.FixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();

					auto camera = cameraComponent["Camera"];
					float aspectRatio = camera["AspectRatio"].as<float>();
					float size = camera["Size"].as<float>();
					float farClip = camera["FarClip"].as<float>();
					float nearClip = camera["NearClip"].as<float>();
					uint32_t height = camera["Height"].as<uint32_t>();
					eCameraComponent.Camera = Camera(aspectRatio, size, farClip, nearClip, height);
				}

				auto spriteRendererComponent = entity["SpriteRendererComponent"];
				if (spriteRendererComponent) {
					auto& eSpriteRendererComponent = deserializedEntity.AddComponent<SpriteRendererComponent>();
					eSpriteRendererComponent.Color = spriteRendererComponent["Color"].as<glm::vec4>();
					std::string spriteRendererComponentFilename = spriteRendererComponent["Filename"].as<std::string>();
					if (spriteRendererComponentFilename == "White Texture")
						eSpriteRendererComponent.Texture = Texture2D::CreateWhiteTexture();
					else
						eSpriteRendererComponent.Texture = Texture2D::Create(spriteRendererComponentFilename);
				}

				auto nativeScriptComponent = entity["NativeScriptComponent"];
				if (nativeScriptComponent) {
					std::string scriptName = nativeScriptComponent["ScriptName"].as<std::string>();
					//ParseNativeScriptComponent<ALL_ENTITY_SCRIPTS>(eNativeScriptComponent, scriptName);
					bool enableOnUpdate = nativeScriptComponent["EnableOnUpdate"].as<bool>();
					ParseNativeScriptComponent(deserializedEntity, scriptName, enableOnUpdate);
				}
			}

		}

		return true;
	}
}