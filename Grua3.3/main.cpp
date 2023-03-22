#include <glad.h>
#include <glfw3.h>
#include <stdio.h>
#include <math.h> 
#include <iostream>
#include <lecturaShader.h>
#include "esfera.h"


//transformaciones
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//equivalencia a radianes
#define ARADIANES 0.0174

void processInput(GLFWwindow* window);

//settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

extern GLuint setShaders(const char* nVertix, const char* nFrag);
GLuint shaderProgram;

unsigned int VAOEjes;
unsigned int VAOCuadrado;
unsigned int VAOCubo;
unsigned int VAOEsfera;
float angulo_x, angulo_z;

typedef struct {
	float px, py, pz; //posición inicial
	float angulo_trans; //angulo giro x
	float angulo_trans_2; //angulo giro z
	float sx, sy, sz; //escalado en los dos ejes
	unsigned int listarender; //VAO
} objeto;

objeto base = {0, 0, 0.10, 0, 0, 0.3, 0.2, 0.2, 0};
objeto baseA1 = {0, 0, 0.10, 0, 0, 0.07, 0.07, 0.07, 0};
objeto base1 = {0, 0, 0.10, 0, 0, 0.05, 0.05, 0.3, 0};
objeto baseA2 = {0, 0, 0.15, 0, 0, 0.05, 0.05, 0.05, 0};
objeto base2 = { 0, 0, 0.11, 0, 0, 0.05, 0.05, 0.3, 0 };

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void dibujaEsfera() {
	unsigned int VBO;

	glGenVertexArrays(1, &VAOEsfera);
	glGenBuffers(1, &VBO);
	// bind the Vertex Array Object first.
	glBindVertexArray(VAOEsfera);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_esfera), vertices_esfera, GL_STATIC_DRAW);

	//Normales (COLOR)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	////Textura
	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	//glEnableVertexAttribArray(2);

	//Vertices
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glDeleteBuffers(1, &VBO);
}
void dibujaEjes() {
	unsigned int VBO, EBO;
	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		//Vertices          //Colores
		0.0f, 0.0f, 0.0f,	 1.0f, 1.0f, 1.0f,  // 0
		.5f, 0.0f, 0.0f,	 1.0f, 0.0f, 0.0f, //x
		0.0f, .5f, 0.0f,	 0.0f, 1.0f, 0.0f,// y
		0.0f, .5f, 0.0f,	 0.0f, 0.0f, 1.0f, // z  
		.5f , .5f, 0.5f,	1.0f, 1.0f, 1.0f // 1,1,1 bueno realmente la mitad
	};
	unsigned int indices[] = {  // empieza desde cero
		0, 1,
		0, 2,
		0, 3,
		0, 4
	};

	glGenVertexArrays(1, &VAOEjes);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAOEjes);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// position Color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
};

void dibujaCubo() {
	unsigned int VBO, EBO;

	float vertices[] = {
		-0.5f, 0.5f,  0.5f,   1.0f, 1.0f, 0.4f, //0
		 0.5f, 0.5f,  0.5f,  1.0f, 0.85f, 0.47f, //1
		 0.5f,  -0.5f,  0.5f,  1.0f, 0.84f, 0.0f, //2
		 -0.5f,  -0.5f,  0.5f, 0.8f, 0.68f, 0.0f, //3
		-0.5f, 0.5f,  -0.5f,   1.0f, 1.0, 0.88f, //4
		 0.5f, 0.5f,  -0.5f,  0.68f, 1.0f, 0.18f, //5
		 0.5f,  -0.5f,  -0.5f,  1.0f, 0.65f, 0.0f, //6
		 -0.5f,  -0.5f,  -0.5f, 1.0f, 0.4f, 0.2f //7
	};

	unsigned int indices[] = {
		0,2,1,0,3,2,	//cara frontal
		7,5,6,4,5,7,	//cara posterior
		5,1,2,5,2,6,	//cara derecha
		0,4,7,0,7,3,	//cara izquierda
		7,6,2,7,2,3,	//cara inferior
		5,4,0,1,5,0		//cara superior
	};

	glGenVertexArrays(1, &VAOCubo);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// bind the Vertex Array Object first.
	glBindVertexArray(VAOCubo);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// position Color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
};

