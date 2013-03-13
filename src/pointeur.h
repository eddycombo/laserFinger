#ifndef POINTEUR_H_INCLUDED
#define POINTEUR_H_INCLUDED





#include "ofMain.h"

class pointeur{

public:

pointeur();
pointeur(int ids, int xs, int ys, float sizes);

void update();
void draw();

int x, y;  // position entre 0..résolution
float x01, y01; // poosition x y scalée entre 0 et 1
int id;  // numero Id du blob pointeur
float size; // sa taille en pixels (area)


};




#endif
