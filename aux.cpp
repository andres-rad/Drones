#include "aux.h"


int buscarDrone (const Sistema & s, const Drone & d){
  int i=0;
  int ans=-1;

  while (ans==-1 && i<s.enjambreDrones().size()){
    if (d==s.enjambreDrones()[i]) ans=i;
  }

  return ans;
}
//Implementen aqui sus funciones auxiliares globales definidas en aux.h...
