#ifndef IMGUI_IMPL_H
#define IMGUI_IMPL_H

struct GLFWwindow;
void InitializeImGui(GLFWwindow *window);
void DeinitializeImGui();

void BeginImGuiFrame();
void EndImGuiFrame();

#endif