
/* Copyright (c) Anthony Ruth, 2018.
 * polytope.c
 * 
 * Available from: https://github.com/aruth2/schlafli
 * 
 * Compile with 
 * gcc -w -o "polytope" "polytope.c" -lGL -lGLU -lglut -lm
 * */

//#include "geometry.h"
#include "geodraw.h"

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
"solid - draw faces. Misbehaves when drawing star polytopes because GL_POLYGON does not support concave shapes\n"
"autorotatex - automatically rotates, mostly around x-axis\n"
"autorotaten - automatically rotates, mostly around r_n - r_{n-1} direction\n"
"\n\n";

geo *mainGeo;

//Options controlled by commandline
int points = 0;
int wireframe = 0;
int solid = 0;
int projector = SCHLEGEL3D;
int schlegelrotate = 0;
//int autorotate = 1;
int autorotate = 0;
double schlegeldistance = 0;
double scale=1;
double linewidth = 2;
double pointsize = 7;
int vertexlimit = 1000;

enum OPTIONS {POINTS, WIREFRAME, SOLID, AUTOROTATE1, AUTOROTATE2};
const char *optionsText[] = {"points", "wireframe", "solid", "autorotate (x-first)", "autorotate (dim_n first)"};
#define numoptions 5

enum ROTATORS {X_POST,Y_POST,Z_POST,RN_1R_N,SCALE,PROJ_DISTANCE,NONE};
const char *rotatorText[] = {"YZ (post-projection)","XZ (post-projection)","XY (post-projection)","high dimension rotation","scale","projector distance","NONE"};
#define numrotators 7
float angles[numrotators];
int xprev,yprev;
int xrot=1,yrot=0;

const char *defaultsymbols[] = {"3","5/2","3 3","4 3","3 4","5 3","3 5","5/2 5","4 3 3","5 3 3","3 3 5","5/2 3 5","4 3 3 3","4 3 3 4"};
const char *defaultsymbolnames[] = {"Triangle","Star","Tetrahedron","Cube","Octahedron","Dodecahedron","Ventihedron","Small Stellated Dodecahedron","Tesseract","120 cell", "600 cell","Great Stellated 120-cell","Double tesseract","Tesseract Lattice"};
#define numdefaultsymbols 14

//Internal default variables
float omega[4] = {0.13, 0.04, 0.011, 0.003};
char *mainsymbol;


//Options used for color and lighting
GLfloat light_diffuse[] = {0.05, 0.05, 0.05, 0.0};  /* White diffuse light. */
GLfloat light_ambient[] = {0.25, 0.25, 0.25, 1.0};  /* ambient light. */
GLfloat light_position[] = {0.0, 5.0, 0.0, 1.0};  /* Infinite light location. */
int windowWidth = 800, windowHeight = 800;

GLfloat text_color[] = {1.0,1.0,1.0,1.0};

void *font = GLUT_BITMAP_TIMES_ROMAN_24;
void
textstring(int x, int y, char *string)
{
  int len, i;

  glRasterPos2f(x, y);
  len = (int) strlen(string);
  for (i = 0; i < len; i++) {
    glutBitmapCharacter(font, string[i]);
  }
}

