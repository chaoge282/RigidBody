#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#include <cmath>
#include <vector>
#include"Vector.h"
#include "Camera.h"

#include "rigidBody.h"
#include "stateDot.h"


bool  resetSign = false;
const int timeDelay = 20;
const float thr = 0.25;
const int number = 8;
const float mass = 1.0;
const float size = 5.0;
const float halfSize = 2.5;
const float wallY = 0;
const double Cr = 0.5;
const double Ct = 0.9;
const int WIDTH = 1000;
const int HEIGHT = 800;

Matrix3x3 Io(mass*1.0/12*(size*size*2),0,0, 0,mass*1.0/12*(size*size*2),0, 0,0,mass*1.0/12*(size*size*2));

Rigidstate  rigidStateNew;
Rigidstate  rigidState;
StateDot    rigidStateDot;

Vector3d center(0,0,0);
Vector3d bodyForce(0,0,0);
Vector3d vertexForce[8]={Vector3d(0,0,0),Vector3d(0,100,0),Vector3d(0,0,0),Vector3d(0,0,0),
                        Vector3d(0,0,0),Vector3d(0,0,0),Vector3d(0,0,0),Vector3d(0,-100,0)};
Vector3d Vpn(0,1,0);
Vector3d vertexPos[number];
Vector3d vertexPosNew[number];

Camera *camera;

GLuint texGround;
GLuint texCube;

GLuint loadTexture( const char * filename, int width, int height )
{
    GLuint texture;
    unsigned char * data;
    FILE * file;

    //The following code will read in our RAW file
    file = fopen( filename, "rb" );
    if ( file == NULL ) return 0;
    data = (unsigned char *)malloc( width * height * 3 );
    fread( data, width * height * 3, 1, file );
    fclose( file );

    glGenTextures( 1, &texture ); //generate the texture with the loaded data
    glBindTexture( GL_TEXTURE_2D, texture ); //bind the texture to it’s array
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_MODULATE ); //set texture environment parameters

    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_REPEAT );

    //Generate the texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,GL_RGB, GL_UNSIGNED_BYTE, data);
    free( data ); //free the texture
    return texture; //return whether it was successfull
}

void freeTexture( GLuint texture )
{
  glDeleteTextures( 1, &texture );
}




void drawCube(){
    glBindTexture( GL_TEXTURE_2D, texCube ); //bind our texture to our shape
    glBegin(GL_QUADS);
  // front
  glTexCoord2f(0.0f, 0.0f); glVertex3f(-halfSize, -halfSize,  halfSize);
  glTexCoord2f(1.0f, 0.0f); glVertex3f( halfSize, -halfSize,  halfSize);
  glTexCoord2f(1.0f, 1.0f); glVertex3f( halfSize,  halfSize,  halfSize);
  glTexCoord2f(0.0f, 1.0f); glVertex3f(-halfSize,  halfSize,  halfSize);
  // back
  glTexCoord2f(1.0f, 0.0f); glVertex3f(-halfSize, -halfSize, -halfSize);
  glTexCoord2f(1.0f, 1.0f); glVertex3f(-halfSize,  halfSize, -halfSize);
  glTexCoord2f(0.0f, 1.0f); glVertex3f( halfSize,  halfSize, -halfSize);
  glTexCoord2f(0.0f, 0.0f); glVertex3f( halfSize, -halfSize, -halfSize);
  // top
  glTexCoord2f(0.0f, 1.0f); glVertex3f(-halfSize,  halfSize, -halfSize);
  glTexCoord2f(0.0f, 0.0f); glVertex3f(-halfSize,  halfSize,  halfSize);
  glTexCoord2f(1.0f, 0.0f); glVertex3f( halfSize,  halfSize,  halfSize);
  glTexCoord2f(1.0f, 1.0f); glVertex3f( halfSize,  halfSize, -halfSize);
  // bottom
  glTexCoord2f(1.0f, 1.0f); glVertex3f(-halfSize, -halfSize, -halfSize);
  glTexCoord2f(0.0f, 1.0f); glVertex3f( halfSize, -halfSize, -halfSize);
  glTexCoord2f(0.0f, 0.0f); glVertex3f( halfSize, -halfSize,  halfSize);
  glTexCoord2f(1.0f, 0.0f); glVertex3f(-halfSize, -halfSize,  halfSize);
  // right
  glTexCoord2f(1.0f, 0.0f); glVertex3f( halfSize, -halfSize, -halfSize);
  glTexCoord2f(1.0f, 1.0f); glVertex3f( halfSize,  halfSize, -halfSize);
  glTexCoord2f(0.0f, 1.0f); glVertex3f( halfSize,  halfSize,  halfSize);
  glTexCoord2f(0.0f, 0.0f); glVertex3f( halfSize, -halfSize,  halfSize);
  // left
  glTexCoord2f(0.0f, 0.0f); glVertex3f(-halfSize, -halfSize, -halfSize);
  glTexCoord2f(1.0f, 0.0f); glVertex3f(-halfSize, -halfSize,  halfSize);
  glTexCoord2f(1.0f, 1.0f); glVertex3f(-halfSize,  halfSize,  halfSize);
  glTexCoord2f(0.0f, 1.0f); glVertex3f(-halfSize,  halfSize, -halfSize);
 glEnd();
}

