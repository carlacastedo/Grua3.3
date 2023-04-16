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


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//equivalencia a radianes
#define ARADIANES 0.0174
#define ESCALADO_CAM 4
#define ESCALADO_1P 0.5
#define ESCALADO_3P 1
#define ESCALADO_ENFOQUE 10
#define ESCALADO_LUZ 1.25
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
	unsigned int* textura;
} objeto;

typedef struct {
	float x, y, z;
}punto;

unsigned int sueloTex;
unsigned int gruaTex;
unsigned int articulacionTex;

objeto base = {0, 0, 0.10, 0, 0, 0.5, 0.2, 0.2, &VAOCubo, 36, &gruaTex};
objeto articulacion1 = {0, 0, 0.10, 0, 0, 0.07, 0.07, 0.07, &VAOEsfera, 1080, &articulacionTex};
objeto brazo1 = {0, 0, 0.20, 0, 0, 0.05, 0.05, 0.45, &VAOCubo, 36, &gruaTex};
objeto articulacion2 = {0, 0, 0.25, 0, 0, 0.04, 0.04, 0.04, &VAOEsfera, 1080, &articulacionTex};
objeto brazo2 = { 0, 0, 0.15, 0, 0, 0.03, 0.03, 0.25, &VAOCubo, 36, &gruaTex};

punto luz;

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
	projection = glm::perspective(45.0f,(float) ANCHO/ (float) ALTO, 0.01f, 6.0f);
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
glm::mat4 dibujaObjeto(objeto o, glm::mat4 model) {
	glm::mat4 modeltemp;
	unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
	//trasladamos
	model = glm::translate(model, glm::vec3(o.px, o.py, o.pz));
	//rotaciones con respecto al eje z
	model = glm::rotate(model, (float)(o.angulo_trans * ARADIANES), glm::vec3(0.0f, 0.0f, 1.0f));
	//rotaciones con respecto al eje y (solo en las articulaciones)
	model = glm::rotate(model, (float)(o.angulo_trans_2 * ARADIANES), glm::vec3(0.0f, 1.0f, 0.0f));
	//guardamos las tranformaciones realizadas en la matriz temporal para que las los siguientes objetos
	modeltemp = model;
	model = glm::scale(model, glm::vec3(o.sx, o.sy, o.sz));
	glBindTexture(GL_TEXTURE_2D, *(o.textura));
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//La cargo
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glBindVertexArray(*(o.listarender));
	glDrawArrays(GL_TRIANGLES, 0, o.numvertices);
	glBindTexture(GL_TEXTURE_2D, 0);
	//Devolvemos la matriz para usarla en el siguiente fragmento
	return modeltemp;
}

void dibujaEsfera() {
	unsigned int VBO;

	glGenVertexArrays(1, &VAOEsfera);
	glGenBuffers(1, &VBO);
	// bind the Vertex Array Object first.
	glBindVertexArray(VAOEsfera);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_esfera), vertices_esfera, GL_STATIC_DRAW);

	//Vertices
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(0);
	//Normales
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	//Textura
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);

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
		//3 coordenadas de vertices, 3 de normales y 2 de textura
		-0.5f, 0.5f,  0.5f,   0.0f, 0.0f, 1.0f, 0.0f, 1.0f, //0
		0.5f,  -0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 1.0f, 0.0f, //2
		0.5f, 0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 1.0f, 1.0f, //1
		-0.5f, 0.5f,  0.5f,   0.0f, 0.0f, 1.0f, 0.0f, 1.0f, //0
		-0.5f,  -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, //3
		0.5f,  -0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 1.0f, 0.0f,//2

		//Cara posterior
		-0.5f,  -0.5f,  -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, //7
		0.5f, 0.5f,  -0.5f,  0.0f, 0.0f, -1.0f, 0.0f, 1.0f, //5
		0.5f,  -0.5f,  -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, //6
	   -0.5f, 0.5f,  -0.5f,   0.0f, 0.0f, -1.0f, 1.0f, 1.0f, //4
		0.5f, 0.5f,  -0.5f,  0.0f, 0.0f, -1.0f, 0.0f, 1.0f, //5
		-0.5f,  -0.5f,  -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, //7

		//Cara derecha
		0.5f, 0.5f,  -0.5f,  1.0f, 0.0f, 0.0f, 1.0f, 1.0f, //5
		0.5f, 0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f, //1
		0.5f,  -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f, //2
		0.5f, 0.5f,  -0.5f,  1.0f, 0.0f, 0.0f, 1.0f, 1.0f,//5
		0.5f,  -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f, //2
		0.5f,  -0.5f,  -0.5f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f, //6

		//Cara izquierda
		-0.5f, 0.5f,  0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, //0
		-0.5f, 0.5f,  -0.5f,   -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, //4
		-0.5f,  -0.5f,  -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, //7
		-0.5f, 0.5f,  0.5f,  -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, //0
		-0.5f,  -0.5f,  -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, //7
		-0.5f,  -0.5f,  0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, //3

		//Cara inferior
		-0.5f,  -0.5f,  -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, //7
		0.5f,  -0.5f,  -0.5f,  0.0f, -1.0f, 0.0f, 1.0f, 0.0f, //6
		0.5f,  -0.5f,  0.5f,  0.0f, -1.0f, 0.0f, 1.0f, 1.0f, //2
		-0.5f,  -0.5f,  -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, //7
		0.5f,  -0.5f,  0.5f,  0.0f, -1.0f, 0.0f, 1.0f, 1.0f,//2
		 -0.5f,  -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,//3

		 //Cara superior
		 0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, //5
		 -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, //4
		 -0.5f, 0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f, //0
		 0.5f, 0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, //1
		 0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, //5
		 -0.5f, 0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f //0
	};

	glGenVertexArrays(1, &VAOCubo);
	glGenBuffers(1, &VBO);
	// bind the Vertex Array Object first.
	glBindVertexArray(VAOCubo);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//posicion
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//normales
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//texturas
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glDeleteBuffers(1, &VBO);
};

