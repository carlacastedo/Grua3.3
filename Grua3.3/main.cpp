//grua con OpenlGL 3.3 de Antonio Farina Elorza y Carla Castedo Pereira
#include <glad.h>
#include <glfw3.h>
#include <stdio.h>
#include <iostream>
#include <lecturaShader.h>
#include "geometrias.h"

//transformaciones
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//para la carga de texturas
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "objeto.h"

#define ESCALADO_CAM 4
#define ESCALADO_1P 0.5
#define ESCALADO_3P 1
#define ESCALADO_ENFOQUE 10
#define ESCALADO_LUZ 1.25
#define ALTURA_1P 0.4
#define ALTURA_3P 0.5
#define LIMITE 1

//ancho y alto de la ventana
GLuint ANCHO = 800;
GLuint ALTO = 800;

//velocidad de la grua
float velocidad = 0;
//camara (por defecto alejada)
GLuint modoCamara = 0;

GLuint shaderProgram;

//VAOS para crear los objetos
GLuint VAOCuadrado;
GLuint VAOCubo;
GLuint VAOEsfera;
//angulos de giro de la camara alejada
float alfa = 0, beta = 0;

GLuint sueloTex;
GLuint gruaTex;
GLuint articulacionTex;

//objetos
Objeto base(Punto(0, 0, 0.10), 0, 0, Punto(0.5, 0.2, 0.2), &VAOCubo, 36, &gruaTex);
Objeto articulacion1(Punto(0, 0, 0.10), 0, 0, Punto(0.07, 0.07, 0.07), &VAOEsfera, 1080, &articulacionTex);
Objeto brazo1(Punto(0, 0, 0.20), 0, 0, Punto(0.05, 0.05, 0.45), &VAOCubo, 36, &gruaTex);
Objeto articulacion2(Punto(0, 0, 0.25), 0, 0, Punto(0.04, 0.04, 0.04), &VAOEsfera, 1080, &articulacionTex);
Objeto brazo2(Punto(0, 0, 0.15), 0, 0, Punto(0.03, 0.03, 0.25), &VAOCubo, 36, &gruaTex);

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
	view = glm::lookAt(glm::vec3(ESCALADO_CAM * glm::sin(glm::radians(alfa)) * glm::cos(glm::radians(beta)), ESCALADO_CAM * glm::sin(glm::radians(beta)), ESCALADO_CAM * glm::cos(glm::radians(alfa)) * glm::cos(glm::radians(beta))), glm::vec3(.0f, .0f, .0f), glm::vec3(.0f, glm::cos(glm::radians(beta)), .0f));
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
	view = glm::lookAt(glm::vec3(px - ESCALADO_3P * glm::cos(glm::radians(angulo)), py - ESCALADO_3P * glm::sin(glm::radians(angulo)), pz + ALTURA_3P),
		glm::vec3(px + ESCALADO_ENFOQUE * glm::cos(glm::radians(angulo)), py + ESCALADO_ENFOQUE * glm::sin(glm::radians(angulo)), pz),
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
	view = glm::lookAt(glm::vec3(px-ESCALADO_1P*glm::cos(glm::radians(angulo)),py-ESCALADO_1P*glm::sin(glm::radians(angulo)), pz + ALTURA_1P),
		glm::vec3(px + ESCALADO_ENFOQUE * glm::cos(glm::radians(angulo)), py + ESCALADO_ENFOQUE * glm::sin(glm::radians(angulo)), pz),
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
		primeraPersona(base.getPosicion().getX(), base.getPosicion().getY(), base.getPosicion().getZ(), base.getAnguloTrans1());
		break;
		//camara en tercera persona
	case 3:
		terceraPersona(base.getPosicion().getX(), base.getPosicion().getY(), base.getPosicion().getZ(), base.getAnguloTrans1());
		break;
	default:
		camaraAlejada();
	}
}

//funcion que dibuja el objeto aplicandole las transformaciones previas y las suyas propias, devolviendo una matriz
//con las transformaciones aplicadas
glm::mat4 dibujaObjeto(Objeto o, glm::mat4 model) {
	glm::mat4 modeltemp;
	unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
	//trasladamos
	model = glm::translate(model, glm::vec3(o.getPosicion().getX(), o.getPosicion().getY(), o.getPosicion().getZ()));
	//rotaciones con respecto al eje z
	model = glm::rotate(model, (float)(glm::radians(o.getAnguloTrans1())), glm::vec3(0.0f, 0.0f, 1.0f));
	//rotaciones con respecto al eje y (solo en las articulaciones)
	model = glm::rotate(model, (float)(glm::radians(o.getAnguloTrans2())), glm::vec3(0.0f, 1.0f, 0.0f));
	//guardamos las tranformaciones realizadas en la matriz temporal para que las los siguientes objetos
	modeltemp = model;
	model = glm::scale(model, glm::vec3(o.getEscalado().getX(), o.getEscalado().getY(), o.getEscalado().getZ()));
	glBindTexture(GL_TEXTURE_2D, *(o.getTextura()));
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//La cargo
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glBindVertexArray(*(o.getListarender()));
	glDrawArrays(GL_TRIANGLES, 0, o.getNumvertices());
	glBindTexture(GL_TEXTURE_2D, 0);
	//Devolvemos la matriz para usarla en el siguiente fragmento
	return modeltemp;
}

