#include "application.h"

#include <thread>

// #include "glErrors.h"

// Runs the main application loop
void Application::Run()
{
	// Link VAO pointers
	m_Renderer->SetVAOs(m_Scene->m_VAOs);

	// Create Terrain
	std::vector<Vertex> vertices = std::vector<Vertex>();

	vertices.emplace_back(glm::vec4(-50.0f, -3.0f,  50.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f));
	vertices.emplace_back(glm::vec4( 50.0f, -3.0f,  50.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f));
	vertices.emplace_back(glm::vec4( 50.0f, -3.0f, -50.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f));

	vertices.emplace_back(glm::vec4(-50.0f, -3.0f,  50.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f));
	vertices.emplace_back(glm::vec4( 50.0f, -3.0f, -50.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f));
	vertices.emplace_back(glm::vec4(-50.0f, -3.0f, -50.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f));

	m_Scene->SetTerrain(vertices);

	// Create Terrain Shader
	std::shared_ptr<ShaderProgram> terrainShader = m_Renderer->AddShaderProgram(
		std::vector<std::pair<GLenum, const char*>>
		{
			{ GL_VERTEX_SHADER, "src\\shaders\\vertex\\terrain.vert" },
			{ GL_FRAGMENT_SHADER, "src\\shaders\\fragment\\terrain.frag" }
		}
	);

	// Create Droplets
	m_Scene->CreateDroplets(10, std::vector<glm::vec3>{ glm::vec3(0.0f, 3.0f, -3.0f), glm::vec3(0.0f, 5.0f, -3.0f) });

	// Create Droplets shader
	std::shared_ptr<ShaderProgram> dropletShader = m_Renderer->AddShaderProgram(
		std::vector<std::pair<GLenum, const char*>>
		{
			{ GL_VERTEX_SHADER, "src\\shaders\\vertex\\droplets.vert" },
			{ GL_FRAGMENT_SHADER, "src\\shaders\\fragment\\droplets.frag" }
		}
	);

	// Register Shader ID with VAO ID
	m_Renderer->RegisterVAOShaderMatch(m_Scene->m_Terrain->m_VAO->GetID(), terrainShader->GetID());
	m_Renderer->RegisterVAOShaderMatch(m_Scene->m_Droplets->m_VAO->GetID(), dropletShader->GetID());

	m_Renderer->RegisterUniformBuffer(m_Scene->m_Camera->m_ViewProjMatrices);

	// Main loop
	std::thread renderThread(Render);
	std::thread phyisicsThread(Simulate);

	renderThread.join();
	phyisicsThread.join();
}

// Initializes GLFW, glad, and ImGui
void Application::Init()
{
	// Initialize GLFW to use OpenGL 3.3
	// ---------------------------------
	glfwInit();
#ifdef OPENGL_DEBUGGING
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
#else // OPENGL_BEBUGGING
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#endif // OPENGL_BEBUGGING
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create Window
	// -------------
	m_Window = glfwCreateWindow(m_WindowWidth, m_WindowHeight, "Overflow Shallow Water Simulation", nullptr, nullptr);
	if (!m_Window)
	{
		CleanUp();
		PrintAndThrowException(GLFWWindowCreationException());
	}

	glfwMakeContextCurrent(m_Window);

	// Register window resizing callback
	glfwSetFramebufferSizeCallback(m_Window, FramebufferSizeCallback);

	// Register mouse callback
	glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(m_Window, MouseCallback);

	// Register scroll callback
	glfwSetScrollCallback(m_Window, ScrollCallback);

	// Initialize GLAD
	// ---------------
	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
	{
		CleanUp();
		PrintAndThrowException(GLADLoaderException());
	}

	// Set OpenGL flags and variables
#ifdef OPENGL_DEBUGGING
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(MessageCallback, nullptr);
#endif // OPENGL_DEBUGGING

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	// Initialize ImGui
	// ----------------
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	m_Renderer = std::make_unique<Renderer>();
	m_Scene = std::make_unique<Scene>();

	m_Scene->m_Camera->m_AspectRatio = static_cast<float>(m_WindowWidth) / static_cast<float>(m_WindowHeight);
	m_Scene->m_Camera->m_ViewProjMatrices.SetEmptyBuffer(2 * sizeof(glm::mat4));
	g_ActiveCamera = m_Scene->m_Camera;
}

// Sets up start of new frame
void Application::OnFrameStart()
{
	processInput(m_Window);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

// Renders ImGui elements, swaps glfw buffers, and polls glfw events
void Application::OnFrameEnd()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(m_Window);
	glfwPollEvents();
}

void Application::Render()
{
	while (WindowIsOpen())
	{
		OnFrameStart();

		m_Scene->OnUpdate();

		m_Renderer->Render();

		OnFrameEnd();
	}
}
