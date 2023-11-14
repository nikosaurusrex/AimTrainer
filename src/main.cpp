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

struct Crosshair {
    f32 size;
    f32 thickness;
    f32 gap;
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

f32 retangle_vertices[] = {
	-1, -1, 0,
    1, -1, 0,
    1, 1, 0,
    1, 1, 0,
    -1, 1, 0,
    -1, -1, 0
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
    glfwWindowHint(GLFW_SAMPLES, 4);
}

Window CreateWindow(const char *title, int width, int height) {
    Window window = {};

	window.handle = glfwCreateWindow(width, height, title, glfwGetPrimaryMonitor(), 0);
    if (!window.handle) {
        LogFatal("Failed to create GLFW window!");
    }

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

glm::mat4 TranslateScale(glm::vec3 trans, glm::vec3 scale) {
    return glm::translate(glm::mat4(1.0f), trans) * 
                glm::scale(glm::mat4(1.0f), scale);
}

glm::mat4 TranslateRotateScale(glm::vec3 trans, glm::vec3 rot, glm::vec3 scale) {
    return glm::translate(glm::mat4(1.0f), trans) * 
                glm::toMat4(glm::quat(glm::radians(rot))) *
                glm::scale(glm::mat4(1.0f), scale);
}

void DrawCrosshair(Crosshair crosshair, Shader *ui_shader, Window window, SimpleMesh *crosshair_mesh) {
    glm::mat4 ui_proj = glm::ortho(0.0f, (f32) window.width, (f32) window.height, 0.0f, -1.0f, 1.0f);
    glm::mat4 center_mat = glm::translate(glm::mat4(1.0f), glm::vec3(window.width / 2, window.height / 2, 0.0f));

    Use(ui_shader);
    LoadMatrix(ui_shader, "proj_mat", ui_proj);
    LoadMatrix(ui_shader, "view_mat", center_mat);
    Bind(crosshair_mesh);

    f32 size = crosshair.size;
    f32 thickness = crosshair.thickness;
    f32 gap = crosshair.gap;

    // right
    LoadMatrix(ui_shader, "model_mat", TranslateRotateScale({gap, 0, 0}, {0, 0, 0}, {size, thickness, 1}));
    Draw(crosshair_mesh);
    
    // left
    LoadMatrix(ui_shader, "model_mat", TranslateRotateScale({-gap, 0, 0}, {180, 0, 0}, {size, thickness, 1}));
    Draw(crosshair_mesh);

    // bottom
    LoadMatrix(ui_shader, "model_mat", TranslateRotateScale({0, gap, 0}, {0, 0, 0}, {thickness, size, 1}));
    Draw(crosshair_mesh);

    // top
    LoadMatrix(ui_shader, "model_mat", TranslateRotateScale({0, -gap, 0}, {0, 180, 0}, {thickness, size, 1}));
    Draw(crosshair_mesh);
}

int main() {  
	AimTrainer trainer = {};

    InitGLFW();
    Window window = CreateWindow("AimTrainer", 1920, 1080);

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
    
    Shader *ui_shader = CreateShader("assets/shaders/ui.vert", "assets/shaders/ui.frag");
    SimpleMesh *crosshair_mesh = CreateSimpleMesh(retangle_vertices, 18);

    trainer.window = window;
    trainer.camera = camera;
    trainer.sensitivity = 2.068;
    trainer.m_yaw = 0.022;
    trainer.m_pitch = 0.022;

    glfwGetCursorPos(window.handle, &trainer.last_mouse_x, &trainer.last_mouse_y);

    Mesh *cube_mesh = LoadObjFile("assets/models/cube.obj");
    Mesh *sphere_mesh = LoadObjFile("assets/models/sphere.obj");

    CalculateCamera(camera, window.width, window.height);

    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);  

    while (WindowShouldClose(&window)) {
        Render();

        Use(skybox_shader);
        LoadMatrix(skybox_shader, "view_matrix", camera->view);
        LoadMatrix(skybox_shader, "proj_matrix", camera->projection);

        glDepthMask(GL_FALSE);
        glDisable(GL_DEPTH_TEST);

        glActiveTexture(GL_TEXTURE0);
        Bind(skybox);
        Bind(skybox_mesh);
        Draw(skybox_mesh);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);

        Use(shader);
        LoadMatrix(shader, "proj_mat", camera->projection);
        LoadMatrix(shader, "view_mat", camera->view);

        ChangeOrientation(camera);

        Bind(cube_mesh);

        int scale = 100;
        int pos = 100;
        int mini = 10;

        // front
        LoadMatrix(shader, "model_mat", TranslateScale({0, 0, -pos}, {scale, scale, mini}));
        LoadVec3(shader, "object_color", {0.05, 0.06, 0.12});
        Draw(cube_mesh);
        
        f32 sphere_size = 2;
         
        // Targets
        Bind(sphere_mesh);
        LoadMatrix(shader, "model_mat", TranslateScale({0, 0, -pos + 10}, {sphere_size, sphere_size, sphere_size}));
        LoadVec3(shader, "object_color", {1.0, 0.0, 0.0});
        Draw(sphere_mesh);

        // Crosshair        

        Crosshair crosshair = {2, 1, 4};
        DrawCrosshair(crosshair, ui_shader, window, crosshair_mesh);

        UpdateWindow(&window);
    }

    DestroyCamera(camera);
    DestroyCubeMap(skybox);
    DestroyMesh(cube_mesh);
    DestroyMesh(sphere_mesh);
    DestroyWindow(&window);
    DestroySimpleMesh(skybox_mesh);
    DestroySimpleMesh(crosshair_mesh);
    DestroyShader(skybox_shader);
    DestroyShader(shader);
    DestroyShader(ui_shader);

	return 0;
}