void rigidBodyInit()
{

    Vector3d center(0,20,0);
    vertexPos[0] = Vector3d(-halfSize,-halfSize,halfSize)+center;
    vertexPos[1] = Vector3d(halfSize,-halfSize,halfSize)+center;
    vertexPos[2] = Vector3d(halfSize,-halfSize,-halfSize)+center;
    vertexPos[3] = Vector3d(-halfSize,-halfSize,-halfSize)+center;
    vertexPos[4] = Vector3d(-halfSize,halfSize,halfSize)+center;
    vertexPos[5] = Vector3d(halfSize,halfSize,halfSize)+center;
    vertexPos[6] = Vector3d(halfSize,halfSize,-halfSize)+center;
    vertexPos[7] = Vector3d(-halfSize,halfSize,-halfSize)+center;

    for (int i=0; i<number; i++) {
        vertexPosNew[i] = vertexPos[i];
    }

    Matrix3x3 m(1,0,0, 0,1,0, 0,0,1);
    rigidState.xposition = center;
    rigidState.quater = Quaternion(m);
    rigidState.pfmom = Vector3d(0,0,0);
    rigidState.lamom = Vector3d(0,0,0);

    Vector3d w(0,0,0);
    rigidStateDot.velocity = Vector3d(0,0,0);
    rigidStateDot.quaterA = 0.5*w*(rigidState.quater);
    rigidStateDot.force = Vector3d(0,0,0);
    rigidStateDot.torque = Vector3d(0,0,0);

}

void square () {
    glBindTexture( GL_TEXTURE_2D, texGround ); //bind our texture to our shape
    glBegin (GL_QUADS);
    glTexCoord2d(0.0,0.0); glVertex3d(-2*size,0.0,-2*size); //with our vertices we have to assign a texcoord
    glTexCoord2d(1.0,0.0); glVertex3d( 2*size,0.0,-2*size); //so thatour texture has some points to draw to
    glTexCoord2d(1.0,1.0); glVertex3d( 2*size, 0.0,2*size);
    glTexCoord2d(0.0,1.0); glVertex3d(-2*size,0.0, 2*size);
    glEnd();

}





StateDot F( Rigidstate& rigidState)
{
    StateDot rigidStateDot;

    rigidStateDot.velocity = (1.0/mass)*rigidState.pfmom;
    Matrix3x3 R = rigidState.quater.rotation();
    Matrix3x3 Iinverse = R * (Io.inv()) * (R.transpose());
    Vector3d w = Iinverse * rigidState.lamom;
    rigidStateDot.quaterA = 0.5 * w * rigidState.quater;

    Vector3d totalforce(0,0,0);
    for (int i=0; i<number; i++) {
        totalforce = totalforce + vertexForce[i];
    }
    totalforce = totalforce + bodyForce;
    rigidStateDot.force = totalforce ;


    Vector3d totalTorque(0,0,0);
    Vector3d torque(0,0,0);
    for (int i=0; i<number; i++) {
        torque = (vertexPos[i] - center) % vertexForce[i];
        totalTorque = totalTorque + torque;
    }
    rigidStateDot.torque = totalTorque;

    return rigidStateDot;
}



