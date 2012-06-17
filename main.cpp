/*
Tugas grafkom (revisi)
kelompok 5 : Yulius angga E.P (10108429)
             Imam ashadi (10108436)
             Ghea ratimanjari (10108449)
             R. Idham Aziz Marlin (10108465)

- tekan bawah untuk mundur
- tekan atas untuk maju
- tekan x untuk memutar objek kebawah (arah x)
- tekan z untuk memutar objek kesamping (arah z)
- tekan y untuk memutar objek kesamping (arah y)
*/

#include <cstdlib>
#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <math.h>

using namespace std;

int w=900, h=600, z=0;
int x2=0, y2=0, sudut=0, z2=0;
int i,j;
#define PI 3.1415926535
float sudut1 = 0;
int n;
static float materialA[] = {0.8,1.0,0.97}; // coklat tua
static float materialB[] = {0.55,0.35,0.11}; // biru muda
static float materialC[] = {1.0,1.0,1.0}; // putih
static float materialD[] = {0.016,0.216,0.518}; //biru tua
static float materialE[] = {0.86,0.62,0.12}; // coklat muda
GLUquadric *texture;
GLuint loadTexture;

void kursi(void);
void meja(void);
void gambarRuang(void);

int LoadBitmap(char *filename)
{
    FILE * file;
    char temp;
    long i;

    // own version of BITMAPINFOHEADER from windows.h for Linux compile
    struct {
      int biWidth;
      int biHeight;
      short int biPlanes;
      unsigned short int biBitCount;
      unsigned char *data;
    } infoheader;

    GLuint num_texture;

    if( (file = fopen(filename, "rb"))==NULL) return (-1); // Open the file for reading

    fseek(file, 18, SEEK_CUR);  /* start reading width & height */
    fread(&infoheader.biWidth, sizeof(int), 1, file);

    fread(&infoheader.biHeight, sizeof(int), 1, file);

    fread(&infoheader.biPlanes, sizeof(short int), 1, file);
    if (infoheader.biPlanes != 1) {
      printf("Planes from %s is not 1: %u\n", filename, infoheader.biPlanes);
      return 0;
    }

    // read the bpp
    fread(&infoheader.biBitCount, sizeof(unsigned short int), 1, file);
    if (infoheader.biBitCount != 24) {
      printf("Bpp from %s is not 24: %d\n", filename, infoheader.biBitCount);
      return 0;
    }

    fseek(file, 24, SEEK_CUR);

    // read the data
    if(infoheader.biWidth<0){
      infoheader.biWidth = -infoheader.biWidth;
    }
    if(infoheader.biHeight<0){
      infoheader.biHeight = -infoheader.biHeight;
    }
    infoheader.data = (unsigned char *) malloc(infoheader.biWidth * infoheader.biHeight * 3);
    if (infoheader.data == NULL) {
      printf("Error allocating memory for color-corrected image data\n");
      return 0;
    }

    if ((i = fread(infoheader.data, infoheader.biWidth * infoheader.biHeight * 3, 1, file)) != 1) {
      printf("Error reading image data from %s.\n", filename);
      return 0;
    }

    for (i=0; i<(infoheader.biWidth * infoheader.biHeight * 3); i+=3) { // reverse all of the colors. (bgr -> rgb)
      temp = infoheader.data[i];
      infoheader.data[i] = infoheader.data[i+2];
      infoheader.data[i+2] = temp;
    }

    fclose(file); // Closes the file stream

    glGenTextures(1, &num_texture);
    glBindTexture(GL_TEXTURE_2D, num_texture); // Bind the ID texture specified by the 2nd parameter

    // The next commands sets the texture parameters
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // If the u,v coordinates overflow the range 0,1 the image is repeated
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // The magnification function ("linear" produces better results)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST); //The minifying function

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // Finally we define the 2d texture
    glTexImage2D(GL_TEXTURE_2D, 0, 3, infoheader.biWidth, infoheader.biHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, infoheader.data);

    // And create 2d mipmaps for the minifying function
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, infoheader.biWidth, infoheader.biHeight, GL_RGB, GL_UNSIGNED_BYTE, infoheader.data);
    
    free(infoheader.data); // Free the memory we used to load the texture

    return (num_texture); // Returns the current texture OpenGL ID
}  

