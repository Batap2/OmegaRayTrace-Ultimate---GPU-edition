#ifndef GLOBAL_VARS_H
#define GLOBAL_VARS_H

#include "Object3D.h"
#include "Light.h"
#include "transform.h"
#include "camera.h"
#include "deps/imgui-filebrowser/imfilebrowser.h"

// -------------- MENU APPEARANCE -------------- //
inline float screenSeparation1 = 4;

// -------------- KEYS -------------- //
inline bool rclickHolded, upHolded, downHolded, leftHolded, rightHolded, aHolded, eHolded;


inline int amount;
inline vec3 eye;
inline vec3 up;


inline vec3 eyeinit(0.0, 0.0, 5.0);
inline vec3 upinit(0.0, 1.0, 0.0);
inline vec3 center(0.0, 0.0, 0.0);
inline int mouse_mode    = 1;
inline int keyboard_mode = 0;
inline int window_width  = 1920;
inline int window_height = 1080;
inline int amountinit    = 5;
inline float fovy        = 60.0f;
inline float zFar        = 99.0f;
inline float zNear       = 0.1f;
inline bool firstMouse = true;
inline float yaw   = -90.0f;
inline float pitch =  -20.0f;
inline float lastX =  window_width / 2.0;
inline float lastY =  window_height / 2.0;

inline mat4 projection, modelview, model, view;

// -------------- SHADER & UNIFORM LOCATIONS -------------- //
inline GLuint vertexshader, fragmentshader, shaderprogram;
inline GLuint projectionLoc, modelviewLoc, modelLoc, viewLoc;

inline GLuint lightcol;
inline GLuint lightpos;
inline GLuint numusedcol;
inline GLuint enablelighting;
inline GLuint ambientcol;
inline GLuint diffusecol;
inline GLuint specularcol;
inline GLuint emissioncol;
inline GLuint shininesscol;
inline GLuint camPosLoc;

inline GLuint lightsBufferID, lights_numberID;

// -------------- SHADER BUFFER SIZE -------------- //
inline int lightsMaxNumber = 64;

inline enum { view2, translate, scale } transop;

inline float sx, sy;
inline float tx, ty;

// Globals regarding lighting details
inline constexpr int numLights = 5;
inline GLfloat lightposn[4 * numLights];
inline GLfloat lightcolor[4 * numLights];

inline GLfloat lightransf[4 * numLights];

// Callback and reshape globals
inline int render_mode = 0;
inline float current_vp_width = window_width/screenSeparation1;
inline float current_vp_height = window_height;
// Global mesh
inline Mesh mesh;

// -------------- SCENE -------------- //


inline Camera mainCamera;
inline float cameraSpeed = 0.05f;

inline std::vector<Mesh*> scene_meshes;
inline std::vector<Light*> scene_lights;

// -------------- IMGUI -------------- //

inline bool showMenus = true;
inline bool imguitest1;
inline ImGui::FileBrowser fileDialog;

#endif
