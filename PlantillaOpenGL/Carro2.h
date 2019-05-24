#pragma once
#include "Modelo.h"
#include "glm/gtx/transform.hpp"
#include "glm/glm.hpp"

class Carro2 : public Modelo {
public:
	int indicePuntoTrayectoria = 11;
	float angulo = 0;
	float anguloTrayectoria = 0;
	float velocidad = 0;
	float velocidadMaxima = 25;
	float desaceleracion = 5;
	int contadorVuelta = 0;
	bool ganador = false;
	float anguloVertical = PI / 2;

	float tamanoCarro = 1.0f;
	vec3 coordenadas = vec3(0.0f, 0.1f, 22.0f);

	Carro2();
	void mover(float tiempoDiferencial);
	void actualizar(float tiempoDiferencial, vector<vec3> trayectoria);
	void calcularAnguloTrayectoria(vector<vec3> trayectoria);

private:
	void actualizarMatrizModelo();
	
};