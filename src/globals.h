#ifndef GLOBAL_VARS_H
#define GLOBAL_VARS_H

#include "mesh.h"
#include "Light.h"
#include "transform.h"

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
inline float fovy        = 90.0f;
inline float zFar        = 99.0f;
inline float zNear       = 0.1f;

inline GLuint vertexshader, fragmentshader, shaderprogram;
inline mat4 projection, modelview, model, view;
inline GLuint projectionLoc, modelviewLoc, modelLoc, viewLoc;

inline enum { view2, translate, scale } transop;

inline float sx, sy;
inline float tx, ty;

// Globals regarding lighting details
inline constexpr int numLights = 5;
inline GLfloat lightposn[4 * numLights];
inline GLfloat lightcolor[4 * numLights];
inline GLfloat lightransf[4 * numLights];

// Variables to set uniform params for lighting fragment shader
inline GLuint lightcol;
inline GLuint lightpos;
inline GLuint numusedcol;
inline GLuint enablelighting;
inline GLuint ambientcol;
inline GLuint diffusecol;
inline GLuint specularcol;
inline GLuint emissioncol;
inline GLuint shininesscol;

// Callback and reshape globals
inline int render_mode;
inline int previous_y_position;
inline int previous_x_position;
inline float current_vp_width, current_vp_height;

// Global mesh
inline gl::Mesh mesh;

// Scene

inline std::vector<gl::Mesh> meshes;
inline std::vector<Light> lights;

#endif
