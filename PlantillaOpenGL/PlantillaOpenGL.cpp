// PlantillaOpenGL.cpp: define el punto de entrada de la aplicación de consola.
//

#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>

#define GLEW_STATIC

#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include <iostream>

#include "Shader.h"
#include "Vertice.h"
#include "Modelo.h"
#include "Carro.h"
#include "Carro2.h"
#include "Gano.h"
#include "Pista.h"
#include "Arbol.h"
#include "Nube.h"
#include "NubeFondo.h"
#include "Tierra.h"
#include "Semaforo.h"
#include "cuadroSemaforo.h"

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

//Declaramos apuntador de ventana
GLFWwindow *window;

GLfloat anchoPantalla = 1224.0f;
GLfloat altoPantalla = 768.0f;

using namespace std;

Shader *shader;
GLuint posicionID;
GLuint colorID;
GLuint modeloID;
GLuint vistaID;
GLuint proyeccionID;
GLuint coordenadaUVID;
GLuint samplerImagenID;
GLuint texturaCalleID;

vec3 posicionCamara = vec3(0.0f, 25.0f, 40.0f);

mat4 vista;
mat4 proyeccion;

Carro *carro;
Carro2 *carro2;
Pista *pista;
Arbol *arbol;
Nube *nube;
NubeFondo* nubeFondo;
Tierra *tierra;

Gano* gano;
Semaforo* semaforo;
cuadroSemaforo* cuadroVerde;
cuadroSemaforo* cuadroAmarillo;
cuadroSemaforo* cuadroRojo;

	double tiempoAnterior = 0.0;
	double tiempoActual = 0.0;
	double tiempoDiferencial = 0.0;

	double cronometro = 0;

vector<vec3> trayectoria;

int estadoAnteriorCarro1;
int estadoAnteriorCarro2;

void llenarTrayectoria() {
	trayectoria.push_back(vec3(0.0f, 0.0f, 18.0f)); //Inicio jugador 1
	trayectoria.push_back(vec3(27.0f, 0.0f, 18.0f));
	trayectoria.push_back(vec3(33.0f, 0.0f, 12.0f));
	trayectoria.push_back(vec3(33.0f, 0.0f, -12.0f));
	trayectoria.push_back(vec3(30.0f, 1.0f, -15.0f));
	trayectoria.push_back(vec3(25.0f, 1.0f, -20.0f));
	trayectoria.push_back(vec3(23.0f, 0.0f, -22.0f));
	trayectoria.push_back(vec3(-32.0f, 0.0f, -22.0f));
	trayectoria.push_back(vec3(-37.0f, 0.0f, -16.0f));
	trayectoria.push_back(vec3(-37.0f, 0.0f, 16.0f));
	trayectoria.push_back(vec3(-32.0f, 0.0f, 22.0f));
	trayectoria.push_back(vec3(0.0f, 0.0f, 22.0f)); //Inicio jugador 2
	trayectoria.push_back(vec3(33.0f, 0.0f, 22.0f));
	trayectoria.push_back(vec3(38.0f, 0.0f, 16.0f));
	trayectoria.push_back(vec3(38.0f, 0.0f, -16.0f));
	trayectoria.push_back(vec3(32.0f, 0.0f, -21.0f));
	trayectoria.push_back(vec3(22.0f, 0.0f, -18.0f));
	trayectoria.push_back(vec3(-27.0f, 0.0f, -18.0f));
	trayectoria.push_back(vec3(-32.0f, 0.0f, -11.0f));
	trayectoria.push_back(vec3(-32.0f, 0.0f, 11.0f));
	trayectoria.push_back(vec3(-26.0f, 0.0f, 18.0f));
	trayectoria.push_back(vec3(0.0f, 0.0f, 18.0f));
}

void establecerVista() {
	vista = lookAt(
		posicionCamara, //Posición de la cámara.
		vec3(0.0f, 0.0f, 0.0f), //Posición del objetivo.
		vec3(0.0f, 1.0f, 0.0f) //Vector hacia arriba.
	);

	if (carro2 != NULL)
	{
		carro->vista = vista;
		carro2->vista = vista;
		pista->vista = vista;
		semaforo->vista = vista;
		cuadroVerde->vista = vista;
		cuadroAmarillo->vista = vista;
		cuadroRojo->vista = vista;
	}
}

void establecerProyección() {
	proyeccion = perspective(
		((90.0f * 3.14159265f) / 180), //Campo de visión (FoV).
		anchoPantalla/altoPantalla, //Relación de aspecto.
		0.1f, //Near clipping (desde dónde renderea).
		150.0f //Far clipping (hasta dónde renderea).
	);
}

