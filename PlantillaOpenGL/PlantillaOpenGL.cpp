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
#include "Pista.h"
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

//Declaramos apuntador de ventana
GLFWwindow *window;

GLfloat anchoPantalla = 1024.0f;
GLfloat altoPantalla = 768.0f;

using namespace std;

Shader *shader;
GLuint posicionID;
GLuint colorID;
GLuint modeloID;
GLuint vistaID;
GLuint proyeccionID;

vec3 posicionCamara = vec3(0.0f, 80.0f, 40.0f);

mat4 vista;
mat4 proyeccion;

Carro *carro;
Carro2 *carro2;
Pista *pista;

	double tiempoAnterior = 0.0;
	double tiempoActual = 0.0;
	double tiempoDiferencial = 0.0;

	double cronometro = 0;
	double cronometro2 = 0;

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
	}
}

void establecerProyección() {
	proyeccion = perspective(
		95.0f, //Campo de visión (FoV).
		anchoPantalla/altoPantalla, //Relación de aspecto.
		0.1f, //Near clipping (desde dónde renderea).
		150.0f //Far clipping (hasta dónde renderea).
	);
}

void dibujar() {
	carro->dibujar(GL_QUADS);
	carro2->dibujar(GL_QUADS);
	pista->dibujar(GL_QUADS);
}

void actualizar() {
	tiempoActual = glfwGetTime();
	tiempoDiferencial = tiempoActual - tiempoAnterior;
	tiempoAnterior = tiempoActual;

	carro->actualizar(tiempoDiferencial, trayectoria);
	carro2->actualizar(tiempoDiferencial, trayectoria);

	cronometro2 += tiempoDiferencial;
	cronometro += tiempoDiferencial;

	if (cronometro2 >= 1) {
		cout << carro->velocidad << "\n";
		cronometro2 = 0;
	}

	int estadoZ;
	estadoZ= glfwGetKey(window, GLFW_KEY_Z);
	

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
	}

	if (carro2->ganador) {
		cout << "Ganador: jugador 2\n";
		carro->velocidad = 0;
		carro2->velocidad = 0;
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
		glfwCreateWindow(1024, 768, "Ventana", 
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

	//Desenlazar el shader
	shader->desenlazar();

	establecerVista();
	establecerProyección();

	// AQUÍ MANDAMOS LLAMAR LOS SHADERS Y VERTEX ARRAY DE LOS OBJETOS //
	carro = new Carro();
	carro->vista = vista;
	carro->proyeccion = proyeccion;
	carro->shader = shader;
	carro->inicializarVertexArray(posicionID, colorID, modeloID, vistaID, proyeccionID);

	carro2 = new Carro2();
	carro2->vista = vista;
	carro2->proyeccion = proyeccion;
	carro2->shader = shader;
	carro2->inicializarVertexArray(posicionID, colorID, modeloID, vistaID, proyeccionID);

	pista = new Pista();
	pista->vista = vista;
	pista->proyeccion = proyeccion;
	pista->shader = shader;
	pista->inicializarVertexArray(posicionID, colorID, modeloID, vistaID, proyeccionID);
	// FIN DE: AQUÍ MANDAMOS LLAMAR LOS SHADERS Y VERTEX ARRAY DE LOS OBJETOS //

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//Ciclo de dibujo (Draw loop)
	while (!glfwWindowShouldClose(window)) {
		//Esablecer region de dibujo
		glViewport(0, 0, 1024, 768);
		//Establece el color de borrado
		glClearColor(1, 0.2, 0.5, 1);
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

