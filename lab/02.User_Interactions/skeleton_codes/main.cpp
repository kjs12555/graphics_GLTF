///// main.cpp
///// OpenGL 3+, GLSL 1.20, GLEW, GLFW3

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <fstream>
#include <cassert>
#include "../common/vec.hpp"
#include "../common/transform.hpp"

////////////////////////////////////////////////////////////////////////////////
/// 쉐이더 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
GLuint  program;          // 쉐이더 프로그램 객체의 레퍼런스 값
GLint   loc_a_position;   // attribute 변수 a_position 위치
GLint   loc_a_color;      // attribute 변수 a_color 위치
GLint   loc_u_PVM;        // uniform 변수 u_PVM 위치

GLuint  position_buffer;  // GPU 메모리에서 position_buffer의 위치
GLuint  color_buffer;     // GPU 메모리에서 color_buffer의 위치

GLuint create_shader_from_file(const std::string& filename, GLuint shader_type);
void init_shader_program();
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// 변환 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
kmuvcl::math::mat4x4f     mat_model, mat_view, mat_proj;
kmuvcl::math::mat4x4f     mat_PVM;

// 물체의 이동 변환 관련 변수
float   g_translate_x = 0.0, g_translate_y = 0.0, g_translate_z = 0.0;
bool    g_is_animation = false;

void set_transform();
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
/// GLFW 콜백 함수
////////////////////////////////////////////////////////////////////////////////
// 키보드 이벤트 콜백함수
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);


////////////////////////////////////////////////////////////////////////////////
/// 렌더링 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
// per-vertex 3D positions (x, y, z)
GLfloat g_position[] = {
   0.5f,  0.5f,  0.0f,          // 0th vertex position
  -0.5f, -0.5f,  0.0f,          // 1st vertex position
   0.5f, -0.5f,  0.0f,          // 2nd vertex position
};

// per-vertex RGB color (r, g, b)
GLfloat g_color[] = {
  1.0f, 0.0f, 0.0f,             // 0th vertex color (red)
  1.0f, 0.0f, 0.0f,             // 1st vertex color (red)
  1.0f, 0.0f, 0.0f,             // 2nd vertex color (red)
};

void init_buffer_objects();     // VBO init 함수: GPU의 VBO를 초기화하는 함수.
void render_object();           // rendering 함수: 물체(삼각형)를 렌더링하는 함수.
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

  const GLchar * shader_src = shader_string.c_str();
  glShaderSource(shader, 1, (const GLchar **)&shader_src, NULL);
  glCompileShader(shader);

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

  std::cout << "program id: " << program << std::endl;
  assert(program != 0);

  loc_u_PVM = glGetUniformLocation(program, "u_PVM");

  loc_a_position = glGetAttribLocation(program, "a_position");
  loc_a_color = glGetAttribLocation(program, "a_color");
}

void init_buffer_objects()
{
  glGenBuffers(1, &position_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_position), g_position, GL_STATIC_DRAW);

  glGenBuffers(1, &color_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_color), g_color, GL_STATIC_DRAW);
}

void set_transform()
{
  // set camera transformation
  mat_view.set_to_identity();   // extrinsic param
  mat_proj.set_to_identity();   // intrinsic param

  // TODO: set object transformation
  // mat_model.set_to_identity();     
}

/////////////////////////////////////////////////////////////////////
/// TODO: 키보드 콜백 핸들링 함수 작성 - BEGIN
/////////////////////////////////////////////////////////////////////

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

}

/////////////////////////////////////////////////////////////////////
/// TODO: 키보드 콜백 핸들링 함수 작성 - END
/////////////////////////////////////////////////////////////////////

// object rendering: 현재 scene은 삼각형 하나로 구성되어 있음.
void render_object()
{
  glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // 특정 쉐이더 프로그램 사용
  glUseProgram(program);

  // TODO: Setting PVM = Proj * View * Model


  // 앞으로 언급하는 배열 버퍼(GL_ARRAY_BUFFER)는 position_buffer로 지정
  glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
  // 버텍스 쉐이더의 attribute 중 a_position 부분 활성화
  glEnableVertexAttribArray(loc_a_position);
  // 현재 배열 버퍼에 있는 데이터를 버텍스 쉐이더 a_position에 해당하는 attribute와 연결
  glVertexAttribPointer(loc_a_position, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

  // 앞으로 언급하는 배열 버퍼(GL_ARRAY_BUFFER)는 color_buffer로 지정
  glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
  // 버텍스 쉐이더의 attribute 중 a_color 부분 활성화
  glEnableVertexAttribArray(loc_a_color);
  // 현재 배열 버퍼에 있는 데이터를 버텍스 쉐이더 a_color에 해당하는 attribute와 연결
  glVertexAttribPointer(loc_a_color, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

  // 삼각형 그리기
  glDrawArrays(GL_TRIANGLES, 0, 3);

  // 정점 attribute 배열 비활성화
  glDisableVertexAttribArray(loc_a_position);
  glDisableVertexAttribArray(loc_a_color);
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
  window = glfwCreateWindow(500, 500, "Hello Triangle", NULL, NULL);
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
  init_buffer_objects();

  /// TODO: 키보드 콜백함수 등록


  // Loop until the user closes the window
  while (!glfwWindowShouldClose(window))
  {
    // Poll for and process events
    glfwPollEvents();

    // TODO: 모델 행렬 설정
    
    
    render_object();


    // TODO: 애니메이션 설정
    

    // Swap front and back buffers
    glfwSwapBuffers(window);
  }

  glfwTerminate();

  return 0;
}