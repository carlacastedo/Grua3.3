#include <glad.h>
#include <glfw3.h>
#include <stdio.h>
#include <math.h> 
#include <iostream>
#include "lecturaShader.h"
//transformaciones
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define ARADIANES 0.0174

void processInput(GLFWwindow* window);

//settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

extern GLuint setShaders(const char* nVertix, const char* nFrag);
GLuint shaderProgram;

unsigned int VAO;
unsigned int VAOCuadrado;
unsigned int VAOCubo;
float angulo;
//esfera esfera1(1.0f, 18, 9);

typedef struct {
	float px, py, pz; //posición inicial
	float angulo_trans; //angulo giro x
	float angulo_trans_2; //angulo giro z
	float sx, sy, sz; //escalado en los dos ejes
	unsigned int listarender; //VAO
} objeto;

objeto base = { 0,0,0.15,0,0,.3,.2,0 };
objeto baseA1 = { 0,0,0.10,0,0,.07,.07,0 };
objeto base1 = { 0,0,0.10,0,0,.05,.05,.3,0 };
objeto baseA2 = { 0,0,0.15,0,0,.05,.05,.05,0 };

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

void dibujaEjes() {

};

void dibujaCubo() {

};

void dibujaCuadrado() {

};

void openGlInit() {
	glClearDepth(1.0f); //Valor z-buffer
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //valor limpieza buffer color
	glEnable(GL_DEPTH_TEST); //z-buffer
	//glEnable(GL_CULL_FACE); //ocultacion caras back
	glCullFace(GL_BACK);
}

int main() {
	//glfw: initalize and configure
	//-----------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Creo la ventana

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Clases", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);


	// glad
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	glfwSetKeyCallback(window, keyCallback);
	openGlInit();

	//generarShader();
	shaderProgram = setShaders("shader.vert", "shader.frag");

	dibujaEjes();
	dibujaCuadrado();
	dibujaCubo();

	// Lazo de la ventana mientras no la cierre
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);//Borro el buffer de la ventana
		glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
		
		//Dibujo cuadrado

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		glm::mat4 transform; //es la matriz de transformación
		glm::mat4 transformtemp; //es la matriz de transformación
		//la busco en el shader
		unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
		float i, j;
		float escalasuelo = 10;
		for (i = -2; i <= 2; i += (1 / escalasuelo)) {
			for (j = -2; j <= 2; j += (1 / escalasuelo)) {
				//Calculo la matriz
				transform = glm::mat4(); //identity matrix
				transform = glm::rotate(transform, (float)(angulo * ARADIANES), glm::vec3(1.0f, 0.0f, 0.0f));
				transform = glm::translate(transform, glm::vec3(i, j, 0.0f));
				//transform = glm::rotate(transform, angulo, glm::vex3(1.0f,0.0f,0.0f));
				transform = glm::scale(transform, glm::vec3((1 / escalasuelo), (1 / escalasuelo), (1 / escalasuelo)));
				//La cargo
				glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
				glBindVertexArray(VAOCuadrado);
				glDrawArrays(GL_TRIANGLES, 0, 6);
			}
		}
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:

	glDeleteVertexArrays(1, &VAO);


	// glfw: terminate, clearing all 

	glfwTerminate();
	return 0;
}
