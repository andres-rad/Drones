#include "aux.h"


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

//Implementen aqui sus funciones auxiliares globales definidas en aux.h...