void applyRotatorsAndProjection()
{
    //printf("Applying rotators and projection\n");
    double newdistance=schlegeldistance,newscale;
    int rot;
    
    for(rot = 0;rot<numrotators;rot++)
    {
        switch (rot)
        {
            case X_POST:
            break;
            case Y_POST:
            break;
            case Z_POST:
            break;
            case RN_1R_N:
            dimensionalrotation(mainGeo->Vread,mainGeo->Vread,mainGeo->nV,mainGeo->dimread,mainGeo->dimread-2,mainGeo->dimread-1,angles[RN_1R_N]);
            angles[RN_1R_N] = 0;
            break;

            case SCALE:
            if(angles[SCALE] < 180)
            newscale = (1 + angles[SCALE]/360.0);
            else
            if(angles[SCALE] > 180)
            newscale = (1 - (360-angles[SCALE])/360.0);
            scaleV(mainGeo,newscale);
            angles[SCALE] = 0;
            break;
            case PROJ_DISTANCE:
            newdistance = (90-angles[PROJ_DISTANCE])/90 * schlegeldistance;
            break;
            default:
            break;
        }
    }
    project(mainGeo,projector,newdistance);
    //printf("New scale is %g\n",newscale);
    
    
    for(rot = 0;rot<numrotators;rot++)
    {
        switch (rot)
        {
            case X_POST:
            dimensionalrotation(mainGeo->V,mainGeo->V,mainGeo->nV,mainGeo->dim,1,2,angles[X_POST]);
            break;
            case Y_POST:
            dimensionalrotation(mainGeo->V,mainGeo->V,mainGeo->nV,mainGeo->dim,2,0,angles[Y_POST]);
            break;
            case Z_POST:
            dimensionalrotation(mainGeo->V,mainGeo->V,mainGeo->nV,mainGeo->dim,0,1,angles[Z_POST]);
            break;
            case RN_1R_N:
            break;
            case SCALE:
            break;
            case PROJ_DISTANCE:
            break;
            default:
            break;
        }
    }
}

void incrementRotatorAngle(int chosenrotator,int change)
{
    float anglechange = 180 * (float)change/windowWidth;
    angles[chosenrotator] = fmod(angles[chosenrotator] + anglechange+360,360);
    //printf("Change is %d New rotator %s angle is %g\n",change,rotatorText[chosenrotator],angles[chosenrotator]);
}

void click(int button, int state, int x, int y)
{
    if(state == GLUT_DOWN)
    {
        xprev = x;
        yprev = y;
    }
}

void drag(int x, int y)
{
    //printf("Mouse at %d x %d\n",x,y);
    int dx = x-xprev;
    int dy = y-yprev;
    xprev = x;
    yprev = y;
    incrementRotatorAngle(xrot,dx);
    incrementRotatorAngle(yrot,dy);
}

void
display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
    //Call each of the used plotting functions
    if(points)
        geoPoints(mainGeo);
    if(wireframe)
        geoWireframe(mainGeo);
    if(solid)
        geoSolid(mainGeo);

 
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS , text_color);
    textstring(0,-2.5,(char *)rotatorText[xrot]);
    textstring(0,2.5,(char *)rotatorText[yrot]);
    
    glutSwapBuffers();
}

