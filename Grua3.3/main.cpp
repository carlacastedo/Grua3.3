//grua con OpenlGL 3.3 de Antonio Farina Elorza y Carla Castedo Pereira
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
#define ESCALADO_CAM 4
#define ESCALADO_1P 0.5
#define ESCALADO_3P 1
#define ESCALADO_ENFOQUE 10
#define ALTURA_1P 0.4
#define ALTURA_3P 0.5

//ancho y alto de la ventana
unsigned int ANCHO = 800;
unsigned int ALTO = 800;

//velocidad de la grua
float velocidad = 0;
//camara (por defecto alejada)
unsigned int modoCamara = 0;

GLuint shaderProgram;

unsigned int VAOEjes;
unsigned int VAOCuadrado;
unsigned int VAOCubo;
unsigned int VAOEsfera;
//angulos de giro de la camara alejada
float alfa = 0, beta = 0;

typedef struct {
	float px, py, pz; //posición inicial
	float angulo_trans; //angulo giro x
	float angulo_trans_2; //angulo giro z
	float sx, sy, sz; //escalado en los dos ejes
	unsigned int *listarender; //VAO
	unsigned int numvertices; //Número de vérices del poligono
} objeto;

objeto base = {0, 0, 0.10, 0, 0, 0.5, 0.2, 0.2, &VAOCubo, 36};
objeto articulacion1 = {0, 0, 0.10, 0, 0, 0.07, 0.07, 0.07, &VAOEsfera, 1080};
objeto brazo1 = {0, 0, 0.10, 0, 0, 0.05, 0.05, 0.3, &VAOCubo, 36};
objeto articulacion2 = {0, 0, 0.15, 0, 0, 0.05, 0.05, 0.05, &VAOEsfera, 1080};
objeto brazo2 = { 0, 0, 0.11, 0, 0, 0.03, 0.03, 0.2, &VAOCubo, 36};

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
extern GLuint setShaders(const char* nVertix, const char* nFrag);

