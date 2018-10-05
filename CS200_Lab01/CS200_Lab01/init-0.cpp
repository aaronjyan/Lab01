/*!
@file    init-0.cpp
@author  pghali@digipen.edu

This is a very basic program with a minimal game loop that:
1) initializes GLFW,
2) creates and initializes an OpenGL 4.5 context (the context is the
entire OpenGL state plus framebuffer (colorbuffer and depthbuffer),
3) initializes extension loading library GLEW,
4) prints the OpenGL version and other context parameters, and
5) sets callbacks for the following events: keyboard key (press, repeat,
release), mouse button (left/right press and release), mouse scroll
offset, and mouse cursor position (relative to top-left corner of window)
6) loops for ever until ESC key is pressed or the window's close flag 
is clicked or Alt+F4 is pressed.

This source file uses the following OpenGL commands:
1) glGetString
2) glGetIntegerv
3) glViewport
4) glClearColor
5) glClear

*//*__________________________________________________________________________*/

/*                                                                   includes
----------------------------------------------------------------------------- */
// Extension loader library's header must be included before GLFW's header!!!
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
/*                                                      function declarations
----------------------------------------------------------------------------- */
// instead of a monolithic function, split code into functional units ...
static GLFWwindow* create_gl_context();
static void init_gl_loader();
static void query_gl_context();
static void init_gl_state(GLFWwindow *pwin);
static void draw(GLFWwindow*);
static void update(GLFWwindow*);
static void cleanup();

static void error_cb(int error, char const* description);
static void fbsize_cb(GLFWwindow *pwin, int width, int height);
static void key_cb(GLFWwindow *pwin, int key, int scancode, int action, int mod);
static void mousebutton_cb(GLFWwindow *pwin, int button, int action, int mod);
static void mousescroll_cb(GLFWwindow *pwin, double xoffset, double yoffset);
static void mousepos_cb(GLFWwindow *pwin, double xpos, double ypos);

/*                                                   objects with file scope
----------------------------------------------------------------------------- */


/*                                                      function definitions
----------------------------------------------------------------------------- */

/*                                                      enums
----------------------------------------------------------------------------- */

/*  _________________________________________________________________________ */

/*  _________________________________________________________________________ */
/*! main

@param none

@return int

Indicates how the program existed. Normal exit is signaled by a return value of
0. Abnormal termination is signaled by a non-zero return value.
Note that the C++ compiler will insert a return 0 statement if one is missing.
*/
int main() {
  GLFWwindow *pWindow = create_gl_context();
  init_gl_loader();
  query_gl_context();
  init_gl_state(pWindow);

  // window's close flag is set by clicking close widget or Alt+F4
	while (!glfwWindowShouldClose(pWindow)) {
		draw(pWindow);
		update(pWindow);
	}
	cleanup();
}

/*  _________________________________________________________________________ */
/*! draw

@param GLFWwindow*
Handle to window that defines the OpenGL context

@return none

For now, there's nothing to draw - just paint colorbuffer with constant color
*/
static void draw(GLFWwindow *pWindow) {
  glClear(GL_COLOR_BUFFER_BIT);

  float clear_clr[] = { .0f, 0.f, 0.f, 1.f };
  // clear colorbuffer with rgb value in clear_clr
  glClearBufferfv(GL_COLOR, 0, clear_clr);

  // swap buffers: front <-> back
	glfwSwapBuffers(pWindow);
}

