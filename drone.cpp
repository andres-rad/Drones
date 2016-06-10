#include "drone.h"
#include "tipos.h"
#include "auxiliares.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <ctype.h>
#include <algorithm>



Drone::Drone() {
}

Drone::Drone(ID i, const std::vector<Producto> &ps) {
    _id = i;
    _bateria = 100;
    _enVuelo = false;
    _productos = ps;
    _posicionActual= Posicion {0,0}; //Esto de aca no es del todo necesario, pero lo puse para que no fallen los tests que mandaron elllos
                                    //el constructor por defecto de vector lo crea vacio, lo cual es
    _trayectoria = Secuencia<Posicion>();		//necesario para preservar el invariante
}

ID Drone::id() const {
	return _id;
}

Carga Drone::bateria() const {
	return _bateria;
}

bool Drone::enVuelo() const {
	return _enVuelo;
}

const Secuencia<Posicion> &Drone::vueloRealizado() const {
	return _trayectoria;
}

Posicion Drone::posicionActual() const {
	return _posicionActual;
}

const Secuencia<Producto> &Drone::productosDisponibles() const {
	return _productos;
}

bool Drone::vueloEscalerado() const {
	bool ans=true;
	int ultDifX=1;
	int ultDifY=1;
	if (_trayectoria.size()>2){
		unsigned int i=0;
		ultDifX=_trayectoria[0].x - _trayectoria[2].x;
		ultDifY=_trayectoria[0].y - _trayectoria[2].y;
		while (i < _trayectoria.size() - 2) {

			int difX = _trayectoria[i].x - _trayectoria[i+2].x;
			int difY = _trayectoria[i].y - _trayectoria[i+2].y;

			ans=ans && ultDifY==difY && ultDifX==difX;

			i++;
		}
	}

	return ans && abs(ultDifY)==1 && abs(ultDifX)==1 && _enVuelo;
}

Secuencia <InfoVueloCruzado> Drone::vuelosCruzados(const Secuencia<Drone> &ds) {
    Secuencia<InfoVueloCruzado> res;
	if (ds.empty()) return res;	//me aseguro que no se rompe lo siguiente

	Secuencia<Posicion> posCruzadas = posConCruces(ds);
	unsigned int i = 0;
	while ( i < posCruzadas.size()){
		res.push_back(InfoVueloCruzado{posCruzadas[i], cantidadDronesCruzados(posCruzadas[i], ds)});
		i++;
	}
	std::sort(res.begin(), res.end(), ordenCruzados);

    return res; //esto no estoy seguro, pero como no habia ningun return lo puse (Teo)
}

void Drone::mostrar(std::ostream &os) const {
    os << "Mostrando Drone:" << std::endl;
	os << "ID: " << _id << std::endl;
	os << "Nivel de bateria: " << _bateria << std::endl;
	os << "En vuelo?: " << (_enVuelo ? "Si" : "No");
	os << std::endl;		//no anda si lo pongo en la linea de arriba

	os << "Vuelos Realizados: ";
	for (Posicion vuelo : _trayectoria) {	//v�lido a partir de C++11
		os << "(" << vuelo.x << ", " << vuelo.y << ") ";
	}
	os << std::endl;

	os << "Posicion Actual: " << "(" << _posicionActual.x << ", "
		<< _posicionActual.y << ")" << std::endl;

	os << "Productos Disponibles: "<<std::endl;;
	//Cuento la cantidad de cada uno en un array
	int cant[5]={0};
	for (Producto producto : _productos) {
		//os << producto << " ";
		cant[producto]++;
	}

	for (int i=0; i<5;i++){
		os<<"*"<<Producto(i)<<": "<<cant[i]<<std::endl;
	}
	//os << std::endl;

	os << "Vuelos Escalerados?: " << (vueloEscalerado() ? "Si" : "No");
	os << std::endl;

	//TODO mostrar los vuelos cruzados
}

void Drone::guardar(std::ostream &os) const {
    os << "{D ";		//un drone
	os << _id << " ";	//ID
	os << _bateria << " "; //carga

	// Trayectoria
    os<<_trayectoria;
	// os << "[";
	// if (_trayectoria.size() > 0)	{		//tira segfault si intenta _trayectoria[0] cuando es vacio
	// 	for (unsigned int i = 0; i < _trayectoria.size() - 1; i++) {
	// 		os << "[" << _trayectoria[i].x << "," << _trayectoria[i].y << "],";
	// 	}
	// 	os << "[" << _trayectoria[_trayectoria.size() - 1].x << "," << _trayectoria[_trayectoria.size() - 1].y << "]";
	// }
	// os << "]";

	//Productos
    os << _productos;
    // os << "[";
	// for (unsigned int i = 0; i < _productos.size() - 1; i++) {
	// 	os << _productos[i] << ",";
	// }
	// os << _productos[_productos.size() -1 ] << "]";

	// En vuelo
	if (_enVuelo) {			//pone 1 y 0 si hago os << _enVuelo
		os << "true";
	}else {
		os << "false";
	}

	// Pos Actual
	os << "[" << _posicionActual.x << "," << _posicionActual.y << "]";

	os << "}";
}

