//
// Created by bat on 20/11/23.
//

#ifndef OBJECT_VIEWER_GUI_H
#define OBJECT_VIEWER_GUI_H

#include <cmath>

#include "deps/imgui/imgui.h"
#include "deps/imgui/imgui_impl_glfw.h"
#include "deps/imgui/imgui_impl_opengl3.h"
//#include "deps/ImGui-Addons/FileBrowser/ImGuiFileBrowser.h"


#include "ShaderUtils.h"
#include "SceneOperations.h"

#include "globals.h"

// FLAGS : https://pixtur.github.io/mkdocs-for-imgui/site/api-imgui/Flags---Enumerations/#ImGuiWindowFlags_NoMove
// GIZMO : https://github.com/CedricGuillemet/ImGuizmo
// FILE DIALOG : https://github.com/gallickgunner/ImGui-Addons

namespace GUI{

    void displayMainToolbar(GLFWwindow* window){

        ImGui::SetNextWindowPos({0, 0});

        float panelW = std::fmax(static_cast<float>(window_width)/4, 400.0f);
        ImGui::SetNextWindowSize({panelW, static_cast<float>(window_height)});
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImColor(0,0,0,100).Value);

        auto  flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar;

        ////////////////////////////////////////////////////////////////////////////////////////////////

        ImGui::Begin("Scene Configuration", &imguitest1, flags );
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Open..", "Ctrl+O"))
                {
                    SceneOperations::openFile();
                }

                if (ImGui::MenuItem("Save", "Ctrl+S"))   { /* Do stuff */ }
                if (ImGui::MenuItem("Close", "Ctrl+W"))  {}
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }


        ImGui::Text("Application (%.1f FPS)", ImGui::GetIO().Framerate);
        ImGui::Text(" ");

        ////////////////////////////////////////////////////////////////////////////////////////////////

        ImGui::Separator(); ImGui::TextColored({0.0f,1.0f,1.0f,1.0f}, "Render Mode"); ImGui::Separator();
        // ImGui::Text("Primitive object "); ImGui::SameLine();
        ImGui::RadioButton("Smooth", &render_mode, 0); ImGui::SameLine();
        ImGui::RadioButton("Lines", &render_mode, 1); ImGui::SameLine();
        ImGui::RadioButton("Point Cloud", &render_mode, 2); ImGui::SameLine();
        ImGui::RadioButton("Mesh", &render_mode, 3);


        ////////////////////////////////////////////////////////////////////////////////////////////////

        ImGui::Separator(); ImGui::TextColored({0.0f,1.0f,1.0f,1.0f}, "Object Properties"); ImGui::Separator();
        ImGui::Text("These set of boxes and sliders change the way the object reacts");
        ImGui::Text("with light giving us the impression of its material. The initial");
        ImGui::Text("sliders assume the object stays white, and the checkbox 'Custom");
        ImGui::Text("Colors' allows us to change the associated colors");
        ImGui::Separator();

        ImGui::Text(" ");

        ////////////////////////////////////////////////////////////////////////////////////////////////

        ImGui::Separator(); ImGui::TextColored({0.0f,1.0f,1.0f,1.0f}, "Input Devices"); ImGui::Separator();
        ImGui::Text("Decide which input device associates to which transformation.");
        ImGui::Text("By default the scroll wheel is associated with scaling");
        ImGui::Separator();

        ImGui::Text("Use mouse to "); ImGui::SameLine();
        ImGui::RadioButton("translate", &mouse_mode, 0); ImGui::SameLine();
        ImGui::RadioButton("rotate", &mouse_mode, 1);

        ImGui::Text("Using keyboard to"); ImGui::SameLine();
        ImGui::RadioButton("translate ", &keyboard_mode, 0); ImGui::SameLine();
        ImGui::RadioButton("rotate ", &keyboard_mode, 1);

        keyboard_mode = !(mouse_mode);

        ////////////////////////////////////////////////////////////////////////////////////////////////

        ImGui::Text(" ");
        ImGui::Separator(); ImGui::TextColored({0.0f,1.0f,1.0f,1.0f}, "Camera"); ImGui::Separator();
        if(ImGui::SliderFloat("Field of view2", &fovy, 0.0f, 180.0f)) {
            ShaderUtils::reshape(window, window_width, window_height);
        }


        ImGui::SliderFloat("Frustum near plane", &zNear, 0.0f, 15.0f);
        ImGui::SliderFloat("Frustum far plane", &zFar, 0.0f, 150.0f);
        ImGui::Text(" ");
        ImGui::SliderFloat3("Camera position ", &eye[0], -10.0f, 10.0f);

        ////////////////////////////////////////////////////////////////////////////////////////////////

        ImGui::Separator(); ImGui::TextColored({0.0f,1.0f,1.0f,1.0f}, "Lighting"); ImGui::Separator();
        ImGui::Text("Most lights are switched off by default, and the below");
        ImGui::Text("sliders can play with the light positions and color intensities");

        ImGui::TextColored(ImVec4(1,1,0,1), "Important Stuff");
        ImGui::BeginChild("Scrolling");
        for (int n = 0; n < 50; n++)
            ImGui::Text("%04d: Some text", n);
        ImGui::EndChild();

        ImGui::PopStyleColor();
        ImGui::End();
    }

    void draw(GLFWwindow* window){
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        displayMainToolbar(window);


        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
};

#endif //OBJECT_VIEWER_GUI_H
