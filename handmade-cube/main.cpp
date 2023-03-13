#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>

using vec3 = glm::vec<3, float>;
using mat4 = glm::mat<4, 4, float>;
const float F_PI = (float)M_PI;

void windowSizeCallback(GLFWwindow *wind, int w, int h)
{
	glViewport(0, 0, w, h);
}

class Window
{
private:
	GLFWwindow *m_window;
public:
	Window()
		: m_window(nullptr)
	{
		glfwInit();
	}

	~Window()
	{
		if (m_window != nullptr)
		{
			glfwDestroyWindow(m_window);
		}
		glfwTerminate();
	}

	
	bool init()
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		m_window = glfwCreateWindow(800, 600, "Rotating cube", nullptr, nullptr);
		
		if (m_window == nullptr)
			return false;
		glfwMakeContextCurrent(m_window);
		glfwSetWindowSizeCallback(m_window, windowSizeCallback);

		if (glewInit() != GLEW_OK)
		{
			glfwTerminate();
			return false;
		}
		glEnable(GL_DEPTH_TEST);
		return true;
	}

	void run()
	{
		// generate the necessarry buffers
		unsigned int vao, vbo, ebo;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

		float vertices[] = {
			-1, 1, 1,
			1, 1, 1,
			1, -1, 1,
			-1, -1, 1,

			-1, 1, -1,
			1, 1, -1,
			1, -1, -1,
			-1, -1, -1,
		};
		unsigned int indices[] = {
			// front face
			0, 1, 3,
			1, 2, 3,

			// back face
			4, 5, 7,
			5, 6, 7,

			// left face
			4, 0, 3,
			4, 3, 7,
			
			// right face
			1, 5, 6,
			1, 6, 2,

			// top face
			0, 4, 6,
			0, 6, 1,

			// bottom face
			7, 3, 2,
			7, 2, 6
		};

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
		glEnableVertexAttribArray(0);
		// shader source code
		// todo: put these in seperate files and load them from disk instead
		const char *vss = "#version 330 core\n"
						  "layout (location = 0) in vec3 aPos;"
						  "uniform mat4 viewMat;"
						  "uniform mat4 modelMat;"
						  "uniform mat4 projMat;"
						  "void main()"
						  "{"
						  "	gl_Position = projMat * viewMat * modelMat * vec4(aPos, 1);"
						  "}";
		const char *fss = "#version 330 core\n"
						  "out vec4 color;"
						  "void main()"
						  "{"
						  "	color = vec4(1, 1, 1, 1);"
						  "}";

		// create our handles for the vertex and fragment shaders
		unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
		unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);

		// shader source 
		glShaderSource(vs, 1, &vss, nullptr);
		glShaderSource(fs, 1, &fss, nullptr);

		// compile the shaders
		glCompileShader(vs);
		glCompileShader(fs);

		// create the program
		unsigned int program = glCreateProgram();
		glAttachShader(program, vs);
		glAttachShader(program, fs);

		glLinkProgram(program);
		
		glDeleteShader(vs);
		glDeleteShader(fs);

		glUseProgram(program);

		// setup some transformation matrices
		// model matrix
		mat4 model(1);
		// view matrix
		vec3 cameraPosition(0, 2, 5);
		vec3 cameraLookAt(0, 0, -1);
		mat4 view = glm::lookAt(cameraPosition, cameraLookAt, vec3(0, 1, 0));
		// projection matrix
		mat4 proj = glm::perspective(F_PI/4.0f, 800.0f/600.0f, 0.01f, 1000.0f);

		// get all the uniform locations
		unsigned int modelLoc = glGetUniformLocation(program, "modelMat");
		unsigned int viewLoc = glGetUniformLocation(program, "viewMat");
		unsigned int projLoc = glGetUniformLocation(program, "projMat");

		// rotation stuff
		float rotationSpeed = F_PI / 2.0f;
		float deltaTime = 0.0f;

		while (!glfwWindowShouldClose(m_window))
		{
			// begin calculating delta time
			float frameStartTime = glfwGetTime();

			glfwPollEvents();

			model = glm::rotate(model, rotationSpeed * deltaTime, vec3(0, 1, 0));

			// setting the uniform matrices in the shader
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

			// draw the cube to the screen
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(unsigned int), GL_UNSIGNED_INT, nullptr);
			glfwSwapBuffers(m_window);

			// get the duration of the current frame and use it as the delta time
			deltaTime = glfwGetTime() - frameStartTime;
		}
	}
};

int main()
{
	Window window;
	if (!window.init())
	{
		std::cerr << "Failed to create window" << std::endl;
		return 1;
	}
	window.run();
	return 0;
}