void dibujar() {

	if (semaforo->semaforo) {
		semaforo->dibujar(GL_QUADS);
		cuadroVerde->dibujar(GL_QUADS);
		cuadroAmarillo->dibujar(GL_QUADS);
		cuadroRojo->dibujar(GL_QUADS);
	}

	carro->dibujar(GL_QUADS);
	carro2->dibujar(GL_QUADS);
	pista->dibujar(GL_QUADS);
	arbol->dibujar(GL_QUADS);
	nube->dibujar(GL_QUADS);
	nubeFondo->dibujar(GL_QUADS);
	tierra->dibujar(GL_QUADS);
}

void actualizar() {
	tiempoActual = glfwGetTime();
	tiempoDiferencial = tiempoActual - tiempoAnterior;
	tiempoAnterior = tiempoActual;

	carro->actualizar(tiempoDiferencial, trayectoria);
	carro2->actualizar(tiempoDiferencial, trayectoria);

	if (semaforo->semaforo) {
		semaforo->actualizar();
		cuadroVerde->actualizar();
		cuadroAmarillo->actualizar();
		cuadroRojo->actualizar();

		cronometro += tiempoDiferencial;

		if (cronometro >= 1) {
			cuadroVerde->escala = vec3(1.0f,1.0f,1.0f);
		}
		 if (cronometro >= 2) {
			cuadroAmarillo->escala = vec3(1.0f, 1.0f, 1.0f);;
		}
		 if (cronometro >= 3) {
			cuadroRojo->escala = vec3(1.0f, 1.0f, 1.0f);;
		}
		 if (cronometro >= 4) {
			semaforo->semaforo = false;
			cronometro = 0;
		}
	}
	else {


		

		cronometro += tiempoDiferencial;

		int estadoZ;
		estadoZ = glfwGetKey(window, GLFW_KEY_Z);


		if (estadoZ == GLFW_PRESS)
		{
			if (estadoAnteriorCarro1 == GLFW_RELEASE)
			{
				carro->velocidad += 1;
			}

		}

		int estadoM;
		estadoM = glfwGetKey(window, GLFW_KEY_M);


		if (estadoM == GLFW_PRESS)
		{
			if (estadoAnteriorCarro2 == GLFW_RELEASE)
			{
				carro2->velocidad += 1;
			}

		}

		estadoAnteriorCarro1 = estadoZ;
		estadoAnteriorCarro2 = estadoM;

		if (carro->ganador) {
			cout << "Ganador: jugador 1\n";
			carro->velocidad = 0;
			carro2->velocidad = 0;
			carro->tamanoCarro = 5.0f;
			carro->coordenadas = vec3(5.0f, 15.0f, 0.0f);
			gano->dibujar(GL_QUADS);
		}

		if (carro2->ganador) {
			cout << "Ganador: jugador 2\n";
			carro->velocidad = 0;
			carro2->velocidad = 0;
			carro2->tamanoCarro = 5.0f;
			carro2->coordenadas = vec3(5.0f, 15.0f, 0.0f);
			gano->dibujar(GL_QUADS);
		}
	}
}