void dibujaCuadrado() {
	unsigned int VBO, EBO;

	//vertices del cuadrado
	float vertices[] = {
		-0.5f, -0.5f,  0.0f,  0.0f, 1.0f, 1.0f, //0
		 0.5f, -0.5f,  0.0f,  0.0f, 0.0f, 1.0f, //1
		 0.5f,  0.5f,  0.0f,  0.0f, 1.0f, 1.0f, //2
		 -0.5f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f //3
	};

	//indices
	unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

	glGenVertexArrays(1, &VAOCuadrado);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// bind the Vertex Array Object first.
	glBindVertexArray(VAOCuadrado);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// position Color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
};

//funcion de dibujo del suelo
void dibujaSuelo(glm::mat4 transform, unsigned int transformLoc) {
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	float i, j;
	float escalasuelo = 10;
	for (i = -2; i <= 2; i += (1 / escalasuelo)) {
		for (j = -2; j <= 2; j += (1 / escalasuelo)) {
			//Calculo la matriz
			transform = glm::mat4(); //identity matrix
			//rotación de la camara al variar el angulo
			transform = glm::rotate(transform, (float)(angulo_x * ARADIANES), glm::vec3(1.0f, 0.0f, 0.0f));
			transform = glm::rotate(transform, (float)(angulo_z * ARADIANES), glm::vec3(0.0f, 0.0f, 1.0f));
			//trasladamos para dibujar cada cuadrado
			transform = glm::translate(transform, glm::vec3(i, j, 0.0f));
			//transform = glm::rotate(transform, angulo, glm::vex3(1.0f,0.0f,0.0f));
			//escalamos
			transform = glm::scale(transform, glm::vec3((1 / escalasuelo), (1 / escalasuelo), (1 / escalasuelo)));
			//La cargo
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
			//dibujamos el cuadrado
			glBindVertexArray(VAOCuadrado);
			glDrawElements(GL_TRIANGLES, 6,GL_UNSIGNED_INT, 0);
		}
	}
}

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
	if (window == NULL)	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);


	// glad
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	glfwSetKeyCallback(window, keyCallback);
	openGlInit();

	//generarShader();
	shaderProgram = setShaders("shader.vert", "shader.frag");
	glUseProgram(shaderProgram);
	dibujaEjes();
	dibujaCuadrado();
	dibujaCubo();
	dibujaEsfera();

	// Lazo de la ventana mientras no la cierre
	// -----------
	while (!glfwWindowShouldClose(window)){
		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);//Borro el buffer de la ventana
		glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

		//creamos las matrices del modelo
		glm::mat4 transform; //es la matriz de transformación
		glm::mat4 transformtemp; //es la matriz de transformación
		//la busco en el shader
		unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
		//Dibujo del suelo
		dibujaSuelo(transform, transformLoc);

		//dibujamos la base de la grua
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		transform = glm::mat4(); //identity matrix
		//rotación de la camara al variar el angulo
		transform = glm::rotate(transform, (float)(angulo_x * ARADIANES), glm::vec3(1.0f, 0.0f, 0.0f));
		transform = glm::rotate(transform, (float)(angulo_z * ARADIANES), glm::vec3(0.0f, 0.0f, 1.0f));
		//trasladamos
		transform = glm::translate(transform, glm::vec3(base.px, base.py, base.pz));
		//guardamos las tranformaciones realizadas en la matriz temporal para que las hereden las rotulas
		transformtemp = transform;
		transform = glm::scale(transform, glm::vec3(base.sx, base.sy, base.sz));
		//La cargo
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
		//dibujamos el cubo
		glBindVertexArray(VAOCubo);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		//dibujamos los ejes
		glBindVertexArray(VAOEjes);
		glDrawElements(GL_LINE, 8, GL_UNSIGNED_INT, 0);
		
		//primera articulacion
		transform = glm::mat4(); //buenas praticas pero no hace falta
		transform = transformtemp;
		transform = glm::translate(transform, glm::vec3(baseA1.px, baseA1.py, baseA1.pz));
		transform = glm::rotate(transform, (float)(baseA1.angulo_trans * ARADIANES), glm::vec3(1.0f, 0.0f, 0.0f));
		transform = glm::rotate(transform, (float)(baseA1.angulo_trans_2 * ARADIANES), glm::vec3(0.0f, 1.0f, 0.0f));
		//guardamos la matriz
		transformtemp = transform;
		//escalamos
		transform = glm::scale(transform, glm::vec3(baseA1.sx, baseA1.sy, baseA1.sz));
		//La cargo
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
		//dibujamos la esfera
		glBindVertexArray(VAOEsfera);
		glDrawArrays(GL_TRIANGLES, 0, 1080);

		//primer brazo
		transform = glm::mat4(); //buenas praticas pero no hace falta
		transform = transformtemp;
		transform = glm::translate(transform, glm::vec3(base1.px, base1.py, base1.pz));
		//guardamos la matriz
		transformtemp = transform;
		//escalamos
		transform = glm::scale(transform, glm::vec3(base1.sx, base1.sy, base1.sz));
		//La cargo
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
		//dibujamos el cubo
		glBindVertexArray(VAOCubo);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		//segunda articulacion
		transform = glm::mat4(); //buenas praticas pero no hace falta
		transform = transformtemp;
		transform = glm::translate(transform, glm::vec3(baseA2.px, baseA2.py, baseA2.pz));
		transform = glm::rotate(transform, (float)(baseA2.angulo_trans * ARADIANES), glm::vec3(1.0f, 0.0f, 0.0f));
		transform = glm::rotate(transform, (float)(baseA2.angulo_trans_2 * ARADIANES), glm::vec3(0.0f, 1.0f, 0.0f));
		//guardamos la matriz
		transformtemp = transform;
		//escalamos
		transform = glm::scale(transform, glm::vec3(baseA2.sx, baseA2.sy, baseA2.sz));
		//La cargo
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
		//dibujamos la esfera
		glBindVertexArray(VAOEsfera);
		glDrawArrays(GL_TRIANGLES, 0, 1080);

		//segundo brazo
		transform = glm::mat4(); //buenas praticas pero no hace falta
		transform = transformtemp;
		transform = glm::translate(transform, glm::vec3(base2.px, base2.py, base2.pz));
		//guardamos la matriz
		transformtemp = transform;
		//escalamos
		transform = glm::scale(transform, glm::vec3(base2.sx, base2.sy, base2.sz));
		//La cargo
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
		//dibujamos el cubo
		glBindVertexArray(VAOCubo);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &VAOCubo);
	glDeleteVertexArrays(1, &VAOCuadrado);
	glDeleteVertexArrays(1, &VAOEjes);
	glDeleteVertexArrays(1, &VAOEsfera);

	// glfw: terminate, clearing all 

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window){
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

//funcion de teclado
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	//angulos de giro con respecto al eje x
	if (key == 265) {//flecha arriba
		angulo_x += 1;
	}
	if (key == 264) {//flecha abajo
		angulo_x -= 1;
	}
	//angulos de giro con respecto al eje z
	if (key == 262) {//flecha derecha
		angulo_z += 1;
	}
	if (key == 263) {//flecha izquierda
		angulo_z -= 1;
	}
	if (key == 65) {//Letra A, orienta la base a la izquierda
		base.angulo_trans -= 1;
	}
	if (key == 68) {//Letra D, orienta la base a la derecha
		base.angulo_trans += 1;
	}
	if (key == 83) {//Letra W, avanza la base hacia adelante
		
	}
	if (key == 87) {//Letra S, avanza la base atrás

	}
	printf("%d\n", key);
}
