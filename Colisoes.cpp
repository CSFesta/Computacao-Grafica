#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>

// Vertex Shader Source Code
const char* vertexShaderSource = R"(
    #version 400 core
    layout(location = 0) in vec2 aPos;
    layout(location = 1) in vec3 aColor;

    out vec3 vertexColor;
    uniform vec2 offset;

    void main() {
        gl_Position = vec4(aPos + offset, 0.0, 1.0);
        vertexColor = aColor;
    }
)";

// Fragment Shader Source Code
const char* fragmentShaderSource = R"(
    #version 400 core
    in vec3 vertexColor;
    out vec4 FragColor;

    void main() {
        FragColor = vec4(vertexColor, 1.0);
    }
)";

void checkCompileErrors(GLuint shader, std::string type) {
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
                << infoLog << "\n" << std::endl;
        }
    }
    else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
                << infoLog << "\n" << std::endl;
        }
    }
}

// Estrutura para armazenar o triângulo em movimento
struct Square {
    float x, y;        // Posição
    float speedX, speedY; // Velocidade
    float size = 0.2f; // Tamanho do triângulo
};

int main() {
    // Inicializa GLFW
    if (!glfwInit()) {
        std::cerr << "Erro ao inicializar GLFW" << std::endl;
        return -1;
    }

    // Configuração da versão do OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Cria uma janela GLFW
    GLFWwindow* window = glfwCreateWindow(800, 600, "Triângulo em Movimento", NULL, NULL);
    if (!window) {
        std::cerr << "Erro ao criar janela GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Inicializa GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Erro ao inicializar GLEW" << std::endl;
        return -1;
    }

    // Define a área de renderização
    glViewport(0, 0, 800, 600);

    // Dados dos vértices: Posição (x, y) e Cor (r, g, b)
    GLfloat vertices[] = {
        // Posições     // Cores
        -0.1f, -0.1f,  1.0f, 0.0f, 1.0f,  
         0.1f, -0.1f,  1.0f, 0.0f, 1.0f,  
        -0.1f,  0.1f,  1.0f, 0.0f, 1.0f,  
         0.1f,  0.1f,  1.0f, 0.0f, 1.0f   
    };

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Compilar shaders
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    checkCompileErrors(vertexShader, "VERTEX");

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    checkCompileErrors(fragmentShader, "FRAGMENT");

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    checkCompileErrors(shaderProgram, "PROGRAM");

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Posição inicial e velocidade do triângulo
    Square square = { -0.4f, 0.3f, 0.0005f, 0.0003f };
    Square square2 = { 0.8f, 0.7f, 0.0005f, 0.0003f };

    int offsetLocation = glGetUniformLocation(shaderProgram, "offset");
    int count = 0; 
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        // Atualiza a posição dos quadrados
        square.x += square.speedX;
        square.y += square.speedY;

        square2.x += square2.speedX;
        square2.y += square2.speedY;

        // Verifica colisão com as bordas da tela
        if (square.x + square.size > 1.0f || square.x - square.size < -1.0f)
            square.speedX *= -1;
        if (square.y + square.size > 1.0f || square.y - square.size < -1.0f)
            square.speedY *= -1;

        if (square2.x + square2.size > 1.0f || square2.x - square2.size < -1.0f)
            square2.speedX *= -1;
        if (square2.y + square2.size > 1.0f || square2.y - square2.size < -1.0f)
            square2.speedY *= -1;

        bool colideX = square.x + square.size*0.6 > square2.x - square2.size*0.6 &&
            square.x - square.size*0.6 < square2.x + square2.size*0.6;

        bool colideY = square.y + square.size*0.6 > square2.y - square2.size*0.6 &&
            square.y - square.size*0.6 < square2.y + square2.size*0.6;

        if (colideX && colideY) {
            if (count % 2 == 0) {
                square.speedX *= 1;
                square.speedY *= 1;
                square2.speedX *= 1;
                square2.speedY *= 1;
            }
            if (count % 2 == 1) {
                square.speedX *= -1;
                square.speedY *= -1;
                square2.speedX *= -1;
                square2.speedY *= -1;
            }
            
        }
        count++;

        glUniform2f(offsetLocation, square.x, square.y);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glUniform2f(offsetLocation, square2.x, square2.y);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}