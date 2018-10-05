/*!
@file    Lab-2.cpp
@author  pghali@digipen.edu

This is an extension to init-0.cpp (from Lab 1) with a runtime logging
mechanism to help with debugging incorrect program behavior. Just like
init-0.cpp, this basic program has a minimal game loop that:
1) creates a debug log file,
2) initializes GLFW,
3) creates and initializes an OpenGL 4.5 context (the context is the
entire OpenGL state plus framebuffer (colorbuffer and depthbuffer),
4) initializes extension loading library GLEW,
5) prints the OpenGL version and other context parameters to a log file, and
6) sets callbacks for the following events: keyboard key (press, repeat,
release), mouse button (left/right press and release), mouse scroll
offset, and mouse cursor position (relative to top-left corner of window)
7) loops for ever until ESC key is pressed or the window's close flag
is clicked or Alt+F4 is pressed.

This source file uses the following OpenGL commands:
1) glGetString
2) glGetIntegerv
3) glGetBooleanv
4) glViewport
5) glClear
6) glClearColor

*//*__________________________________________________________________________*/

/*                                                                   includes
----------------------------------------------------------------------------- */
// Extension loader library's header must be included before GLFW's header!!!
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <utility> // for std::forward, std::pair
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <ctime>
#include <cstdlib>

/*                                                      function declarations
-----------------------------------------------------------------------------*/
// instead of a monolithic function, split code into functional units ...

// initialization of OpenGL state
static GLFWwindow* create_gl_context( GLuint framebuffer_width,
                                      GLuint framebuffer_height,
                                      std::string app_title);
static GLboolean init_gl_loader();
static void query_gl_context();
static void init_gl_state(GLFWwindow *pwin);

// game loop ...
static void draw(GLFWwindow*);
static void update(GLFWwindow*);
static void cleanup(GLFWwindow *pwin);

// create log file ...
static GLboolean create_log_file();
// functionality related to writing to the log file is implemented in
// function template log_to_debug_file ...

// I/O callbacks ...
static void glfw_error_cb(int error, char const* description);
static void fbsize_cb(GLFWwindow *pwin, int width, int height);
static void key_cb(GLFWwindow *pwin, int key, int scancode, int action, int mod);
static void mousebutton_cb(GLFWwindow *pwin, int button, int action, int mod);
static void mousescroll_cb(GLFWwindow *pwin, double xoffset, double yoffset);
static void mousepos_cb(GLFWwindow *pwin, double xpos, double ypos);

/*                                                   objects with file scope
-----------------------------------------------------------------------------*/
std::string const g_logfilename{ "gl-debug-log.txt" };

/*                                                function template definitions
-----------------------------------------------------------------------------*/
/*! log_to_debug_file
This is a template function that writes the values of one or more parameters
to a file whose name is specified by the first function parameter.

@param std::string const&
Read-only reference to file name to which function will append debug
information

@param Args&& ...
template <typename... Args> represents a parameter pack (rather than a single
template parameter), i.e., Args is a list of type parameters.
Args&& ... args means args represents a list of forwarding references. 

@return bool
Returns true if the function was able to open the specified file and log the
debug information. Otherwise, the function will return false if it was
unable to open the specified file.
*/
template <typename... Args>
bool log_to_debug_file(std::string file_name, Args&&... args) {
  std::ofstream ofs(file_name.c_str(), std::ofstream::app);
  if (!ofs) {
    std::cerr << "ERROR: could not open log file "
              << file_name << " for writing" << std::endl;
    return false;
  }
  int dummy[sizeof...(Args)] = { (ofs << std::forward<Args>(args) << ' ', 1)... };
  ofs << std::endl;
  ofs.close();
  return true;
}

/*                                                      function definitions
-----------------------------------------------------------------------------*/

/*  _________________________________________________________________________*/

