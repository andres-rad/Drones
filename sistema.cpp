#include "sistema.h"
#include "aux.h"
#include <algorithm>

//Prototipos

bool enRango(const Sistema &s, Posicion p);
bool posVacia(const Sistema &s, Posicion p);
Posicion suma(Posicion p, Posicion q);
Posicion posG(const Sistema &s);
bool hayProducto(const Secuencia<Producto>& ps, Producto p);
Secuencia<Posicion> movimientos();
//

Sistema::Sistema() {
}

Sistema::Sistema(const Campo &c, const Secuencia<Drone> &ds) {
  _campo = c;
	_enjambre = ds;
	_estado = Grilla<EstadoCultivo> (c.dimensiones());
	int i = 0;
	/*while(i < _enjambre.size()){
		_enjambre[i].setBateria(100);
		_enjambre[i].borrarVueloRealizado();
		_enjambre[i].cambiarPosicionActual(posG(*this));
    i++;
		//Si no están en vuelo como hago que PosiciónActual devuelva el granero, porque vuelosRealizados tiene que estar vacío.
	}*/
  //todo eso no hace falta por los requiere
	int j = 0;
	while( j < c.dimensiones().ancho ){
		int i = 0;
		while (i < c.dimensiones().largo){
			_estado.parcelas[j][i] = NoSensado;
			i++;
		}
		j++;
	}

}

const Campo &Sistema::campo() const {
	return _campo;
}

EstadoCultivo Sistema::estadoDelCultivo(const Posicion &p) const {
	return _estado.parcelas[p.x][p.y];
}

const Secuencia<Drone> &Sistema::enjambreDrones() const {
	return _enjambre;
}

void Sistema::crecer() {
  int i = 0;
	while (i < _campo.dimensiones().ancho){
		int j = 0;
		while (j < _campo.dimensiones().largo){
			if (_estado.parcelas[i][j] == RecienSembrado){
				_estado.parcelas[i][j] = EnCrecimiento;
			}
			else if ( _estado.parcelas[i][j] == EnCrecimiento){
				_estado.parcelas[i][j] = ListoParaCosechar;
			}
			j++;
		}
		i++;
	}
	return;
}

void Sistema::seVinoLaMaleza(const Secuencia<Posicion> &ps) {
  int i = 0;
	while(i < ps.size()){
		_estado.parcelas[ps[i].x][ps[i].y] = ConMaleza;
		i++;
	}
	return;
}

void Sistema::seExpandePlaga() {
  Grilla<EstadoCultivo> estadoTemp = _estado;
  int i = 0;

	while(i < _campo.dimensiones().ancho){
		int j = 0;
		while (j < _campo.dimensiones().largo){
			if(_estado.parcelas[i][j] == ConPlaga){

				if( j - 1 >= 0 && _campo.contenido(Posicion { i, j-1}) == Cultivo){
					estadoTemp.parcelas[i][j-1] = ConPlaga;
				}

				if ( j != _campo.dimensiones().largo - 1 && _campo.contenido(Posicion { i, j+1}) == Cultivo){
					estadoTemp.parcelas[i][j+1] = ConPlaga;
				}

				if (i - 1 >= 0 && _campo.contenido(Posicion { i-1, j}) == Cultivo) {
					estadoTemp.parcelas[i-1][j] = ConPlaga;
				}

				if (i != _campo.dimensiones().ancho - 1 && _campo.contenido(Posicion { i+1, j}) == Cultivo){
					estadoTemp.parcelas[i+1][j] = ConPlaga;
				}
			}
			j++;
		}
		i++;
	}

  _estado=estadoTemp;
  return;
}

void Sistema::despegar(const Drone &d) {
  Secuencia<Posicion> posicionesActuales;
	int i = 0;
	int despegado = 0;
  int indice=buscarDrone(*this, d);
	Posicion adyacentes[] = {Posicion {-1, 0}, Posicion {1, 0}, Posicion {0, -1}, Posicion {0, 1} };
	Posicion granero = posG(*this);

	while (i < 4 && !despegado){
		if(enRango(*this, suma(granero, adyacentes[i])) && posVacia(*this, suma(granero, adyacentes[i]))){
		//Aca hay que usar moverA que puede que no está claro que quieren que haga.
    _enjambre[indice].moverA(suma(granero, adyacentes[i]));
		despegado++;
		}
		i++;
	}
  return ;
}

bool Sistema::listoParaCosechar() const {
    int listas = 0;
    int total = _campo.dimensiones().ancho * _campo.dimensiones().largo - 2;
    int i = 0;
    while (i < _campo.dimensiones().ancho * _campo.dimensiones().largo){
        Posicion pos {i % _campo.dimensiones().ancho, i / _campo.dimensiones().ancho};
        //std::cout<<"pos x "<<pos.x<<"  pos y  "<<pos.y<<"  i  "<<i<< std::endl;
        if (_campo.contenido(pos)==Cultivo &&
            _estado.parcelas[pos.x][pos.y] == ListoParaCosechar) listas++;
        i++;
    }
    //std::cout<<"Listos para cos  "<<listas<<"  de  "<<total<<std::endl;
    return 10 * listas >= 9 * total;
}

void Sistema::aterrizarYCargarBaterias(Carga b) {
  int i = 0;
	while (i < _enjambre.size()){
		if(_enjambre[i].bateria() < b){
			_enjambre[i].borrarVueloRealizado();
			_enjambre[i].cambiarPosicionActual(posG(*this));
			_enjambre[i].setBateria(100);

		}
		i++;
	}
	return;
}

