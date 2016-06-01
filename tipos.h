#ifndef TIPOS_H
#define TIPOS_H

#include <iostream>
#include <vector>

typedef int ID;
typedef int Carga;
typedef int Ancho;
typedef int Largo;

#define Secuencia std::vector

enum Parcela {
    Cultivo, Granero, Casa
};

std::ostream &operator<<(std::ostream &os, const Parcela &p);

enum Producto {
    Fertilizante, Plaguicida, PlaguicidaBajoConsumo, Herbicida, HerbicidaLargoAlcance
};

std::ostream &operator<<(std::ostream &os, const Producto &p);

enum EstadoCultivo {
    RecienSembrado, EnCrecimiento, ListoParaCosechar, ConMaleza, ConPlaga, NoSensado
};

std::ostream &operator<<(std::ostream &os, const EstadoCultivo &e);

struct Posicion {
    int x;
    int y;
};

const bool operator==(const Posicion &p1, const Posicion &p2);

struct Dimension {
    Ancho ancho;
    Largo largo;
};

template<class T>
struct Grilla {
    Grilla() { }

    Grilla(Dimension d) {
        parcelas.resize(d.ancho, std::vector<T>(d.largo, static_cast<T>(0)));
    }

    std::vector<std::vector<T> > parcelas;
};

struct InfoVueloCruzado {
    Posicion posicion;
    int cantidadCruces;
};

const bool operator==(const InfoVueloCruzado &a, const InfoVueloCruzado &b);

#endif /*TIPOS_H*/