void dibujaCuadrado() {
	unsigned int VBO;

	//vertices del cuadrado
	float vertices[] = {
		//3 coordenadas de vertices, 3 de normales y 2 de textura
		-0.5f, -0.5f,  0.0f,  0.0f, 0.0f, 1.0f, 0.0f, 0.0f, //0
		 0.5f, -0.5f,  0.0f,  0.0f, 0.0f, 1.0f, 1.0f, 0.0f, //1
		 0.5f,  0.5f,  0.0f,  0.0f, 0.0f, 1.0f, 1.0f, 1.0f, //2
		 0.5f,  0.5f,  0.0f,  0.0f, 0.0f, 1.0f, 1.0f, 1.0f, //2
		 -0.5f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, //3
		 -0.5f, -0.5f,  0.0f,  0.0f, 0.0f, 1.0f, 0.0f, 0.0f //0
	};
	glGenVertexArrays(1, &VAOCuadrado);
	glGenBuffers(1, &VBO);
	// bind the Vertex Array Object first.
	glBindVertexArray(VAOCuadrado);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//posicion
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//normales
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//textura
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glDeleteBuffers(1, &VBO);
};

//funcion de dibujo del suelo
void dibujaSuelo(GLuint shaderProgram) {
	//creamos las matrices del modelo
	glm::mat4 model; //es la matriz de transformación
	//la busco en el shader
	unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	float i, j;
	float escalasuelo = 1;
	int limite = 2;
	for (i = -limite; i <= limite; i += (1 / escalasuelo)) {
		for (j = -limite; j <= limite; j += (1 / escalasuelo)) {
			//Calculo la matriz
			model = glm::mat4(); //identity matrix
			//trasladamos para dibujar cada cuadrado
			model = glm::translate(model, glm::vec3(i, j, 0.0f));
			//escalamos
			model = glm::scale(model, glm::vec3((1 / escalasuelo), (1 / escalasuelo), (1 / escalasuelo)));
			glBindTexture(GL_TEXTURE_2D, sueloTex);
			//La cargo
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			//dibujamos el cuadrado
			glBindVertexArray(VAOCuadrado);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}
}

void actualizaPosicion() {
	//actualizacion de la posicion de la base con la velocidad
	base.px += velocidad * cos(base.angulo_trans * ARADIANES);
	base.py += velocidad * sin(base.angulo_trans * ARADIANES);
	//Corte al salir del suelo
	if (base.px >= 2.5 - base.sx / 2.0) base.px = -2.4 + base.sx / 2.0;
	if (base.py >= 2.5 - base.sx / 2.0) base.py = -2.4 + base.sx / 2.0;
	if (base.px <= -2.5 + base.sx / 2.0) base.px = 2.5 - base.sx / 2.0;
	if (base.py <= -2.5 + base.sx / 2.0) base.py = 2.5 - base.sx / 2.0;
}

void iluminacion() {
	//el color del objeto
	unsigned int colorLoc = glGetUniformLocation(shaderProgram, "objectColor");
	//blanco por defecto
	glUniform3f(colorLoc, 1.0f, 1.0f, 1.0f);
	//el color de la luz ambiente 
	unsigned int lightLoc = glGetUniformLocation(shaderProgram, "lightColor");
	//luz blanca
	glUniform3f(lightLoc, 1.0f, 1.0f, 1.0f);
	//luz difusa 
	unsigned int lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
	glUniform3f(lightPosLoc, (float)luz.x, (float)luz.y, (float)luz.z + ESCALADO_LUZ);
}

void cargaTextura(unsigned int* textura,const char* ruta) {
	int width, height, nrChannels;
	//geneneramos la textura
	glGenTextures(1, textura);
	glBindTexture(GL_TEXTURE_2D, *textura);
	//parametros wrap
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//parametros de escala
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//cargamos la imagen
	unsigned char* imagen = stbi_load(ruta, &width, &height, &nrChannels, 0);
	if (imagen) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imagen);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
	} else {
		printf("Fallo en la carga de la textura %s\n", imagen);
	}
	stbi_image_free(imagen);
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

	//Cargamos todas las texturas
	cargaTextura(&sueloTex, "../texturas/hierba.jpg");
	cargaTextura(&gruaTex, "../texturas/metal_amarillo.jpg");
	cargaTextura(&articulacionTex, "../texturas/metal.jpg");

	//menu de controles
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
		
		glActiveTexture(GL_TEXTURE0);

		//Dibujo del suelo
		dibujaSuelo(shaderProgram);
		actualizaPosicion();

		//dibujamos la grua
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glm::mat4 model;
		model = glm::mat4();
		model = dibujaObjeto(base, model);
		model = dibujaObjeto(articulacion1, model);
		model = dibujaObjeto(brazo1, model);
		model = dibujaObjeto(articulacion2, model);
		model = dibujaObjeto(brazo2, model);

		//recuperamos la matriz del modelo
		const float* pSource = (const float*)glm::value_ptr(model);
		//posicion del brazo
		luz.x = pSource[12];
		luz.y = pSource[13];
		luz.z = pSource[14];
		//iluminamos
		iluminacion();

		glBindVertexArray(0);

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
			velocidad += 0.0001;
			break;
		case 88://Letra X, reduce la velocidad
			velocidad -= 0.0001;
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
