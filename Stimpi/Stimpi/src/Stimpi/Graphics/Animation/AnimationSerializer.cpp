#include "stpch.h"
#include "Stimpi/Graphics/Animation/AnimationSerializer.h"

#include "Stimpi/Log.h"
#include "Stimpi/Utils/FilePath.h"
#include "Stimpi/Scene/ResourceManager.h"

#include <yaml-cpp/yaml.h>

namespace Stimpi
{

	AnimationSerializer::AnimationSerializer(Animation* animation)
		: m_Animation(animation)
	{

	}

	void AnimationSerializer::Serialize(const std::string& filePath)
	{
		YAML::Emitter out;

		out << YAML::Block;
		out << YAML::BeginMap;
		{
			out << YAML::Key << "Animation" << YAML::Value;
			out << YAML::BeginMap;
			{
				auto asset = AssetManager::GetAsset(m_Animation->GetSubTexture()->GetAssetHandle());
				out << YAML::Key << "Texture" << YAML::Value << asset.GetFilePath().string();

				out << YAML::Key << "Frames" << YAML::Value;
				out << YAML::BeginMap;
				for (auto& frame : m_Animation->m_Frames)
				{
					out << YAML::Key << "Frame" << YAML::Value;
					out << YAML::BeginMap;
					{
						out << YAML::Key << "Duration" << YAML::Value << frame.m_Duration;

						out << YAML::Key << "FrameSize" << YAML::Value;
						out << YAML::BeginSeq;
						{
							out << frame.m_FrameSize.x << frame.m_FrameSize.y;
						}
						out << YAML::EndSeq;

						out << YAML::Key << "FramePosition" << YAML::Value;
						out << YAML::BeginSeq;
						{
							out << frame.m_FramePosition.x << frame.m_FramePosition.y;
						}
						out << YAML::EndSeq;
					}
					out << YAML::EndMap;
				}
				out << YAML::EndMap;
			}
			out << YAML::EndMap;
		}
		out << YAML::EndMap;

		ResourceManager::Instance()->WriteToFile(filePath, out.c_str());
	}

	bool AnimationSerializer::Deseriealize(const std::string& filePath)
	{
		bool loaded = false;
		YAML::Node loadData = YAML::LoadFile(filePath);

		if (loadData["Animation"])
		{
			YAML::Node animation = loadData["Animation"];

			if (animation["Texture"])
			{
				FilePath assetPath(animation["Texture"].as<std::string>());
				m_Animation->m_SubTexture = std::make_shared<SubTexture>(assetPath, glm::vec2{ 0.0f, 0.0f }, glm::vec2{ 0.0f, 0.0f });;
			}

			if (animation["Frames"])
			{
				YAML::Node frames = animation["Frames"];
				for (YAML::const_iterator it = frames.begin(); it != frames.end(); it++)
				{
					YAML::Node frame = it->second;
					AnimationFrameData frameData;

					if (frame["Duration"])
					{
						frameData.m_Duration = frame["Duration"].as<float>();
					}
					if (frame["FrameSize"])
					{
						YAML::Node size = frame["FrameSize"];
						frameData.m_FrameSize = glm::vec2(size[0].as<float>(), size[1].as<float>());
					}
					if (frame["FramePosition"])
					{
						YAML::Node position = frame["FramePosition"];
						frameData.m_FramePosition = glm::vec2(position[0].as<float>(), position[1].as<float>());
					}

					m_Animation->m_Frames.push_back(frameData);
				}
			}

			loaded = true;
		}

		return loaded;
	}

}