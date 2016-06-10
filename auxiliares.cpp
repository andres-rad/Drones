#include "auxiliares.h"


int buscarDrone (const Sistema & s, const Drone & d){
  int i=0;
  int ans=-1;

  while (ans==-1 && i<s.enjambreDrones().size()){
    if (d==s.enjambreDrones()[i]) ans=i;
    i++;
  }

  return ans;
}

std::string getLineTwo (std::istream &is, char del1, char del2){
  std::string str="";
  while (is.peek()!=del1 && is.peek()!= del2){
    char currChar;
    is>>currChar;
    str+=currChar;
  }
  return str;
}


std::istream &operator>>(std::istream &is,  Drone &d){
  d.cargar(is);
  return is;
}

/*std::ostream &operator<<(std::ostream &os,  Drone &d){
	std::cout<<"hel";
	d.guardar(os);
	std::cout<<"hel";
	return os;
}*/

std::istream &operator>>(std::istream &is, Parcela &p){
	std::string str;
	str = getLineTwo(is, ']', ',');

	if (str=="Cultivo") p=Cultivo;
	else if (str=="Granero") p=Granero;
	else if (str=="Casa") p=Casa;

	return is;
}

std::istream &operator>>(std::istream &is, Producto &p){
	std::string str;
	str = getLineTwo(is, ']', ',');

	if (str=="Fertilizante") p=Fertilizante;
	else if (str=="Plaguicida") p=Plaguicida;
	else if (str=="PlaguicidaBajoConsumo") p=PlaguicidaBajoConsumo;
	else if (str=="Herbicida") p=Herbicida;
	else if (str=="HerbicidaLargoAlcance") p=HerbicidaLargoAlcance;



return is;
}

std::istream &operator>>(std::istream &is, Posicion &p){
	char currChar;
	int x,y;
	is>>currChar>>x>>currChar>>y>>currChar;
	p=Posicion {x,y};
	return is;
}

std::istream &operator>>(std::istream &is,  Campo &c){
  c.cargar(is);
  return is;
}

std::istream &operator>>(std::istream &is,  EstadoCultivo &c){
  std::string str;
  str = getLineTwo(is, ',',']');


  if (str=="RecienSembrado") c=RecienSembrado;
  else if (str=="EnCrecimiento") c=EnCrecimiento;
  if (str=="ListoParaCosechar") c=ListoParaCosechar;
  else if (str=="ConMaleza") c=ConMaleza;
  if (str=="ConPlaga") c=ConPlaga;
  else if (str=="NoSensado") c=NoSensado;



  return is;
}

std::ostream &operator<<(std::ostream &os, const Posicion &p){
    os << '[' << p.x << ',' << p.y << ']';
    return os;
}

bool enRango(const Sistema &s, Posicion p){
	return p.x >= 0 && p.x < s.campo().dimensiones().ancho && p.y >= 0 && p.y < s.campo().dimensiones().largo;
}


bool posVacia(const Sistema &s, Posicion p){
	bool res = true;
	unsigned int i = 0;
	while( i < s.enjambreDrones().size()){
		if(s.enjambreDrones()[i].posicionActual() == p){
			res = false;
		}
		i++;
	}
	return res;
}

Posicion suma(Posicion p, Posicion q){
	Posicion s { p.x + q.x, p.y+ q.y};
	return s;
}

Posicion posG(const Sistema & s){
	Posicion granero;
	int i = 0;
	while (i < s.campo().dimensiones().ancho){
		int j = 0;
		while (j < s.campo().dimensiones().largo){
			if (s.campo().contenido(Posicion { i, j }) == Granero){
				granero = Posicion { i, j};
			}
			j++;
		}
		i++;
	}
	return granero;
}

bool hayProducto(const Secuencia<Producto>& ps, Producto p){
	unsigned int i = 0;
	bool b = false;
	while(i < ps.size() && !b){
		if(ps[i] == p){
			b = true;
		}
		i++;
	}
	return b;
}

Secuencia<Posicion> movimientos(){
  Secuencia<Posicion> mov(4);
  mov = {Posicion {-1, 0}, Posicion {1, 0}, Posicion {0, -1}, Posicion {0, 1} };
  return mov;
}

Secuencia<Posicion> posConCruces(const Secuencia<Drone>& ds) {
    // Devuelve una lista de posiciones en las que hubo cruces sin elementos repetidos.
	Secuencia<Posicion> res;

	unsigned int i = 0;
	while(i < ds.size()){
		unsigned int j = 0;
		while (j < ds[i].vueloRealizado().size()){
			if(seCruzoConOtro(ds[i], ds, j)) {
				Posicion pos = ds[i].vueloRealizado()[j];
				if(!pertenece(pos, res)) res.push_back(pos);
			}
			j++;
		}
		i++;
	}

	/*for (Drone d : ds) {

		for (int i = 0; i < d.vueloRealizado().size(); i++) {
			if (seCruzoConOtro(d, ds, i)) {
				Posicion pos = d.vueloRealizado()[i];
				if (!pertenece(pos, res))
					res.push_back(pos);
			}
		}
	}
	*/
	return res;
}

bool const ordenCruzados(const InfoVueloCruzado& a, const InfoVueloCruzado& b) {
	return a.cantidadCruces <= b.cantidadCruces;       // Comparador para el ordenamiento de la lista de vuelsoCruzados. Ordena ascendiente por cantidad de cruces
}

int cantidadDronesCruzados(Posicion pos, const Secuencia<Drone>& ds) {
	int cant = 0;
	unsigned int i = 0;
	while(i < ds.size()){                          // Itero por todos los drones
		unsigned int j = 0;
		while(j < ds[i].vueloRealizado().size()){  // y por los vuelosRealizados
			if (ds[i].vueloRealizado()[j] == pos && seCruzoConOtro(ds[i], ds, j)) cant ++;
			j++;                                   // si en pos se cruzó con otro drone, sumo uno al contador de cruces
		}                                          // seCruzoConOtro se encarga de no contar los "cruces" de un drone consigo mismo
		i++;
	}
	/*
	for (Drone d : ds) {
		for (int i = 0; i < d.vueloRealizado().size(); i++) {
			if (d.vueloRealizado()[i] == pos && seCruzoConOtro(d, ds, i))
				cant++;
		}
	}
	*/
	return cant;

}

bool seCruzoConOtro (Drone d, const Secuencia<Drone>& ds, int i) {
    // Devuelve true si d se cruzó con otro drone en ds (distinto) en el "momento" i.
	unsigned int j = 0;
	while (j < ds.size()){
		if(ds[j] != d && d.vueloRealizado()[i] == ds[j].vueloRealizado()[i]) return true;
    j++;
  }

	/*
	 for (Drone d2 : ds) {
		if (d2 != d && d.vueloRealizado()[i] == d2.vueloRealizado()[i])
			return true;
	}
	*/
	return false;
}
//Implementen aqui sus funciones auxiliares globales definidas en aux.h...