void statesNumIntRK4(Rigidstate& rigidState,  StateDot& rigidStateDot, Rigidstate& rigidStateNew, double h)
{
   StateDot K1 = F(rigidState);
    Rigidstate tmp1 = rigidState + K1*(h*0.5);

    StateDot K2 = F(tmp1);
    Rigidstate tmp2 = rigidState + K2*(h*0.5);

    StateDot K3 = F(tmp2);
    Rigidstate tmp3 = rigidState + K3*h;

    StateDot K4 = F(tmp3);

    rigidStateDot = (K1 + (K2*2) + (K3*2) + K4)*(1.0/6);
    rigidStateNew = rigidState + rigidStateDot*(h);

    Vector3d centerNew = rigidStateNew.xposition;
    Matrix3x3 R = rigidStateNew.quater.rotation();

    vertexPosNew[0] = R*(Vector3d(-halfSize,-halfSize,halfSize))+centerNew;
    vertexPosNew[1] = R*(Vector3d(halfSize,-halfSize,halfSize))+centerNew;
    vertexPosNew[2] = R*(Vector3d(halfSize,-halfSize,-halfSize))+centerNew;
    vertexPosNew[3] = R*(Vector3d(-halfSize,-halfSize,-halfSize))+centerNew;
    vertexPosNew[4] = R*(Vector3d(-halfSize,halfSize,halfSize))+centerNew;
    vertexPosNew[5] = R*(Vector3d(halfSize,halfSize,halfSize))+centerNew;
    vertexPosNew[6] = R*(Vector3d(halfSize,halfSize,-halfSize))+centerNew;
    vertexPosNew[7] = R*(Vector3d(-halfSize,halfSize,-halfSize))+centerNew;


}


void collisionDetectRK4(Rigidstate& rigidState,  StateDot& rigidStateDot, Rigidstate& rigidStateNew, double h)
{

    for (int i=0; i<number; i++) {
        if ((vertexPosNew[i].y-wallY)*(vertexPos[i].y-wallY)<0) {

            if ((vertexPosNew[i] - vertexPos[i]).norm() > 0.1) {
                while (1)
                {
                    statesNumIntRK4(rigidState, rigidStateDot, rigidStateNew, h*0.5);
                    if (vertexPosNew[i].y <wallY) {
                        break;
                    }
                    else
                    {
                        rigidState = rigidStateNew;
                        center = rigidState.xposition;
                        for (int i=0; i<number; i++) {
                            vertexPos[i] = vertexPosNew[i];
                        }
                    }
                }
                collisionDetectRK4(rigidState, rigidStateDot, rigidStateNew, h*0.5);

            }
            else
            {
                //resting
                if (rigidStateDot.velocity.norm()< thr) {

                    unsigned int counter=0;
                    for (int i=0; i<number; i++) {

                        if ( vertexPosNew[i].y < thr ) {
                            counter++;
                        }
                    }
                    if (counter > 3) {
                        resetSign =true;
                        return;
                    }

                }

                Matrix3x3 R = rigidState.quater.rotation();
                Matrix3x3 Iinverse = R * (Io.inv()) * (R.transpose());
                Vector3d w = Iinverse * rigidState.lamom;
                Vector3d r = vertexPos[i] - center;

                double Vn =  Vpn * (rigidStateDot.velocity + w%r);
                double j = -1*(1+Cr)*Vn / ( 1.0/mass + Vpn* (Iinverse*(r%Vpn)%r) );
                Vector3d J = j * Vpn;


                rigidState.pfmom = rigidState.pfmom + J ;
                rigidState.lamom = rigidState.lamom + r%J ;

                statesNumIntRK4(rigidState, rigidStateDot, rigidStateNew, h);
            }


        }
    }
}


void update()
{
    double hStep = 0.01;

     statesNumIntRK4(rigidState, rigidStateDot, rigidStateNew, hStep);
     collisionDetectRK4(rigidState, rigidStateDot, rigidStateNew, hStep);

    for (int i=0; i<number; i++) {
        vertexForce[i] = Vector3d(0,0,0);
    }

    rigidState = rigidStateNew;
    center = rigidState.xposition;
    for (int i=0; i<number; i++) {
        vertexPos[i] = vertexPosNew[i];
    }

}



void timeProc(int id)
{
    if (id == 1) {
        update();
        glutPostRedisplay();
        if (resetSign == false) {
            glutTimerFunc(timeDelay, timeProc, 1);
        }
    }

}

