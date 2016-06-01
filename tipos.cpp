#include "tipos.h"

std::ostream &operator<<(std::ostream &os, const Parcela &p) {
    return os;
}

std::ostream &operator<<(std::ostream &os, const Producto &p) {
    return os;
}

std::ostream &operator<<(std::ostream &os, const EstadoCultivo &e) {
    return os;
}

const bool operator==(const Posicion &p1, const Posicion &p2) {
    return p1.x == p2.x && p1.y == p2.y;
}

const bool operator==(const InfoVueloCruzado &a, const InfoVueloCruzado &b) {
    return a.posicion == b.posicion && a.cantidadCruces == b.cantidadCruces;
}