void 
idle(void)
{
    if(autorotate == 1)
    {
        angles[X_POST] += omega[0];
        angles[Y_POST] += omega[1];
        angles[Z_POST] += omega[2];
        angles[RN_1R_N] += omega[3];
    }
    if(autorotate == 2)
    {
        angles[X_POST] += omega[3];
        angles[Y_POST] += omega[2];
        angles[Z_POST] += omega[1];
        angles[RN_1R_N] += omega[0];
    }
    applyRotatorsAndProjection();
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

void changeXRotator(int rot)
{
    xrot = rot;
}

void changeYRotator(int rot)
{
    yrot = rot;
}

void toggleOption(int opt)
{
    enum OPTIONS {POINTS, WIREFRAME, SOLID, AUTOROTATE1, AUTOROTATE2};
const char *optionsText[] = {"points", "wireframe", "solid", "autorotate (x-first)", "autorotate (dim_n first)"};
    switch (opt)
    {
        case POINTS:
        points = !points;
        break;
        case WIREFRAME:
        wireframe = !wireframe;
        break;
        case SOLID:
        solid = !solid;
        break;
        case AUTOROTATE1:
        if(autorotate == 1)
        autorotate = 0;
        else
        autorotate = 1;
        break;
        case AUTOROTATE2:
        if(autorotate == 2)
        autorotate = 0;
        else
        autorotate = 2;
        break;
    }
}

void setSymbol(int sym)
{
    mainsymbol = (char *)defaultsymbols[sym];
    mainGeo = geoFromSchlafliSymbol(mainsymbol,vertexlimit);
    if(scale != 1)
        scaleV(mainGeo,scale);
    project(mainGeo,projector,schlegeldistance);    
}

void setupMenu()
{
    
    int rot;    
    int rotx_submenu = glutCreateMenu(changeXRotator);
    for(rot=0;rot<numrotators;rot++)
    glutAddMenuEntry(rotatorText[rot], rot);
    
    int roty_submenu = glutCreateMenu(changeYRotator);
    for(rot=0;rot<numrotators;rot++)
    glutAddMenuEntry(rotatorText[rot], rot);
    
    int opt;
    int opt_submenu = glutCreateMenu(toggleOption);
    for(opt=0;opt<numoptions;opt++)
    glutAddMenuEntry(optionsText[opt],opt);

    int sym;
    int sym_submenu = glutCreateMenu(setSymbol);
    for(sym=0;sym<numdefaultsymbols;sym++)
    glutAddMenuEntry(defaultsymbolnames[sym],sym);
    
    int menu = glutCreateMenu(changeXRotator);

    glutAddSubMenu("XRotator", rotx_submenu);
    glutAddSubMenu("YRotator", roty_submenu);
    glutAddSubMenu("Options", opt_submenu);
    glutAddSubMenu("Symbols", sym_submenu);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void
reshape(int w, int h)
{
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, w, h, 0);
  glMatrixMode(GL_MODELVIEW);
}

void
init(void)
{
    //Initialize the display window. Set up callbacks to visibility and display functions
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_ALPHA);
    glutCreateWindow("Polytope Drawing");
    glutReshapeWindow(windowWidth, windowHeight);
    glutDisplayFunc(display);  
    glutVisibilityFunc(visible);
    glutMouseFunc(click);
    glutMotionFunc(drag);
    //glutReshapeFunc(reshape);
    //Set up lighting. Lighting model uses ambient and diffuse lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightModelfv(GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glDisable(GL_LIGHT_MODEL_TWO_SIDE);
  
    //Turn on blending options
    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendFunc(GL_ONE, GL_ONE);
//    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LINE_SMOOTH);
    glLineWidth(linewidth);
    glPointSize(pointsize);

    // Setup the view.
    glMatrixMode(GL_PROJECTION);
    gluPerspective( /* field of view in degree */ 50.0,
    /* aspect ratio */ 1.0,
    /* Z near */ 3.0, /* Z far */ -3.0);
    glMatrixMode(GL_MODELVIEW);
    gluLookAt(0.0, 0.0, 5.0,  /* eye is at (3,3,3) */
    0.0, 0.0, 0.0,      /* center is at (0,0,0) */
    0.0, 1.0, 0.);      /* up is in positive Y direction */
    angles[X_POST] = 15;
    angles[Y_POST] = 15;
    
    setupMenu();
    
}

void readCommandLineArguments(int argc, char **argv)
{
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
            mainsymbol = argv[iarg+1];
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
        if(!strcmp(argv[iarg],"autorotatex"))
            autorotate = 1;
        if(!strcmp(argv[iarg],"autorotaten"))
            autorotate = 2;    
        }
    //Perform corrections based on mismatching commandline options
    if(!points && !wireframe && !solid)
    wireframe = 1;
    if(schlegeldistance == 0)
    schlegeldistance = 2*scale;
    if(schlegelrotate)
    autorotate = 0;
}

int
main(int argc, char **argv)
{
    glutInit(&argc, argv);
    mainsymbol = (char *)defaultsymbols[5];
    readCommandLineArguments(argc, argv);
      
    mainGeo = geoFromSchlafliSymbol(mainsymbol,vertexlimit);
    if(scale != 1)
        scaleV(mainGeo,scale);
    if(mainGeo->dimread == 2)
        projector = SCHLEGEL2D;
    project(mainGeo,projector,schlegeldistance);

    //Initialize GL options and begin glloop
    init();
    glutMainLoop();
    return 0;    
}
