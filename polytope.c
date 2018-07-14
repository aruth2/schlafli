
/* Copyright (c) Mark J. Kilgard, 1997. */

/* This program is freely distributable without licensing fees 
   and is provided without guarantee or warrantee expressed or 
   implied. This program is -not- in the public domain. */

/* This program was requested by Patrick Earl; hopefully someone else
   will write the equivalent Direct3D immediate mode program. */

#include <GL/glut.h>
#include <math.h>
#include <stdio.h>

GLfloat *V, *Vread;
int *E,*F;
int nV,nE,nEv,nF,nFv;
int dim = 3, dimread;
double scale=1;
char *symbol = NULL;
char *interpreter = "python3 ./schlafli_interpreter.py";
GLfloat angle1 = 0.43, angle2 = 0.023;

enum proj {SCHLEGEL3D, SCHLEGEL2D};
int projector = SCHLEGEL3D;


GLfloat light_diffuse[] = {1.0, 0.0, 0.0, 1.0};  /* Red diffuse light. */
GLfloat light_ambient[] = {1.0, 0.0, 0.0, 0.0};  /* Red ambient light. */
GLfloat light_position[] = {1.0, 1.0, 1.0, 1.0};  /* Infinite light location. */


#define cubenV 8
GLfloat cubeV[cubenV*3] = 
{
-1,-1,-1,
-1,-1,1,
-1,1,-1,
-1,1,1,
1,-1,-1,
1,-1,1,
1,1,-1,
1,1,1
};    

#define cubenE 12
#define cubenEv 2
int cubeE[cubenE*cubenEv] =
{
0,1,
0,2,
0,4,
1,3,
1,5,
2,3,
2,6,
3,7,
4,5,
4,6,
5,7,
6,7
};

#define cubenF 6
#define cubenFv 4
int cubeF[cubenF*cubenFv]=
{
0,1,3,2,
0,1,5,4,
0,2,6,4,
6,7,5,4,
3,2,6,7,
3,1,5,7
};

double p4[12] = {
1.0,1.0,-1.0,-1.0,
-1.0,1.0,-1.0,1.0,
1.0,-1.0,-1.0,1.0
};


/*
void faceNorm(GLfloat V[nV][3], int *F, GLfloat *norm)
{
	//Computes a normal to a face F
	//The cross product of the vectors from vertex 0 to 1 and 1 to 2 are used
	//The normal vector is not normalized
	
	GLfloat *V0,*V1,*V2;
	V0 = V[F[0]]; V1 = V[F[1]]; V2 = V[F[2]];
	//V0 = *(V+F[0]); V1 = *(V+F[1]); V2 = *(V+F[2]);
	GLfloat P[3],Q[3];
	P[0] = V1[0]-V0[0]; P[1] = V1[1]-V0[1]; P[2] = V1[2]-V0[2];
	Q[0] = V2[0]-V1[0]; Q[1] = V2[1]-V1[1]; Q[2] = V2[2]-V1[2];
	 
	norm[0] = P[1]*Q[2]-Q[1]*P[2];
	norm[1] = P[2]*Q[0]-P[0]*Q[2];
	norm[2] = P[0]*Q[1]-P[1]*Q[0];
	
	//printf("Norm is %f %f %f\n",norm[0],norm[1],norm[2]);
}  
 
void FGAPIENTRY glutSolid( )
{
    //many more options could be used for glBegin GL_POINTS, GL_LINES, GL_LINE_STRIP, GL_LINE_LOOP, GL_TRIANGLES, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_QUADS, GL_QUAD_STRIP, and GL_POLYGON
	

	int iF,iV,vert;
    GLfloat norm[3];
	//glBegin( GL_QUADS );
	//glBegin( GL_LINES_STRIP );
	
    for(iF=0;iF<nF;iF++)
		{
		//glBegin( GL_POLYGON );
		glBegin( GL_LINE_LOOP );

		faceNorm(V,F[iF],norm);
		glNormal3d(norm[0],norm[1],norm[2]);
		//for(iV=0;iV<nFv;iV++)
		for(iV=0;iV<nFv;iV++)
			{
			vert = F[iF][iV];
			glVertex3d(V[vert][0],V[vert][1],V[vert][2]);
			}
		glEnd();
	}
	//glEnd();
		

}*/

void FGAPIENTRY glutWireframe( )
{

	int iE,vert1,vert2;
    
    for(iE=0;iE<nE;iE++)
		{
        glBegin( GL_LINES );
        vert1 = E[iE*nEv+0];
        vert2 = E[iE*nEv+1];
        glVertex3d(V[vert1*3+0],V[vert1*3+1],V[vert1*3+2]);		
        glVertex3d(V[vert2*3+0],V[vert2*3+1],V[vert2*3+2]);		
        //printf("Edge %d from %g %g %g to %g %g %g\n",iE,V[vert1*3+0],V[vert1*3+1],V[vert1*3+2],V[vert2*3+0],V[vert2*3+1],V[vert2*3+2]);
        glEnd();		
	    }
}

