#include "campo.h"
#include "auxiliares.h"
#include <iostream>

Campo::Campo() {
}

Campo::Campo(const Posicion &posG, const Posicion &posC, Dimension dimension) {
    _dimension = dimension;
    _grilla = Grilla<Parcela>(_dimension);
    _grilla.parcelas[posG.x][posG.y] = Granero;
    _grilla.parcelas[posC.x][posC.y] = Casa;
}

Campo::Campo(const Posicion &posG, const Posicion &posC) {
	//asumo que se cumple el requiere
	_dimension.ancho = std::max(10,std::max(posG.x, posC.x)) + 1;
	_dimension.largo = std::max(posG.y, posC.y) + 1;
	_grilla = Grilla<Parcela>(_dimension);

	//creo el granero
	_grilla.parcelas[posG.x][posG.y] = Granero;

	//creo la casa
	_grilla.parcelas[posC.x][posC.y] = Casa;
}

Dimension Campo::dimensiones() const {
	//Devuelve las dimensiones del campo
	return _dimension;
}

Parcela Campo::contenido(const Posicion &p) const {
 	//asume que esta en rango	
       	return _grilla.parcelas[p.x][p.y];
}

void Campo::mostrar(std::ostream &os) const {
 	os << "Mostrando Campo:" << std::endl;
	os << "Dimensiones: (" << _dimension.ancho << ", " << _dimension.largo << ")" << std::endl;
	os << "Parcelas: " << std::endl;

	for (int i = 0; i < _dimension.ancho; i++) {
		for (int j = 0; j < _dimension.largo - 1; j++) {			//es -1 para que no quede una coma fea al final
			Posicion pos{ i, j };
			os << contenido(pos);
		       if (contenido(pos)==Casa) os<< ",    ";
		       else os<<", ";
		}
		Posicion pos{ i, _dimension.largo - 1 };
		os << contenido(pos) << "." << std::endl;
	}
}

void Campo::guardar(std::ostream &os) const {

	  os << "{ C";															//campo
	  os << " [" << _dimension.ancho << "," << _dimension.largo << "] ";	//su dimension

	  os<<_grilla.parcelas;

	  os << "}";

	  return;
}

void Campo::cargar(std::istream &is) {

  	/* Grilla<Parcela> grillTemp; */
  	char currChar;

  	is >> currChar;
  	while (currChar != '[') {
  		is >> currChar;

  	}

  	int currAncho, currLargo;
  	is >> currAncho >> currChar >> currLargo>>currChar;

  	Dimension currDim;
  	currDim.ancho = currAncho;
  	currDim.largo = currLargo;

  	Grilla<Parcela> grillTemp(Dimension {0,0});


	is>>grillTemp.parcelas;

	is>>currChar;
	while (currChar!='}') is>>currChar;
	
  	_dimension = currDim;
  	_grilla = grillTemp;

	return;

}

bool Campo::operator==(const Campo &otroCampo) const {
	
	bool ans=true;
	if (!(_dimension.ancho == otroCampo.dimensiones().ancho			//si tienen distintas
		&& _dimension.largo == otroCampo.dimensiones().largo))		//dimensiones, son distintos
		ans=false;

	for (int i = 0; i < _dimension.ancho; i++) {					//compara todas las parcelas
		for (int j = 0; j < _dimension.largo; j++) {
			Posicion pos{ i, j };
			if (contenido(pos) != otroCampo.contenido(pos))			//Si una parcela no coincide, son distintos
				ans=false;
		}
	}

	return ans;
}

std::ostream &operator<<(std::ostream &os, const Campo &c) {
  c.guardar(os);
  return os;
}
