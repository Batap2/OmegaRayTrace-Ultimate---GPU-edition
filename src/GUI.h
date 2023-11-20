//
// Created by bat on 20/11/23.
//

#ifndef OBJECT_VIEWER_GUI_H
#define OBJECT_VIEWER_GUI_H

#include "deps/imgui/imgui.h"
#include "deps/imgui/imgui_impl_glfw.h"
#include "deps/imgui/imgui_impl_opengl3.h"

#include "globals.h"

namespace GUI{

    void displayMainToolbar(){
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos({0, 0});
        ImGui::SetNextWindowSize({current_vp_width, current_vp_height});

        ////////////////////////////////////////////////////////////////////////////////////////////////

        ImGui::Begin("Object Properties");
        ImGui::Text("Application %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
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
        //reshape(window, window_width, window_height);
        ImGui::Separator(); ImGui::TextColored({0.0f,1.0f,1.0f,1.0f}, "Camera"); ImGui::Separator();
        ImGui::SliderFloat("Field of view2", &fovy, 0.0f, 180.0f);
        ImGui::SliderFloat("Frustum near plane", &zNear, 0.0f, 15.0f);
        ImGui::SliderFloat("Frustum far plane", &zFar, 0.0f, 150.0f);
        ImGui::Text(" ");
        ImGui::SliderFloat3("Camera position ", &eye[0], -10.0f, 10.0f);

        ////////////////////////////////////////////////////////////////////////////////////////////////

        ImGui::Separator(); ImGui::TextColored({0.0f,1.0f,1.0f,1.0f}, "Lighting"); ImGui::Separator();
        ImGui::Text("Most lights are switched off by default, and the below");
        ImGui::Text("sliders can play with the light positions and color intensities");

        ////////////////////////////////////////////////////////////////////////////////////////////////

        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        ////////////////////////////////////////////////////////////////////////////////////////////////
    }

    void draw(){

        displayMainToolbar();
    }
};

#endif //OBJECT_VIEWER_GUI_H
