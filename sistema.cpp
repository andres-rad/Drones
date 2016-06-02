#include "sistema.h"
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
	while(i < _enjambre.size()){
		_enjambre[i].setBateria(100);
		_enjambre[i].borrarVueloRealizado();
		_enjambre[i].cambiarPosicionActual(posG(*this));
		//Si no están en vuelo como hago que PosiciónActual devuelva el granero, porque vuelosRealizados tiene que estar vacío.
	}
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
  int i = 0;

	while(i < _campo.dimensiones().ancho){
		int j = 0;
		while (j < _campo.dimensiones().largo){
			if(_estado.parcelas[i][j] == ConPlaga){

				if( j - 1 >= 0 && _campo.contenido(Posicion { i, j-1}) == Cultivo){
					_estado.parcelas[i][j-1] = ConPlaga;
				}

				if ( j != _campo.dimensiones().largo - 1 && _campo.contenido(Posicion { i, j+1}) == Cultivo){
					_estado.parcelas[i][j+1] = ConPlaga;
				}

				if (i - 1 >= 0 && _campo.contenido(Posicion { i-1, j}) == Cultivo) {
					_estado.parcelas[i-1][j] = ConPlaga;
				}

				if (i != _campo.dimensiones().ancho - 1 && _campo.contenido(Posicion { i+1, j}) == Cultivo){
					_estado.parcelas[i+1][j] = ConPlaga;
				}
			}
			j++;
		}
		i++;
	}
  return;
}

void Sistema::despegar(const Drone &d) {
  Secuencia<Posicion> posicionesActuales;
	int i = 0;
	int despegado = 0;
	Secuencia<Posicion> adyacentes = movimientos();
	Posicion granero = posG(*this);

	while (i < movimientos.size() && !despegado){
		if(enRango(*this, suma(granero, adyacentes[i])) && posVacia(*this, suma(granero, adyacentes[i]))){
		//Aca hay que usar moverA que puede que no está claro que quieren que haga.
		despegado++;
		}
		return;
	}
}

bool Sistema::listoParaCosechar() const {
  //Completar Teo
    return false;
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


		while(_enjambre[i].bateria()>0 && _enjambre[i].productosDisponibles().size()>0 && _campo.contenido(_enjambre[i].posicionActual()) == Cultivo){

			if( estadoDelCultivo(_enjambre[i].posicionActual()) == ConPlaga){ //se fija si hay plaga

				if ( hayProducto(_enjambre[i].productosDisponibles(), PlaguicidaBajoConsumo) &&  _enjambre[i].bateria() > 4 ){ //si tiene algun plaguicida y suficiente bateria para usarlo, elimina la plaga
					_estado.parcelas[_enjambre[i].posicionActual().x][_enjambre[i].posicionActual().y] = RecienSembrado;
					_enjambre[i].setBateria(_enjambre[i].bateria() - 5);
					_enjambre[i].sacarProducto(PlaguicidaBajoConsumo);
				}
				else if (hayProducto(_enjambre[i].productosDisponibles(), Plaguicida) &&  _enjambre[i].bateria() > 9){
					_estado.parcelas[_enjambre[i].posicionActual().x][_enjambre[i].posicionActual().y]  = RecienSembrado;
					_enjambre[i].setBateria(_enjambre[i].bateria() - 10);
					_enjambre[i].sacarProducto(Plaguicida);
				}
			}
			else if ( estadoDelCultivo(_enjambre[i].posicionActual()) == ConMaleza && _enjambre[i].bateria() > 4 ){//si no era plaga, se fija si es maleza y le da la bateria para eliminarla
				//chequea que haya productos. ademas, si la siguiente parcela tiene maleza (o no queda herbicida normal), usa la de largo alcance
				if ( hayProducto(_enjambre[i].productosDisponibles(), HerbicidaLargoAlcance) && (estadoDelCultivo(Posicion {_enjambre[i].posicionActual().x, _enjambre[i].posicionActual().y - 1}) == ConMaleza || !( hayProducto(_enjambre[i].productosDisponibles(), Herbicida) ) ) ){
					_estado.parcelas[_enjambre[i].posicionActual().x][_enjambre[i].posicionActual().y]  = RecienSembrado;
					_estado.parcelas[_enjambre[i].posicionActual().x][_enjambre[i].posicionActual().y - 1]  = RecienSembrado;//elimina la maleza en la siguiente parecela
					_enjambre[i].setBateria(_enjambre[i].bateria() - 5);
					_enjambre[i].sacarProducto(HerbicidaLargoAlcance);
				}
				else if(hayProducto(_enjambre[i].productosDisponibles(), Herbicida)){
					_estado.parcelas[_enjambre[i].posicionActual().x][_enjambre[i].posicionActual().y]  = RecienSembrado;
					_enjambre[i].setBateria(_enjambre[i].bateria() - 5);
					_enjambre[i].sacarProducto(Herbicida);
				}
			}
			//si puede fertilizar
			else if((estadoDelCultivo(_enjambre[i].posicionActual()) == RecienSembrado || estadoDelCultivo(_enjambre[i].posicionActual()) == EnCrecimiento) && (hayProducto(_enjambre[i].productosDisponibles(), Fertilizante))){
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
  //Encuentro una parcela libre y lo muevo ahi.
  while(i < mov.size() && !movido){
    seMueveA = suma(posicionActual(d), mov[i]);
    if(enRango(*this, seMueveA) && posVacia(*this, seMueveA)){
      d.moverA(seMueveA);
      d.setBateria(d.bateria() - 1);
      movido = true;
    }
    i++;
  }
  //Me fijo si esta sensado o hago lo que tenga que hacer.
  EstadoCultivo comoEsta = _estado.parcela[seMueveA.x][seMueveA.y];

  if(_campo.parcela.contenido(seMueveA) != Cultivo || comoEsta == NoSensado){
    _estado.parcela[seMueveA.x][seMueveA.y] = RecienSembrado;
  }
  else {
    if((comoEsta == RecienSembrado || comoEsta == EnCrecimiento) && hayProducto(d.productosDisponibles(), Fertilizante)){
      comoEsta = EnCrecimiento;
      d.sacarProducto(Fertilizante);
    }
    else if(comoEsta == ConPlaga && hayProducto(d.productosDisponibles(), PlaguicidaBajoConsumo) && d.bateria() >= 5 ){
      comoEsta = RecienSembrado;
      d.setBateria(d.bateria() - 5);
      d.sacarProducto(PlaguicidaBajoConsumo);
    }
    else if(comoEsta == ConPlaga && hayProducto(d.productosDisponibles(), Plaguicida) && d.bateria() >= 10){
      comoEsta = RecienSembrado;
      d.setBateria(d.bateria() - 10);
      d.sacarProducto(Plaguicida);
    }
    else if(comoEsta == ConMaleza && hayProducto(d.productosDisponibles(), Herbicida) && d.bateria() >= 5){
      comoEsta = RecienSembrado;
      d.setBateria(d.bateria() - 5);
      d.sacarProducto(Herbicida);
    }
    else if(comoEsta == ConMaleza && hayProducto(d.productosDisponibles(), HerbicidaLargoAlcance) && d.bateria >= 5){
      unsigned int j = 0;
      while(j < mov.size()){
        Posicion afectada = suma(seMueveA, mov[i]);
        if(_estado.parcela[afectada.x][afectada.y] == ConMaleza)
          _estado.parcela[afectada.x][afectada.y] == RecienSembrado;
        j++;
      }
      d.setBateria(d.bateria() - 5);
      d.sacarProducto(HerbicidaLargoAlcance);
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
