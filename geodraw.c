/*
 * Contains code for drawing geo objects using opengl
 * */
#include "geodraw.h"

GLfloat face_color[] = {0.5, 0.5, 0.5, 1.0};
GLfloat edge_color[] = {1.0, 1.0, 1.0, 1.0};
GLfloat vert_color[] = {0.0, 1.0, 1.0, 1.0};


void FGAPIENTRY geoSolid(geo *G )
{
    //many more options could be used for glBegin GL_POINTS, GL_LINES, GL_LINE_STRIP, GL_LINE_LOOP, GL_TRIANGLES, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_QUADS, GL_QUAD_STRIP, and GL_POLYGON
	//glColor3f (1.0, 0.0, 0.0);
	int iF,iV,vert;
    GLfloat norm[3];
    for(iF=0;iF<G->nF;iF++)
		{
		glBegin( GL_POLYGON );
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE , face_color);
		faceNorm(G->V,G->F+iF*G->nFv,norm);
		glNormal3d(norm[0],norm[1],norm[2]);
		for(iV=0;iV<G->nFv;iV++)
			{
			vert = G->F[iF*G->nFv+iV];
            glVertex3d(G->V[vert*G->dim+0],G->V[vert*G->dim+1],G->V[vert*G->dim+2]);
			}
		glEnd();
	}
		

}

void FGAPIENTRY geoWireframe(geo *G)
{
	int iE,vert1,vert2;
    
    for(iE=0;iE<G->nE;iE++)
		{
        glBegin( GL_LINES );
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE , edge_color);
        vert1 = G->E[iE*G->nEv+0];
        vert2 = G->E[iE*G->nEv+1];
        glVertex3d(G->V[vert1*3+0],G->V[vert1*3+1],G->V[vert1*3+2]);		
        glVertex3d(G->V[vert2*3+0],G->V[vert2*3+1],G->V[vert2*3+2]);		
        //printf("Edge %d from %g %g %g to %g %g %g\n",iE,V[vert1*3+0],V[vert1*3+1],V[vert1*3+2],V[vert2*3+0],V[vert2*3+1],V[vert2*3+2]);
        glEnd();		
	    }
}

void FGAPIENTRY geoPoints(geo *G)
{
    //many more options could be used for glBegin GL_POINTS, GL_LINES, GL_LINE_STRIP, GL_LINE_LOOP, GL_TRIANGLES, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_QUADS, GL_QUAD_STRIP, and GL_POLYGON
	//glColor3f (1.0, 0.0, 0.0);
	int iV;
    glBegin( GL_POINTS );
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE , vert_color);
    for(iV=0;iV<G->nV;iV++)
		{
        glVertex3d(G->V[iV*G->dim+0],G->V[iV*G->dim+1],G->V[iV*G->dim+2]);
		
	}
	glEnd();	

}

