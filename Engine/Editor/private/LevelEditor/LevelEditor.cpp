#include <LevelEditor/LevelEditor.h>
#include <LevelEditor/LevelSerializer.h>
#include <LevelEditor/ECS/ecsLevelEditor.h>

#include <Filesystem.h>
#include <ECS/ecsSystems.h>
#include <Parser/WorldParser.h>

#include <imgui.h>
#include <imgui_stdlib.h>
#include <regex>

namespace
{
	void ParsePosition(const char* cstr, float* output)
	{
		char* end;

		output[0] = std::strtof(cstr, &end);
		cstr = end + 1;

		output[1] = std::strtof(cstr, &end);
		cstr = end + 1;

		output[2] = std::strtof(cstr, &end);
	}

	void SerializePosition(const float* values, std::string& output)
	{
		output.clear();
		output += std::to_string(values[0]);
		output += ',';
		output += std::to_string(values[1]);
		output += ',';
		output += std::to_string(values[2]);
	}
}

namespace GameEngine
{
	namespace Editor
	{
		LevelEditor::LevelEditor(flecs::world& world) : m_World(world)
		{
			m_Level = LevelSerializer::Deserialize(Core::g_FileSystem->GetFilePath("Levels/Main.xml").generic_string());

			for (World::LevelObject& levelObject : m_Level->GetLevelObjects())
			{
				CreateECSEntityForLevelObject(levelObject);
			}

			EntitySystem::LevelEditorECS::RegisterLevelEditorEcsSystems(m_World);
		}

		void LevelEditor::Draw()
		{
			ImGui::Begin(GetName());

			if (m_Level.has_value()) [[likely]]
			{
				for (World::LevelObject& levelObject : m_Level->GetLevelObjects())
				{
					if (ImGui::TreeNode(levelObject.GetName().c_str()))
					{
						for (World::LevelObject::Component& component : levelObject.GetComponents())
						{
							if (component.first == "Position")
							{
								float pos[3];
								ParsePosition(component.second->c_str(), pos);
								ImGui::InputFloat3(component.first.c_str(), pos);
								SerializePosition(pos, *component.second);

								continue;
							}

							ImGui::InputText(component.first.c_str(), component.second.get());
						}

						ImGui::TreePop();
					}
				}

				if (ImGui::Button("Add object"))
				{
					AddDefaultObject();
				}
			}

			if (ImGui::Button("Save"))
			{
				m_SaveButtonMessageTimer.Reset();
				m_SaveButtonPressed = true;

				Save();
			}

			if (m_SaveButtonPressed)
			{
				ImGui::SameLine();
				ImGui::Text("Saved!");
			}

			ImGui::End();
		}

		void LevelEditor::Update(float dt)
		{
			m_SaveButtonMessageTimer.Tick();

			if (m_SaveButtonMessageTimer.GetTotalTime() > m_TimeToShowSaveButtonMessage)
			{
				m_SaveButtonPressed = false;
			}
		}

		void LevelEditor::Save()
		{
			assert(m_Level.has_value());
			LevelSerializer::Serialize(Core::g_FileSystem->GetFilePath("Levels/Main.xml").generic_string(), m_Level.value());
		}

		void LevelEditor::AddDefaultObject()
		{
			assert(m_Level.has_value());

			World::LevelObject newLevelObject;
			newLevelObject.SetName(CreateUniqueObjectName().c_str());
			newLevelObject.AddComponent("Position", "0.0,0.0,0.0");
			newLevelObject.AddComponent("GeometryPtr", "Cube");
			m_Level->AddLevelObject(newLevelObject);

			CreateECSEntityForLevelObject(newLevelObject);
		}

		std::string LevelEditor::CreateUniqueObjectName()
		{
			static const std::string baseName = "New Object ";
			static const std::regex pattern("^New Object (\\d+)$");

			int maxNumber = 0;
			std::smatch matches;

			for (const World::LevelObject& levelObject : m_Level->GetLevelObjects()) {
				if (std::regex_match(levelObject.GetName(), matches, pattern)) {
					int number = std::stoi(matches[1].str());
					if (number > maxNumber) {
						maxNumber = number;
					}
				}
			}

			return baseName + std::to_string(maxNumber + 1);
		}

		void LevelEditor::CreateECSEntityForLevelObject(World::LevelObject& levelObject)
		{
			flecs::entity entity = m_World.entity(levelObject.GetName().c_str());

			World::LevelObject::ComponentList& componentList = levelObject.GetComponents();

			World::LevelObject::ComponentList::iterator positionAttribute = std::ranges::find_if(componentList,
				[](World::LevelObject::Component& component)
				{
					return !std::strcmp(component.first.c_str(), "Position");
				}
			);

			World::LevelObject::ComponentList::iterator geometryAttribute = std::ranges::find_if(componentList,
				[](World::LevelObject::Component& component)
				{
					return !std::strcmp(component.first.c_str(), "GeometryPtr");
				}
			);

			if (positionAttribute != componentList.end() &&
				geometryAttribute != componentList.end())
			{
				assert(World::WorldParser::GetCustomComponents().contains(*geometryAttribute->second));

				entity.set(EntitySystem::LevelEditorECS::PositionDesc{ positionAttribute->second });

				// Can be set to 0 since it doesn't matter now, will be updated by the system
				entity.set(EntitySystem::EditorECS::Position{ 0.0f, 0.0f, 0.0f });
				entity.set(GeometryPtr{
					reinterpret_cast<RenderCore::Geometry*>(
						World::WorldParser::GetCustomComponents()[*geometryAttribute->second]
						)
					});
			}
		}
	}
}