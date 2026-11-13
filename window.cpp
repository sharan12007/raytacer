
#include <iostream>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "renderer.h"
#include "camera.h"
#include "vec3.h"
#include <string>
// ==========================================
// Camera and mouse settings
// ==========================================

float fpsTimer = 0.0f;
int frameCount = 0;

camera cam(16.0 / 9.0, point3(0, 0, 0));

double lastX = 640.0;
double lastY = 360.0;

float speed = 3.0f;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool firstMouse = true;
bool cameraIsChanged = false;

double sensitivity = 0.1;

// ==========================================
// Mouse callback
// ==========================================

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
        return;
    }

    double xoffset = xpos - lastX;
    double yoffset = ypos-lastY;

    lastX = xpos;
    lastY = ypos;

    if (xoffset == 0.0 && yoffset == 0.0)
        return;

    cam.update_rotate(
        xoffset * sensitivity,
        -yoffset * sensitivity
    );

    cameraIsChanged = true;
}

// ==========================================
// Vertex shader
// ==========================================

const char* vertexShaderSource = R"(
#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

void main()
{
    gl_Position = vec4(aPos, 0.0, 1.0);
    TexCoord = aTexCoord;
}
)";

// ==========================================
// Fragment shader
// ==========================================

const char* fragmentShaderSource = R"(
#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D screenTexture;

void main()
{
    FragColor = texture(screenTexture, TexCoord);
}
)";

// ==========================================
// Main
// ==========================================

int main()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(
        1280, 720,
        "Ray Tracer",
        nullptr,
        nullptr
    );

    if (!window)
    {
        std::cerr << "Failed to create window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    glfwSetCursorPosCallback(window, mouse_callback);

    glfwSetInputMode(
        window,
        GLFW_CURSOR,
        GLFW_CURSOR_DISABLED
    );

    // ==========================================
    // Image settings
    // ==========================================

    const int IMAGE_WIDTH = 400;
    const int IMAGE_HEIGHT = 225;

    std::cout << "Rendering initial image in the background...\n";

    std::vector<unsigned char> pixels(
        static_cast<size_t>(IMAGE_WIDTH) * IMAGE_HEIGHT * 3, 0);

    // Display a sky-colored placeholder until the first background render completes.
    for (int y = 0; y < IMAGE_HEIGHT; ++y) {
        const double blend = static_cast<double>(y) / (IMAGE_HEIGHT - 1);
        const unsigned char red = static_cast<unsigned char>(255.0 - 128.0 * blend);
        const unsigned char green = static_cast<unsigned char>(255.0 - 76.0 * blend);
        const unsigned char blue = static_cast<unsigned char>(255.0 - 0.0 * blend);

        for (int x = 0; x < IMAGE_WIDTH; ++x) {
            const size_t offset = (static_cast<size_t>(y) * IMAGE_WIDTH + x) * 3;
            pixels[offset] = red;
            pixels[offset + 1] = green;
            pixels[offset + 2] = blue;
        }
    }

    // ==========================================
    // Create OpenGL texture
    // ==========================================

    GLuint texture;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        IMAGE_WIDTH,
        IMAGE_HEIGHT,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        pixels.data()
    );

    std::mutex renderMutex;
    std::condition_variable renderCondition;
    camera requestedCamera = cam;
    std::vector<unsigned char> completedPixels;
    bool renderRequested = true;
    bool renderResultReady = false;
    bool stopRenderer = false;

    std::thread renderThread([&]() {
        while (true) {
            {
                std::unique_lock lock(renderMutex);
                renderCondition.wait(lock, [&]() {
                    return renderRequested || stopRenderer;
                });

                if (stopRenderer && !renderRequested)
                    return;

                camera cameraSnapshot = requestedCamera;
                renderRequested = false;

                lock.unlock();

                auto renderedPixels = render_image(
                    IMAGE_WIDTH,
                    IMAGE_HEIGHT,
                    cameraSnapshot);

                lock.lock();
                completedPixels = std::move(renderedPixels);
                renderResultReady = true;
            }
        }
    });

    // ==========================================
    // Compile shaders
    // ==========================================

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // ==========================================
    // Fullscreen rectangle
    // ==========================================

    float vertices[] = {
        -1.0f,  1.0f,      0.0f, 1.0f,
        -1.0f, -1.0f,      0.0f, 0.0f,
         1.0f, -1.0f,      1.0f, 0.0f,

        -1.0f,  1.0f,      0.0f, 1.0f,
         1.0f, -1.0f,      1.0f, 0.0f,
         1.0f,  1.0f,      1.0f, 1.0f
    };

    GLuint VAO, VBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(
        0, 2, GL_FLOAT, GL_FALSE,
        4 * sizeof(float),
        (void*)0
    );

    glEnableVertexAttribArray(0);

    glVertexAttribPointer(
        1, 2, GL_FLOAT, GL_FALSE,
        4 * sizeof(float),
        (void*)(2 * sizeof(float))
    );

    glEnableVertexAttribArray(1);

    // ==========================================
    // Main loop
    // ==========================================

    while (!glfwWindowShouldClose(window))
    {
        // Calculate delta time
        float currentFrame = static_cast<float>(glfwGetTime());

        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        frameCount++;
        fpsTimer += deltaTime;

        if (fpsTimer >= 1.0f)
        {
            std::string title = "Ray Tracer | FPS: " + std::to_string(frameCount);

            glfwSetWindowTitle(window, title.c_str());

            frameCount = 0;
            fpsTimer = 0.0f;
        }

        glfwPollEvents();

        // --------------------------------------
        // Forward / backward
        // --------------------------------------

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            cam.position += cam.forward *static_cast<double>(speed * deltaTime);
            cameraIsChanged = true;
        }

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            cam.position -= cam.forward * static_cast<double>(speed * deltaTime);
            cameraIsChanged = true;
        }

        // --------------------------------------
        // Left / right
        // --------------------------------------

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            cam.position -= cam.right * static_cast<double>(speed * deltaTime);
            cameraIsChanged = true;
        }

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            cam.position += cam.right * static_cast<double>(speed * deltaTime);
            cameraIsChanged = true;
        }

        // --------------------------------------
        // Escape
        // --------------------------------------

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }

        // --------------------------------------
        // Re-render when camera changes
        // --------------------------------------

        if (cameraIsChanged)
        {
            {
                std::lock_guard lock(renderMutex);
                requestedCamera = cam;
                renderRequested = true;
            }
            renderCondition.notify_one();

            cameraIsChanged = false;
        }

        // Upload completed renders only on the OpenGL thread.
        {
            std::lock_guard lock(renderMutex);
            if (renderResultReady) {
                pixels = std::move(completedPixels);
                renderResultReady = false;

                glBindTexture(GL_TEXTURE_2D, texture);
                glTexSubImage2D(
                    GL_TEXTURE_2D,
                    0,
                    0, 0,
                    IMAGE_WIDTH,
                    IMAGE_HEIGHT,
                    GL_RGB,
                    GL_UNSIGNED_BYTE,
                    pixels.data()
                );
            }
        }

        // --------------------------------------
        // Display texture
        // --------------------------------------

        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        glUniform1i(
            glGetUniformLocation(shaderProgram, "screenTexture"),
            0
        );

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
    }

    {
        std::lock_guard lock(renderMutex);
        stopRenderer = true;
        renderRequested = false;
    }
    renderCondition.notify_one();
    renderThread.join();

    // ==========================================
    // Cleanup
    // ==========================================

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteTextures(1, &texture);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
