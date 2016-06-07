#include "campo.h"
#include "tipos.h"
#include "aux.h"
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
  //El que use la funcion tendrá que fijarse, si corresponde
  //si su ofstream está open

  os << "{ C";															//campo
  os << " [" << _dimension.ancho << "," << _dimension.largo << "]";	//su dimension
  /* os << " [";															//sus parcelas */

  os<<_grilla.parcelas;
  /*
  for (int i = 0; i < _dimension.ancho; i++) {
    os << "[";

    for (int j = 0; j < _dimension.largo - 1; j++) {
      Posicion pos{ i, j };
      os << contenido(pos);
      os << ",";
    }

    Posicion pos{ i, _dimension.largo - 1 };

    if (i < _dimension.ancho - 1)
      os << contenido(pos) << "], ";
    else
      os << contenido(pos) << "]]";
  }*/

  os << "}";
}

void Campo::cargar(std::istream &is) {

  	/* Grilla<Parcela> grillTemp; */
  	char currChar;

  	is >> currChar;
  	while (currChar != '[') {
  		is >> currChar;

  	}

  	int currAncho, currLargo;
  	is >> currAncho >> currChar >> currLargo;

  	Dimension currDim;
  	currDim.ancho = currAncho;
  	currDim.largo = currLargo;

  	Grilla<Parcela> grillTemp(currDim);

  	int i = 0, j = 0;

	is>>grillTemp.parcelas;
/*
  	while (currChar != '[') {
  		is >> currChar;


  	}

  	is >> currChar;

  	for (i = 0; i < currAncho; i++) {
  		is>>currChar;
  		for (j = 0; j < currLargo; j++) {
  			is >> currChar;
  			is >> currChar;
  			is >> currChar;


  			if (currChar == 'l') {
  				grillTemp.parcelas[i][j] = Cultivo;
  			}
  			else if (currChar == 's') {
  				grillTemp.parcelas[i][j] = Casa;
  			}
  			else if (currChar == 'a') {
  				grillTemp.parcelas[i][j] = Granero;
  			}

  			while (currChar != ','&&currChar != ']') is >> currChar;

  		}
  		while (i < currAncho - 1 && currChar != '[') is >> currChar;


  	}*/

  	_dimension = currDim;
  	_grilla = grillTemp;

	this->mostrar(std::cout);

}

bool Campo::operator==(const Campo &otroCampo) const {

	if (!(_dimension.ancho == otroCampo.dimensiones().ancho			//si tienen distintas
		&& _dimension.largo == otroCampo.dimensiones().largo))		//dimensiones, son distintos
		return false;

	for (int i = 0; i < _dimension.ancho; i++) {					//compara todas las parcelas
		for (int j = 0; j < _dimension.largo; j++) {
			Posicion pos{ i, j };
			if (contenido(pos) != otroCampo.contenido(pos))			//Si una parcela no coincide, son distintos
				return false;
		}
	}

	return true;
}

std::ostream &operator<<(std::ostream &os, const Campo &c) {
  c.mostrar(os);
  return os;
}