void init() {
    //LoadParameters(ParamFilename);
    // set up camera
    // parameters are eye point, aim point, up vector
    camera = new Camera(Vector3d(0, 5, 90), Vector3d(0, 0, 0), Vector3d(0, 1, 0));

    // black background for window
    glShadeModel(GL_SMOOTH);
    glDepthRange(0, 1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    glEnable(GL_NORMALIZE);
   // glDepthMask(GL_TRUE);

    texGround = loadTexture("ground.bmp",512,512);
    texCube = loadTexture("mofang.bmp",256,256);
    //Setting lights

    GLfloat light0_position[] = {30.0f, 30.0f, 30.0f, 1.0f}; //spot light
    GLfloat light1_position[] = {-30.0f, -30.0f,-30.0f, 1.0f};
    GLfloat light0_ambient[]  = {1.0f, 1.0f, 1.0f, 0.0f};
    GLfloat light0_diffuse[]  = {1.0f, 1.0f, 1.0f, 0.0f};
    GLfloat light0_specular[] = {1.0f, 1.0f, 1.0f, 0.0f};


    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  light0_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light0_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);


    glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
    glLightfv(GL_LIGHT1, GL_AMBIENT,  light0_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE,  light0_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light0_specular);


    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHTING);

    rigidBodyInit();

}


void myDisplay(void)
{

    glClearColor(0.7, 0.7, 0.7, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw the camera created in perspective
    camera->PerspectiveDisplay(WIDTH, HEIGHT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslatef(0,0,0);
    glPushMatrix();
    square();
    glPopMatrix();


    //Setting ball material
    GLfloat ball_mat_ambient[]  = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat ball_mat_diffuse[]  = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat ball_mat_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat ball_mat_emission[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat ball_mat_shininess  = 10.0f;

    glMaterialfv(GL_FRONT, GL_AMBIENT,   ball_mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   ball_mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  ball_mat_specular);
    glMaterialfv(GL_FRONT, GL_EMISSION,  ball_mat_emission);
    glMaterialf (GL_FRONT, GL_SHININESS, ball_mat_shininess);


    glPushMatrix();
    Matrix3x3 m = rigidState.quater.rotation();
    GLdouble RM[16] = {m[0][0],m[1][0],m[2][0],0,  m[0][1],m[1][1],m[2][1],0,  m[0][2],m[1][2],m[2][2],0, rigidState.xposition.x, rigidState.xposition.y, rigidState.xposition.z,1};
    glMultMatrixd(RM);
    //glTranslated(rigidState.xposition.x, rigidState.xposition.y, rigidState.xposition.z);
    //glutSolidCube(size);
    drawCube();
    glPopMatrix();

    glFlush();
    glutSwapBuffers();
}


void mouseEventHandler(int button, int state, int x, int y) {
    // let the camera handle some specific mouse events (similar to maya)
    camera->HandleMouseEvent(button, state, x, y);
    glutPostRedisplay();
}

void motionEventHandler(int x, int y) {
    // let the camera handle some mouse motions if the camera is to be moved
    camera->HandleMouseMotion(x, y);
    glutPostRedisplay();
}

void handleKey(unsigned char key, int x, int y){
    switch(key){
        case 's':
        case 'S':
            resetSign = false;
            vertexForce[1].y=100;
            vertexForce[7].y=-100;
            bodyForce = Vector3d(0,-20,0);
            glutTimerFunc(timeDelay, timeProc, 1);
            break;

        case 'p':
        case 'P':
            resetSign = true;
            break;

        case 'r':
        case 'R':
            resetSign = false;
            init();
            bodyForce = Vector3d(0,0,0);
            glutTimerFunc(timeDelay, timeProc, 1);
            break;

        case 'q':       // q - quit
        case 'Q':
        case 27:        // esc - quit
            exit(0);

        default:        // not a valid key -- just ignore it
            return;
    }
}




int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Rigid Body");

    init();

    glutDisplayFunc(myDisplay);
    glutMouseFunc(mouseEventHandler);
    glutMotionFunc(motionEventHandler);
    glutKeyboardFunc(handleKey);

    glutMainLoop();
    freeTexture(texGround);
    freeTexture(texCube);
    return 0;
}