void Init(void)
{
     glClearColor(0,0,0,1);
     glEnable(GL_DEPTH_TEST);
     glDepthFunc(GL_LEQUAL);
     glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
     glMatrixMode(GL_PROJECTION);
     glLoadIdentity();
     gluPerspective(45.0,(GLdouble) w/(GLdouble) h, 1.0,300.0);
     glMatrixMode(GL_MODELVIEW);
     glEnable(GL_LIGHTING); // mengaktifkan fungsi pencahayaan
     glEnable(GL_LIGHT0); // mengaktifkan sumber cahaya
     glShadeModel(GL_SMOOTH); //mengaktifkan shade model
     glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
     glEnable(GL_NORMALIZE);
     glDisable(GL_CULL_FACE);
     texture = gluNewQuadric();
     gluQuadricTexture(texture, GL_TRUE);
}

void resize(int w1,int h1)
{
     if (h1==0)
     {
           h1=1;
     }
     // mereset viewport yang sekarang
     glViewport(0,0,w1,h1);
     glMatrixMode(GL_PROJECTION);
     glLoadIdentity();
     // menghitung aspek rasio window
     gluPerspective(60.0,(float) w1/(float) h1, 0.1,100.0);
     glMatrixMode(GL_MODELVIEW);
     glLoadIdentity();
}

void lukisan()
{
     glPushMatrix();
     glEnable ( GL_TEXTURE_2D );
     glBindTexture ( GL_TEXTURE_2D, loadTexture);
     loadTexture = LoadBitmap("seohyun.bmp");
     glTranslatef(0.0, 0.0, 13.8);
     
     glScalef(5.0, 3.0, 0.4);
     glTranslatef(0.0, 0.5, 1.0);
     glRotatef(180,0,1,0);
      glBegin(GL_POLYGON);
        glTexCoord2f(0.0, 0.0);
        glVertex3f( -1.0,-1.0, 0.0);
        glTexCoord2f(1.0, 0.0);
        glVertex3f(  1.0,-1.0, 0.0);
        glTexCoord2f(1.0, 1.0);
        glVertex3f(  1.0, 1.0, 0.0);
        glTexCoord2f(0.0, 1.0);
        glVertex3f( -1.0, 1.0, 0.0);
        glTexCoord2f(0.0, 0.0);
        glVertex3f( -1.0,-1.0, 0.0);
      glEnd();
//glutSolidCube(1.0);
glDisable ( GL_TEXTURE_2D );
glPopMatrix();

}

void kursi(void)
{
     glClearColor(0,0,0,1);
     glLoadIdentity();
     glTranslatef(0,0,z);
     glRotatef(sudut,x2,y2,z2);

     glPushMatrix();
        glTranslatef(-5.0,-7.0,0);
        glScalef(3.0,0.1,3.0);
        glutSolidCube(1.0);
     glPopMatrix();
    
     glPushMatrix();
        glTranslatef(-6.5,-5.5,0);
        glRotatef(90,0.0,0.0,1.0);
        glScalef(3.0,0.1,3.0);
        glutSolidCube(1.0);
     glPopMatrix();
    
     glPushMatrix();
        glTranslatef(-5.0,-8.0,0);
        glScalef(0.5,2.0,0.5);
        glutSolidCube(1.0);
     glPopMatrix();
}



