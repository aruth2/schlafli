
/* Copyright (c) Anthony Ruth, 2018.
 * polytope.c
 * 
 * Available from: https://github.com/aruth2/schlafli
 * 
 * Compile with 
 * gcc -w -o "polytope" "polytope.c" -lGL -lGLU -lglut -lm
 * */

#include <GL/glut.h>
#include <math.h>
#include <stdio.h>

char *helpstring = 
"Usage: polytope [options]\n\n"
"Command line [options] are:\n"
"-symbol '[schlafli symbol]' - by default this is set to -symbol '4 3', a cube\n"
"-vertexlimit [limit] - limits the number of vertices produced by the interpreter, necessary for tesselations\n"
"-scale [multiplier] - scales the geometry of the shape by the multiplier\n"
"-schlegeldistance [distance] - specifies the distance from the origin used in schlegel projection\n"
"-pointsize [size]\n"
"-linewidth [size]\n"
"schlegel3d - {default} projects a more-than-3 dimensional shape into 3 dimensions using the schlegel projection method.\n"
"schlegel2d - projects a more-than-2 dimensional shape into 2 dimensions using the schlegel projection method.\n"
"points - draw points\n"
"wireframe - {default} draw edges\n"
"solid - draw faces. Misbehaves when drawing star polytopes because GL_POLYGON does not support concave shapes\n";

//Internal variables and storage of essential data
typedef struct{
GLfloat *V, *Vread;
int *E,*F;
int nV,nE,nEv,nF,nFv;
int dim, dimread;
char *symbol;
} geo;

geo *mainGeo;

//Internal default variables
char *interpreter = "python3 ./schlafli_interpreter.py";
GLfloat omegax = 0.13, omegay = 0.023, omega_dim_dimminus1 = 0;
char defaultsymbol[4] = "4 3";

//Options controlled by commandline
enum proj {SCHLEGEL3D, SCHLEGEL2D};
int points = 0;
int wireframe = 0;
int solid = 0;
int projector = SCHLEGEL3D;
int schlegelrotate = 0;
int autorotate = 1;
double schlegeldistance = 0;
double scale=1;
double linewidth = 2;
double pointsize = 7;
int vertexlimit = 0;

//Options used for color and lighting
GLfloat light_diffuse[] = {0.05, 0.05, 0.05, 0.0};  /* White diffuse light. */
GLfloat light_ambient[] = {0.25, 0.25, 0.25, 1.0};  /* ambient light. */
GLfloat light_position[] = {0.0, 5.0, 0.0, 1.0};  /* Infinite light location. */

GLfloat face_color[] = {0.5, 0.5, 0.5, 1.0};
GLfloat edge_color[] = {1.0, 1.0, 1.0, 1.0};
GLfloat vert_color[] = {0.0, 1.0, 1.0, 1.0};

void faceNorm(GLfloat *V, int *F, GLfloat *norm)
{
	//Computes a normal to a face F
	//The cross product of the vectors from vertex 0 to 1 and 1 to 2 are used
	//The magnitude of the normal vector is not normalized
	
	GLfloat *V0,*V1,*V2;
	V0 = V + 3*F[0]; V1 = V+ 3*F[1]; V2 = V + 3*F[2];

	GLfloat P[3],Q[3];
	P[0] = V1[0]-V0[0]; P[1] = V1[1]-V0[1]; P[2] = V1[2]-V0[2];
	Q[0] = V2[0]-V1[0]; Q[1] = V2[1]-V1[1]; Q[2] = V2[2]-V1[2];
	 
	norm[0] = P[1]*Q[2]-Q[1]*P[2];
	norm[1] = P[2]*Q[0]-P[0]*Q[2];
	norm[2] = P[0]*Q[1]-P[1]*Q[0];
	
	//printf("Norm is %f %f %f\n",norm[0],norm[1],norm[2]);
}  
 
void FGAPIENTRY glutSolid(geo *G )
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

void FGAPIENTRY glutWireframe(geo *G)
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

void FGAPIENTRY glutPoints(geo *G)
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

void
display(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
  //Call each of the used plotting functions
  if(points)
  glutPoints(mainGeo);
  if(wireframe)
  glutWireframe(mainGeo);
  if(solid)
  glutSolid(mainGeo);
 
  if(autorotate)
  {
  glRotatef(omegax, 1.0, 0.0, 0.0);
  glRotatef(omegay, 0.0, 1.0, 0.0);
    }
  glutSwapBuffers();
}