void
display(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //glutSolid();
  glutWireframe();
  glRotatef(angle1, 1.0, 0.0, 0.0);
  glRotatef(angle2, 1.0, 5.0, 0.0);
  glutSwapBuffers();
}

void
init(void)
{
	
  /* Setup the view of the cube. */
  glMatrixMode(GL_PROJECTION);
  gluPerspective( /* field of view in degree */ 50.0,
    /* aspect ratio */ 1.0,
    /* Z near */ 3.0, /* Z far */ -3.0);
  glMatrixMode(GL_MODELVIEW);
  gluLookAt(3.0, 3.0, 3.0,  /* eye is at (10,10,10) */
    0.0, 0.0, 0.0,      /* center is at (0,0,0) */
    0.0, 1.0, 0.);      /* up is in positive Y direction */

}

void 
idle(void)
{
  glutPostRedisplay();
}

void 
visible(int vis)
{
  if (vis == GLUT_VISIBLE)
    glutIdleFunc(idle);
  else
    glutIdleFunc(NULL);
}

void center()
{
    double moment[dimread];
    int iV,iD;
    for(iD=0;iD<dimread;iD++)
        moment[iD]=0;
    for(iV=0;iV<nV;iV++)
        for(iD=0;iD<dimread;iD++)
            moment[iD] += *(Vread+iV*dimread+iD)/nV;
    for(iV=0;iV<nV;iV++)
        for(iD=0;iD<dimread;iD++)
            *(Vread+iV*dimread+iD) -= moment[iD];
            
}

void scaleV(double scale)
{
     int iV,iD;
    for(iV=0;iV<nV;iV++)
        for(iD=0;iD<dimread;iD++)
            *(Vread+iV*dimread+iD) *= scale;
}

void simpleproject()
{
    //Constructs 3 basic projectors to convert the n-dimensional shape to a 3D shape
    int d1,d2;
    double *p;
    if(dimread == 4)
    {
        p = p4;
    }
    else
    {
    p = malloc(3*dimread*sizeof(double));
    for(d1=0;d1<3;d1++)
        for(d2=0;d2<dimread;d2++)
            {
            if(d1 == d2)
            p[dimread*d1+d2] = 1;
            else
            if(d2<3)
            p[dimread*d1+d2] = 0;
            else
            p[dimread*d1+d2] = 1;
            }
     }       
    V = malloc(3*nV*sizeof(double));
    int iV;
    for(iV = 0;iV<nV;iV++)
        for(d1=0;d1<3;d1++)
            {
            V[iV*3+d1] = 0;    
            for(d2=0;d2<dimread;d2++)
                V[iV*3+d1] += p[dimread*d1+d2] *Vread[iV*dimread+d2];
            }
}

void pointprojectdown(double *point, double *pointlowerdim, int dim, double d)
{
    //Projects from [0,0,....,d] towards the hyperplane where [x,y,....,0]
    //If a hyperface is oriented along the nth-direction, i.e. the hyperface neareast the point [0,0,....,d]
    // has a constant value for the nth direction [x,y,...,n] n=constant n<d
    //Then this can be used for a shlegel projection
    double projectionvector[dim];
    int i;
    double projmultiplier;
    for(i=0;i<dim;i++)
    {
        if(i==dim-1)
        projectionvector[i] = point[i]-d;
        else
        projectionvector[i] = point[i];
    }
    projmultiplier = -point[dim-1]/(d-point[dim-1]);
    for(i=0;i<dim-1;i++)
    {
        pointlowerdim[i] = point[i] + projmultiplier*projectionvector[i];
    }
}
void printmatrix(double *matrix, int numberOfRows, int numberOfColumns, int stride, int rowmajor)
{
	//printf("Printing a matrix with dimensions of %d X %d using a stride of %d\n",numberOfRows,numberOfColumns,stride);
	/****************************************************/
	/* This program outputs the contents of a matrix in */
	/* nice neat format.								*/
	/****************************************************/
	int i;
	double value;
	for (i = 0;i<numberOfRows*numberOfColumns ;i++ )
	{
			if(rowmajor)
			value = *(matrix+i*stride);
			else
			{
			int column = i%numberOfColumns;
			int row = i/numberOfColumns;
			value = *(matrix+column*numberOfRows+row);
			}
			printf("%.03f \t",value);
			
			if ((i+1) % numberOfColumns == 0)
			{
				printf("\n");
			}
			
	}
}

void schlegel(int finaldim, double distance)
{
    double *Vbuffer1 = malloc(dimread*nV*sizeof(double)),*Vbuffer2 = malloc(dimread*nV*sizeof(double));
    double *bufferswap;
    int iV;
    int currentdim;
    int iD;
    for(iV =0;iV<nV;iV++)
    for(iD=0;iD<dimread;iD++)
    Vbuffer1[iV*dimread+iD] = Vread[iV*dimread+iD];
    
    for(currentdim=dimread;currentdim>finaldim;currentdim--)
    {
    for(iV =0;iV<nV;iV++)
    {
        pointprojectdown(Vbuffer1+iV*currentdim,Vbuffer2+iV*(currentdim-1),currentdim,distance);
    }
        bufferswap=Vbuffer1;
        Vbuffer1=Vbuffer2;
        Vbuffer2=bufferswap;
    }
    V = malloc(dim*nV*sizeof(double));
    
    for(iV =0;iV<nV;iV++)
    for(iD=0;iD<dim;iD++)
    if(iD<finaldim)
    V[iV*dim+iD] = Vbuffer1[iV*finaldim+iD];
    else
    V[iV*dim+iD] = 0;
    free(Vbuffer1); free(Vbuffer2);
}

