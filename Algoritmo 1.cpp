//Autor: Alberto Maurel
//Algoritmo tomadado de:
//Williams, B.G. (s.f.).Optimal pooling strategies for laboratory testing. 
//Recuperado 4 abril, 2020, de https ://arxiv.org/ftp/arxiv/papers/1007/1007.4903.pdf

#include <iostream>
#include <vector>
#include <random>
#include <set>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <cassert>

using namespace std;

//Random numbers generator
auto seed = chrono::high_resolution_clock::now().time_since_epoch().count();
std::mt19937 generator(seed);

//Constantes
const int POBLACION_TOTAL = 1000000;
const double PORCENTAJE_POBLACION_INFECTADA = 0.001;
const int PERS_MAX_TEST = 32;
const int NUM_PRUEBAS = 100;

int tests_necesarios = 0;
int num_infectados = 0;

//Resultado del test: true - positivo, false - negativo
bool test(set<int> const& personas, set<int> const& infectados) {
	++tests_necesarios;

	for (int p : personas) {
		if (infectados.find(p) != infectados.end()) return true;
	}
	return false;
}


//Testea las veces necesarias el grupo hasta que se detecta exactamente quiénes son positivos y quiénes no
void testear(set<int> const& grupo, set<int> const& infectados, int nivel) {
	if (grupo.size() == 1) {
		if (test(grupo, infectados)) ++num_infectados;
		return;
	}
	else {
		//Hacemos el test al grupo
		//Si sale negativo hemos acabado
		//Si sale positivo lo partimos en grupos más pequeños y repetimos
		if (test(grupo, infectados)) {
			int tamgrupo; // = sqrt(grupo.size());
			if (nivel == 1) tamgrupo = 9;
			else if (nivel == 2) tamgrupo = 3;
			else tamgrupo = 1;

			set<int> subgrupo;
			for (int per : grupo) {
				subgrupo.insert(per);

				if (subgrupo.size() == tamgrupo) {
					testear(subgrupo, infectados, nivel + 1);
					subgrupo.clear();
				}
			}
			if (subgrupo.size() > 0) {
				testear(subgrupo, infectados, nivel + 1);
			}
		}
	}
}

void prueba() {
	int n = POBLACION_TOTAL;
	int d = POBLACION_TOTAL * PORCENTAJE_POBLACION_INFECTADA;

	//Escogemos las personas infectadas
	set<int> infectados;
	std::uniform_int_distribution<int> dis_int(0, n - 1);

	while (infectados.size() < d) {
		int infected = dis_int(generator);
		if (infectados.find(infected) == infectados.end()) infectados.insert(infected);
	}

	//Aplicamos la estrategia de testeo grupal
	set<int> grupo;
	for (int i = 0; i < n; ++i) {
		grupo.insert(i);
		if (grupo.size() % PERS_MAX_TEST == 0) {
			testear(grupo, infectados,1);
			grupo.clear();
		}
	}
	if (grupo.size() > 0) {
		testear(grupo, infectados,1);
	}

	//cout << "Poblacion total: " << n << '\n';
	//cout << "Poblacion infectada: " << d << '\n';
	//cout << "Numero de tests necesarios: " << tests_necesarios << '\n';
	assert(d == num_infectados);
}



int main() {
	int mintests = 1000 * 1000 * 1000;
	int maxtests = 0;
	long long int tests_totales = 0;

	for (int i = 0; i < NUM_PRUEBAS; ++i) {
		prueba();
		mintests = min(mintests, tests_necesarios);
		maxtests = max(maxtests, tests_necesarios);
		tests_totales += tests_necesarios;

		tests_necesarios = 0;
		num_infectados = 0;
		cout << "Test: " << i << '\n';
	}

	cout << "Minimo de tests necesarios: " << mintests << '\n';
	cout << "Maximo de tests necesarios: " << maxtests << '\n';
	cout << "Media de tests necesarios: " << tests_totales / NUM_PRUEBAS << '\n';

 	return 0;
}