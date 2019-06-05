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

////////////////////////////////////////////////////////////////////////////////
/// 쉐이더 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
GLuint  program;          // 쉐이더 프로그램 객체의 레퍼런스 값
GLint   loc_a_position;   // attribute 변수 a_position 위치
GLint   loc_a_color;      // attribute 변수 a_color 위치

GLuint  position_buffer;  // GPU 메모리에서 position_buffer의 위치
GLuint  color_buffer;     // GPU 메모리에서 color_buffer의 위치

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

	loc_a_position = glGetAttribLocation(program, "a_position");
	loc_a_color = glGetAttribLocation(program, "a_color");
}

void init_buffer_objects()
{
    const std::vector<tinygltf::Mesh>& meshes = model.meshes;
    const std::vector<tinygltf::Accessor>& accessors = model.accessors;
    const std::vector<tinygltf::BufferView>& bufferViews = model.bufferViews;
    const std::vector<tinygltf::Buffer>& buffers = model.buffers;
    
    for (size_t i=0; i< meshes.size(); ++i)
    {
		const tinygltf::Mesh& mesh = meshes[i];

        if (mesh.name.compare("Triangle") == 0)
        {
			for (size_t j = 0; j < mesh.primitives.size(); ++j)
			{
				const tinygltf::Primitive& primitive = mesh.primitives[j];

				for (std::map<std::string, int>::const_iterator it = primitive.attributes.cbegin();
					it != primitive.attributes.cend();
					++it)
				{
					const std::pair<std::string, int>& attrib = *it;

					const int accessor_index = attrib.second;
					const tinygltf::Accessor& accessor = accessors[accessor_index];

					int bufferView_index = accessor.bufferView;
					const tinygltf::BufferView& bufferView = bufferViews[bufferView_index];
					const tinygltf::Buffer& buffer = buffers[bufferView.buffer];

					if (attrib.first.compare("POSITION") == 0)
					{
						glGenBuffers(1, &position_buffer);
						glBindBuffer(bufferView.target, position_buffer);
						glBufferData(bufferView.target, bufferView.byteLength,
							&buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);
					}
					else if (attrib.first.compare("COLOR_0") == 0)
					{
						glGenBuffers(1, &color_buffer);
						glBindBuffer(bufferView.target, color_buffer);
						glBufferData(bufferView.target, bufferView.byteLength,
							&buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);
					}
				}
			}
        }
    }    
}

// object rendering: 현재 scene은 삼각형 하나로 구성되어 있음.
void render_object()
{
	// 특정 쉐이더 프로그램 사용
	glUseProgram(program);

    const std::vector<tinygltf::Mesh>& meshes = model.meshes;
    const std::vector<tinygltf::Accessor>& accessors = model.accessors;
    const std::vector<tinygltf::BufferView>& bufferViews = model.bufferViews;
    const std::vector<tinygltf::Buffer>& buffers = model.buffers;

	for (size_t i = 0; i < meshes.size(); ++i)
	{
		const tinygltf::Mesh& mesh = meshes[i];

        if (mesh.name.compare("Triangle") == 0)
        {
			for (size_t j = 0; j < mesh.primitives.size(); ++j)
			{
				const tinygltf::Primitive& primitive = mesh.primitives[j];

                int count = 0;

				for (std::map<std::string, int>::const_iterator it = primitive.attributes.cbegin();
					it != primitive.attributes.cend();
					++it)
				{
					const std::pair<std::string, int>& attrib = *it;

                    const int accessor_index = attrib.second;
                    const tinygltf::Accessor& accessor = accessors[accessor_index];
                    
					count = accessor.count;

                    int bufferView_index = accessor.bufferView;
                    const tinygltf::BufferView& bufferView = bufferViews[bufferView_index];                    

                    if (attrib.first.compare("POSITION") == 0)
                    {
                        glBindBuffer(bufferView.target, position_buffer);
                        glEnableVertexAttribArray(loc_a_position);
                        glVertexAttribPointer(loc_a_position,
                            accessor.type, accessor.componentType,
                            accessor.normalized ? GL_TRUE : GL_FALSE, 0,
                            BUFFER_OFFSET(accessor.byteOffset));
                    }
                    else if (attrib.first.compare("COLOR_0") == 0)
                    {
                        glBindBuffer(bufferView.target, color_buffer);
                        glEnableVertexAttribArray(loc_a_color);
                        glVertexAttribPointer(loc_a_color,
                            accessor.type, accessor.componentType,
                            accessor.normalized ? GL_TRUE : GL_FALSE, 0,
                            BUFFER_OFFSET(accessor.byteOffset));
                    }
                }

                glDrawArrays(primitive.mode, 0, count);

                // 정점 attribute 배열 비활성화
                glDisableVertexAttribArray(loc_a_position);
                glDisableVertexAttribArray(loc_a_color);
            }            
        }
    }	

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
	window = glfwCreateWindow(500, 500, "Hello Triangle with glTF 2.0", NULL, NULL);
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
    	
	load_model(model, "simple_triangle.gltf");

    // GPU의 VBO를 초기화하는 함수 호출
	init_buffer_objects();

	// Loop until the user closes the window
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// TODO: 물체(삼각형)를 렌더링하는 함수 호출
		render_object();

		// Swap front and back buffers
		glfwSwapBuffers(window);

		// Poll for and process events
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}