int main()
{	
	llenarTrayectoria();
	//Si no se pudo iniciar glfw
	//terminamos ejcución
	if (!glfwInit()) {
		exit(EXIT_FAILURE);
	}
	//Si se pudo iniciar GLFW
	//entonces inicializamos la ventana
	window =
		glfwCreateWindow(1224, 768, "Ventana", 
			NULL, NULL);
	//Si no podemos iniciar la ventana
	//Entonces terminamos ejecucion
	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	//Establecemos el contexto
	glfwMakeContextCurrent(window);

	//Una vez establecido  el contexto
	//Activamos funciones modernas
	glewExperimental = true;
	GLenum errorGlew = glewInit();
	if (errorGlew != GLEW_OK) {
		cout << 
			glewGetErrorString(errorGlew);
	}

	const GLubyte *versionGL =
		glGetString(GL_VERSION);
	cout << "Version OpenGL: "
		<< versionGL;

	const char *rutaVertex = 
		"VertexShader.shader";
	const char *rutaFragment = 
		"FragmentShader.shader";

	shader = new Shader(rutaVertex, rutaFragment);

	//Mapeo de atributos
	posicionID =
		glGetAttribLocation(shader->getID(), "posicion");
	colorID =
		glGetAttribLocation(shader->getID(), "color");
	modeloID =
		glGetUniformLocation(shader->getID(), "modelo");
	vistaID =
		glGetUniformLocation(shader->getID(), "vista");
	proyeccionID =
		glGetUniformLocation(shader->getID(), "proyeccion");
	samplerImagenID =
		glGetUniformLocation(shader->getID(), "samplerImagen");
	coordenadaUVID =
		glGetAttribLocation(shader->getID(), "coordenadaUV");

	// TEXTURAS
	texturaCalleID =
		Shader::cargarBMP("texturas\\pavimento.bmp");

	//Desenlazar el shader
	shader->desenlazar();

	establecerVista();
	establecerProyección();

	// AQUÍ MANDAMOS LLAMAR LOS SHADERS Y VERTEX ARRAY DE LOS OBJETOS //
	carro = new Carro();
	carro->vista = vista;
	carro->proyeccion = proyeccion;
	carro->shader = shader;
	carro->inicializarVertexArray(posicionID, colorID, modeloID, vistaID, proyeccionID, 
		samplerImagenID, coordenadaUVID, 0);

	carro2 = new Carro2();
	carro2->vista = vista;
	carro2->proyeccion = proyeccion;
	carro2->shader = shader;
	carro2->inicializarVertexArray(posicionID, colorID, modeloID, vistaID, proyeccionID,
		samplerImagenID, coordenadaUVID, 0);

	pista = new Pista();
	pista->vista = vista;
	pista->proyeccion = proyeccion;
	pista->shader = shader;
	pista->inicializarVertexArray(posicionID, colorID, modeloID, vistaID, proyeccionID,
		samplerImagenID, coordenadaUVID, 0);

	arbol = new Arbol();
	arbol->vista = vista;
	arbol->proyeccion = proyeccion;
	arbol->shader = shader;
	arbol->inicializarVertexArray(posicionID, colorID, modeloID, vistaID, proyeccionID,
		samplerImagenID, coordenadaUVID, 0);

	nube = new Nube();
	nube->vista = vista;
	nube->proyeccion = proyeccion;
	nube->shader = shader;
	nube->inicializarVertexArray(posicionID, colorID, modeloID, vistaID, proyeccionID,
		samplerImagenID, coordenadaUVID, 0);

	nubeFondo = new NubeFondo();
	nubeFondo->vista = vista;
	nubeFondo->proyeccion = proyeccion;
	nubeFondo->shader = shader;
	nubeFondo->inicializarVertexArray(posicionID, colorID, modeloID, vistaID, proyeccionID,
		samplerImagenID, coordenadaUVID, 0);

	tierra = new Tierra();
	tierra->vista = vista;
	tierra->proyeccion = proyeccion;
	tierra->shader = shader;
	tierra->inicializarVertexArray(posicionID, colorID, modeloID, vistaID, proyeccionID,
		samplerImagenID, coordenadaUVID, 0);

	semaforo = new Semaforo();
	semaforo->vista = vista;
	semaforo->proyeccion = proyeccion;
	semaforo->shader = shader;
	semaforo->inicializarVertexArray(posicionID, colorID, modeloID, vistaID, proyeccionID,
		samplerImagenID, coordenadaUVID, 0);

	cuadroVerde = new cuadroSemaforo(vec4(0.0f, 1.0f, 0.0f, 1.0f), vec3(-4.5f, 25.1f, 25.1f));
	cuadroVerde->vista = vista;
	cuadroVerde->proyeccion = proyeccion;
	cuadroVerde->shader = shader;
	cuadroVerde->inicializarVertexArray(posicionID, colorID, modeloID, vistaID, proyeccionID,
		samplerImagenID, coordenadaUVID, 0);

	cuadroAmarillo = new cuadroSemaforo(vec4(1.0f, 1.0f, 0.0f, 1.0f), vec3(0.0f, 25.1f, 25.1f));
	cuadroAmarillo->vista = vista;
	cuadroAmarillo->proyeccion = proyeccion;
	cuadroAmarillo->shader = shader;
	cuadroAmarillo->inicializarVertexArray(posicionID, colorID, modeloID, vistaID, proyeccionID,
		samplerImagenID, coordenadaUVID, 0);

	cuadroRojo = new cuadroSemaforo(vec4(1.0f, 0.0f, 0.0f, 1.0f), vec3(4.5f, 25.1f, 25.1f));
	cuadroRojo->vista = vista;
	cuadroRojo->proyeccion = proyeccion;
	cuadroRojo->shader = shader;
	cuadroRojo->inicializarVertexArray(posicionID, colorID, modeloID, vistaID, proyeccionID,
		samplerImagenID, coordenadaUVID, 0);

	gano = new Gano();
	gano->vista = vista;
	gano->proyeccion = proyeccion;
	gano->shader = shader;
	gano->inicializarVertexArray(posicionID, colorID, modeloID, vistaID, proyeccionID,
		samplerImagenID, coordenadaUVID, 0);
	// FIN DE: AQUÍ MANDAMOS LLAMAR LOS SHADERS Y VERTEX ARRAY DE LOS OBJETOS //

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//Ciclo de dibujo (Draw loop)
	while (!glfwWindowShouldClose(window)) {
		//Esablecer region de dibujo
		glViewport(0, 0, 1224, 768);
		//Establece el color de borrado
		glClearColor(0.0588, 0.0588, 0.3098, 1);
		//Borramos
		glClear(
			GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Rutina de dibujo
		dibujar();
		actualizar();

		//Cambiar los buffers
		glfwSwapBuffers(window);
		//Reconocer si hay entradas
		glfwPollEvents();

	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