void Sistema::fertilizarPorFilas() {
	int i = 0;

	while (i < _enjambre.size()){


		while(_enjambre[i].bateria()>0 && _enjambre[i].enVuelo() && hayProducto(_enjambre[i].productosDisponibles(), Fertilizante) && _campo.contenido(_enjambre[i].posicionActual()) == Cultivo){

			if((estadoDelCultivo(_enjambre[i].posicionActual()) == RecienSembrado || estadoDelCultivo(_enjambre[i].posicionActual()) == EnCrecimiento) ){
				_estado.parcelas[_enjambre[i].posicionActual().x][_enjambre[i].posicionActual().y] = ListoParaCosechar;
				_enjambre[i].sacarProducto(Fertilizante);
			}


			//si queda bateria y no se sale del rango, avanza
			if(_enjambre[i].bateria() > 0 && enRango(*this, Posicion {_enjambre[i].posicionActual().x, _enjambre[i].posicionActual().y - 1} ) ){
				_enjambre[i].moverA(Posicion {_enjambre[i].posicionActual().x, _enjambre[i].posicionActual().y - 1});
				_enjambre[i].setBateria(_enjambre[i].bateria() - 1);
				//si queda bateria, avanza. no estoy seguro si solo gasta bateria si la parcela que sigue esta en NoCensado
			}
		}
		i++;
	}
	return;
}

void Sistema::volarYSensar(const Drone &d) {
  Secuencia<Posicion> mov = movimientos();
  int i = 0;
  bool movido = false;
  Posicion seMueveA;
  int indice=buscarDrone(*this, d);
  //Encuentro una parcela libre y lo muevo ahi.
  while(i < mov.size() && !movido){
    seMueveA = suma(_enjambre[indice].posicionActual(), mov[i]);
    if(enRango(*this, seMueveA) && posVacia(*this, seMueveA)){
      _enjambre[indice].moverA(seMueveA);
      _enjambre[indice].setBateria(_enjambre[indice].bateria() - 1);
      movido = true;
    }
    i++;
  }
  //Me fijo si esta sensado o hago lo que tenga que hacer.
  EstadoCultivo comoEsta = _estado.parcelas[seMueveA.x][seMueveA.y];

  if(_campo.contenido(seMueveA) != Cultivo || comoEsta == NoSensado){
    _estado.parcelas[seMueveA.x][seMueveA.y] = RecienSembrado;
  }
  else {
    if((comoEsta == RecienSembrado || comoEsta == EnCrecimiento) && hayProducto(_enjambre[indice].productosDisponibles(), Fertilizante)){
      comoEsta = EnCrecimiento;
      _enjambre[indice].sacarProducto(Fertilizante);
    }
    else if(comoEsta == ConPlaga && hayProducto(_enjambre[indice].productosDisponibles(), PlaguicidaBajoConsumo) && _enjambre[indice].bateria() >= 5 ){
      comoEsta = RecienSembrado;
      _enjambre[indice].setBateria(_enjambre[indice].bateria() - 5);
      _enjambre[indice].sacarProducto(PlaguicidaBajoConsumo);
    }
    else if(comoEsta == ConPlaga && hayProducto(_enjambre[indice].productosDisponibles(), Plaguicida) && _enjambre[indice].bateria() >= 10){
      comoEsta = RecienSembrado;
      _enjambre[indice].setBateria(_enjambre[indice].bateria() - 10);
      _enjambre[indice].sacarProducto(Plaguicida);
    }
    else if(comoEsta == ConMaleza && hayProducto(_enjambre[indice].productosDisponibles(), Herbicida) && _enjambre[indice].bateria() >= 5){
      comoEsta = RecienSembrado;
      _enjambre[indice].setBateria(_enjambre[indice].bateria() - 5);
      _enjambre[indice].sacarProducto(Herbicida);
    }
    else if(comoEsta == ConMaleza && hayProducto(_enjambre[indice].productosDisponibles(), HerbicidaLargoAlcance) && _enjambre[indice].bateria() >= 5){
      unsigned int j = 0;
      while(j < mov.size()){
        Posicion afectada = suma(seMueveA, mov[i]);
        if(_estado.parcelas[afectada.x][afectada.y] == ConMaleza)
          _estado.parcelas[afectada.x][afectada.y] == RecienSembrado;
        j++;
      }
      _enjambre[indice].setBateria(_enjambre[indice].bateria() - 5);
      _enjambre[indice].sacarProducto(HerbicidaLargoAlcance);
    }
  }
  return;
}

void Sistema::mostrar(std::ostream &os) const {
}

void Sistema::guardar(std::ostream &os) const {
}

void Sistema::cargar(std::istream &is) {
}

bool Sistema::operator==(const Sistema &otroSistema) const {
    return false;
}

std::ostream &operator<<(std::ostream &os, const Sistema &s) {
    return os;
}

//Auxiliares


bool enRango(const Sistema &s, Posicion p){
	return p.x >= 0 && p.x < s.campo().dimensiones().ancho && p.y >= 0 && p.y < s.campo().dimensiones().largo;
}


bool posVacia(const Sistema &s, Posicion p){
	bool res = true;
	int i = 0;
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
	int i = 0;
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