void camaraAlejada() {
	glViewport(0, 0, ANCHO, ALTO);
	//Matriz de vista
	glm::mat4 view;
	//Cargamos la identidad
	view = glm::mat4();
	//colocamos la camara, a donde mira y su orientacion
	view = glm::lookAt(glm::vec3(ESCALADO_CAM* sin(alfa * ARADIANES) * cos(beta * ARADIANES), ESCALADO_CAM * sin(beta * ARADIANES), ESCALADO_CAM * cos(alfa * ARADIANES) * cos(beta * ARADIANES)), glm::vec3(.0f, .0f, .0f), glm::vec3(.0f, cos(beta * ARADIANES), .0f));
	unsigned int viewLoc = glad_glGetUniformLocation(shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	//Matriz de proyección
	glm::mat4 projection;
	//Cargamos la identidad
	projection = glm::mat4();
	projection = glm::perspective(45.0f,(float) ANCHO/ (float) ALTO, 0.01f, 10.0f);
	unsigned int projectionLoc = glad_glGetUniformLocation(shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void terceraPersona(float px, float py, float pz, float angulo) {
	glViewport(0, 0, ANCHO, ALTO);
	//Matriz de vista
	glm::mat4 view;
	//Cargamos la identidad
	view = glm::mat4();
	view = glm::lookAt(glm::vec3(px - ESCALADO_3P * cos(angulo * ARADIANES), py - ESCALADO_3P * sin(angulo * ARADIANES), pz + ALTURA_3P),
		glm::vec3(px + ESCALADO_ENFOQUE * cos(angulo * ARADIANES), py + ESCALADO_ENFOQUE * sin(angulo * ARADIANES), pz),
		glm::vec3(.0f, .0f, 1.0f));
	unsigned int viewLoc = glad_glGetUniformLocation(shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	//Matriz de proyección
	glm::mat4 projection;
	//Cargamos la identidad
	projection = glm::mat4();
	projection = glm::perspective(45.0f, (float)ANCHO / (float)ALTO, 0.01f, 6.0f);
	unsigned int projectionLoc = glad_glGetUniformLocation(shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void primeraPersona(float px, float py, float pz, float angulo) {
	glViewport(0, 0, ANCHO, ALTO);
	//Matriz de vista
	glm::mat4 view;
	//Cargamos la identidad
	view = glm::mat4();
	view = glm::lookAt(glm::vec3(px-ESCALADO_1P*cos(angulo*ARADIANES),py-ESCALADO_1P*sin(angulo*ARADIANES), pz + ALTURA_1P),
		glm::vec3(px + ESCALADO_ENFOQUE * cos(angulo * ARADIANES), py + ESCALADO_ENFOQUE * sin(angulo * ARADIANES), pz),
		glm::vec3(.0f, .0f, 1.0f));
	unsigned int viewLoc = glad_glGetUniformLocation(shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	//Matriz de proyección
	glm::mat4 projection;
	//Cargamos la identidad
	projection = glm::mat4();
	projection = glm::perspective(45.0f, (float)ANCHO / (float)ALTO, 0.01f, 6.0f);
	unsigned int projectionLoc = glad_glGetUniformLocation(shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void seleccionaCamara() {
	//modo de la camara
	switch (modoCamara) {
		//camara alejada
	case 0:
		camaraAlejada();
		break;
		//camara en primera persona
	case 1:
		primeraPersona(base.px, base.py, base.pz, base.angulo_trans);
		break;
		//camara en tercera persona
	case 3:
		terceraPersona(base.px, base.py, base.pz, base.angulo_trans);
		break;
	default:
		camaraAlejada();
	}
}

//funcion que dibuja el objeto aplicandole las transformaciones previas y las suyas propias, devolviendo una matriz
//con las transformaciones aplicadas
glm::mat4 dibujaObjeto(objeto o, glm::mat4 transform) {
	glm::mat4 transformtemp;
	unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
	//trasladamos
	transform = glm::translate(transform, glm::vec3(o.px, o.py, o.pz));
	//rotaciones con respecto al eje z
	transform = glm::rotate(transform, (float)(o.angulo_trans * ARADIANES), glm::vec3(0.0f, 0.0f, 1.0f));
	//rotaciones con respecto al eje y (solo en las articulaciones)
	transform = glm::rotate(transform, (float)(o.angulo_trans_2 * ARADIANES), glm::vec3(0.0f, 1.0f, 0.0f));
	//guardamos las tranformaciones realizadas en la matriz temporal para que las los siguientes objetos
	transformtemp = transform;
	transform = glm::scale(transform, glm::vec3(o.sx, o.sy, o.sz));
	//La cargo
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
	glBindVertexArray(*(o.listarender));
	glDrawArrays(GL_TRIANGLES, 0, o.numvertices);
	//Devolvemos la matriz para usarla en el siguiente fragmento
	return transformtemp;
}

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
		0.0f, .0f, 0.5f,	 0.0f, 0.0f, 1.0f, // z  
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
	unsigned int VBO;

	float vertices[] = {
		//Cara frontal
		-0.5f, 0.5f,  0.5f,   1.0f, 1.0f, 0.4f, //0
		0.5f,  -0.5f,  0.5f,  0.0f, 0.0f, 1.0f, //2
		0.5f, 0.5f,  0.5f,  0.0f, 0.0f, 1.0f, //1
		-0.5f, 0.5f,  0.5f,   1.0f, 1.0f, 0.4f, //0
		-0.5f,  -0.5f,  0.5f, 0.8f, 0.68f, 0.0f, //3
		0.5f,  -0.5f,  0.5f,  0.0f, 0.0f, 1.0f, //2
		  
		 //Cara posterior
		 -0.5f,  -0.5f,  -0.5f, 1.0f, 0.4f, 0.2f, //7
		 0.5f, 0.5f,  -0.5f,  0.0f, 0.0f, 1.0f, //5
		 0.5f,  -0.5f,  -0.5f,  0.0f, 0.0f, 1.0f, //6
		-0.5f, 0.5f,  -0.5f,   1.0f, 1.0, 0.88f, //4
		 0.5f, 0.5f,  -0.5f,  0.0f, 0.0f, 1.0f, //5
		 -0.5f,  -0.5f,  -0.5f, 1.0f, 0.4f, 0.2f, //7

		 //Cara derecha
		 0.5f, 0.5f,  -0.5f,  0.0f, 0.0f, 1.0f, //5
		 0.5f, 0.5f,  0.5f,  0.0f, 0.0f, 1.0f, //1
		 0.5f,  -0.5f,  0.5f,  0.0f, 0.0f, 1.0f, //2
		 0.5f, 0.5f,  -0.5f,  0.0f, 0.0f, 1.0f, //5
		 0.5f,  -0.5f,  0.5f,  0.0f, 0.0f, 1.0f, //2
		 0.5f,  -0.5f,  -0.5f,  0.0f, 0.0f, 1.0f, //6

		 //Cara izquierda
		 -0.5f, 0.5f,  0.5f,   1.0f, 1.0f, 0.4f, //0
		 -0.5f, 0.5f,  -0.5f,   1.0f, 1.0, 0.88f, //4
		 -0.5f,  -0.5f,  -0.5f, 1.0f, 0.4f, 0.2f, //7
		 -0.5f, 0.5f,  0.5f,   1.0f, 1.0f, 0.4f, //0
		 -0.5f,  -0.5f,  -0.5f, 1.0f, 0.4f, 0.2f, //7
		 -0.5f,  -0.5f,  0.5f, 0.8f, 0.68f, 0.0f, //3
		 
		 //Cara inferior
		 -0.5f,  -0.5f,  -0.5f, 1.0f, 0.4f, 0.2f, //7
		 0.5f,  -0.5f,  -0.5f,  0.0f, 0.0f, 1.0f, //6
		 0.5f,  -0.5f,  0.5f,  0.0f, 0.0f, 1.0f, //2
		 -0.5f,  -0.5f,  -0.5f, 1.0f, 0.4f, 0.2f, //7
		 0.5f,  -0.5f,  0.5f,  0.0f, 0.0f, 1.0f, //2
		  -0.5f,  -0.5f,  0.5f, 0.8f, 0.68f, 0.0f, //3

		 //Cara superior
		 0.5f, 0.5f,  -0.5f,  0.0f, 0.0f, 1.0f, //5
		 -0.5f, 0.5f,  -0.5f,   1.0f, 1.0, 0.88f, //4
		 -0.5f, 0.5f,  0.5f,   1.0f, 1.0f, 0.4f, //0
		 0.5f, 0.5f,  0.5f,  0.0f, 0.0f, 1.0f, //1
		 0.5f, 0.5f,  -0.5f,  0.0f, 0.0f, 1.0f, //5
		 - 0.5f, 0.5f,  0.5f,   1.0f, 1.0f, 0.4f //0
	};

	glGenVertexArrays(1, &VAOCubo);
	glGenBuffers(1, &VBO);
	// bind the Vertex Array Object first.
	glBindVertexArray(VAOCubo);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// position Color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glDeleteBuffers(1, &VBO);
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
			//trasladamos para dibujar cada cuadrado
			transform = glm::translate(transform, glm::vec3(i, j, 0.0f));
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

void actualizaPosicion() {
	//actualizacion de la posicion de la base con la velocidad
	base.px += velocidad * cos(base.angulo_trans * ARADIANES);
	base.py += velocidad * sin(base.angulo_trans * ARADIANES);
	//Corte al salir del suelo
	if (base.px >= 2.0 - base.sx / 2.0) base.px = -1.9 + base.sx / 2.0;
	if (base.py >= 2.0 - base.sx / 2.0) base.py = -1.9 + base.sx / 2.0;
	if (base.px <= -2.0 + base.sx / 2.0) base.px = 2.0 - base.sx / 2.0;
	if (base.py <= -2.0 + base.sx / 2.0) base.py = 2.0 - base.sx / 2.0;
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

	GLFWwindow* window = glfwCreateWindow(ANCHO, ALTO, "Grua con OpenGL 3.3", NULL, NULL);
	if (window == NULL)	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

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
	printf("Controles de la camara:\n\t0: Camara alejada\n\t1: Camara en primera persona\n\t3: Camara en tercera persona\n\tFlechas: Mover la camara (solo si esta alejada)\n");
	printf("Controles de la grua:\nBase:\n\tw: Acelera\n\tx: Frena\n\ts: Detiene la grua\n\ta: Rota a la izquierda\n\td: Rota a la derecha\n\tr: Resetea la grua a sus posiciones iniciales\n");
	printf("Primera articulacion:\n\ti: Arriba\n\tk: Abajo\n\tj: Rota izquierda\n\tl: Rota derecha\n");
	printf("Segunda articulacion:\n\th: Arriba\n\tn: Abajo\n\tb: Rota izquierda\n\tm: Rota derecha\n");
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
		seleccionaCamara();
		//creamos las matrices del modelo
		glm::mat4 transform; //es la matriz de transformación
		//la busco en el shader
		unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
		//Dibujo del suelo
		dibujaSuelo(transform, transformLoc);
		actualizaPosicion();

		//dibujamos la grua
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		transform = glm::mat4(); //identity matrix
		transform = dibujaObjeto(base, transform);
		transform = dibujaObjeto(articulacion1, transform);
		transform = dibujaObjeto(brazo1, transform);
		transform = dibujaObjeto(articulacion2, transform);
		transform = dibujaObjeto(brazo2, transform);


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
	switch (key){
		//MOVIMIENTO DE LA CAMARA ALEJADA
		case 265://flecha arriba
			if (modoCamara == 0) {
				beta += 1;
			}
			break;
		case 264://flecha abajo
			if (modoCamara == 0) {
				beta -= 1;
			}
			break;
		case 262://flecha derecha
			if (modoCamara == 0) {
				alfa -= 1;
			}
			break;
		case 263://flecha izquierda
			if (modoCamara == 0) {
				alfa += 1;
			}
			break;

		//MOVIMIENTO DE LA GRUA
		case 65://Letra A, orienta la base a la izquierda
			base.angulo_trans += 1;
			break;
		case 68://Letra D, orienta la base a la derecha
			base.angulo_trans -= 1;
			break;
		case 87://Letra W, aumenta la velocidad
			if (action == GLFW_RELEASE) {
				velocidad += 0.001;
			}
			break;
		case 88://Letra X, reduce la velocidad
			if (action == GLFW_RELEASE) {
				velocidad -= 0.001;
			}
			break;
		case 83://Letra S, detiene la grua
			velocidad = 0;
			break;
		case 82://tecla R, reset de la grua a sus posiciones iniciales
			base.angulo_trans = 0;
			base.px = 0;
			base.px = 0;
			articulacion1.angulo_trans = 0;
			articulacion1.angulo_trans_2 = 0;
			articulacion2.angulo_trans = 0;
			articulacion2.angulo_trans_2 = 0;
			velocidad = 0;
			alfa = 0;
			beta = 0;
			break;

		//MOVIMIENTO DE LA PRIMERA ARTICULACION
		case 74://Letra J orienta el primer brazo a la izquierda
			articulacion1.angulo_trans += 1;
			break;
		case 76://Letra L orienta el primer brazo a la derecha
			articulacion1.angulo_trans -= 1;
			break;
		case 73://Letra I orienta el primer brazo arriba
			if (articulacion1.angulo_trans_2 < 65) articulacion1.angulo_trans_2 += 1;
			break;
		case 75://Letra K orienta el primer brazo abajo
			if (articulacion1.angulo_trans_2 >- 65) articulacion1.angulo_trans_2 -= 1;
			break;

		//MOVIMIENTO DE LA SEGUNDA ARTICULACION
		case 66://Letra B orienta el segundo brazo a la izquierda
			articulacion2.angulo_trans += 1;
			break;
		case 77://Letra M orienta el segundo brazo a la derecha
			articulacion2.angulo_trans -= 1;
			break;
		case 72://Letra H orienta el segundo brazo arriba
			if (articulacion2.angulo_trans_2 < 135) articulacion2.angulo_trans_2 += 1;
			break;
		case 78://Letra N orienta el segundo brazo abajo
			if (articulacion1.angulo_trans_2 > -135) articulacion2.angulo_trans_2 -= 1;
			break;
		//MODOS DE CAMARA
		case 49://Numero 1, primera persona
			modoCamara = 1;
			break;
		case 51://Numero 3, tercera persona
			modoCamara = 3;
			break;
		case 48://Numero 0, camara alejada
			modoCamara = 0;
			break;
		default:
			break;
	}
}

//funcion de reescalado
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	ANCHO = width;
	ALTO = height;
}