//static void input()
//{
//  GLint presentController = glfwJoystickPresent(GLFW_JOYSTICK_1);
//
//  if (presentController == 1)
//  {
//    GLint axesCount;
//    const GLfloat *axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);
//  
//    GLint buttonCount;
//    const unsigned char *buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);
//  
//    std::cout << std::endl;
//    std::cout << std::endl;
//    std::cout << std::endl;
//    std::cout << std::endl;
//    std::cout << std::endl;
//    std::cout << std::endl;
//    std::cout << std::endl;
//    std::cout << "Left Joystick X: " << axes[L_X_JOYSTICK] << std::endl;
//    std::cout << "Left Joystick Y: " << axes[L_Y_JOYSTICK] << std::endl;
//    std::cout << "Right Joystick X: " << axes[R_X_JOYSTICK] << std::endl;
//    std::cout << "Right Joystick Y: " << axes[R_Y_JOYSTICK] << std::endl;
//    std::cout << "Left Trigger: " << axes[L_TRIGGER] << std::endl;
//    std::cout << "Right Trigger: " << axes[R_TRIGGER] << std::endl;
//    //std::cout << "A Button: " << buttons[A_BUTTON] << std::endl;
//    //std::cout << "B Button: " << buttons[B_BUTTON] << std::endl;
//    //std::cout << "X Button: " << buttons[X_BUTTON] << std::endl;
//    //std::cout << "Y Button: " << buttons[Y_BUTTON] << std::endl;
//    //std::cout << "L BUMPER" << buttons[L_BUMPER] << std::endl;
//    //std::cout << "R BUMPER" << buttons[R_BUMPER] << std::endl;
//    //std::cout << "START Button: " << buttons[START_BUTTON] << std::endl;
//    //std::cout << "BACK Button: " << buttons[BACK_BUTTON] << std::endl;
//    //std::cout << "UP: " << buttons[U_DPAD] << std::endl;
//    //std::cout << "DOWN: " << buttons[D_DPAD] << std::endl;
//    //std::cout << "LEFT: " << buttons[L_DPAD] << std::endl;
//    //std::cout << "RIGHT: " << buttons[R_DPAD] << std::endl;
//  }
//}

// Checks whether or not a given button is pressed
// Params:
//     p_button - Enum for what button should be checked
// Return:
//     1 if true, otherwise 0
//static GLboolean IsButtonPressed(Buttons p_button)
//{
//  GLint buttonCount; // Will hold the total number of buttons
//    // Gets the current values from all buttons on controller 1, stores the number of buttons in button count
//  const unsigned char *buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);
//
//    // If the given button is pressed, return true. Otherwise returns false
//  return buttons[p_button] ? true : false;
//}

/*  _________________________________________________________________________ */
/*! update

@param GLFWwindow*
Handle to window that defines the OpenGL context

@return none

For now, there are no objects to animate nor any other parameters to update.
We just use GLFW to process events (keyboard, mouse button click, mouse 
movement, and mouse scroller) that have occurred and call the appropriate
callback function.
*/
static void update(GLFWwindow *pwin) {
	glfwPollEvents();
}

/*  _________________________________________________________________________ */
/*! init_gl_state

@param none

@return none

OpenGL is a complicated state machine - this function sets certaub useful
aspects of the state that will remain unchanged throughout this program's
execution. More specifically, we set the size of the drawing region to be
the entire window and specify the color used to clear the color buffer.
*/
static void init_gl_state(GLFWwindow *pwin) {
  // we'll use the entire window as viewport ...
  GLint width, height;
  glfwGetFramebufferSize(pwin, &width, &height);
  fbsize_cb(pwin, width, height);
}

/*  _________________________________________________________________________ */
/*! cleanup

@param none

@return none

For now, there are no resources allocated by the application program.
The only task is to have GLFW return resources back to the system and
gracefully terminate.
*/
static void cleanup() {
	glfwTerminate();
}

/*  _________________________________________________________________________ */
/*! error_cb

@param int
GLFW error code

@parm char const*
Human-readable description of the code

@return none

The error callback receives a human-readable description of the error and 
(when possible) its cause.
*/
static void error_cb(int error, char const* description) {
	std::cerr << "GLFW error: " << description << std::endl;
}

/*  _________________________________________________________________________ */
/*! fbsize_cb

@param GLFWwindow*
Handle to window that is being resized

@parm int
Width in pixels of new window size

@parm int
Height in pixels of new window size

@return none

This function is called when the window is resized - it receives the new size
of the window in pixels.
*/
static void fbsize_cb(GLFWwindow *pwin, int width, int height) {
  // use the entire framebuffer as drawing region
  glViewport(0, 0, width, height);
  // later, we'll have to set the projection matrices here ...
}