void meja(void)
{     
    glPushMatrix();
       glTranslatef(0.0,-5.0,0.0);
       glScalef(6.0,0.1,8.0);
       glutSolidCube(1.0);
    glPopMatrix();
    
    glPushMatrix();
       glTranslatef(-2.5,-7.0,3.5);
       glScalef(1.0,4.0,1.0);
       glutSolidCube(1.0);
    glPopMatrix();
    
    glPushMatrix();
       glTranslatef(2.5,-7.0,3.5);
       glScalef(1.0,4.0,1.0);
       glutSolidCube(1.0);
    glPopMatrix();
    
    glPushMatrix();
       glTranslatef(-2.5,-7.0,-3.5);
       glScalef(1.0,4.0,1.0);
       glutSolidCube(1.0);
    glPopMatrix();
    
    glPushMatrix();
       glTranslatef(2.5,-7.0,-3.5);
       glScalef(1.0,4.0,1.0);
       glutSolidCube(1.0);
    glPopMatrix();
}

void jendela(float x,float y)
{
     glMaterialfv(GL_FRONT,GL_DIFFUSE,materialA);
     glMaterialfv(GL_FRONT,GL_AMBIENT,materialA);    
     glBegin(GL_QUADS); // jendela   
     glVertex3f(10.0,0.0,0);
     glVertex3f(10.0,0.0,-x);
     glVertex3f(10.0,y,-x);
     glVertex3f(10.0,y,0);
     glEnd();
          
     glMaterialfv(GL_FRONT,GL_DIFFUSE,materialB);
     glMaterialfv(GL_FRONT,GL_AMBIENT,materialB);
     glBegin(GL_QUADS); // kusen jendela
     glVertex3f(10.0,-0.05,0.05); // bawah
     glVertex3f(10.0,-0.05,-x-0.05);
     glVertex3f(10.0,-0.01,-x-0.05);
     glVertex3f(10.0,-0.01,0.05);

     glVertex3f(10.0,y+0.01,0.05); // atas
     glVertex3f(10.0,y+0.01,-x-0.05);
     glVertex3f(10.0,y+0.05,-x-0.05);
     glVertex3f(10.0,y+0.05,0.05);

     glVertex3f(10.0,-0.01,0+0.05); // kiri
     glVertex3f(10,-0.01,0.01);
     glVertex3f(10,y+0.01,0.01);
     glVertex3f(10,y+0.01,+0.05);

     glVertex3f(10.0,-0.01,-x-0.05); // kanan
     glVertex3f(10,-0.01,-x-0.01);
     glVertex3f(10,y+0.01,-x-0.01);
     glVertex3f(10,y+0.01,-x-0.05);
     glEnd();     
}

void kaki(float h, float tebal)
{
     glPushMatrix();
        glBegin(GL_QUADS);
        glScalef(1.0,4.0,1.0);
        glNormal3f(0,0,1);
        glVertex3f(0,0,0);
        glVertex3f(tebal,0,0);
        glVertex3f(tebal,h,0);
        glVertex3f(0,h,0);

        glNormal3f(1,0,0);
        glVertex3f(tebal,0,0);
        glVertex3f(tebal,0,-tebal);
        glVertex3f(tebal,h,-tebal);
        glVertex3f(tebal,h,0);

        glNormal3f(0,0,-1);
        glVertex3f(tebal,0,-tebal);
        glVertex3f(0,0,-tebal);
        glVertex3f(0,h,-tebal);
        glVertex3f(tebal,h,-tebal);

        glNormal3f(-1,0,0);
        glVertex3f(0,0,-tebal);
        glVertex3f(0,0,0);
        glVertex3f(0,h,0);
        glVertex3f(0,h,-tebal);
	    glEnd();
	 glPopMatrix();
} 

void kotak1(float x, float y, float z)
{
     glPushMatrix();
	    glBegin(GL_QUADS);
        glNormal3f(1,0,0);
        glVertex3f(x,0,0);
        glVertex3f(x,0,-z);
        glVertex3f(x,y,-z);
        glVertex3f(x,y,0);

        glNormal3f(0,1,0);
        glVertex3f(x,y,0);
        glVertex3f(x,y,-z);
        glVertex3f(10,y,-z);
        glVertex3f(0,y,0);

        glNormal3f(-1,0,0);
        glVertex3f(0,y,0);
        glVertex3f(0,y,-z);
        glVertex3f(0,0,-z);
        glVertex3f(0,0,0);

        glNormal3f(0,-1,0);
        glVertex3f(0,0,0);
        glVertex3f(0,0,-z);
        glVertex3f(x,0,-z);
        glVertex3f(x,0,0);
	    glEnd();
	 glPopMatrix();
}

