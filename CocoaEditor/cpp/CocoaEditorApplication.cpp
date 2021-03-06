#include "Cocoa.h"
#include "Gui/ImGuiHeader.h"
#include "EditorWindows/InspectorWindow.h"
#include "nativeScripting/SourceFileWatcher.h"

#include "CocoaEditorApplication.h"
#include "LevelEditorSceneInitializer.h"
#include "ImGuiLayer.h"
#include "cocoa/file/IFile.h"
#include "cocoa/util/Settings.h"
#include "cocoa/systems/RenderSystem.h"

#include <glad/glad.h>
#include <nlohmann/json.hpp>

#ifdef CopyFile
#undef CopyFile
#endif

namespace Cocoa
{
	// ===================================================================================
	// Editor Layer
	// ===================================================================================
	EditorLayer::EditorLayer(Scene* scene)
		: Layer(scene), m_PickingTexture(3840, 2160)
	{
		m_PickingShader = std::make_shared<Shader>(CPath(Settings::General::s_EngineAssetsPath + "shaders/Picking.glsl"));
		m_DefaultShader = std::make_shared<Shader>(Settings::General::s_EngineAssetsPath + "shaders/SpriteRenderer.glsl");
		m_OutlineShader = std::make_shared<Shader>(Settings::General::s_EngineAssetsPath + "shaders/SingleColor.glsl");
		Settings::General::s_EngineExeDirectory = IFile::GetExecutableDirectory().GetDirectory(-1);
		Settings::General::s_EngineSourceDirectory = IFile::GetExecutableDirectory().GetDirectory(-4);
		Log::Info("%s", Settings::General::s_EngineExeDirectory.Filepath());
	}

	bool EditorLayer::CreateProject(const CPath& projectPath, const char* filename)
	{
		Settings::General::s_CurrentProject = projectPath + (std::string(filename) + ".cprj");
		Settings::General::s_CurrentScene = projectPath + "scenes" + "NewScene.cocoa";
		Settings::General::s_WorkingDirectory = Settings::General::s_CurrentProject.GetDirectory(-1);

		json saveData = {
			{"ProjectPath", Settings::General::s_CurrentProject.Filepath()},
			{"CurrentScene", Settings::General::s_CurrentScene.Filepath()},
			{"WorkingDirectory", Settings::General::s_WorkingDirectory.Filepath() }
		};

		IFile::WriteFile(saveData.dump(4).c_str(), Settings::General::s_CurrentProject);
		IFile::CreateDirIfNotExists(projectPath + "assets");
		IFile::CreateDirIfNotExists(projectPath + "scripts");
		IFile::CreateDirIfNotExists(projectPath + "scenes");

		CocoaEditor* application = (CocoaEditor*)Application::Get();
		application->GetEditorLayer()->m_Scene->Reset();
		application->GetEditorLayer()->m_Scene->Save(Settings::General::s_CurrentScene);
		application->GetEditorLayer()->m_SourceFileWatcher = std::make_shared<SourceFileWatcher>(projectPath + "scripts");
		SaveEditorData();

		return true;
	}

	void EditorLayer::SaveEditorData()
	{
		if (static_cast<CocoaEditor*>(Application::Get())->GetEditorLayer()->m_ProjectLoaded)
		{
			ImGui::SaveIniSettingsToDisk(Settings::General::s_ImGuiConfigPath.Filepath());
		}

		json saveData = {
			{"ProjectPath", Settings::General::s_CurrentProject.Filepath()},
			{"EditorStyle", Settings::General::s_EditorStyleData.Filepath()},
			{"ImGuiConfig", Settings::General::s_ImGuiConfigPath.Filepath()}
		};

		IFile::WriteFile(saveData.dump(4).c_str(), Settings::General::s_EditorSaveData);
	}

	bool EditorLayer::LoadEditorData(const CPath& path)
	{
		File* editorData = IFile::OpenFile(path);
		if (editorData->m_Data.size() > 0)
		{
			json j = json::parse(editorData->m_Data);
			if (!j["EditorStyle"].is_null())
			{
				Settings::General::s_EditorStyleData = CPath(j["EditorStyle"], false);
			}

			if (!j["ImGuiConfigPath"].is_null())
			{
				Settings::General::s_ImGuiConfigPath = CPath(j["ImGuiConfig"], false);
			}

			if (!j["ProjectPath"].is_null())
			{
				LoadProject(CPath(j["ProjectPath"], false));
			}
		}
		IFile::CloseFile(editorData);

		return true;
	}

	void EditorLayer::SaveProject()
	{
		json saveData = {
			{"ProjectPath", Settings::General::s_CurrentProject.Filepath()},
			{"CurrentScene", Settings::General::s_CurrentScene.Filepath()},
			{"WorkingDirectory", Settings::General::s_CurrentProject.GetDirectory(-1) }
		};

		IFile::WriteFile(saveData.dump(4).c_str(), Settings::General::s_CurrentProject);

		SaveEditorData();
	}

	bool EditorLayer::LoadProject(const CPath& path)
	{
		Settings::General::s_CurrentProject = path;
		File* projectData = IFile::OpenFile(Settings::General::s_CurrentProject);
		if (projectData->m_Data.size() > 0)
		{
			json j = json::parse(projectData->m_Data);
			if (!j["CurrentScene"].is_null())
			{
				Settings::General::s_CurrentScene = CPath(j["CurrentScene"], false);
				Settings::General::s_WorkingDirectory = CPath(j["WorkingDirectory"], false);

				CocoaEditor* application = (CocoaEditor*)Application::Get();
				application->GetEditorLayer()->m_Scene->Load(Settings::General::s_CurrentScene);

				SaveEditorData();
				std::string winTitle = std::string(Settings::General::s_CurrentProject.Filename()) + " -- " + std::string(Settings::General::s_CurrentScene.Filename());
				Application::Get()->GetWindow()->SetTitle(winTitle.c_str());

				application->GetEditorLayer()->m_SourceFileWatcher = std::make_shared<SourceFileWatcher>(Settings::General::s_WorkingDirectory + "scripts");
				static_cast<CocoaEditor*>(Application::Get())->SetProjectLoaded();
				return true;
			}
		}

		return false;
	}

