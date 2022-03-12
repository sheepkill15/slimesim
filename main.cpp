#include <iostream>
#include <fstream>
#include <streambuf>
#include <string>
#include <vector>
#include <cmath>
#include <ctime>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "components/constants.h"
#include "components/window.h"
#include "components/texture.h"
#include "components/renderer.h"
#include "components/ubo.h"
#include "components/ssbo.h"

#include "components/compute_shader.h"
#include "components/vert_frag_shader.h"
#define M_PI 3.14159265358979323846
struct Agent
{
    float pos_x = 0;
    float pos_y = 0;
    float angle = 0;
};

Window simWindow;
Texture texture;

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
    simWindow.on_resize({(unsigned int)width, (unsigned int)height});

    texture.resize(width, height);
}

void imgui_setup()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(simWindow.get_glfwwindow(), true);
    ImGui_ImplOpenGL3_Init("#version 130");
}

void imgui_newframe()
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

struct SimSettings
{
    int offsetAngle = 30;
    int sensorDistance = 50;
    int sensorSize = 3;
    float turnSpeed = 0.17f;
    int moveSpeed = 100.f;
} SimulationSettings;

struct ImgSettings
{
    float diffuseRate = 10;
    float evaporationRate = 0.9f;
} ImageSettings;

void load_save()
{
    ifstream save_file("save.txt");
    if (save_file.fail())
        return;
    save_file >> SimulationSettings.offsetAngle;
    save_file >> SimulationSettings.sensorDistance;
    save_file >> SimulationSettings.sensorSize;
    save_file >> SimulationSettings.turnSpeed;
    save_file >> SimulationSettings.moveSpeed;

    save_file >> ImageSettings.diffuseRate;
    save_file >> ImageSettings.evaporationRate;
    save_file.close();
}

