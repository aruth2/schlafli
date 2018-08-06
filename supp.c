/* Contains 1-off helper functions used by other functions in this package 
 * 
 * */
#include "supp.h"

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
void printfloatmatrix(float *matrix, int numberOfRows, int numberOfColumns, int stride, int rowmajor)
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
int tokcount(char *line, char *del)
{
    //Counts the number of tokens that a string will be broken into
    int i=0;
    char *tok;
    char cpy[strlen(line)+1];
    strcpy(cpy,line); 
    
    tok = strtok(cpy,del);
    while( tok != NULL)
    {
        i++;
        tok = strtok(NULL,del);
    }
    return i;
}

void matrixmultiplication(float *output, float *leftmatrix, float *rightmatrix, int m, int n, int o)
{
	/**********************************************/
	/* multiplies a mxn matrix by a nxo matrix    */
	/* it is safe for output and rightmatrix to be*/
	/* the same or output and leftmatrix to be the*/
	/* same.
	/*
	/**********************************************/
	
	int index1,index2,index3;
	float *values = malloc(m*o*sizeof(float));
	
	for(index1=0;index1<m;index1++)
	for(index3=0;index3<o;index3++)
	{
	for(index2=0,*(values+index1*o+index3)=0;index2<n;index2++)
	*(values+index1*o+index3) += *(leftmatrix + index1*n+index2) * *(rightmatrix+index2*o+index3);
	}
	for(index1=0;index1<m;index1++)
	for(index3=0;index3<o;index3++)
	*(output+index1*o+index3) = *(values+index1*o+index3);
	free(values);
	
}
