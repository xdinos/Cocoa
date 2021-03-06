#include "externalLibs.h"
#include "cocoa/core/Core.h"

#include "cocoa/core/Application.h"
#include "cocoa/renderer/DebugDraw.h"
#include "cocoa/core/Entity.h"

namespace Cocoa
{
	Application::Application()
	{
		m_LayerInsertIndex = 0;
		m_Running = true;
		m_Layers = std::vector<Layer*>();

		std::string title = std::string("Test Window");
		Log::Info("Initializing GLAD functions in DLL.");
		m_Window = CWindow::Create(1920, 1080, title);
		s_Instance = this;

		m_Framebuffer = new Framebuffer(3840, 2160);

		m_Window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));
	}

	Application::~Application()
	{

	}

	void Application::Run()
	{
		for (Layer* layer : m_Layers)
		{
			layer->OnAttach();
		}

		while (m_Running)
		{
			float time = (float)glfwGetTime();
			float dt = time - m_LastFrameTime;
			m_LastFrameTime = time;

			BeginFrame();
			for (Layer* layer : m_Layers)
			{
				layer->OnUpdate(dt);
				layer->OnRender();
			}
			EndFrame();

			m_Window->OnUpdate();
			m_Window->Render();
		}

		for (Layer* layer : m_Layers)
		{
			layer->OnDetach();
		}

		m_Window->Destroy();
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(std::bind(&Application::OnWindowClose, this, std::placeholders::_1));

		for (auto it = m_Layers.end(); it != m_Layers.begin();)
		{
			(*--it)->OnEvent(e);
			if (e.Handled())
			{
				break;
			}
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
		m_LayerInsertIndex++;
	}

	void Application::PushOverlay(Layer* overlay)
	{
		m_Layers.emplace_back(overlay);
	}

	void Application::PopLayer(Layer* layer)
	{
		auto it = std::find(m_Layers.begin(), m_Layers.end(), layer);
		if (it != m_Layers.end())
		{
			m_Layers.erase(it);
			m_LayerInsertIndex--;
		}
	}

	void Application::PopOverlay(Layer* overlay)
	{
		auto it = std::find(m_Layers.begin(), m_Layers.end(), overlay);
		if (it != m_Layers.end())
		{
			m_Layers.erase(it);
		}
	}

	Framebuffer* Application::GetFramebuffer() const
	{
		return m_Framebuffer;
	}

	void Application::ChangeScene(SceneInitializer* sceneInitializer)
	{
		if (m_CurrentScene != nullptr)
		{
			delete m_CurrentScene;
		}

		this->m_CurrentScene = new Scene(sceneInitializer);
		this->m_CurrentScene->Init();
		this->m_CurrentScene->Start();
		
		Entity::SetScene(this->m_CurrentScene);
		DebugDraw::Init(this->m_CurrentScene);
	}

	void Application::Stop()
	{
		m_Running = false;
	}

	CWindow* Application::GetWindow() const
	{
		return m_Window;
	}

	Application* Application::s_Instance = nullptr;
	Application* Application::Get()
	{
		if (!s_Instance)
		{
			Log::Error("Cannot get application. It is nullptr.");
		}
		return s_Instance;
	}
}