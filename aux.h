#ifndef AUX_H
#define AUX_H

#include "tipos.h"
#include "campo.h"
#include "drone.h"
#include "sistema.h"

int buscarDrone (const Sistema & s, const Drone & d); //Busca un dron en un enjambre de un sistema y devuelve su indice

std::string getLineTwo (std::istream &is, char del1, char del2);

std::istream &operator>>(std::istream &is,  Drone &d);//Para leer Drone

//std::ostream &operator<<(std::ostream &os,  Drone &d);//Para guardar Drone

std::istream &operator>>(std::istream &is,  Campo &c); //Para leer Campo

std::istream &operator>>(std::istream &is,  EstadoCultivo &c); //Para leer EstadoCultivo

std::istream &operator>>(std::istream &is, Parcela &p); //Para leer parcelas

template<class T>
inline std::ostream& operator<<(std::ostream &os, const std::vector <T> &v){
	os<<'[';
	int i =0;
	while (i<v.size()-1){
		os<<v[i]<<',';
		i++;
	}
	if (v.size()>0) os<<v[i];
	os<<']';

	return os;

}

template<class T>
inline std::istream &operator>>(std::istream &is,   std::vector <T> &sec){
  char currChar;
  is>>currChar;

  T temp;
  sec=Secuencia <T> (0);

  while (currChar!=']'){

    is>>temp;
    sec.push_back(temp);
    is>>currChar;
  }

  return is;
}




//Declaren aqui sus funciones auxiliares globales...

#endif //AUX_H
