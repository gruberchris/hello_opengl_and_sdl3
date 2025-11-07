#include <SDL3/SDL.h>
#include <iostream>
#include <cmath>
#include <vector>

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

constexpr int WINDOW_WIDTH = 1024;
constexpr int WINDOW_HEIGHT = 768;

auto vertexShaderSource = R"(
#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 ourColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    ourColor = aColor;
}
)";

auto fragmentShaderSource = R"(
#version 410 core
in vec3 ourColor;
out vec4 FragColor;

void main()
{
    FragColor = vec4(ourColor, 1.0);
}
)";

struct Camera {
    float distance = 5.0f;
    float targetDistance = 5.0f;
    const float minDistance = 2.0f;
    const float maxDistance = 15.0f;
    const float zoomSpeed = 3.0f;
    const float smoothing = 8.0f;
    
    void zoomIn() {
        targetDistance = std::max(minDistance, targetDistance - zoomSpeed);
    }
    
    void zoomOut() {
        targetDistance = std::min(maxDistance, targetDistance + zoomSpeed);
    }
    
    void update(float deltaTime) {
        distance += (targetDistance - distance) * smoothing * deltaTime;
    }
};

struct Cube {
    float rotation = 0.0f;
    bool rotating = false;
    const float rotationSpeed = 50.0f;
    
    void update(const float deltaTime) {
        if (rotating) {
            rotation += rotationSpeed * deltaTime;
            if (rotation >= 360.0f) rotation -= 360.0f;
        }
    }
};

void multiplyMatrix(float* result, const float* a, const float* b) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result[i * 4 + j] = 0;
            for (int k = 0; k < 4; k++) {
                result[i * 4 + j] += a[i * 4 + k] * b[k * 4 + j];
            }
        }
    }
}

GLuint compileShader(const GLenum type, const char* source) {
    const GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compilation failed: " << infoLog << std::endl;
    }
    return shader;
}

GLuint createShaderProgram() {
    const GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    const GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    const GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "Program linking failed: " << infoLog << std::endl;
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return program;
}

void setupCubeData(GLuint& VAO, GLuint& VBO) {
    constexpr float vertices[] = {
        // Positions // Colors (gradients)
        // Front face - Red to Yellow
        -1.0f, -1.0f,  1.0f,  1.0f, 0.0f, 0.0f,
         1.0f, -1.0f,  1.0f,  1.0f, 1.0f, 0.0f,
         1.0f,  1.0f,  1.0f,  1.0f, 1.0f, 0.0f,
         1.0f,  1.0f,  1.0f,  1.0f, 1.0f, 0.0f,
        -1.0f,  1.0f,  1.0f,  1.0f, 0.0f, 0.0f,
        -1.0f, -1.0f,  1.0f,  1.0f, 0.0f, 0.0f,
        
        // Back face - Blue to Cyan
        -1.0f, -1.0f, -1.0f,  0.0f, 0.0f, 1.0f,
        -1.0f,  1.0f, -1.0f,  0.0f, 0.0f, 1.0f,
         1.0f,  1.0f, -1.0f,  0.0f, 1.0f, 1.0f,
         1.0f,  1.0f, -1.0f,  0.0f, 1.0f, 1.0f,
         1.0f, -1.0f, -1.0f,  0.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,  0.0f, 0.0f, 1.0f,
        
        // Top face - Green to Yellow
        -1.0f,  1.0f, -1.0f,  0.0f, 1.0f, 0.0f,
        -1.0f,  1.0f,  1.0f,  0.0f, 1.0f, 0.0f,
         1.0f,  1.0f,  1.0f,  1.0f, 1.0f, 0.0f,
         1.0f,  1.0f,  1.0f,  1.0f, 1.0f, 0.0f,
         1.0f,  1.0f, -1.0f,  1.0f, 1.0f, 0.0f,
        -1.0f,  1.0f, -1.0f,  0.0f, 1.0f, 0.0f,
        
        // Bottom face - Magenta to Purple
        -1.0f, -1.0f, -1.0f,  1.0f, 0.0f, 1.0f,
         1.0f, -1.0f, -1.0f,  0.5f, 0.0f, 0.5f,
         1.0f, -1.0f,  1.0f,  0.5f, 0.0f, 0.5f,
         1.0f, -1.0f,  1.0f,  0.5f, 0.0f, 0.5f,
        -1.0f, -1.0f,  1.0f,  1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,  1.0f, 0.0f, 1.0f,
        
        // Right face - Orange to Pink
         1.0f, -1.0f, -1.0f,  1.0f, 0.5f, 0.0f,
         1.0f,  1.0f, -1.0f,  1.0f, 0.5f, 0.0f,
         1.0f,  1.0f,  1.0f,  1.0f, 0.4f, 0.7f,
         1.0f,  1.0f,  1.0f,  1.0f, 0.4f, 0.7f,
         1.0f, -1.0f,  1.0f,  1.0f, 0.4f, 0.7f,
         1.0f, -1.0f, -1.0f,  1.0f, 0.5f, 0.0f,
        
        // Left face - Teal to Lime
        -1.0f, -1.0f, -1.0f,  0.0f, 0.5f, 0.5f,
        -1.0f, -1.0f,  1.0f,  0.0f, 0.5f, 0.5f,
        -1.0f,  1.0f,  1.0f,  0.5f, 1.0f, 0.0f,
        -1.0f,  1.0f,  1.0f,  0.5f, 1.0f, 0.0f,
        -1.0f,  1.0f, -1.0f,  0.5f, 1.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,  0.0f, 0.5f, 0.5f,
    };
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

int main(int argc, char* argv[]) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return 1;
    }
    
    // Set OpenGL attributes
