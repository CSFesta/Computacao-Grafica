// testegl.cpp : Este arquivo contém a função 'main'. A execução do programa começa e termina ali.
//

 // incluir GLEW antes da GLFW para usar versões mais recentes da OpenGL 
#include <GL/glew.h> /* include GLEW and new version of GL on Windows */ 
#include <GLFW/glfw3.h>
#include <iostream>

void resize(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	std::cout << "janela redimensionada" << std::endl;
}

void logErro(int cod, const char* description) {

	std::cout << description << std::endl;
}

const char* vertexShaderSource = R"(
	#version 400 core
	layout(location=0) in vec3 vp;
	layout(location=1) in vec3 vc;

	out vec3 color; 

	void main(){
		color = vc;
		gl_Position = vec4(vp,1.0);
	}
)";


const char* fragmentShaderSource = R"(
	#version 400 core
	in vec3 color;
	out vec4 frag_color;
	void main(){
		frag_color = vec4(color,1.0);
	}
)";

int main() {

	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return 1;
	}
	/* Caso necessário, definições específicas para SOs, p. e. Apple OSX *
		 /* Definir como 3.2 para Apple OS X */
		 /*glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
		 glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 2);
		 glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		 glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);*/
	GLFWwindow* window = glfwCreateWindow(
		640, 480, "Teste de versão OpenGL", NULL, NULL);
	if (!window) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(window);
	glfwSetWindowSizeCallback(window, resize);
	glfwSetErrorCallback(logErro);
	// inicia manipulador da extensão GLEW 
	glewExperimental = GL_TRUE;
	glewInit();
	// obtenção de versão suportada da OpenGL e renderizador 
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	printf("Renderer: %s\n", renderer);
	printf("OpenGL (versão suportada) %s\n", version);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);


	GLfloat vertices[] = {
		0.5f, -0.5f, 0.0f, 0.2f, 0.7f, 0.3f, // Bottom Right
		-0.5f, -0.5f, 0.0f, 0.5f, 0.4f, 0.7f, // Bottom Left
		0.0f, 0.5f, 0.0f, 0.5f, 0.3f, 0.4f // Top
	};


	//criando vao e vbo para os vértices
	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	//configuração de VAO e VBO
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//configuração do atributo de posição
	GLuint posAtrrib = 0;
	glVertexAttribPointer(posAtrrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLfloat*)0);
	glEnableVertexAttribArray(posAtrrib);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
		6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	//Criando e compilando um Vertex Shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	//criando o fragment Shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	//cria o programa de shader na GPU e linka com os shaders anteriores
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	//depois de passar para a GPU podemos deletar os shaders da CPU
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//obtém a localização do uniform uCOLOR
	//GLuint colorUniformLocation = glGetUniformLocation(shaderProgram, "uColor");


	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shaderProgram);
		//glUniform3f(colorUniformLocation, 0.0f, 0.0f, 0.0f);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);

	// encerra contexto GL e outros recursos da GLFW 
	glfwTerminate();
	return 0;
}