/*  _________________________________________________________________________ */
/*! key_cb

@param GLFWwindow*
Handle to window that is receiving event

@param int
the keyboard key that was pressed or released

@parm int
Platform-specific scancode of the key

@parm int
GLFW_PRESS, GLFW_REPEAT or GLFW_RELEASE
action will be GLFW_KEY_UNKNOWN if GLFW lacks a key token for it, 
for example E-mail and Play keys.

@parm int
bit-field describing which modifier keys (shift, alt, control)
were held down

@return none

This function is called when keyboard buttons are pressed.
When the ESC key is pressed, the close flag of the window is set.
*/
void key_cb(GLFWwindow *pwin, int key, int scancode, int action, int mod) {
  if (GLFW_PRESS == action) {
#ifdef _DEBUG
    std::cout << "Key pressed" << std::endl;
#endif
  } else if (GLFW_REPEAT == action) {
#ifdef _DEBUG
    std::cout << "Key repeatedly pressed" << std::endl;
#endif
  } else if (GLFW_RELEASE == action) {
#ifdef _DEBUG
    std::cout << "Key released" << std::endl;
#endif
  }

  if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action) {
    glfwSetWindowShouldClose(pwin, GLFW_TRUE);
  }
}

/*  _________________________________________________________________________ */
/*! mousebutton_cb

@param GLFWwindow*
Handle to window that is receiving event

@param int
the mouse button that was pressed or released
GLFW_MOUSE_BUTTON_LEFT and GLFW_MOUSE_BUTTON_RIGHT specifying left and right
mouse buttons are most useful

@parm int
action is either GLFW_PRESS or GLFW_RELEASE

@parm int
bit-field describing which modifier keys (shift, alt, control)
were held down

@return none

This function is called when mouse buttons are pressed.
*/
static void mousebutton_cb(GLFWwindow *pwin, int button, int action, int mod) {
  switch (button) {
  case GLFW_MOUSE_BUTTON_LEFT:
#ifdef _DEBUG
    std::cout << "Left mouse button ";
#endif
    break;
  case GLFW_MOUSE_BUTTON_RIGHT:
#ifdef _DEBUG
    std::cout << "Right mouse button ";
#endif
    break;
  }
  switch (action) {
  case GLFW_PRESS:
#ifdef _DEBUG
    std::cout << "pressed!!!" << std::endl;
#endif
    break;
  case GLFW_RELEASE:
#ifdef _DEBUG
    std::cout << "released!!!" << std::endl;
#endif
    break;
  }
}

/*  _________________________________________________________________________ */
/*! mousepos_cb

@param GLFWwindow*
Handle to window that is receiving event

@param double
new cursor x-coordinate, relative to the left edge of the client area

@param double
new cursor y-coordinate, relative to the top edge of the client area

@return none

This functions receives the cursor position, measured in screen coordinates but 
relative to the top-left corner of the window client area.
*/
static void mousepos_cb(GLFWwindow *pwin, double xpos, double ypos) {
#ifdef _DEBUG
  std::cout << "Mouse cursor position: (" << xpos << ", " << ypos << ")" << std::endl;
#endif
}

/*  _________________________________________________________________________ */
/*! mousescroll_cb

@param GLFWwindow*
Handle to window that is receiving event

@param double
Scroll offset along X-axis

@param double
Scroll offset along Y-axis

@return none

This function is called when the user scrolls, whether with a mouse wheel or
touchpad gesture. Although the function receives 2D scroll offsets, a simple
mouse scroll wheel, being vertical, provides offsets only along the Y-axis.
*/
static void mousescroll_cb(GLFWwindow *pwin, double xoffset, double yoffset) {
#ifdef _DEBUG
  std::cout << "Mouse scroll wheel offset: (" << xoffset << ", " << yoffset << ")" << std::endl;
#endif
}

