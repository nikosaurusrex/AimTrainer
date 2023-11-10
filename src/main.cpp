#include <stdio.h>

#include "common.h"

#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>

#include "camera.h"
#include "gfx.h"
#include "shader.h"

struct Window {
	GLFWwindow *handle;
    s32 width;
    s32 height;
};

struct AimTrainer {
    Window window;
    Camera *camera;
    
    f64 last_mouse_x;
    f64 last_mouse_y;

    f64 sensitivity;
    f64 m_yaw;
    f64 m_pitch;
};

const f32 SKYBOX_SIZE = 200.0f;
f32 skybox_vertices [] = {
	-SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
	SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
	SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE,
	SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE,
	-SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE,
	-SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
	-SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE,
	SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE,
	SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,
	SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,
	-SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,
	-SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE,
	-SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,
	-SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE,
	-SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
	-SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
	-SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE,
	-SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,
	SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,
	SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE,
	SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
	SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
	SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE,
	SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,
	-SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
	SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
	SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE,
	SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE,
	-SKYBOX_SIZE, -SKYBOX_SIZE,  SKYBOX_SIZE,
	-SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE,
	-SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE,
	SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE,
	SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,
	SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,
	-SKYBOX_SIZE,  SKYBOX_SIZE,  SKYBOX_SIZE,
	-SKYBOX_SIZE,  SKYBOX_SIZE, -SKYBOX_SIZE,
};

void LogInfo(const char *format, ...) {
    va_list args;
    va_start(args, format);

    vfprintf(stdout, format, args);
    putc('\n', stdout);
    va_end(args);
}

void LogError(const char *format, ...) {
    va_list args;
    va_start(args, format);

    vfprintf(stdout, format, args);
    putc('\n', stdout);
    va_end(args);
}
 
void LogFatal(const char *format, ...) {
    va_list args;
    va_start(args, format);

    vfprintf(stdout, format, args);
    putc('\n', stdout);
    va_end(args);

    exit(EXIT_FAILURE);
}

char *ReadEntireFile(const char *file_path) {
    FILE *file = fopen(file_path, "rb");
    if (!file) return 0;

    fseek(file, 0, SEEK_END);
    auto length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *data = new char[length + 1];

    fread(data, 1, length, file);
    data[length] = 0;

    fclose(file);

    return data;
}

void WindowResizeCallback(GLFWwindow *handle, s32 width, s32 height) {
    AimTrainer *trainer = (AimTrainer *) glfwGetWindowUserPointer(handle);
    if (!trainer) {
        return;
    }

    Window *window = &trainer->window;

    window->width = width;
    window->height = height;

    glViewport(0, 0, window->width, window->height);
}

void InitWindow(Window *window, AimTrainer *trainer) {
    int w, h;
    
    glfwSetWindowUserPointer(window->handle, trainer);

    glfwGetWindowSize(window->handle, &w, &h);
    window->width = w;
    window->height = h;

    glViewport(0, 0, w, h);
}

void MouseButtonCallback(GLFWwindow *handle, s32 button, s32 action, s32 mods) {

}

void CursorPositionCallback(GLFWwindow *handle, f64 xpos, f64 ypos) {
    AimTrainer *trainer = (AimTrainer *) glfwGetWindowUserPointer(handle);
    if (!trainer) {
        return;
    }

    f64 dx = xpos - trainer->last_mouse_x;
    f64 dy = ypos - trainer->last_mouse_y;

    f64 *yaw = &trainer->camera->yaw;
    f64 *pitch = &trainer->camera->pitch;

    *yaw += dx * trainer->m_yaw * trainer->sensitivity;
    *pitch += dy * trainer->m_pitch * trainer->sensitivity;

    if (*pitch > 89.0) *pitch = 89.0;
    if (*pitch < -89.0) *pitch = -89.0;

    trainer->last_mouse_x = xpos;
    trainer->last_mouse_y = ypos;
}

void KeyCallback(GLFWwindow *handle, s32 key, s32 scancode, s32 action, s32 mods) {
    if (action == GLFW_RELEASE) {
        if (key == GLFW_KEY_Q) {
            glfwTerminate();
            exit(EXIT_SUCCESS);
        }
    }
}