	void EditorLayer::OnAttach()
	{
		// Set the assets path as CWD (which should be where the exe is currently located)
		Settings::General::s_EngineAssetsPath = IFile::GetCwd() + Settings::General::s_EngineAssetsPath;
		Settings::General::s_ImGuiConfigPath = Settings::General::s_EngineAssetsPath + Settings::General::s_ImGuiConfigPath;

		// Create application store data if it does not exist
		IFile::CreateDirIfNotExists(IFile::GetSpecialAppFolder() + "CocoaEngine");

		Settings::General::s_EditorSaveData = IFile::GetSpecialAppFolder() + "CocoaEngine" + Settings::General::s_EditorSaveData;
		Settings::General::s_EditorStyleData = IFile::GetSpecialAppFolder() + "CocoaEngine" + Settings::General::s_EditorStyleData;
		
		// Copy default script files to the assets path
		CPath scriptHFile = IFile::GetSpecialAppFolder() + "CocoaEngine" + "DefaultScript.h";
		CPath scriptCppFile = IFile::GetSpecialAppFolder() + "CocoaEngine" + "DefaultScript.cpp";
		IFile::CopyFile(Settings::General::s_EngineAssetsPath + "defaultCodeFiles" + "DefaultScript.h", IFile::GetSpecialAppFolder() + "CocoaEngine", "DefaultScript");
		IFile::CopyFile(Settings::General::s_EngineAssetsPath + "defaultCodeFiles" + "DefaultScript.cpp", IFile::GetSpecialAppFolder() + "CocoaEngine", "DefaultScript");

		LoadEditorData(Settings::General::s_EditorSaveData);
	}

	void EditorLayer::OnUpdate(float dt)
	{
		if (m_Scene->IsPlaying())
		{
			m_EditorUpdate = false;
		}
		else
		{
			m_EditorUpdate = true;
		}

		if (CocoaEditor::IsProjectLoaded() && !m_EditorUpdate)
		{
			DebugDraw::BeginFrame();
			m_Scene->Update(dt);
		}
		else if (CocoaEditor::IsProjectLoaded())
		{
			DebugDraw::BeginFrame();
			m_Scene->EditorUpdate(dt);
		}
	}

	void EditorLayer::OnRender()
	{
		if (CocoaEditor::IsProjectLoaded())
		{
			glDisable(GL_BLEND);
			m_PickingTexture.EnableWriting();

			glViewport(0, 0, 3840, 2160);
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			RenderSystem::BindShader(m_PickingShader);
			m_Scene->Render();

			m_PickingTexture.DisableWriting();
			glEnable(GL_BLEND);

			glBindFramebuffer(GL_FRAMEBUFFER, Application::Get()->GetFramebuffer()->GetId());

			glViewport(0, 0, 3840, 2160);
			glClearColor(0.45f, 0.55f, 0.6f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			RenderSystem::BindShader(m_DefaultShader);
			RenderSystem::UploadUniform1ui("uActiveEntityID", InspectorWindow::GetActiveEntity().GetID() + 1);
			DebugDraw::DrawBottomBatches();
			m_Scene->Render();
			DebugDraw::DrawTopBatches();
		}
	}

	void EditorLayer::OnEvent(Event& e)
	{
		if (CocoaEditor::IsProjectLoaded())
		{
			const auto& systems = m_Scene->GetSystems();

			for (const auto& system : systems)
			{
				system->OnEvent(e);
			}
		}
	}

	// ===================================================================================
	// Editor Application
	// ===================================================================================
	CocoaEditor::CocoaEditor()
		: Application()
	{
	}

	void CocoaEditor::Init()
	{
		// Initialize GLAD here, so that it works in DLL and exe
		Log::Info("Initializing GLAD functions in exe.");
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			Log::Error("Unable to initialize GLAD.");
		}

		// Engine initialization
		Cocoa::AssetManager::Init(0);
		Cocoa::IFileDialog::Init();
		Cocoa::IFile::Init();
		Cocoa::ProjectWizard::Init();
		Cocoa::Physics2D::Init();
		Cocoa::Input::Init();

		m_EditorLayer = new EditorLayer(nullptr);
		ChangeScene(new LevelEditorSceneInitializer());
		m_EditorLayer->SetScene(m_CurrentScene);

		m_ImGuiLayer = new ImGuiLayer(m_CurrentScene);
		PushOverlay(m_EditorLayer);
		PushOverlay(m_ImGuiLayer);
	}

	void CocoaEditor::Shutdown()
	{
		// Engine shutdown sequence
		Cocoa::IFileDialog::Destroy();
		Cocoa::IFile::Destroy();
	}

	void CocoaEditor::BeginFrame()
	{
		m_ImGuiLayer->BeginFrame();
	}

	void CocoaEditor::EndFrame()
	{
		m_ImGuiLayer->EndFrame();
	}

	// ===================================================================================
	// Create application entry point
	// ===================================================================================
	Application* CreateApplication()
	{
		CocoaEditor* editor = new CocoaEditor();
		return editor;
	}
}