/*  _________________________________________________________________________ */
/*! create_gl_context

@param none

@return GLFWwindow* - return a handle to a window of size 1600x 900 pixels and
its associated OpenGL context that matches a core profile that is compatible
with OpenGL 4.5 and doesn't support "old" OpenGL,
has 32-bit RGBA, double-buffered color buffer,
24-bit depth buffer and 8-bit stencil buffer

Uses GLFW to create an OpenGL context.
GLFW's initialization follows from here: http://www.glfw.org/docs/latest/quick.html
*/
static GLFWwindow* create_gl_context() {
  if (!glfwInit()) {
    std::cout << "Initialization of GLFW has failed - abort program!!!" << std::endl;
    std::exit(EXIT_FAILURE);
  }
  // In case a GLFW function fails, an error is reported to callback function
  glfwSetErrorCallback(error_cb);

  // Before asking GLFW to create an OpenGL context, we specify the minimum constraints
  // in that context:

  // specify OpenGL version 4.5
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
  // specify modern OpenGL only - no compatibility with "old" OpenGL
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  // applications will be double-buffered ...
  glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
  // default behavior: colorbuffer is 32-bit RGBA, depthbuffer is 24-bits

  // size of viewport: 2400 x 1350
  GLFWwindow *pwin = glfwCreateWindow(
    2400, 1350,
    "init-0 - Hello OpenGL version 4.5 - clear colorbuffer with constant color",
    NULL, NULL);
  if (!pwin) {
    std::cerr << "GLFW is unable to create an OpenGL context" << std::endl;
    glfwTerminate();
    std::exit(EXIT_FAILURE);
  }

  // set callback for events associated with window size changes; keyboard;
  // mouse buttons, cursor position, and scroller
  glfwSetFramebufferSizeCallback(pwin, fbsize_cb);
  glfwSetKeyCallback(pwin, key_cb);
  glfwSetMouseButtonCallback(pwin, mousebutton_cb);
  glfwSetCursorPosCallback(pwin, mousepos_cb);
  glfwSetScrollCallback(pwin, mousescroll_cb);

  // make the previously created OpenGL context current ...
  glfwMakeContextCurrent(pwin);

  // this is the default setting ...
  glfwSetInputMode(pwin, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  // uncomment one line at a time to see the effect on mouse cursor
  //glfwSetInputMode(pwin, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
  //glfwSetInputMode(pwin, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  return pwin;
}

/*  _________________________________________________________________________ */
/*! init_gl_loader

@param none

@return none

Uses GLEW API to expose OpenGL core and extension functionality
GLEW's initialization follows from here: http://glew.sourceforge.net/basic.html
*/
static void init_gl_loader() {
  // initialize OpenGL (and extension) function loading library
  GLenum err = glewInit();
  if (GLEW_OK != err) {
    std::cerr << "Unable to initialize GLEW - error: " << glewGetErrorString(err) << std::endl;
    std::exit(EXIT_FAILURE);
  }
  if (GLEW_VERSION_4_5) {
    std::cout << "\tUsing glew " << glewGetString(GLEW_VERSION) << std::endl;
    std::cout << "Driver supports OpenGL 4.5\nDetails:" << std::endl;
  }
}

/*  _________________________________________________________________________ */
/*! query_gl_context

@param none

@return none

Prints current hardware capabilities relevant to OpenGL and GLSL.
*/
static void query_gl_context() {
  // print OpenGL version information ...
  std::cout << "\tGL Vendor: " << glGetString(GL_VENDOR) << std::endl;
  std::cout << "\tGL Renderer: " << glGetString(GL_RENDERER) << std::endl;
  std::cout << "\tGL Version (string): " << glGetString(GL_VERSION) << std::endl;

  GLint majorVer, minorVer;
  glGetIntegerv(GL_MAJOR_VERSION, &majorVer);
  glGetIntegerv(GL_MINOR_VERSION, &minorVer);
  std::cout << "\tGL Version (integer): " << majorVer << "." << minorVer << std::endl;
  std::cout << "\tGLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

  // how many vertex attributes does your OpenGL driver support?
  GLint maxVtxAttribs;
  glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVtxAttribs);
  std::cout << "\tMaximum of vertex attributes: " << maxVtxAttribs << std::endl;

  // how many vertices and indices does your OpenGL driver support?
  GLint maxVertices, maxIndices;
  glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &maxVertices);
  glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &maxIndices);
  std::cout << "\tMax Elements Vertices: " << maxVertices << std::endl;
  std::cout << "\tMax Elements Indices: " << maxIndices << std::endl;
  glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES, &maxVertices);
  std::cout << "\tMax vertices by geometry shader: " << maxVertices << std::endl;

  // enumerate extensions
#if 0 // don't want to print the long list of extensions ...
  GLint maxExtensions;
  glGetIntegerv(GL_NUM_EXTENSIONS, &maxExtensions);
  for (int i = 0; i < maxExtensions; ++i) {
    std::cout << (i + 1) << ": " << glGetStringi(GL_EXTENSIONS, i) << std::endl;
  }
#endif
}
