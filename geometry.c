#include "geometry.h"

/*
 * Functions for processing geo objects.
 * Each geo object is a collection of vertices. The vertices are connected by edges,faces,hyperfaces,etc
 * Some functions alter vertices for instance project vertices into a lower-dimension
 * Other function operate on edges,etc changing the connectivity of the vertices.
 * */

void freeGeo(geo *G)
{
    if(G->V != NULL)
    free(G->V);
    free(G->Vread); free(G->E); free(G->F); free(G);
}


void faceNorm(float *V, int *F, float *norm)
{
	//Computes a normal to a face F
	//The cross product of the vectors from vertex 0 to 1 and 1 to 2 are used
	//The magnitude of the normal vector is not normalized
	
	float *V0,*V1,*V2;
	V0 = V + 3*F[0]; V1 = V+ 3*F[1]; V2 = V + 3*F[2];

	float P[3],Q[3];
	P[0] = V1[0]-V0[0]; P[1] = V1[1]-V0[1]; P[2] = V1[2]-V0[2];
	Q[0] = V2[0]-V1[0]; Q[1] = V2[1]-V1[1]; Q[2] = V2[2]-V1[2];
	 
	norm[0] = P[1]*Q[2]-Q[1]*P[2];
	norm[1] = P[2]*Q[0]-P[0]*Q[2];
	norm[2] = P[0]*Q[1]-P[1]*Q[0];
	
	//printf("Norm is %f %f %f\n",norm[0],norm[1],norm[2]);
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

void project(geo *G, int projector,double schlegeldistance)
{
    //Perform projection to obtain final geometry. 
    //This is for static projection only where original geometry and mapping between original geometry and projected geometry is unchanged.
    switch(projector)
        {
        case SCHLEGEL3D:
            schlegel(G,3,schlegeldistance);
            break;
        case SCHLEGEL2D:
            schlegel(G,2,schlegeldistance);
            break;
        }
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


geo * geoFromSchlafliSymbol(char *symbol,int vertexlimit)
{
    char *interpreter = "python3 ./schlafli_interpreter.py";
    geo *G = malloc(sizeof(geo));
    G->V = NULL;
    G->dim = 3;
    //Call the interpreter to generate the geometry. Then load the geometry into memory    
    char command[1000];
    //sprintf(command,"echo \"%s \" | %s > geo",mainGeo->symbol,interpreter);
    //printf("%s\n",command);
    if(vertexlimit)
    sprintf(command,"%s %s -vlimit=%d > geo",interpreter,symbol,vertexlimit);
    else
    sprintf(command,"%s %s > geo",interpreter,symbol);
    printf("%s\n",command);
    system(command);
    G->dimread = tokcount(symbol," ")+1;
    readAll(G,"geo");
    printGeoCount(G);
    center(G);
    reorderFaceVertices(G);
    return G;
}

void dimensionalrotation(float *inputV,float *outputV,int nV,int dim,int dim1,int dim2,float angle)
{
    //printf("Applying rotation of angle %f around dimensions %d and %d\n",angle,dim1,dim2);
    float rotmatrix[dim*dim];
    int row,col;
    for(row=0;row<dim;row++)
    for(col=0;col<dim;col++)
    {
        if(row == col)
        rotmatrix[row*dim+col] = 1;
        else
        rotmatrix[row*dim+col] = 0;
    }
    angle = angle * M_PI/180;
    rotmatrix[dim1*dim+dim1] = cos(angle);
    rotmatrix[dim2*dim+dim2] = cos(angle);
    rotmatrix[dim2*dim+dim1] = sin(angle);
    rotmatrix[dim1*dim+dim2] = -sin(angle);
    //printfloatmatrix(rotmatrix,dim,dim,1,1);
    int iV,offset;
    for(iV=0;iV<nV;iV++)
    {
    offset = dim*iV;
    matrixmultiplication(outputV+offset, rotmatrix, inputV+offset, dim, dim, 1);
    }
}