/*  _________________________________________________________________________*/
/*! main

@param none

@return int

Indicates how the program existed. Normal exit is signaled by a return value of
0. Abnormal termination is signaled by a non-zero return value.
Note that the C++ compiler will insert a return 0 statement if one is missing.
*/
int main() {
  create_log_file();
  GLFWwindow *pWindow = create_gl_context(2400, 1350, "init-1: OpenGL 4.5 - "
           "create debug log file and clear colorbuffer with constant color");
  if (GL_FALSE == init_gl_loader()) {
    glfwDestroyWindow(pWindow);
    glfwTerminate();
    std::exit(EXIT_FAILURE);
  }
  query_gl_context();
  init_gl_state(pWindow);

  // window's close flag is set by clicking close widget or Alt+F4
	while (!glfwWindowShouldClose(pWindow)) {
		draw(pWindow);
		update(pWindow);
	}
	cleanup(pWindow);
}

/*  _________________________________________________________________________*/
/*! draw

@param GLFWwindow*
Handle to window that defines the OpenGL context

@return none

For now, there's nothing to draw - just paint colorbuffer with constant color
*/
static void draw(GLFWwindow *pWindow) {
  // clear the framebuffer's front color buffer (recall we're using double
  // buffered framebuffer) with the value selected by glClearColor in
  // init_gl_state()
  glClear(GL_COLOR_BUFFER_BIT);

  // swap framebuffers: front becomes back; back becomes front
	glfwSwapBuffers(pWindow);
}

/*  _________________________________________________________________________*/
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

/*  _________________________________________________________________________*/
/*! init_gl_state

@param none

@return none

OpenGL is a complicated state machine - this function sets certain useful
aspects of the state that will remain unchanged throughout this program's
execution. More specifically, we set the size of the drawing region to be
the entire window and specify the color used to clear the color buffer.
*/
static void init_gl_state(GLFWwindow *pwin) {
  glClearColor(0.0, 1.0, 0.0, 1.0);

  // we'll use the entire window as viewport ...
  GLint width, height;
  glfwGetFramebufferSize(pwin, &width, &height);
  // fbsize_cb is the callback function automatically called whenever the user
  // changes the window size
  // here, we explicitly call it to set the viewport region
  fbsize_cb(pwin, width, height);
}

/*  _________________________________________________________________________*/
/*! cleanup

@param GLFWwindow*
Handle to window that defines the OpenGL context

@return none

For now, there are no resources allocated by the application program.
The only task is to have GLFW return resources back to the system and
gracefully terminate.
*/
static void cleanup(GLFWwindow *pwin) {
	glfwTerminate();
}

/*  _________________________________________________________________________*/
/*! glfw_error_cb

@param int
GLFW error code

@parm char const*
Human-readable description of the code

@return none

The error callback receives a human-readable description of the error and 
(when possible) its cause.
*/
static void glfw_error_cb(int error, char const* description) {
  log_to_debug_file(g_logfilename,
                    "GLFW Error id: ",
                    error,
                    " | description: ",
                    description);
}

/*  _________________________________________________________________________*/
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

/*  _________________________________________________________________________*/
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

/*  _________________________________________________________________________*/
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

/*  _________________________________________________________________________*/
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

/*  _________________________________________________________________________*/
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
  std::cout << "Mouse scroll wheel offset: ("
            << xoffset << ", " << yoffset << ")" << std::endl;
#endif
}

