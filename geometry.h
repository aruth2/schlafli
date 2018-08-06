#include "supp.h"

typedef struct{
float *V, *Vread;
int *E,*F;
int nV,nE,nEv,nF,nFv;
int dim, dimread;
} geo;

enum proj {SCHLEGEL3D, SCHLEGEL2D};

void faceNorm(float *V, int *F, float *norm);
void center(geo *G);
void reorderFaceVertices(geo *G);
void scaleV(geo *G, double scale);
void pointprojectdown(double *point, double *pointlowerdim, int dim, double d);
void printGeoCount(geo *G);
void schlegel(geo *G, int finaldim, double distance);
void project(geo *G, int projector,double schlegeldistance);
void readAll(geo *G,char *filename);
geo * geoFromSchlafliSymbol(char *symbol,int vertexlimit);
void dimensionalrotation(float *inputV,float *outputV,int nV,int dim,int dim1,int dim2,float angle);