#ifdef __APPLE__
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
#else
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#endif
    
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    
    SDL_Window* window = SDL_CreateWindow(
        "OpenGL 3D Cube - SDL3",
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );
    
    if (!window) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        std::cerr << "SDL_GL_CreateContext failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    
    SDL_GL_SetSwapInterval(1);
    
#ifndef __APPLE__
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return 1;
    }
#endif
    
    // Print version information
    const int sdlVersion = SDL_GetVersion();
    const int sdlMajor = SDL_VERSIONNUM_MAJOR(sdlVersion);
    const int sdlMinor = SDL_VERSIONNUM_MINOR(sdlVersion);
    const int sdlPatch = SDL_VERSIONNUM_MICRO(sdlVersion);
    std::cout << "SDL Version: " << sdlMajor << "." << sdlMinor << "." << sdlPatch << std::endl;
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "OpenGL Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  R - Start rotation" << std::endl;
    std::cout << "  T - Stop rotation" << std::endl;
    std::cout << "  + - Zoom in" << std::endl;
    std::cout << "  - - Zoom out" << std::endl;
    std::cout << "  ESC - Exit" << std::endl;
    
    // Setup OpenGL
    glEnable(GL_DEPTH_TEST);

    const GLuint shaderProgram = createShaderProgram();
    GLuint VAO, VBO;
    setupCubeData(VAO, VBO);
    
    Camera camera;
    Cube cube;
    bool running = true;
    Uint64 lastTime = SDL_GetTicks();
    
    int windowWidth = WINDOW_WIDTH;
    int windowHeight = WINDOW_HEIGHT;
    
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            } else if (event.type == SDL_EVENT_KEY_DOWN) {
                switch (event.key.key) {
                    case SDLK_ESCAPE:
                        running = false;
                        break;
                    case SDLK_R:
                        cube.rotating = true;
                        break;
                    case SDLK_T:
                        cube.rotating = false;
                        break;
                    case SDLK_EQUALS:
                    case SDLK_PLUS:
                    case SDLK_KP_PLUS:
                        camera.zoomIn();
                        break;
                    case SDLK_MINUS:
                    case SDLK_KP_MINUS:
                        camera.zoomOut();
                        break;
                }
            } else if (event.type == SDL_EVENT_WINDOW_RESIZED) {
                SDL_GetWindowSize(window, &windowWidth, &windowHeight);
                glViewport(0, 0, windowWidth, windowHeight);
            }
        }
        
        Uint64 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;
        
        camera.update(deltaTime);
        cube.update(deltaTime);
        
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glUseProgram(shaderProgram);
        
        // Model matrix (rotation)
        const float angleRad = cube.rotation * M_PI / 180.0f;
        const float c = cos(angleRad);
        const float s = sin(angleRad);
        float axis[] = {0.5f, 1.0f, 0.0f};
        const float len = sqrt(axis[0]*axis[0] + axis[1]*axis[1] + axis[2]*axis[2]);
        axis[0] /= len; axis[1] /= len; axis[2] /= len;

        const float model[16] = {
            axis[0]*axis[0]*(1-c) + c,           axis[0]*axis[1]*(1-c) - axis[2]*s, axis[0]*axis[2]*(1-c) + axis[1]*s, 0,
            axis[1]*axis[0]*(1-c) + axis[2]*s,   axis[1]*axis[1]*(1-c) + c,         axis[1]*axis[2]*(1-c) - axis[0]*s, 0,
            axis[2]*axis[0]*(1-c) - axis[1]*s,   axis[2]*axis[1]*(1-c) + axis[0]*s, axis[2]*axis[2]*(1-c) + c,         0,
            0, 0, 0, 1
        };
        
        // View matrix (camera)
        const float view[16] = {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, -camera.distance, 1
        };
        
        // Projection matrix
        const float aspect = (float)windowWidth / (float)windowHeight;
        constexpr float fov = 45.0f * M_PI / 180.0f;
        constexpr float near = 0.1f;
        constexpr float far = 100.0f;
        const float f = 1.0f / tan(fov / 2.0f);

        const float projection[16] = {
            f / aspect, 0, 0, 0,
            0, f, 0, 0,
            0, 0, (far + near) / (near - far), -1,
            0, 0, (2.0f * far * near) / (near - far), 0
        };

        const GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
        const GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
        const GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, view);
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, projection);
        
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        SDL_GL_SwapWindow(window);
    }
    
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    
    SDL_GL_DestroyContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}