/*  _________________________________________________________________________ */
/*! create_gl_context

@param GLuint
width of the framebuffer attached to the new OpenGL context

@param GLuint
height of the framebuffer attached to the new OpenGL context

@param std::string
application title to be printed to the window attached to the new
OpenGL context

@return GLFWwindow* - return a handle to a window of size width x height pixels
and its associated OpenGL context that matches a core profile that is
compatible with OpenGL 4.5 and doesn't support "old" OpenGL, has 32-bit RGBA,
double-buffered color buffer, 24-bit depth buffer and 8-bit stencil buffer

Uses GLFW to create an OpenGL context.
GLFW's initialization follows from here: http://www.glfw.org/docs/latest/quick.html
*/
static GLFWwindow* create_gl_context( GLuint fbwd,
                                      GLuint fbht,
                                      std::string wintitle) {
  if (!glfwInit()) {
    log_to_debug_file(g_logfilename,
              "ERROR: Initialization of GLFW has failed - program aborted");
    std::exit(EXIT_FAILURE);
  } else {
    // write GLFW3 version number to debug log file
    log_to_debug_file(g_logfilename, "GLFW Version: ", glfwGetVersionString());
  }
  // In case a GLFW function fails, an error is reported to callback function
  glfwSetErrorCallback(glfw_error_cb);

  // Before asking GLFW to create an OpenGL context, we specify the minimum
  // constraints in that context:
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // specify OpenGL version 4.5
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  // specify modern OpenGL only - no compatibility with "old" OpenGL
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  // make sure deprecated parts of spec are actually removed from context
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
  // our framebuffer will consist of 24-bit depthbuffer and 
  // double buffered 32-bit RGBA color buffer
  glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
  glfwWindowHint(GLFW_DEPTH_BITS, 24);
  glfwWindowHint(GLFW_RED_BITS, 8);
  glfwWindowHint(GLFW_GREEN_BITS, 8);
  glfwWindowHint(GLFW_BLUE_BITS, 8);
  glfwWindowHint(GLFW_ALPHA_BITS, 8);

  // size of viewport: 2400 x 1350
  GLFWwindow *pwin = glfwCreateWindow(fbwd, fbht, wintitle.c_str(), NULL, NULL);
  if (!pwin) {
    log_to_debug_file(g_logfilename,
                 "ERROR: GLFW is unable to create an OpenGL context.\n");
    glfwTerminate();
    std::exit(EXIT_FAILURE);
  }

  // make the previously created OpenGL context current ...
  glfwMakeContextCurrent(pwin);
  
  // set callback for events associated with window size changes; keyboard;
  // mouse buttons, cursor position, and scroller
  glfwSetFramebufferSizeCallback(pwin, fbsize_cb);
  glfwSetKeyCallback(pwin, key_cb);
  glfwSetMouseButtonCallback(pwin, mousebutton_cb);
  glfwSetCursorPosCallback(pwin, mousepos_cb);
  glfwSetScrollCallback(pwin, mousescroll_cb);
  
  // this is the default setting ...
  glfwSetInputMode(pwin, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

  return pwin;
}

/*  _________________________________________________________________________*/
/*! init_gl_loader

@param none

@return GLboolean
Return GL_TRUE if system is able to support appropriate GL version.
Otherwise, return GL_FALSE.

Uses GLEW API to expose OpenGL core and extension functionality
GLEW's initialization follows from here: http://glew.sourceforge.net/basic.html

Within this function, debug information is logged to a file whose name is
defined at file scope by object g_logfilename (of type std::string)
*/
static GLboolean init_gl_loader() {
  // initialize OpenGL (and extension) function loading library
  GLenum err = glewInit();
  if (GLEW_OK != err) {
    log_to_debug_file(g_logfilename,
      "ERROR: Unable to initialize GLEW ", glewGetErrorString(err));
    return GL_FALSE;
  }

  if (GLEW_VERSION_4_5) { // check support for core GL 4.5
    log_to_debug_file(g_logfilename, "GLEW Version: ", glewGetString(GLEW_VERSION));
    log_to_debug_file(g_logfilename, "Graphics driver supports OpenGL version 4.5\n");
    return GL_TRUE;
  } else {
    log_to_debug_file(g_logfilename, "ERROR: System doesn't support GL 4.5 API\n");
    return GL_FALSE;
  }
}

/*  _________________________________________________________________________*/
/*! query_gl_context

@param none

@return none

Prints current hardware capabilities relevant to OpenGL and GLSL.
The function writes to a file whose name is defined at file scope by
object g_logfilename (of type std::string)
*/
static void query_gl_context() {
  // set up correspondence between OpenGL constants and strings that
  // have the names of these constants
  std::pair<GLenum, std::string> const params[] = {
    { GL_VENDOR, std::string("GL_VENDOR") },
    { GL_RENDERER, std::string("GL_RENDERER") },
    { GL_VERSION, std::string("GL_VERSION") },
    { GL_SHADING_LANGUAGE_VERSION, std::string("GL_SHADING_LANGUAGE_VERSION")}, // 3

    { GL_MAJOR_VERSION, std::string("GL_MAJOR_VERSION") },
    { GL_MINOR_VERSION, std::string("GL_MINOR_VERSION") },
    { GL_MAX_ELEMENTS_VERTICES, std::string("GL_MAX_ELEMENTS_VERTICES") },
    { GL_MAX_ELEMENTS_INDICES, std::string("GL_MAX_ELEMENTS_INDICES") },
    { GL_MAX_GEOMETRY_OUTPUT_VERTICES, std::string("GL_MAX_GEOMETRY_OUTPUT_VERTICES") },
    { GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, std::string("GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS") },
    { GL_MAX_CUBE_MAP_TEXTURE_SIZE, std::string("GL_MAX_CUBE_MAP_TEXTURE_SIZE") },
    { GL_MAX_DRAW_BUFFERS, std::string("GL_MAX_DRAW_BUFFERS") },
    { GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, std::string("GL_MAX_FRAGMENT_UNIFORM_COMPONENTS") },
    { GL_MAX_TEXTURE_IMAGE_UNITS, std::string("GL_MAX_TEXTURE_IMAGE_UNITS") },
    { GL_MAX_TEXTURE_SIZE, std::string("GL_MAX_TEXTURE_SIZE") },
    { GL_MAX_VARYING_FLOATS, std::string("GL_MAX_VARYING_FLOATS") },
    { GL_MAX_VERTEX_ATTRIBS, std::string("GL_MAX_VERTEX_ATTRIBS") },
    { GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, std::string("GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS") },
    { GL_MAX_VERTEX_UNIFORM_COMPONENTS, std::string("GL_MAX_VERTEX_UNIFORM_COMPONENTS") }, // 18
    { GL_MAX_VIEWPORT_DIMS, std::string("GL_MAX_VIEWPORT_DIMS") }, // 19
    { GL_STEREO, std::string("GL_STEREO") }
  };

  // C-strings for 1st four parameters
  log_to_debug_file(g_logfilename, "GL version information and context parameters:");
  GLint i = 0;
  for (i = 0; i < 4; ++i) {
    log_to_debug_file(g_logfilename, params[i].second, glGetString(params[i].first));
  }

  // one integer for next set of fifteen parameters
  for (; i < 19; ++i) {
    GLint val;
    glGetIntegerv(params[i].first, &val);
    log_to_debug_file(g_logfilename, params[i].second, val);
  }

  // two integers for next parameter
  GLint dim[2];
  glGetIntegerv(params[19].first, dim);
  log_to_debug_file(g_logfilename, params[19].second, dim[0], dim[1]);

  // bool for next parameter
  GLboolean flag;
  glGetBooleanv(params[20].first, &flag);
  log_to_debug_file(g_logfilename, params[20].second, static_cast<GLint>(flag));

  log_to_debug_file(g_logfilename, "-----------------------------");

  // enumerate extensions
  GLint maxExtensions;
  glGetIntegerv(GL_NUM_EXTENSIONS, &maxExtensions);
  for (GLint i = 0; i < maxExtensions; ++i) {
    log_to_debug_file(g_logfilename, i + 1, ": ", glGetStringi(GL_EXTENSIONS, i));
  }
}

/*  _________________________________________________________________________*/
/*! create_log_file
Starts a new debug log file with current time and data stamps followed by
timestamp of application build.

@param none

@return bool
Returns GL_TRUE if debug log file was successfully created, GL_FALSE otherwise.

The function writes to a file whose name is defined at file scope by
object g_logfilename (of type std::string)
*/
GLboolean create_log_file() {
  std::ofstream ofs(g_logfilename.c_str(), std::ofstream::out);
  if (!ofs) {
    std::cerr << "ERROR: could not open log file " 
              << g_logfilename << " for writing" << std::endl;
    return GL_FALSE;
  }

  std::time_t curr_time = time(nullptr); // get current time
  // convert current time to C-string format
  ofs << "OpenGL Application Log File local time: " << std::ctime(&curr_time);
  ofs << "Build version: " << __DATE__ << " " << __TIME__ << std::endl << std::endl;
  ofs.close();
  return GL_TRUE;
}