void InitGLFW() {
    if (!glfwInit()) {
        LogFatal("Failed to initialize GLFW!");
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
}

Window CreateWindow(const char *title, int width, int height) {
    Window window = {};

	window.handle = glfwCreateWindow(width, height, title, 0, 0);
    if (!window.handle) {
        LogFatal("Failed to create GLFW window!");
    }

    const GLFWvidmode *vid_mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    glfwSetWindowPos(window.handle, vid_mode->width / 2 - width / 2, vid_mode->height / 2 - height / 2);
    glfwMakeContextCurrent(window.handle);

	glfwSwapInterval(1);

    glewInit();

    LogInfo("OpenGL Info:");
    LogInfo("  Vendor: %s", glGetString(GL_VENDOR));
    LogInfo("  Renderer: %s", glGetString(GL_RENDERER));
    LogInfo("  Version: %s", glGetString(GL_VERSION));
	LogInfo("  Shading Language %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

	LogInfo("GLFW Info:");
	LogInfo("  Version %s", glfwGetVersionString());

    bool raw_mouse_input = glfwRawMouseMotionSupported();
    if (raw_mouse_input) {
        glfwSetInputMode(window.handle, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        LogInfo("Raw Input Enabled");
    }
    
    glfwSetWindowSizeCallback(window.handle, WindowResizeCallback);
    glfwSetMouseButtonCallback(window.handle, MouseButtonCallback);
    glfwSetCursorPosCallback(window.handle, CursorPositionCallback);
    glfwSetKeyCallback(window.handle, KeyCallback);

    glfwShowWindow(window.handle);

    return window;
}

void UpdateWindow(Window *window) {
    glfwPollEvents();
    glfwSwapBuffers(window->handle);
}

bool WindowShouldClose(Window *window) {
    return !glfwWindowShouldClose(window->handle);
}

void DestroyWindow(Window *window) {
    glfwDestroyWindow(window->handle);
    glfwTerminate();
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

glm::mat4 TempModel(glm::vec3 trans, glm::vec3 rot, glm::vec3 scale) {
    return glm::translate(glm::mat4(1.0f), trans) * 
                glm::toMat4(glm::quat(glm::radians(rot))) *
                glm::scale(glm::mat4(1.0f), scale);
}

int main() {  
	AimTrainer trainer = {};

    InitGLFW();
    Window window = CreateWindow("AimTrainer", 1280, 720);

    InitWindow(&window, &trainer);

    glfwSetInputMode(window.handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    const char *skybox_faces[6] = {
        "assets/textures/right.jpg",
        "assets/textures/left.jpg",
        "assets/textures/top.jpg",
        "assets/textures/bottom.jpg",
        "assets/textures/front.jpg",
        "assets/textures/back.jpg"
    };

    SimpleMesh *skybox_mesh = CreateSimpleMesh(&skybox_vertices[0], 108);
    CubeMap skybox = CreateCubeMap(skybox_faces);
    Shader *skybox_shader = CreateShader("assets/shaders/skybox.vert", "assets/shaders/skybox.frag");

    Shader *shader = CreateShader("assets/shaders/simple.vert", "assets/shaders/simple.frag");
    Camera *camera = CreateCamera(103, 1.0f, 0.1f, 1000.0f);
    
    trainer.window = window;
    trainer.camera = camera;
    trainer.sensitivity = 2.068;
    trainer.m_yaw = 0.022;
    trainer.m_pitch = 0.022;

    glfwGetCursorPos(window.handle, &trainer.last_mouse_x, &trainer.last_mouse_y);

    Mesh *mesh = LoadObjFile("assets/models/cube.obj");

    CalculateCamera(camera, window.width, window.height);

    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    while (WindowShouldClose(&window)) {
        Render();

        Use(skybox_shader);
        LoadMatrix(skybox_shader, "view_matrix", camera->view);
        LoadMatrix(skybox_shader, "proj_matrix", camera->projection);

        glDepthMask(GL_FALSE);
        glDisable(GL_DEPTH_TEST);

        glActiveTexture(GL_TEXTURE0);
        Bind(skybox);
        Draw(skybox_mesh);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);

        Use(shader);
        LoadMatrix(shader, "proj_mat", camera->projection);
        LoadMatrix(shader, "view_mat", camera->view);

        ChangeOrientation(camera);

        Bind(mesh);

        int scale = 100;
        int pos = 100;
        int mini = 10;

        // front
        LoadMatrix(shader, "model_mat", TempModel({0, 0, -pos}, {0, 0, 0}, {scale, scale, mini}));
        Draw(mesh);
        
        /*
        // back
        LoadMatrix(shader, "model_mat", TempModel({0, 0, pos}, {0, 0, 0}, {scale, scale, mini}));
        Draw(mesh);

        // left
        LoadMatrix(shader, "model_mat", TempModel({-pos, 0, 0}, {0, 0, 0}, {mini, scale, scale}));
        Draw(mesh);
        
        // right
        LoadMatrix(shader, "model_mat", TempModel({pos, 0, 0}, {0, 0, 0}, {mini, scale, scale}));
        Draw(mesh);
        
        // floor
        LoadMatrix(shader, "model_mat", TempModel({0, -pos, 0}, {0, 0, 0}, {scale, mini, scale}));
        Draw(mesh);
        */

        UpdateWindow(&window);
    }

    DestroyCamera(camera);
    DestroyCubeMap(skybox);
    DestroyMesh(mesh);
    DestroyWindow(&window);
    DestroySimpleMesh(skybox_mesh);
    DestroyShader(skybox_shader);
    DestroyShader(shader);

	return 0;
}