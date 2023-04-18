#include "objeto.h"
#include <glm/glm.hpp>

Punto::Punto(float x, float y, float z) {
    this->x = x;
    this->y = y;
    this->z = z;
}

Punto::Punto() {};

void Punto::setX(float x) {
    this->x = x;
}

void Punto::setY(float y) {
    this->y = y;
}

void Punto::setZ(float z) {
    this->z = z;
}

float Punto::getX() {
    return this->x;
}

float Punto::getY() {
    return this->y;
}

float Punto::getZ() {
    return this->z;
}

Objeto::Objeto(Punto posicion, float angulo_trans_1, float angulo_trans_2, Punto escalado, GLuint listarender, GLuint numvertices, GLuint textura){
    this->posicion = posicion;
    this->angulo_trans_1 = angulo_trans_1;
    this->angulo_trans_2 = angulo_trans_2;
    this->escalado = escalado;
    this->listarender = listarender;
    this->numvertices = numvertices;
    this->textura = textura;
}

Punto Objeto::getPosicion(){
    return this->posicion;
}

void Objeto::setPosicion(Punto p) {
    this->posicion = p;
}

float Objeto::getAnguloTrans1() {
    return angulo_trans_1;
}

void Objeto::setAnguloTrans1(float a1) {
    this->angulo_trans_1 = a1;
}

float Objeto::getAnguloTrans2() {
    return angulo_trans_2;
}

void Objeto::setAnguloTrans2(float a2) {
    this->angulo_trans_2 = a2;
}

Punto Objeto::getEscalado(){
    return this->escalado;
}

void Objeto::setEscalado(Punto p){
    this->escalado = p;
}

GLuint Objeto::getListarender() {
    return listarender;
}

GLuint Objeto::getNumvertices() {
    return numvertices;
}

GLuint Objeto::getTextura() {
    return textura;
}

void Objeto::setListarender(GLuint listarender) {
    this->listarender = listarender;
}

void Objeto::setNumvertices(GLuint numvertices) {
    this->numvertices = numvertices;
}

void Objeto::setTextura(GLuint textura) {
    this->textura = textura;
}

void Objeto::actualizaPosicion(float velocidad, float limite){
    limite += 0.5;

    //actualizacion de la posicion de la base con la velocidad
    this->setPosicion(Punto(this->getPosicion().getX() + velocidad * glm::cos(glm::radians(this->getAnguloTrans1())),
        this->getPosicion().getY() + velocidad * glm::sin(glm::radians(this->getAnguloTrans1())), this->getPosicion().getZ()));
    //comprobamos si se ha salido por algun borde y la colocamos en el borde opuesto
    if (this->getPosicion().getX() >= limite - this->getEscalado().getX() / 2.0) {
        this->setPosicion(Punto(-(limite - 0.1) + this->getEscalado().getX() / 2.0, this->getPosicion().getY(), this->getPosicion().getZ()));
    } 
    if (this->getPosicion().getY() >= limite - this->getEscalado().getX() / 2.0) {
        this->setPosicion(Punto(this->getPosicion().getX(), -(limite - 0.1) + this->getEscalado().getX() / 2.0, this->getPosicion().getZ()));
    }
    if (this->getPosicion().getX() <= -limite + this->getEscalado().getX() / 2.0) {
        this->setPosicion(Punto(limite - this->getEscalado().getX() / 2.0, this->getPosicion().getY(), this->getPosicion().getZ()));
    }
    if (this->getPosicion().getY() <= -limite + this->getEscalado().getX() / 2.0) {
        this->setPosicion(Punto(this->getPosicion().getX(), limite - this->getEscalado().getX() / 2.0, this->getPosicion().getZ()));
    }
}

void Objeto::resetear(){
    this->posicion = Punto(0, 0, this->getPosicion().getZ());
    this->angulo_trans_1 = 0;
    this->angulo_trans_2 = 0;
}