void highDrotation()
{
    //This oscillates the point used for schlegel projection from its maximum value to minus its maximum value passing through the figure
    omega_dim_dimminus1 = fmod(omega_dim_dimminus1+0.43,360);
    double newdistance;
    newdistance = (180-omega_dim_dimminus1)/180 * schlegeldistance;
    switch(projector)
  {
      case SCHLEGEL3D:
      schlegel(3,newdistance);
      break;
      case SCHLEGEL2D:
      schlegel(2,newdistance);
      break;
  }
}

void 
idle(void)
{
  if(schlegelrotate)
     highDrotation();
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

void center(geo *G)
{
    //Computes the average location of the vertices, then translates the figure so that the average lies at (0,0,0)
    double moment[G->dimread];
    int iV,iD;
    for(iD=0;iD<G->dimread;iD++)
        moment[iD]=0;
    for(iV=0;iV<G->nV;iV++)
        for(iD=0;iD<G->dimread;iD++)
            moment[iD] += *(G->Vread+iV*G->dimread+iD)/G->nV;
    for(iV=0;iV<G->nV;iV++)
        for(iD=0;iD<G->dimread;iD++)
            *(G->Vread+iV*G->dimread+iD) -= moment[iD];
            
}

int intcontains(int *list, int search, int numitems)
{
    //Returns 1 if the list contains the search, 0 otherwise
	if(numitems == 0)
	return 0;
	
	int index;
	for(index = 0;index<numitems;index++)
	if(*(list+index) == search)
	return 1;
	
	return 0;
}

void reorderFaceVertices(geo *G)
{
    //This reorders the vertices in a face so that the vertices are ordered in a way that completes a loop
    //This is necessary when using the solid option as the list of vertices in the face must be ordered.
    int faceStore[G->nFv];
    int usedVertices[G->nFv];
    int numUsed = 0;
    int iF,iFv1,iFv2,iE,v1,v2;
    int edgeFound=0;
    for(iF=0;iF<G->nF;iF++)
    {
        for(iFv1=0;iFv1<G->nFv;iFv1++)
        {
            faceStore[iFv1] = G->F[iF*G->nFv+iFv1];
        }
        numUsed = 1; //The first vertex in the list is left untouched
        usedVertices[0] = G->F[iF*G->nFv];
        for(iFv1=0;iFv1<G->nFv-1;iFv1++)
        {
            edgeFound = 0;
            v1 = usedVertices[iFv1];
            //printf("First edge %d\n",v1);
            for(iFv2=0;iFv2<G->nFv;iFv2++)
            {
                v2 = faceStore[iFv2];
                if(intcontains(usedVertices,v2,numUsed))
                continue;
                for(iE=0;iE<G->nE;iE++)
                if(intcontains(G->E+iE*G->nEv,v1,2) && intcontains(G->E+iE*G->nEv,v2,2))
                {
                    edgeFound = 1;
                    G->F[iF*G->nFv+iFv1+1] = v2;
                    usedVertices[numUsed] = v2;
                    numUsed++;
                    //printf("Next edge %d\n",v2);
                    break;   
                }
                if(edgeFound)
                break;
            }
            if(!edgeFound)
            printf("No vertex was found to connect to vertex %d \n",iFv1);
        }
        if(numUsed != G->nFv)
        printf("Not all vertices found. %d expected %d found\n",G->nFv,numUsed);
    }
}

void scaleV(geo *G, double scale)
{
    //Scale the geometry by a constant multiplier
     int iV,iD;
    for(iV=0;iV<G->nV;iV++)
        for(iD=0;iD<G->dimread;iD++)
            *(G->Vread+iV*G->dimread+iD) *= scale;
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

void printGeoCount(geo *G)
{
    printf("dim %d nV %d nE %d nEv %d nF %d nFv %d\n",G->dimread,G->nV,G->nE,G->nEv,G->nF,G->nFv);
}

void schlegel(geo *G, int finaldim, double distance)
{
    //Projects the vertices from dimread to dimfinal using a schlegel projection method
    //Each projection from dimension d to d-1 is achieved by placing a point P = (0,0,...distance)
    //Each point is then projected along a line from P to the hyperplane where (x,y,...,0)
    //At each step a new set of vertices is calculated from the previous set and then the buffers are swapped.
    
    double *VbufferPrevious = malloc(G->dimread*G->nV*sizeof(double)),*VbufferNext = malloc(G->dimread*G->nV*sizeof(double));
    double *bufferswap;
    int currentdim;
    int iV,iD;
    
    for(iV=0;iV<G->nV;iV++)
    for(iD=0;iD<G->dimread;iD++)
    VbufferPrevious[iV*G->dimread+iD] = G->Vread[iV*G->dimread+iD];
    
    for(currentdim=G->dimread;currentdim>finaldim;currentdim--)
        {
        for(iV =0;iV<G->nV;iV++)
            {
            pointprojectdown(VbufferPrevious+iV*currentdim,VbufferNext+iV*(currentdim-1),currentdim,distance);
            }
        bufferswap=VbufferPrevious;
        VbufferPrevious=VbufferNext;
        VbufferNext=bufferswap;
        }
    if(G->V == NULL)
    G->V = malloc(G->dim*G->nV*sizeof(double));
    
    //For most cases this simply copies over the results in the final buffer.
    //Since a 3D geometry is always used for the plotting, and since a 2dimensional projection does not have a z component
    //This will place a 0 in the z component of projections to 2D
    for(iV =0;iV<G->nV;iV++)
        for(iD=0;iD<G->dim;iD++)
            if(iD<finaldim)
                G->V[iV*G->dim+iD] = VbufferPrevious[iV*finaldim+iD];
            else
                G->V[iV*G->dim+iD] = 0;
    free(VbufferPrevious); free(VbufferNext);
}

int tokcount(char *line, char *del)
{
    //Counts the number of tokens that a string will be broken into
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

void readAll(geo *G,char *filename)
{
    //Reads the geometry data generated by the interpreter. Currently this only works with schlafli_interpreter.py 
    int iV,iE,iEv,iF,iFv,iD;
    FILE *infile = fopen(filename,"r");
    char *line = malloc(1024*sizeof(char));
    fgets(line,1024,infile);
    //printf("Got line %s\n",line);
    G->nV = atof(line);
    //printf("%d vertices\n",nV);
    G->Vread = malloc(G->nV*G->dimread*sizeof(double));
    char *tok;
    for(iV=0;iV<G->nV;iV++)
            {
        fgets(line,1024,infile);
        //printf("Got line %s\n",line);
        tok = strtok(line," ");
        for(iD=0;iD<G->dimread;iD++)
            {
            //printf("Tok is %s\n",tok);
            G->Vread[iV*G->dimread + iD] = atof(tok);
            tok = strtok(NULL," ");
            }
        }
    fgets(line,1024,infile);//Should be a blank line
    fgets(line,1024,infile);
    //printf("Got line %s\n",line);
    G->nE = atoi(line);
    G->nEv = 2;
    //printf("%d edges\n",nE);
    G->E = malloc(G->nE*2*sizeof(int));
    for(iE=0;iE<G->nE;iE++)
        {
        fgets(line,1024,infile);
        //printf("Got line %s\n",line);
        tok = strtok(line," ");
        for(iEv=0;iEv<G->nEv;iEv++)
            {
            //printf("Tok is %s\n",tok);
            G->E[iE*G->nEv + iEv] = atoi(tok);
            tok = strtok(NULL," ");
            }
        }
    
    //For two dimensional structures faces are not printed. Instead, the program will create a face from the listed vertices and edges    
    //This should be corrected to create face from edges not just list vertices in order
    if(G->dimread == 2)
        {
        G->F = malloc(G->nV*sizeof(int));
        G->nFv = G->nV;
        G->nF = 1;
        for(iV=0;iV<G->nV;iV++)
        G->F[iV]=iV;
        return;
        }   
    
    fgets(line,1024,infile);//Should be a blank line
    fgets(line,1024,infile);
    //printf("Got line %s\n",line);
    G->nF = atoi(line);
    G->nFv = atoi(strstr(line,"(")+1);
    //printf("%d faces each with %d vertices\n",nF,nFv);
    G->F = malloc(G->nF*G->nFv*sizeof(int));
    for(iF=0;iF<G->nF;iF++)
        {
        fgets(line,1024,infile);
        //printf("Got line %s\n",line);
        tok = strtok(line," ");
        for(iFv=0;iFv<G->nFv;iFv++)
            {
            // printf("Tok is %s\n",tok);
            G->F[iF*G->nFv + iFv] = atoi(tok);
            tok = strtok(NULL," ");
            }
        }
}

void
init(void)
{
    //Initialize the display window. Set up callbacks to visibility and display functions
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_ALPHA);
    glutCreateWindow("Polytope Drawing");
    glutDisplayFunc(display);  
    glutVisibilityFunc(visible);
  
    //Set up lighting. Lighting model uses ambient and diffuse lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightModelfv(GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glDisable(GL_LIGHT_MODEL_TWO_SIDE);
  
    //Turn on blending options
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LINE_SMOOTH);
    glLineWidth(linewidth);
    glPointSize(pointsize);

    // Setup the view.
    glMatrixMode(GL_PROJECTION);
    gluPerspective( /* field of view in degree */ 50.0,
    /* aspect ratio */ 1.0,
    /* Z near */ 3.0, /* Z far */ -3.0);
    glMatrixMode(GL_MODELVIEW);
    gluLookAt(3.0, 3.0, 3.0,  /* eye is at (3,3,3) */
    0.0, 0.0, 0.0,      /* center is at (0,0,0) */
    0.0, 1.0, 0.);      /* up is in positive Y direction */
}

int
main(int argc, char **argv)
{
    glutInit(&argc, argv);
    mainGeo = malloc(sizeof(geo));
    mainGeo->dim = mainGeo->dim = 3;
    mainGeo->symbol = defaultsymbol;
    mainGeo->V = NULL;
  
    //If no command line options are presented print the help menu
    if(argc == 1)
    printf("%s\n",helpstring);
  
    //Read command line options
    int iarg;
    for(iarg=1;iarg<argc;iarg++)
        {
        if(!strcmp(argv[iarg],"-scale"))
            scale = atof(argv[iarg+1]);
        if(!strcmp(argv[iarg],"-symbol"))
            mainGeo->symbol = argv[iarg+1];
        if(!strcmp(argv[iarg],"-schlegeldistance"))
            schlegeldistance = atof(argv[iarg+1]);
        if(!strcmp(argv[iarg],"-linewidth"))
            linewidth = atof(argv[iarg+1]);
        if(!strcmp(argv[iarg],"-pointsize"))
            pointsize = atof(argv[iarg+1]);
        if(!strcmp(argv[iarg],"-vertexlimit"))
            vertexlimit = atoi(argv[iarg+1]);
        if(!strcmp(argv[iarg],"schlegel3d"))
            projector = SCHLEGEL3D;
        if(!strcmp(argv[iarg],"schlegel2d"))
            projector = SCHLEGEL2D;
        if(!strcmp(argv[iarg],"schlegelrotate"))
            schlegelrotate = 1;
        if(!strcmp(argv[iarg],"points"))
            points = 1;
        if(!strcmp(argv[iarg],"wireframe"))
            wireframe = 1;
        if(!strcmp(argv[iarg],"solid"))
            solid = 1;
        }
      
    //Call the interpreter to generate the geometry. Then load the geometry into memory    
    char command[1000];
    //sprintf(command,"echo \"%s \" | %s > geo",mainGeo->symbol,interpreter);
    //printf("%s\n",command);
    if(vertexlimit)
    sprintf(command,"%s %s -vlimit=%d > geo",interpreter,mainGeo->symbol,vertexlimit);
    else
    sprintf(command,"%s %s > geo",interpreter,mainGeo->symbol);
    printf("%s\n",command);
    system(command);
    mainGeo->dimread = tokcount(mainGeo->symbol," ")+1;
    readAll(mainGeo,"geo");
    printGeoCount(mainGeo);
    center(mainGeo);
    reorderFaceVertices(mainGeo);

    //Perform corrections based on mismatching commandline options
    if(!points && !wireframe && !solid)
    wireframe = 1;
    if(schlegeldistance == 0)
    schlegeldistance = 2*scale;
    if(scale != 1)
    scaleV(mainGeo,scale);
    if(mainGeo->dimread == 2)
    projector = SCHLEGEL2D;
    if(schlegelrotate)
    autorotate = 0;
  
    //Perform projection to obtain final geometry. 
    //This is for static projection only where original geometry and mapping between original geometry and projected geometry is unchanged.
    switch(projector)
        {
        case SCHLEGEL3D:
            schlegel(mainGeo,3,schlegeldistance);
            break;
        case SCHLEGEL2D:
            schlegel(mainGeo,2,schlegeldistance);
            break;
        }
  
    //Initialize GL options and begin glloop
    init();
    glutMainLoop();
    return 0;    
}