void Drone::cargar(std::istream &is) {
  char currChar; //se usa para el caracter actual, muchas veces para tirarlo
	std::string currString; //se usa para el string actual, si es necesario

	ID id;								//Estos son los valores
	Carga bate;							//que despuen van al drone
	Secuencia<Posicion> trayectoria;
	Secuencia<Producto> productos;
	bool vuelo;
	Posicion posAct;

	//Agarro strings hasta que alguno es un digito y lo meto en id con stoi()
	is >> currString;
	while (!isdigit(currString[0])) {
		is >> currString; //std::cout<<currString;
	}
	id=stoi(currString);

	//El proximo va a ser la carga, la mando directamente
	is>>bate;

	is>>trayectoria;

/*
	//Avanzo hasta que tengo el [ que abre la trayectoria, y agarro uno mas
	is>>currChar;
	while (currChar!='[') is>>currChar;
	is>>currChar;

	//Ciclo general, hasta que llego al final de la trayectoria ]
	while (currChar!=']'){
		//Por las dudas de que haya algun espacio, o algo asi
		if (currChar=='['){
			int x,y;
			//x= posicion x; tiro la coma; y= posicion y; tiro el ]
			is>>x;
			is>>currChar;
			is>>y;
			is>>currChar;

			//pusheo la posicion a trayectoria
			Posicion posTemp{x,y};
			trayectoria.push_back(posTemp);
		}
		is>>currChar;
	}
*/

	//Avanzo hasta que tengo el [ que abre los productos
	while (currChar!='[') is>>currChar;

	//Ciclo general, hasta que llego al final de los productos ]
	while (currChar!=']'){


		is>>currChar;
		//Agarro la primer letra y me fijo cual puede ser
		if (currChar=='F'){
			//F igual Fertilizante y avanzo hasta la coma, chequendo si termina la lista
			productos.push_back(Fertilizante);
			//std::cout<<"#1"<<std::endl;
			while (currChar!=']'&&currChar!=',') {is>>currChar;}
		}
		else if (currChar=='P'){
			//P puede ser Plaguicida o PlaguicidaBajoConsumo, avanzo hasta la coma o una mayuscula
			is>>currChar;
			while(currChar!=']'&&currChar!=',' && !isupper(currChar)) is>>currChar;
			//si es mayus, es PlaguicidaBajoConsumo, sino Plaguicida
			if (currChar=='B'){
				productos.push_back(PlaguicidaBajoConsumo);
				while (currChar!=']'&&currChar!=',') is>>currChar;
			}else{
				productos.push_back(Plaguicida);
			}
		}
		else if (currChar=='H'){
			//H puede ser Herbicida o HerbicidaLargoAlcance, avanzo hasta la coma o una mayuscula
			is>>currChar;
			while(currChar!=']'&&currChar!=',' && !isupper(currChar)) is>>currChar;
			//si es mayus, es HerbicidaLargoAlcance, sino Herbicida
			if (currChar=='L'){
				productos.push_back(HerbicidaLargoAlcance);
				while (currChar!=']'&&currChar!=',') is>>currChar;
			}else{
				productos.push_back(Herbicida);
			}
		}


	}



	while (currChar!='t' && currChar!='f') {is>>currChar;}

	if (currChar=='t') vuelo=true;
	else if (currChar=='f') vuelo=false;

	while (currChar!='[') is>>currChar;

	int x,y;

	is>>x>>currChar>>y>>currChar;

	is>>currChar;
	while (currChar!='}') is>>currChar;

	posAct.x=x;
	posAct.y=y;

	//Completo los valores, me parece mas ordenado hacerlo asi al final, kcyo
	_id=id;
	_bateria=bate;
	_trayectoria=trayectoria;
	_productos=productos;
	_enVuelo=vuelo;
	_posicionActual=posAct;

	return;
}

void Drone::moverA(const Posicion pos) {
  _enVuelo = true;
  _trayectoria.push_back(pos);
  _posicionActual = pos;
}

void Drone::setBateria(const Carga c) {
  _bateria = c;
	return;
}

void Drone::borrarVueloRealizado() {
  _enVuelo = false;
	_trayectoria.clear();
	return;
}

void Drone::cambiarPosicionActual(const Posicion p) {
  _posicionActual = p;
	return;
}

void Drone::sacarProducto(const Producto p) {
  unsigned int i = 0;
	int listo = 1;

	while(i < _productos.size() && listo){
		if(_productos[i] == p){

			_productos[i] = _productos[(_productos.size())-1];
			_productos.pop_back();
			listo--;

		}
		i++;
	}

	return;
}

bool Drone::operator==(const Drone &otroDrone) const {
  if (_id != otroDrone.id())				//Si tienen distinto ID, son distintos
		return false;

	if (_bateria != otroDrone.bateria())	//Distinta bateria => distinto drone
		return false;

	if (_enVuelo != otroDrone.enVuelo())	//etc
		return false;

	if (_trayectoria != otroDrone.vueloRealizado())
		return false;

	if(!(_posicionActual == otroDrone.posicionActual()))
		return false;
	//no me fijo posicionActual porque que tengan los mismos vuelos realizados
	//implica que el �ltimo elemento sea el mismo
	//Ahora si me fijo porque cambiaron todo

	if (!mismos(_productos, otroDrone.productosDisponibles()))
		return false;

	return true;
}

bool Drone::operator!=(const Drone & otroDrone) const {
	return !(*this == otroDrone);
}

std::ostream &operator<<(std::ostream &os, const Drone &d) {

	d.guardar(os);

	return os;
}





//Auxiliares
