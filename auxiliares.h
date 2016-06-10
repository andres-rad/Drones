#ifndef AUX_H
#define AUX_H

#include "tipos.h"
#include "campo.h"
#include "drone.h"
#include "sistema.h"

int buscarDrone (const Sistema & s, const Drone & d); //Busca un dron en un enjambre de un sistema y devuelve su indice

std::string getLineTwo (std::istream &is, char del1, char del2); //Agarra un string de is hasta que encuentra alguno de los dos delimitadores

std::istream &operator>>(std::istream &is,  Drone &d);//Para leer Drone

std::istream &operator>>(std::istream &is,  Campo &c); //Para leer Campo

std::istream &operator>>(std::istream &is,  EstadoCultivo &c); //Para leer EstadoCultivo

std::istream &operator>>(std::istream &is, Parcela &p); //Para leer parcelas

std::istream &operator>>(std::istream &is, Posicion &p); //Para leer posiciones

std::istream &operator>>(std::istream &is, Producto &p); //Para leer productos

std::ostream &operator<<(std::ostream &os, const Posicion &p); //Para cargar Posicion

template<class T>
inline std::ostream& operator<<(std::ostream &os, const std::vector <T> &v){ //Carga una secuencia de tipo T
	os<<'[';
	unsigned int i =0;
	while (i+1 < v.size()){
		os<<v[i]<<',';
		i++;
	}
	if (v.size()>0) os<<v[i];
	os<<']';

	return os;

}

template<class T>
inline std::istream &operator>>(std::istream &is,   std::vector <T> &sec){ //Lee una secuencia de tipo T
  char currChar;
  is>>currChar;

  T temp;
  sec=Secuencia <T> (0);

if (is.peek()!=']'){


  while (currChar!=']'){


    is>>temp;
    sec.push_back(temp);
    is>>currChar;
  }
}else {
	is>>currChar;
}


  return is;
}

bool enRango(const Sistema &s, Posicion p);
bool posVacia(const Sistema &s, Posicion p);
Posicion suma(Posicion p, Posicion q);
Posicion posG(const Sistema &s);
bool hayProducto(const Secuencia<Producto>& ps, Producto p);
Secuencia<Posicion> movimientos();

template<class T>
inline bool pertenece(T e, Secuencia<T>& seq) {
  unsigned int i=0;
  bool ans=false;

  while (i<seq.size()){
    if (seq[i]==e) ans=true;
    i++;
  }

  return ans;
}

template <class T>
inline bool mismos(Secuencia<T> seq1, Secuencia<T> seq2) {
	if (seq1.size() != seq2.size())	return false;

	unsigned int i = 0;

	while(i < seq1.size()){
		if(cuenta(seq1[i], seq1) != cuenta(seq1[i], seq2)) return false;
		i++;
	}
	
	return true;
}

template <class T>
inline int cuenta(T e, Secuencia<T> seq) {
	int contador = 0;
	unsigned int i = 0;

	while(i < seq.size()){
		if(seq[i] == e) contador ++;
		i++;
	}
	
	return contador;
}


Secuencia<Posicion> posConCruces(const Secuencia<Drone>& ds);

int cantidadDronesCruzados(Posicion pos, const Secuencia<Drone>& ds);

bool const ordenCruzados(const InfoVueloCruzado& a, const InfoVueloCruzado& b);

bool seCruzoConOtro (Drone d, const Secuencia<Drone>& ds, int i);


#endif //AUX_H
