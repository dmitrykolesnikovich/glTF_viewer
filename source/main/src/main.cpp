#include "render.h"
#include "argparse.hpp"

// Main
void RenderLoop();

string output_folder;
int main(int argc, char **argv)
{
    argparse::ArgumentParser program("glTF_viewer");

    program.add_argument("-x", "--width")
        .default_value(std::string("1920"))
        .help("specify the width.");

    program.add_argument("-y", "--height")
        .default_value(std::string("1080"))
        .help("specify the height.");

    try
    {
        program.parse_args(argc, argv);
    }
    catch (const std::runtime_error &err)
    {
        std::cout << err.what() << std::endl;
        std::cout << program;
        exit(0);
    }
    int width = stoi(program.get<std::string>("-x"));
    int height = stoi(program.get<std::string>("-y"));

    WindowConfig::SetWindowsProperty(width, height, true);
    RenderLoop();
    
    return 0;
}

void RenderLoop()
{
    glm::mat4 projection = glm::perspective(WindowConfig::mainCamera->Zoom,
                                            (float)WindowConfig::SCR_WIDTH / (float)WindowConfig::SCR_HEIGHT,
                                            NEAR, FAR);

   GLTFModel girlModel("../assets/sci-fi_girl/scene.gltf");
    // GLTFModel girlModel("../assets/buster_drone/scene.gltf");
    Shader animationShader("../shaders/skin/model.vert", "../shaders/skin/model.frag");
    animationShader.Use();
    animationShader.setMat4("projection", projection);

    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR)
    {
         cout << "After" << hex << showbase << err << endl;
    }
    mat4 view, pre_view;
    static float deltaTime(0.0f);
    static float lastFrame(0.0f);
    while (!glfwWindowShouldClose(WindowConfig::window))
    {
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // Movement System
        glfwPollEvents();
        Controller::Movement(deltaTime);
        pre_view = view;
        view = WindowConfig::mainCamera->GetViewMatrix();
        // Render System
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // We're not using stencil buffer now
        animationShader.Use();
        animationShader.setMat4("view", view);
        animationShader.setMat4("pre_view", pre_view);
        girlModel.Update(animationShader, currentFrame);

        // Swap the screen buffers, Check and call events
        glfwSwapBuffers(WindowConfig::window);
    }
    // Release  
    girlModel.CleanupModel();
    animationShader.Release();
}