static int stepsPerFrame = 1;
void imgui_render()
{
    ImGui::Begin("Simulation settings"); // Create a window called "Hello, world!" and append into it.

    ImGui::SliderInt("Offset angle", &SimulationSettings.offsetAngle, 0, 180);       // Edit 1 float using a slider
    ImGui::SliderInt("Sensor distance", &SimulationSettings.sensorDistance, 0, 100); // Edit 1 float using a slider
    ImGui::SliderInt("Sensor size", &SimulationSettings.sensorSize, 0, 25);
    ImGui::SliderFloat("Turn speed", &SimulationSettings.turnSpeed, 0, 5);
    ImGui::SliderInt("Move speed", &SimulationSettings.moveSpeed, 0, 100);
    ImGui::Spacing();
    ImGui::SliderInt("Steps per frame", &stepsPerFrame, 1, 10);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();

    ImGui::Begin("Image settings");

    ImGui::SliderFloat("Diffuse rate", &ImageSettings.diffuseRate, 0, 100);
    ImGui::SliderFloat("Evaporation rate", &ImageSettings.evaporationRate, 0, 2.f);

    ImGui::End();

    ImGui::Begin("Utils");

    if (ImGui::Button("Save settings"))
    {
        ofstream save_file("save.txt");
        save_file << SimulationSettings.offsetAngle << endl;
        save_file << SimulationSettings.sensorDistance << endl;
        save_file << SimulationSettings.sensorSize << endl;
        save_file << SimulationSettings.turnSpeed << endl;
        save_file << SimulationSettings.moveSpeed << endl;

        save_file << ImageSettings.diffuseRate << endl;
        save_file << ImageSettings.evaporationRate << endl;
        save_file.close();
    }
    if (ImGui::Button("Load settings"))
    {
        load_save();
    }
    ImGui::End();
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    simWindow.create();
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }
    simWindow.on_resize(simWindow.get_size());
    glfwSetFramebufferSizeCallback(simWindow.get_glfwwindow(), framebuffer_size_callback);

    VertFragShader vf("shaders/vertex.vert", "shaders/frag.frag");
    glUseProgram(vf.program);

    Renderer::init();

    ComputeShader agentShader("shaders/compute.comp");
    ComputeShader imageShader("shaders/image.comp");
    ComputeShader drawAgentsShader("shaders/draw.comp");
    srand(time(0));
    Agent *agents = new Agent[Constants::population];
    float centerX = simWindow.get_size().x / 2.f;
    float centerY = simWindow.get_size().y / 2.f;
    int radius = 100;
    for (int i = 0; i < Constants::population; i++)
    {
        float t = 2 * M_PI * rand() / (float)RAND_MAX;
        float u = rand() / (float)RAND_MAX + rand() / (float)RAND_MAX;
        float r = u > 1 ? 2 - u : u;

        float x = cos(t) * r;
        float y = sin(t) * r;

        agents[i].pos_x = centerX + x * simWindow.get_size().x / 2.f;
        agents[i].pos_y = centerY + y * simWindow.get_size().y / 2.f;
        float w = sqrtf((centerX - agents[i].pos_x) * (centerX - agents[i].pos_x) + (centerY - agents[i].pos_y) * (centerY - agents[i].pos_y));
        agents[i].angle = atan2f((centerY - agents[i].pos_y) / w, (centerX - agents[i].pos_x) / w);

    }

    Ssbo ssboAgentBlock(12 * Constants::population, agents);
    ssboAgentBlock.bind(1);

    agentShader.bind_ubo("Agents", 1);
    drawAgentsShader.bind_ubo("Agents", 1);
    agentShader.bind_ubo("WindowSize", 2);
    imageShader.bind_ubo("WindowSize", 2);

    agentShader.bind_ubo("SimulationSettings", 3);
    imageShader.bind_ubo("ImageSettings", 3);

    double previousTime = glfwGetTime();
    GLuint deltaTimeLocation = glGetUniformLocation(agentShader.program, "deltaTime");
    GLuint deltaTimeLocationImage = glGetUniformLocation(imageShader.program, "deltaTime");
    GLuint elapsedTimeLocation = glGetUniformLocation(agentShader.program, "elapsedTime");

    delete[] agents;

    texture.create(simWindow.get_size().x, simWindow.get_size().y);
    texture.bind_as_image();
    imgui_setup();

    load_save();

    while (simWindow.running())
    {
        imgui_newframe();
        imgui_render();
        double currentTime = glfwGetTime();
        double deltaTime = currentTime - previousTime;
        previousTime = currentTime;

        Ubo uboWindowSize(8, (void *)&simWindow.get_size());
        Ubo imgSettings(sizeof(ImgSettings), &ImageSettings);
        Ubo simSettings(sizeof(SimSettings), &SimulationSettings);
        uboWindowSize.bind(2);
        for (int i = 0; i < stepsPerFrame; i++)
        {
            {
                imgSettings.bind(3);
                imageShader.set_uniform(deltaTimeLocationImage, deltaTime);
                imageShader.dispatch(simWindow.get_size().x / 8 + 1, simWindow.get_size().y / 8 + 1, 1);
            }
            {
                simSettings.bind(3);
                agentShader.set_uniform(deltaTimeLocation, deltaTime);
                agentShader.set_uniform(elapsedTimeLocation, currentTime);
                agentShader.dispatch(Constants::population / 64 + 1, 1, 1);
            }
            {
                drawAgentsShader.dispatch(Constants::population / 64 + 1, 1, 1);
            }
        }
        ImGui::Render();
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(vf.program);
        Renderer::render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        GLFWwindow *backup_current_context = glfwGetCurrentContext();
        #if defined(_WIN32)
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        #endif

        glfwMakeContextCurrent(backup_current_context);
        glfwSwapBuffers(simWindow.get_glfwwindow());
        glfwPollEvents();
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(simWindow.get_glfwwindow());
    glfwTerminate();

    return 0;
}
