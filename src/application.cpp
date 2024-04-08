#include "application.h"

#include <thread>

#include <FastNoise/FastNoise.h>
#include "generators/terrainGenerator.h"

// #include "glErrors.h"

// Runs the main application loop
void Application::Run()
{
	// Link VAO pointers
	m_Renderer->SetVAOs(m_Scene->m_VAOs);

	// Create Terrain
	auto generator = FastNoise::New<FastNoise::FractalFBm>();
	generator->SetSource(FastNoise::New<FastNoise::Simplex>());

	static int octaves = 12;
	static float gain = 0.5f;
	static float lacunarity = 1.5f;

	generator->SetOctaveCount(octaves);
	generator->SetGain(gain);
	generator->SetLacunarity(lacunarity);

	auto vertices = m_TerrainGenerator->GenerateVertices(generator);
	m_Scene->SetTerrain(vertices);
	auto positions = std::vector<glm::vec3>();
	for (const auto& vertex : vertices)
	{
		positions.push_back(vertex.Position);
	}
	m_Simulator->SetTerrain(positions);

	// Create Terrain Shader
	std::shared_ptr<ShaderProgram> terrainShader = m_Renderer->AddShaderProgram(
		std::vector<std::pair<GLenum, const char*>>
		{
			{ GL_VERTEX_SHADER, "src\\shaders\\vertex\\terrain.vert" },
			{ GL_FRAGMENT_SHADER, "src\\shaders\\fragment\\terrain.frag" }
		}
	);

	m_Scene->CreateDroplets(g_ParticleRadius);

	m_Simulator->SetIDs(m_Scene->m_IDs);
	m_Simulator->SetIDsToCenters(m_Scene->m_Droplets->GetIDsToCenters());

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
	m_Renderer->RegisterVAOShaderMatch(m_Scene->m_Droplets->GetVAO()->GetID(), dropletShader->GetID());

	m_Renderer->RegisterUniformBuffer(m_Scene->m_Camera->m_ViewProjMatrices);

	// Main loop
	while (WindowIsOpen())
	{
		OnFrameStart();

		m_Simulator->Step();

		m_Scene->OnUpdate();

		m_Renderer->Render();

		OnFrameEnd();
	}
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

	glClearColor(0.44f, 0.76f, 1.0f, 1.0f);

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
	m_Simulator = std::make_unique<Simulator>(100U, 100U, 100U);
	m_TerrainGenerator = std::make_unique<TerrainGenerator>();

	m_Scene->m_Camera->m_AspectRatio = static_cast<float>(m_WindowWidth) / static_cast<float>(m_WindowHeight);
	m_Scene->m_Camera->m_ViewProjMatrices.SetEmptyBuffer(2 * sizeof(glm::mat4));
	g_ActiveCamera = m_Scene->m_Camera;

	m_Simulator->SetDeltaTime(0.01f);
	g_LastFrameTime = glfwGetTime();
}

// Sets up start of new frame
void Application::OnFrameStart()
{
	double currentTime = glfwGetTime();
	ProcessInput(m_Window, static_cast<float>(currentTime - g_LastFrameTime));
	g_LastFrameTime = currentTime;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	SetImGuiWindows();
}

// Renders ImGui elements, swaps glfw buffers, and polls glfw events
void Application::OnFrameEnd()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(m_Window);
	glfwPollEvents();

	g_FrameCount++;
	if (g_FrameCount < 2) {
		ImGui::SetWindowFocus(NULL);
	}
}

