#ifndef OBJETO_H
#define OBJETO_H

#include <glad.h>

class Punto{
    private:
        float x, y, z;
    public:
        Punto(float x, float y, float z);
        Punto();
        void setX(float x);
        void setY(float y);
        void setZ(float z);
        float getX();
        float getY();
        float getZ();
};

class Objeto {
    private:
        Punto posicion; //posicion del objeto
        float angulo_trans_1; //angulo de rotacion en el eje x
        float angulo_trans_2; //angulo de rotacion en el ejez
        Punto escalado; //escalado en los dos ejes
        GLuint* listarender; //VAO
        GLuint numvertices; //Número de vertices del poligono
        GLuint* textura; //textura asociada
    public:
        Objeto(Punto posicion, float angulo_trans_1, float angulo_trans_2, Punto escalado, GLuint* listarender, GLuint numvertices, GLuint* textura); 
        Punto getPosicion();
        void setPosicion(Punto p);
        float getAnguloTrans1();
        void setAnguloTrans1(float a1); 
        float getAnguloTrans2();
        void setAnguloTrans2(float a2); 
        Punto getEscalado();
        void setEscalado(Punto p);
        GLuint* getListarender();
        void setListarender(GLuint* listarender);
        GLuint getNumvertices();
        void setNumvertices(GLuint numvertices);
        GLuint* getTextura();
        void setTextura(GLuint* textura);
        void actualizaPosicion(float velocidad, float limite);
        void resetear();
};

#endif /* OBJETO_H */