void kotak2(float x, float y, float z)
{
     glPushMatrix();
	    glBegin(GL_QUADS);
        glNormal3f(1,0,0);
	    glVertex3f(x,0,0);
	    glVertex3f(x,0,-z);
        glVertex3f(x,y,-z);
        glVertex3f(x,y,0);

        glNormal3f(0,1,0);
        glVertex3f(x,y,0);
        glVertex3f(x,y,-z);
        glVertex3f(0,y,-z);
        glVertex3f(0,y,0);

        glNormal3f(-1,0,0);
        glVertex3f(0,y,0);
        glVertex3f(0,y,-z);
        glVertex3f(0,0,-z);
        glVertex3f(0,0,0);

        glNormal3f(0,-1,0);
        glVertex3f(0,0,0);
        glVertex3f(0,0,-z);
        glVertex3f(x,0,-z);
        glVertex3f(x,0,0);

        glNormal3f(0,0,1);
        glVertex3f(0,0,0);
        glVertex3f(x,0,0);
        glVertex3f(x,y,0);
        glVertex3f(0,y,0);

        glNormal3f(0,0,-1);
        glVertex3f(0,0,-z);
        glVertex3f(x,0,-z);
        glVertex3f(x,y,-z);
        glVertex3f(0,y,-z);
	    glEnd();
	 glPopMatrix();
}

void kotak3(float x, float y, float z)
{
     glPushMatrix();
     
        glBegin(GL_QUADS);
        glNormal3f(1,0,0);
        glVertex3f(x,0,0);
	    glVertex3f(x,0,-z);
	    glVertex3f(x,y,-z);
	    glVertex3f(x,y,0);

        glNormal3f(-1,0,0);
	    glVertex3f(0,y,0);
	    glVertex3f(0,y,-z);
	    glVertex3f(0,0,-z);
	    glVertex3f(0,0,0);

	    glNormal3f(0,-1,0);
	    glVertex3f(0,0,0);
	    glVertex3f(0,0,-z);
	    glVertex3f(x,0,-z);
	    glVertex3f(x,0,0);
	    glEnd();
	 glPopMatrix();
}

void tempatTidur()
{
     glPushMatrix();
       glTranslatef(-15.0,-8.5,-12.0);
       glScalef(6.0,1.0,9.0);
       glutSolidCube(1.5);
    glPopMatrix();
    
    glMaterialfv(GL_FRONT,GL_DIFFUSE,materialD);
     glMaterialfv(GL_FRONT,GL_AMBIENT,materialD);
    glPushMatrix();
       glTranslatef(-15.0,-8.5,-12.0);
       glScalef(6.0,2.0,10.0);
       glutSolidCube(1.3);
    glPopMatrix();   
}

void lingkaran(int radius, int jumlah_titik, int x_tengah, int y_tengah) 
{
     glBegin(GL_POLYGON);
     for (i=0;i<=360;i++)
     {
        float sudut=i*(2*PI/jumlah_titik);
        float x=x_tengah+radius*cos(sudut);
        float y=y_tengah+radius*sin(sudut);
        glVertex2f(x,y);
     }
     glEnd();
}

void panahJam(){
 glBegin(GL_POLYGON);
 glColor3f(0,0,0);
 glVertex2i(0,200);
 glVertex2i(10,180);
 glVertex2i(5,180);
 glVertex2i(5,0);
 glVertex2i(-5,0);
 glVertex2i(-5,180);
 glVertex2i(-10,180);
 glVertex2i(0,200);
 glEnd();
}

