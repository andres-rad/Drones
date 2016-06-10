#include "sistema.h"
#include "auxiliares.h"
#include <algorithm>

Sistema::Sistema() {
}

Sistema::Sistema(const Campo &c, const Secuencia<Drone> &ds) {
    _campo = c;
	_enjambre = ds;
	_estado = Grilla<EstadoCultivo> (c.dimensiones());
	int i = 0;
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
        //En cada guarda me fijo que la parcela adyacente a considerar sea una posicion valida.
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
	unsigned int i = 0;
	int despegado = 0;
  int indice = buscarDrone(*this, d);
	Secuencia<Posicion> adyacentes = movimientos();
	Posicion granero = posG(*this);

	while (i < adyacentes.size() && !despegado){
		if(enRango(*this, suma(granero, adyacentes[i])) && posVacia(*this, suma(granero, adyacentes[i]))){
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
        if (_campo.contenido(pos) == Cultivo && _estado.parcelas[pos.x][pos.y] == ListoParaCosechar) {
          listas++;
        }
        i++;
    }
    return 10 * listas >= 9 * total;
}

void Sistema::aterrizarYCargarBaterias(Carga b) {
  unsigned int i = 0;
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
	unsigned int i = 0;
	while (i < _enjambre.size()){
		while(_enjambre[i].bateria()>0 &&
          _enjambre[i].enVuelo() &&
          hayProducto(_enjambre[i].productosDisponibles(), Fertilizante) &&
          _campo.contenido(_enjambre[i].posicionActual()) == Cultivo){

			if((estadoDelCultivo(_enjambre[i].posicionActual()) == RecienSembrado || estadoDelCultivo(_enjambre[i].posicionActual()) == EnCrecimiento)){
				_estado.parcelas[_enjambre[i].posicionActual().x][_enjambre[i].posicionActual().y] = ListoParaCosechar;
				_enjambre[i].sacarProducto(Fertilizante);
			}
			//si queda bateria y no se sale del rango, avanza
			if(_enjambre[i].bateria() > 0 && enRango(*this, Posicion {_enjambre[i].posicionActual().x, _enjambre[i].posicionActual().y - 1} )){
				_enjambre[i].moverA(Posicion {_enjambre[i].posicionActual().x, _enjambre[i].posicionActual().y - 1});
				_enjambre[i].setBateria(_enjambre[i].bateria() - 1);
				//si queda bateria, avanza. no estoy seguro si solo gasta bateria si la parcela que sigue esta en NoSensado
			}
		}
		i++;
	}
	return;
}

void Sistema::volarYSensar(const Drone &d) {
  Secuencia<Posicion> mov = movimientos();
  unsigned int i = 0;
  bool movido = false;
  Posicion seMueveA;
  int indice = buscarDrone(*this, d);
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
    //Cada guarda chequea si se cumplen todas las condiciones para poder aplicar el producto correspondiente.
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
          _estado.parcelas[afectada.x][afectada.y] = RecienSembrado;
        j++;
      }
      _enjambre[indice].setBateria(_enjambre[indice].bateria() - 5);
      _enjambre[indice].sacarProducto(HerbicidaLargoAlcance);
    }
  }
  return;
}

void Sistema::mostrar(std::ostream &os) const {
    os << "Sistema:" << std::endl;
    _campo.mostrar(os);
    os << std::endl;
    unsigned int i = 0;
    while(i < _enjambre.size()){
      _enjambre[i].mostrar(os);
      os << std::endl;
      i++;
    }

    int j = 0;
    while(j < _campo.dimensiones().ancho){

      int k = 0;
      while(k < _campo.dimensiones().largo{

        if(_estado.parcelas[j][k] == NoSensado){
          os << "NS ";
        }
        else if(_estado.parcelas[j][k] == RecienSembrado){
           os << "RS ";
        }
        else if(_estado.parcelas[j][k] == EnCrecimiento){
          os << "EC "
        }
        else if(_estado.parcelas[j][k] == ListoParaCosechar){
          os << "LC ";
        }
        else if(_estado.parcelas[j][k] == ConPlaga){
          os << "CP ";
        }
        else{
          os << "CM ";
        }
        k++;
      }
      os << std::endl;
      j++;
    }
    return;
}

void Sistema::guardar(std::ostream &os) const {
    os << "{ S ";
    _campo.guardar(os);
    os << _enjambre;
    os << _estado.parcelas << std::endl;
}

void Sistema::cargar(std::istream &is) {
  char currChar;
  is >> currChar;

  while (currChar != 'S'){
    is >> currChar;
  }

  _campo.cargar(is);
  is >> _enjambre;
  is >> _estado.parcelas;
  return;
}

bool Sistema::operator==(const Sistema &otroSistema) const {
    bool sonIguales = true;
    if (!(_campo == otroSistema.campo()))
        sonIguales = false;

    int i = 0;
    while(i < _campo.dimensiones().ancho){
      int j = 0;
      while(j < _campo.dimensiones().largo){
        Posicion pos{i, j};
        if(_campo.contenido(pos) == Cultivo && estadoDelCultivo(pos) != otroSistema.estadoDelCultivo(pos)){
          sonIguales = false;
         }
        j++;
      }
      i++
    }

    if (!(_enjambre == otroSistema.enjambreDrones()))
        sonIguales = false;

    return sonIguales;
}

std::ostream &operator<<(std::ostream &os, const Sistema &s) {
    s.guardar(os);
    return os;
}