//funcion que crea el VAO en la variable VAO a partir del array de vertices
void creaVAO(float* vertices, unsigned int tam, GLuint* VAO, int color) {
	unsigned int VBO, repeticion = 8;
	glGenVertexArrays(1, VAO);
	glGenBuffers(1, &VBO);
	// bind the Vertex Array Object first.
	glBindVertexArray(*VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, tam, vertices, GL_STATIC_DRAW);
	if (color) {
		//Vertices
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(5 * sizeof(float)));
		glEnableVertexAttribArray(0);
		//Normales
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		//Textura
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		//Color
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
		glEnableVertexAttribArray(3);		
	}	else {
		//Vertices
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
		glEnableVertexAttribArray(0);
		//Normales
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		//Textura
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		//color
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(3);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	glDeleteBuffers(1, &VBO);
}

//funcion de dibujo del suelo
void dibujaSuelo(GLuint shaderProgram) {
	//creamos las matrices del modelo
	glm::mat4 model; //es la matriz de transformación
	//la busco en el shader
	unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	float i, j;
	float escalasuelo = 1;
	for (i = -LIMITE; i <= LIMITE; i += (1 / escalasuelo)) {
		for (j = -LIMITE; j <= LIMITE; j += (1 / escalasuelo)) {
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

void iluminacion(Punto luz) {
	//el color del objeto
	unsigned int colorLoc = glGetUniformLocation(shaderProgram, "objectColor");
	//blanco por defecto
	glUniform3f(colorLoc, 1.0f, 0.0f, 1.0f);
	//el color de la luz ambiente 
	unsigned int lightLoc = glGetUniformLocation(shaderProgram, "lightColor");
	//luz blanca
	glUniform3f(lightLoc, 1.0f, 1.0f, 1.0f);
	//luz difusa 
	unsigned int lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
	glUniform3f(lightPosLoc, luz.getX(), luz.getY(), luz.getZ() + ESCALADO_LUZ);
}

void cargaTextura(unsigned int* textura, const char* ruta) {
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
	Punto luz;
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

	//creamos los arrays de vertices
	creaVAO(vertices_cuadrado, sizeof(vertices_cuadrado), &VAOCuadrado, 1);
	creaVAO(vertices_esfera, sizeof(vertices_esfera), &VAOEsfera, 0);
	creaVAO(vertices_cubo, sizeof(vertices_cubo), &VAOCubo, 1);

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
	glm::mat4 model;
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
		base.actualizaPosicion(velocidad, LIMITE);

		//dibujamos la grua
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		//glm::mat4 model;
		model = glm::mat4();
		model = dibujaObjeto(base, model);
		model = dibujaObjeto(articulacion1, model);
		model = dibujaObjeto(brazo1, model);
		model = dibujaObjeto(articulacion2, model);
		model = dibujaObjeto(brazo2, model);
	
		//establecemos la posicion de la luz a la posicion del final del brazo superior
		luz.setX(model[3][0]);
		luz.setY(model[3][1]);
		luz.setZ(model[3][2]);
		//iluminamos
		iluminacion(luz);

		glBindVertexArray(0);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	// optional: de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &VAOCubo);
	glDeleteVertexArrays(1, &VAOCuadrado);
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
			base.setAnguloTrans1(base.getAnguloTrans1() + 1);
			break;
		case 68://Letra D, orienta la base a la derecha
			base.setAnguloTrans1(base.getAnguloTrans1() - 1);
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
			base.resetear();
			articulacion1.resetear();
			articulacion2.resetear();
			velocidad = 0;
			alfa = 0;
			beta = 0;
			break;

		//MOVIMIENTO DE LA PRIMERA ARTICULACION
		case 74://Letra J orienta el primer brazo a la izquierda
			articulacion1.setAnguloTrans1(articulacion1.getAnguloTrans1() + 1);
			break;
		case 76://Letra L orienta el primer brazo a la derecha
			articulacion1.setAnguloTrans1(articulacion1.getAnguloTrans1() - 1);
			break;
		case 73://Letra I orienta el primer brazo arriba
			if (articulacion1.getAnguloTrans2() < 65) articulacion1.setAnguloTrans2(articulacion1.getAnguloTrans2() + 1);
			break;
		case 75://Letra K orienta el primer brazo abajo
			if (articulacion1.getAnguloTrans2() >- 65) articulacion1.setAnguloTrans2(articulacion1.getAnguloTrans2() - 1);
			break;

		//MOVIMIENTO DE LA SEGUNDA ARTICULACION
		case 66://Letra B orienta el segundo brazo a la izquierda
			articulacion2.setAnguloTrans1(articulacion2.getAnguloTrans1() + 1);
			break;
		case 77://Letra M orienta el segundo brazo a la derecha
			articulacion2.setAnguloTrans1(articulacion2.getAnguloTrans1() - 1);
			break;
		case 72://Letra H orienta el segundo brazo arriba
			if (articulacion2.getAnguloTrans2() < 135) articulacion2.setAnguloTrans2(articulacion2.getAnguloTrans2() + 1);
			break;
		case 78://Letra N orienta el segundo brazo abajo
			if (articulacion2.getAnguloTrans2() > -135) articulacion2.setAnguloTrans2(articulacion2.getAnguloTrans2() - 1);
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
