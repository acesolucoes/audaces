#include <stdio.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

bool firstMouse = true;
int lastX;
int lastY;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

GLFWwindow* window = nullptr;

int shaderProgram;
unsigned int VAO, VBO;

glm::vec3 posCamera = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
float angle = 0;

void bindUniforms(int shaderProgram) {
	glm::mat4 projection;
	projection = glm::perspective(glm::radians(45.0f), 1024.0f / 768.0f, 0.1f, 100.0f);
	
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::translate(view, -posCamera); 
	
	glm::mat4 model = glm::mat4(1.0f);
		
	int projectionLocation = glGetUniformLocation(shaderProgram, "proj");
	auto proj = projection * view * model;
	
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(proj));
}

void initShader() {
	const char *vertexShaderSource = "#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"layout (location = 1) in vec3 aColor;\n"
		"out vec3 color;\n"
		"uniform mat4 proj;"
		"void main()\n"
		"{\n"
		"   color = aColor;\n"
		"   gl_Position = proj * vec4(aPos, 1.0);\n"
		"}\0";
	const char *fragmentShaderSource = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"in vec3 color;\n"
		"void main()\n"
		"{\n"
		"   FragColor = vec4(color, 1.0f);\n"
		"}\n\0";

	int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
	glCompileShader(vertexShader);

	int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
	glCompileShader(fragmentShader);

	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void initBuffers() {
	const static float vertices[] = {
		0.5f, -0.5f, 0, 	1.0f, 1.0f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0, 1.0f, 1.0f, 0.0f, // bottom left
		0.0f, 0.5f, 0, 	1.0f, 1.0f, 0.0f    // top 
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0); 
	glBindVertexArray(0); 
}

void drawTriangle() {
	bindUniforms(shaderProgram);
	glUseProgram(shaderProgram);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

bool initGL() {
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW\n");
		return false;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(1024, 768, "Audaces CG Internship Test", NULL, NULL);

	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not OpenGL 3.3 compatible.\n");
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glewExperimental = true; // http://glew.sourceforge.net/basic.html
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetKeyCallback(window, key_callback);

	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	return true;
}

int main() {
	if (!initGL()) {
		return -1;
	}

	initBuffers();
	initShader();
	
	do {		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		drawTriangle();

		glfwSwapBuffers(window);
		glfwPollEvents();

	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && !glfwWindowShouldClose(window));

	glDeleteBuffers(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);

	glfwTerminate();

	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	
	if( action == GLFW_PRESS )
	{
		switch(key) {
			case GLFW_KEY_W:
				posCamera += glm::vec3(0, 1, 0);
				break;
			case GLFW_KEY_A:
				posCamera += glm::vec3(-1, 0, 0);
				break;
			case GLFW_KEY_S:
				posCamera += glm::vec3(0, -1, 0);
				break;
			case GLFW_KEY_D:
				posCamera += glm::vec3(1, 0, 0);
				break;
		}
			
	}		
}