void Application::SetImGuiWindows() const
{
	ImGui::SetNextWindowSizeConstraints(
		ImVec2(400.0f, 0.0f),
		ImVec2(1000.f, 1000.f));

	ImGui::Begin("Overflow", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove);
	ImGui::SetWindowPos(ImVec2(6.0f, 6.0f), ImGuiCond_FirstUseEver);

	if (ImGui::TreeNode("Rain"))
	{
		ImGui::PushItemWidth(150.0f);

		static int count = 100;

		if (ImGui::InputInt("Droplets", &count))
		{
			if (count < 0)
				count = 0;
		}

		ImGui::SameLine();

		if (ImGui::Button("Spawn"))
		{
			float baseX = -static_cast<float>(m_Simulator->GetWorldWidth()) * 0.5 + 0.5f;
			float baseZ = -static_cast<float>(m_Simulator->GetWorldLength()) * 0.5 + 0.5f;

			auto IDsAndSpheres = std::vector<std::pair<uint64_t, DynamicSphere>>(count);
			for (int i = 0; i < count; i++)
			{
				uint64_t id = Scene::GetFreshUUID();

				float x = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (m_Simulator->GetWorldWidth() - 1))) + baseX;
				float y = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (5.0f))) + 25.0f;
				float z = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (m_Simulator->GetWorldLength() - 1))) + baseZ;
				glm::vec3 center = { x, y, z };
				m_Simulator->RegisterParticle(id, { center, g_ParticleRadius });
				m_Scene->m_Droplets->AddDroplet({ id, center });
				m_Scene->m_IDs->push_back(id);
			}

			m_Scene->m_Droplets->UpdateInstanceVBO(*(m_Scene->m_IDs));
		}

		ImGui::SameLine();
		if (ImGui::Button("Clear"))
		{
			ClearScene();
		}

		ImGui::TreePop();
	}

	ImGui::Separator();

	if (ImGui::TreeNode("Terrain"))
	{
		static float size[2] = { m_TerrainGenerator->GetWidth(), m_TerrainGenerator->GetLength() };
		if (ImGui::InputFloat2("Size", size))
		{
			size[0] = std::min(size[0], 5.0f);
			size[1] = std::min(size[1], 5.0f);
			m_TerrainGenerator->SetWidth(size[0]);
			m_TerrainGenerator->SetLength(size[1]);
		}

		static int resolution[2] = { m_TerrainGenerator->GetResX(), m_TerrainGenerator->GetResZ() };
		if (ImGui::InputInt2("Mesh Detail", resolution))
		{
			resolution[0] = std::min(resolution[0], 2);
			resolution[1] = std::min(resolution[1], 2);
			m_TerrainGenerator->SetResX(resolution[0]);
			m_TerrainGenerator->SetResZ(resolution[1]);
		}

		static float baseElevation = m_TerrainGenerator->GetBaseElevation();
		if (ImGui::InputFloat("Base Elevation", &baseElevation))
		{
			m_TerrainGenerator->SetBaseElevation(baseElevation);
		}

		static float heightMul = m_TerrainGenerator->GetHeightMul();
		if (ImGui::InputFloat("Height Multiplier", &heightMul))
		{
			if (heightMul < 0.0f)
				heightMul = 0.0f;
			m_TerrainGenerator->SetHeightMul(heightMul);
		}

		ImGui::Separator();

		static int32_t seed = m_TerrainGenerator->GetSeed();
		if (ImGui::InputInt("Seed", &seed))
		{
			seed = std::clamp(seed, INT32_MIN, INT32_MAX);
			m_TerrainGenerator->SetSeed(seed);
		}

		static float freq = m_TerrainGenerator->GetFreq();
		if (ImGui::InputFloat("Frequency", &freq))
		{
			freq = std::clamp(freq, 0.0000001f, 5.0f);
			m_TerrainGenerator->SetFreq(freq);
		}

		static int octaves = 12;
		if (ImGui::InputInt("Octaves", &octaves))
		{
			octaves = std::min(1, octaves);
		}
		static float gain = 0.5f;
		ImGui::InputFloat("Gain", &gain);
		static float lacunarity = 1.5f;
		ImGui::InputFloat("Lacunarity", &lacunarity);

		ImGui::Separator();

		// Button: Reload
		if (ImGui::Button("Regenerate Terrain"))
		{
			ClearScene();

			auto generator = FastNoise::New<FastNoise::FractalFBm>();
			generator->SetSource(FastNoise::New<FastNoise::Simplex>());
			generator->SetOctaveCount(octaves);
			generator->SetGain(gain);
			generator->SetLacunarity(lacunarity);

			auto vertices = m_TerrainGenerator->GenerateVertices(generator);

			m_Scene->UpdateTerrain(vertices);
			auto positions = std::vector<glm::vec3>();
			for (const auto& vertex : vertices)
			{
				positions.push_back(vertex.Position);
			}

			m_Simulator->SetWorldWidth(static_cast<uint32_t>(floorf(m_TerrainGenerator->GetWidth())));
			m_Simulator->SetWorldLength(static_cast<uint32_t>(floorf(m_TerrainGenerator->GetLength())));
			m_Simulator->SetTerrain(positions);
		}
		ImGui::TreePop();
	}

	ImGui::Separator();

	if (ImGui::TreeNode("Rendering"))
	{
		static bool wireframe = false;
		if (ImGui::Checkbox("Wireframe", &wireframe))
		{
			glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
		}

		ImGui::TreePop();
	}

	ImGui::Separator();

	if (ImGui::TreeNode("Simulation"))
	{
		static float deltaTime = 0.01f;
		static bool isPaused = false;

		if (ImGui::DragFloat("Particle Radius", &g_ParticleRadius, 0.001f, 0.01f, 1.0f))
		{
			m_Scene->m_Droplets->UpdateVertexVBO(g_ParticleRadius);

			ClearScene();
		}

		if (ImGui::InputFloat("Delta Time", &deltaTime))
		{
			if (deltaTime < 0.0f)
				deltaTime = 0.0f;
			if (!isPaused)
				m_Simulator->SetDeltaTime(deltaTime);
		}

		ImGui::SameLine();

		if (ImGui::Checkbox("Pause", &isPaused))
		{
			m_Simulator->SetDeltaTime(isPaused ? 0.0f : deltaTime);
		}
		ImGui::TreePop();
	}

	ImGui::End();
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

void Application::Simulate()
{
	while (WindowIsOpen())
	{
		m_Simulator->Step();
	}
}

void Application::ClearScene() const {
	m_Scene->m_Droplets->ClearDroplets();
	m_Simulator->ClearParticles();
	m_Simulator->ClearParticleGrid();
	m_Scene->m_IDs->clear();
	m_Scene->m_Droplets->UpdateInstanceVBO(*(m_Scene->m_IDs));
}