void panahMenit(){
 glBegin(GL_POLYGON);
 glColor3f(1,1,1);
 glVertex2i(0,220);
 glVertex2i(10,200);
 glVertex2i(5,200);
 glVertex2i(5,0);
 glVertex2i(-5,0);
 glVertex2i(-5,200);
 glVertex2i(-10,200);
 glVertex2i(0,220);
 glEnd();
}
void panahDetik(){
 glBegin(GL_POLYGON);
 glColor3f(1,0,0);
 glVertex2i(0,240);
 glVertex2i(10,210);
 glVertex2i(5,210);
 glVertex2i(5,0);
 glVertex2i(-5,0);
 glVertex2i(-5,210);
 glVertex2i(-10,210);
 glVertex2i(0,240);
 glEnd();
}

void angka(float jarak, float x, float y) {
 glPointSize(10);

 glBegin(GL_POINTS);
  glColor3f(0,0,0);
  for(n=0;n<360;n+=30)
   glVertex2f(jarak*(float)sin(n*PI/180.0)+x,jarak*(float)cos(n*PI/180.0)+y);
 glEnd();
}

void menit(float jarak, float x, float y) {
 glPointSize(2);

 glBegin(GL_POINTS);
  glColor3f(0,0,0);
  for(n=0;n<360;n+=6)
   glVertex2f(jarak*(float)sin(n*PI/180.0)+x,jarak*(float)cos(n*PI/180.0)+y);
 glEnd();
}

void jam()
{
    glPushMatrix();
 glColor3f(0.7,0.7,0.7);
 lingkaran(250,100,0,0);

 angka(230.,0.,0.);
 menit(230.,0.,0.);
 glPopMatrix();

 glPushMatrix();
 glRotatef(sudut1,0,0,1);
 panahDetik();
 glPopMatrix();

 glPushMatrix();
 glRotatef(sudut1/60,0,0,1);
 panahMenit();
 glPopMatrix();

 glPushMatrix();
 glRotatef(sudut1/720,0,0,1);
 panahJam();
 glPopMatrix();
 
 glColor3f(0,0,0);
 lingkaran(20,100,0,0); 
}



void gambarRuang(void)
{
     glClearColor(0,0,0,1);
     glLoadIdentity();
     glTranslatef(0,0,z);
     glRotatef(sudut,x2,y2,z2);
     glMaterialfv(GL_FRONT,GL_DIFFUSE,materialC);
     glMaterialfv(GL_FRONT,GL_AMBIENT,materialC); 

     
     
     glPushMatrix();
     glEnable ( GL_TEXTURE_2D );
     glBindTexture ( GL_TEXTURE_2D, loadTexture);
     loadTexture = LoadBitmap("wood1.bmp");
     glBegin(GL_QUADS);                           
     //lantai
     glNormal3f(0.0f, 1.0f, 0.0f);               
     glTexCoord2f(0.0, 0.0);glVertex3f(-20.0f,-10.0f,-20.0f);           // belakang kiri
     glTexCoord2f(1.0, 0.0);glVertex3f(-20.0f,-10.0f, 20.0f);           // depan kiri
     glTexCoord2f(1.0, 1.0);glVertex3f( 10.0f,-10.0f, 20.0f);           // depan kanan
     glTexCoord2f(0.0, 1.0);glVertex3f( 10.0f,-10.0f,-20.0f);           // belakang kanan
     glEnd();
     glDisable ( GL_TEXTURE_2D );
     glPopMatrix();

     glBegin(GL_QUADS); 
     //atap
     glNormal3f(0.0f,-1.0f, 0.0f);               
     glVertex3f(-20.0f, 10.0f, 20.0f);           // depan kiri
     glVertex3f(-20.0f, 10.0f,-20.0f);           // belakang kiri
     glVertex3f( 10.0f, 10.0f,-20.0f);           // belakang kanan
     glVertex3f( 10.0f, 10.0f, 20.0f);           // depan kanan
     //tembok depan
     glColor3f(1.0,0.0,0.0);
     glNormal3f(0.0f, 0.0f, 1.0f);               
     glVertex3f(-20.0f, 10.0f,-20.0f);           // atas kiri
     glVertex3f(-20.0f,-10.0f,-20.0f);           // bawah kiri
     glVertex3f( 10.0f,-10.0f,-20.0f);           // bawah kanan
     glVertex3f( 10.0f, 10.0f,-20.0f);           // atas kanan
     //tembok belakang
     glNormal3f(0.0f, 0.0f,-1.0f);               
     glVertex3f( 10.0f, 10.0f, 20.0f);           // atas kanan
     glVertex3f( 10.0f,-10.0f, 20.0f);           // bawah kanan
     glVertex3f(-20.0f,-10.0f, 20.0f);           // bawah kiri
     glVertex3f(-20.0f, 10.0f, 20.0f);           // atas kiri
     //tembok kiri
     glNormal3f(1.0f, 0.0f, 0.0f);               
     glVertex3f(-20.0f, 10.0f, 20.0f);           // atas depan
     glVertex3f(-20.0f,-10.0f, 20.0f);           // bawah depan
     glVertex3f(-20.0f,-10.0f,-20.0f);           // bawah belakang
     glVertex3f(-20.0f, 10.0f,-20.0f);           // atas belakang
     //tembok kanan
     glNormal3f(-1.0f, 0.0f, 0.0f);              
     glVertex3f( 10.0f, 10.0f,-20.0f);           // atas belakang
     glVertex3f( 10.0f,-10.0f,-20.0f);           // bawah belakang
     glVertex3f( 10.0f,-10.0f, 20.0f);           // bawah depan
     glVertex3f( 10.0f, 10.0f, 20.0f);           // ayas depan
     glEnd();                    
     glutSwapBuffers();
}

