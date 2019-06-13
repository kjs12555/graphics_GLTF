#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <fstream>
#include <cassert>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "../glTF/tiny_gltf.h"
#define BUFFER_OFFSET(i) ((char*)0 + (i))

#include "../common/transform.hpp"

const float MATH_PI = 3.14159265358979323846f;

template <typename T>
inline T rad2deg(T deg)
{
    T rad = deg * (180.0f / MATH_PI);
    return rad;
}

template <typename T>
inline T deg2rad(T rad)
{
    T deg = rad * (MATH_PI / 180.0f);
    return deg;
}


////////////////////////////////////////////////////////////////////////////////
/// 쉐이더 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
GLuint  program;          // 쉐이더 프로그램 객체의 레퍼런스 값
GLint   loc_a_position;   // attribute 변수 a_position 위치
GLint   loc_u_PVM; // uniform 변수 u_PVM 위치

GLuint  position_buffer;  // GPU 메모리에서 position_buffer의 위치
GLuint  index_buffer;     

GLuint create_shader_from_file(const std::string& filename, GLuint shader_type);
void init_shader_program();
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// 렌더링 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
tinygltf::Model model;

bool load_model(tinygltf::Model &model, const std::string filename);
void init_buffer_objects();     // VBO init 함수: GPU의 VBO를 초기화하는 함수.

void render_object();           // rendering 함수: 물체(삼각형)를 렌더링하는 함수.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// 변환 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
kmuvcl::math::mat4x4f     mat_model, mat_view, mat_proj;
kmuvcl::math::mat4x4f     mat_PVM;

void set_transform();
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// 카메라 및 뷰포트 관련 변수
////////////////////////////////////////////////////////////////////////////////
int camera_index = 0;
float g_aspect = 1.0f;
////////////////////////////////////////////////////////////////////////////////


// GLSL 파일을 읽어서 컴파일한 후 쉐이더 객체를 생성하는 함수
GLuint create_shader_from_file(const std::string& filename, GLuint shader_type)
{
  GLuint shader = 0;

  shader = glCreateShader(shader_type);

  std::ifstream shader_file(filename.c_str());
  std::string shader_string;

  shader_string.assign(
    (std::istreambuf_iterator<char>(shader_file)),
    std::istreambuf_iterator<char>());

  // Get rid of BOM in the head of shader_string
  // Because, some GLSL compiler (e.g., Mesa Shader compiler) cannot handle UTF-8 with BOM
  if (shader_string.compare(0, 3, "\xEF\xBB\xBF") == 0)  // Is the file marked as UTF-8?
  {
    std::cout << "Shader code (" << filename << ") is written in UTF-8 with BOM" << std::endl;
    std::cout << "  When we pass the shader code to GLSL compiler, we temporarily get rid of BOM" << std::endl;
    shader_string.erase(0, 3);                  // Now get rid of the BOM.
  }

  const GLchar* shader_src = shader_string.c_str();
  glShaderSource(shader, 1, (const GLchar * *)& shader_src, NULL);
  glCompileShader(shader);

  GLint is_compiled;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &is_compiled);
  if (is_compiled != GL_TRUE)
  {
    std::cout << "Shader COMPILE error: " << std::endl;

    GLint buf_len;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &buf_len);

    std::string log_string(1 + buf_len, '\0');
    glGetShaderInfoLog(shader, buf_len, 0, (GLchar *)log_string.c_str());

    std::cout << "error_log: " << log_string << std::endl;

    glDeleteShader(shader);
    shader = 0;
  }

  return shader;
}

// vertex shader와 fragment shader를 링크시켜 program을 생성하는 함수
void init_shader_program()
{
    GLuint vertex_shader
        = create_shader_from_file("./shader/vertex.glsl", GL_VERTEX_SHADER);

    std::cout << "vertex_shader id: " << vertex_shader << std::endl;
    assert(vertex_shader != 0);

    GLuint fragment_shader
        = create_shader_from_file("./shader/fragment.glsl", GL_FRAGMENT_SHADER);

    std::cout << "fragment_shader id: " << fragment_shader << std::endl;
    assert(fragment_shader != 0);

    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    GLint is_linked;
    glGetProgramiv(program, GL_LINK_STATUS, &is_linked);
    if (is_linked != GL_TRUE)
    {
      std::cout << "Shader LINK error: " << std::endl;

      GLint buf_len;
      glGetProgramiv(program, GL_INFO_LOG_LENGTH, &buf_len);

      std::string log_string(1 + buf_len, '\0');
      glGetProgramInfoLog(program, buf_len, 0, (GLchar *)log_string.c_str());

      std::cout << "error_log: " << log_string << std::endl;

      glDeleteProgram(program);
      program = 0;
    }

    std::cout << "program id: " << program << std::endl;
    assert(program != 0);

    loc_u_PVM = glGetUniformLocation(program, "u_PVM");

    loc_a_position = glGetAttribLocation(program, "a_position");    
}

bool load_model(tinygltf::Model &model, const std::string filename)
{
	tinygltf::TinyGLTF loader;
	std::string err;
	std::string warn;

	bool res = loader.LoadASCIIFromFile(&model, &err, &warn, filename);
	if (!warn.empty())
	{
		std::cout << "WARNING: " << warn << std::endl;
	}

	if (!err.empty())
	{
		std::cout << "ERROR: " << err << std::endl;
	}

	if (!res)
	{
		std::cout << "Failed to load glTF: " << filename << std::endl;
	}
	else
	{
		std::cout << "Loaded glTF: " << filename << std::endl;
	}

	std::cout << std::endl;

	return res;
}

void init_buffer_objects()
{
    
}

void set_transform()
{
	
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_Q && action == GLFW_PRESS)
	{
		camera_index = camera_index == 0 ? 1 : 0;
	}
}

// object rendering: 현재 scene은 삼각형 하나로 구성되어 있음.
void render_object()
{
    // 특정 쉐이더 프로그램 사용
    glUseProgram(program);
    

    // 쉐이더 프로그램 사용해제
    glUseProgram(0);
}

int main(void)
{
    GLFWwindow* window;

    // Initialize GLFW library
    if (!glfwInit())
        return -1;

    // Create a GLFW window containing a OpenGL context
    window = glfwCreateWindow(500, 500, "Hello Cameras with glTF 2.0", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    // Make the current OpenGL context as one in the window
    glfwMakeContextCurrent(window);

    // Initialize GLEW library
    if (glewInit() != GLEW_OK)
        std::cout << "GLEW Init Error!" << std::endl;

    // Print out the OpenGL version supported by the graphics card in my PC
    std::cout << glGetString(GL_VERSION) << std::endl;

    init_shader_program();

    load_model(model, "Cameras.gltf");

    // GPU의 VBO를 초기화하는 함수 호출
    init_buffer_objects();

    glfwSetKeyCallback(window, key_callback);
	
    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        set_transform();
        render_object();

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}