void readVertices(char *filename)
{
    FILE *infile = fopen(filename,"r");
    fscanf(infile,"%d %d ",&nV,&dimread);//First line of file shows number of vertices and number of dimensions
    printf("%d vertices and %d dimensions\n",nV,dimread);
    Vread = malloc(nV*dimread*sizeof(double));
    int vertex,d;
    for(vertex=0;vertex<nV;vertex++)
    for(d=0;d<dimread;d++)
    {
    fscanf(infile,"%f",Vread+vertex*dimread+d);
    //printf("At position %ld Read %g\n",ftell(infile),*(Vread+vertex*dim+d));
    }
    fclose(infile);
}

void readEdges(char *filename)
{
    FILE *infile = fopen(filename,"r");
    fscanf(infile,"%d %d",&nE,&nEv);
    printf("%d Edges and %d nEv\n",nE,nEv);
    E = malloc(nE*nEv*sizeof(int));
    int edge,edgevertex;
    for(edge=0;edge<nE;edge++)
    for(edgevertex=0;edgevertex<nEv;edgevertex++)
    {
    fscanf(infile,"%d",E+edge*nEv+edgevertex);
    //printf("Read %d\n",*(E+edge*nEv+edgevertex));
    }
    fclose(infile);
}

int tokcount(char *line, char *del)
{
    int i=0;
    char *tok;
    
    tok = strtok(line,del);
    while( tok != NULL)
    {
        i++;
        tok = strtok(NULL,del);
    }
    return i;
}

void readAll(char *filename, int numD)
{
    dimread = numD;
    int iV,iE,iD;
    FILE *infile = fopen(filename,"r");
    char *line = malloc(1024*sizeof(char));
    fgets(line,1024,infile);
    //printf("Got line %s\n",line);
    nV = atof(line);
    //printf("%d vertices\n",nV);
    Vread = malloc(nV*dimread*sizeof(double));
    char *tok;
    for(iV=0;iV<nV;iV++)
        {
        fgets(line,1024,infile);
        //printf("Got line %s\n",line);
        tok = strtok(line," ");
        //if(tok == NULL)
        //printf("NULL tok\n");
        for(iD=0;iD<numD;iD++)
            {
            //printf("Tok is %s\n",tok);
            Vread[iV*numD + iD] = atof(tok);
            tok = strtok(NULL," ");
            }
        }
    fgets(line,1024,infile);//Should be a blank line
    fgets(line,1024,infile);
    //printf("Got line %s\n",line);
    nE = atoi(line);
    nEv = 2;
    //printf("%d edges\n",nE);
    E = malloc(nE*2*sizeof(int));
    for(iE=0;iE<nE;iE++)
        {
        fgets(line,1024,infile);
        //printf("Got line %s\n",line);
        tok = strtok(line," ");
        for(iD=0;iD<2;iD++)
            {
            //printf("Tok is %s\n",tok);
            E[iE*2 + iD] = atoi(tok);
            tok = strtok(NULL," ");
            }
        }
}

int
main(int argc, char **argv)
{
  glutInit(&argc, argv);
  if (argc <= 2)
  {
  }
  else
  {
      int i;
      for(i=1;i<argc;i++)
      {
          if(!strcmp(argv[i],"-scale"))
          scale = atof(argv[i+1]);
          if(!strcmp(argv[i],"-symbol"))
          symbol = argv[i+1];
          if(!strcmp(argv[i],"schlegel3d"))
          projector = SCHLEGEL3D;
          if(!strcmp(argv[i],"schlegel2d"))
          projector = SCHLEGEL2D;
      }
      
  }
  if(symbol == NULL)
  {
      V=cubeV; E=cubeE; F=cubeF;
      nV=cubenV; nE=cubenE; nEv=cubenEv;
      nF=cubenF; nFv=cubenFv;
  }
  else
  {
      char command[1000];
      sprintf(command,"echo \"%s \" | %s > geo",symbol,interpreter);
      //printf("%s\n",command);
      system(command);
      int numD = tokcount(symbol," ")+1;
      printf("Number of dimensions %d\n",numD);
      readAll("geo",numD);
      center();
  }
  if(scale != 1)
  scaleV(scale);
  if(numD == 2)
  project = SCHLEGEL2D;
  switch(projector)
  {
      case SCHLEGEL3D:
      schlegel(3,2*scale);
      break;
      case SCHLEGEL2D:
      schlegel(2,2*scale);
      break;
  }




  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutCreateWindow("Polytope Drawing");
  glutDisplayFunc(display);
  init();
  glutVisibilityFunc(visible);
  glutMainLoop();
  return 0;             /* ANSI C requires main to return int. */
}