void fungsiKeyboard(unsigned char key, int x, int y)
{
     if (key =='s') z+=5;
        else if (key == 'x') 
        {
             x2=1;
             y2=0;
             z2=0;
             sudut+=10;
        }
        else if (key == 'y') 
        {
             y2=1;
             x2=0;
             z2=0;
             sudut+=-10;
        }
        else if (key == 'z') 
        {
             y2=0;
             x2=0;
             z2=1;
             sudut+=-10;
        }
}

void fungsiSpecialKeyboard(int key, int x, int y)
{
     switch(key)
     {
         case GLUT_KEY_UP:
         z+=5;
         break;
         case GLUT_KEY_DOWN:
         z-=5;
         break;
     }
}

void setMaterial()
{
    GLfloat mat_ambient[] = {0.7f,0.7f,0.7f,1.0f};
    GLfloat mat_diffuse[] = {0.6f,0.6f,0.6f,1.0f};
    GLfloat mat_specular[] = {1.0f,1.0f,1.0f,1.0f};
    GLfloat mat_shininess[] = {50.0f};
    glMaterialfv(GL_FRONT,GL_AMBIENT,mat_ambient);
    glMaterialfv(GL_FRONT,GL_DIFFUSE,mat_diffuse);
    glMaterialfv(GL_FRONT,GL_SPECULAR,mat_specular);
    glMaterialfv(GL_FRONT,GL_SHININESS,mat_shininess);
}


void setLighting()
{
    GLfloat lightIntensity[] = {0.7f,0.7f,0.7f,1.0f};
    GLfloat light_position[] = {2.0f,6.0f,3.0f,0.0f};
    glLightfv(GL_LIGHT0,GL_DIFFUSE,lightIntensity);
    glLightfv(GL_LIGHT0,GL_POSITION,light_position);
}

void timer(int value)
{
    glutPostRedisplay();
    glutTimerFunc(50,timer,0);
}

void displayObjek()
{
     setLighting();
     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
     jendela(5.0,5.0);
     kursi();
     meja();
     lukisan();
     tempatTidur();
     gambarRuang();   
     glFlush();
}

int main (int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);
    glutInitWindowPosition(100,100);
    glutInitWindowSize(w,h);
    glutCreateWindow("Objek 3d");
    gluOrtho2D(-w/2,w/2,-h/2,h/2);
    glutDisplayFunc(displayObjek);
    glutReshapeFunc(resize);
    glutKeyboardFunc(fungsiKeyboard);
    glutSpecialFunc(fungsiSpecialKeyboard);
    glutTimerFunc(1,timer,0);
    Init();  
    glutMainLoop();
}










