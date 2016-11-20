#include <iostream> // cout
#include <cmath> // sin cos
#ifdef __APPLE__
# include <OpenGL/gl.h>
# include <GLUT/glut.h>
#else
# include <GL/gl.h>
# include <GL/glut.h>
#endif
using namespace std;

extern int text_w; // tamaño de la textura del piso
extern int text_h; // tamaño de la textura del piso
extern bool animado;
//==========================================
// globales

extern float aang; // orientacion
extern float ax,ay; // posicion en el piso
extern float aacel; // acelerador (eje y del mouse)
extern float aspeed; // velocidad actual
extern float topspeed; // velocidad maxima
extern float rang; // direccion de las ruedas delanteras respecto al auto (eje x del mouse) 
extern float rang2; // giro de las ruedas sobre su eje, cuando el auto avanza 

extern float lpos[]; // posición de la luz
extern int lod; // nivel de detalle para los graficos

//Medida de los componentes
extern float largo_brazo;
extern float largo_antebrazo;

extern float ancho_base; //eje z
extern float largo_base; //eje y
extern float alto_base; //eje x

extern float angulo;
extern float distancia;

extern float ang_brazo;

extern float ang_completo;

extern float ang_brazo_segundo;
static const double PI=4*atan(1.0),R2G=180/M_PI, DOS_PI=2*M_PI, G2R=atan(1.0)/45;

//Pra el antebrazo
extern bool relleno;
extern bool wire;
extern bool blend;
extern bool smooth;

//==========================================

//===Variables agregadas para este proyecto.

float original=0;

int contt=0;

float ca=0;

float co=0;

//==========================================
// algunos objetos
//==========================================
//
//Base del brazo robot.
void drawBase(bool alt_color=false){
	if (alt_color)
		glColor3f(.8,.3,.2);
	else
		
		glPushMatrix();
			
			glTranslatef(1,0,0);
			
			glColor3f(0.75,0.75,0.75);//defino la base en un color gris claro.
			//Esto dibuja multiples triangulos con una invocación de GL_TRIANGLES
			glBegin(GL_TRIANGLES);
			//Primer triangulo
			glNormal3f(.4,-1,0);
			glVertex3f(-1,-1,-1);
			glVertex3f(-1,-1,1);
			glVertex3f(1,0,0);
			glNormal3f(.4,1,0);
			glVertex3f(-1,1,-1);
			glVertex3f(-1,1,1);
			glVertex3f(1,0,0);
			glNormal3f(.4,0,1);
			glVertex3f(-1,1,1);
			glVertex3f(-1,-1,1);
			glVertex3f(1,0,0);
			glNormal3f(.4,0,-1);
			glVertex3f(-1,1,-1);
			glVertex3f(-1,-1,-1);
			glVertex3f(1,0,0);
			glEnd();
			glBegin(GL_QUADS);//Esto dibuja la base de la piramide.
			glNormal3f(-1,0,0);
			glVertex3f(-1,-1,-1);
			glVertex3f(-1,1,-1);
			glVertex3f(-1,1,1);
			glVertex3f(-1,-1,1);
			glEnd();	
		glPopMatrix();
	
}


void drawChasis(bool alt_color=false) {
  if (alt_color)
    glColor3f(.8,.3,.2);
  else
    
    glPushMatrix();
    
  glTranslatef(1,0,0);
    
  glColor3f(1,.5,.3);
  glBegin(GL_TRIANGLES);
  glNormal3f(.4,-1,0);
  glVertex3f(-1,-1,-1);
  glVertex3f(-1,-1,1);
  glVertex3f(1,0,0);
  glNormal3f(.4,1,0);
  glVertex3f(-1,1,-1);
  glVertex3f(-1,1,1);
  glVertex3f(1,0,0);
  glNormal3f(.4,0,1);
  glVertex3f(-1,1,1);
  glVertex3f(-1,-1,1);
  glVertex3f(1,0,0);
  glNormal3f(.4,0,-1);
  glVertex3f(-1,1,-1);
  glVertex3f(-1,-1,-1);
  glVertex3f(1,0,0);
  glEnd();
  glBegin(GL_QUADS);
  glNormal3f(-1,0,0);
  glVertex3f(-1,-1,-1);
  glVertex3f(-1,1,-1);
  glVertex3f(-1,1,1);
  glVertex3f(-1,-1,1);
  glEnd();
  
  glPopMatrix();
  
  
}




/*Le puse miembro a cada parte del brazo, puesto que el brazo y el antebrazo 
igual. No se si está bien, ya que cuando haya que usar los angulos no se como va 
quedar funcionan*/

void drawMiembro(bool alt_color=false){
	float radio_base=0.5;
	float radio_tapa=0.5;
	float altura=2;
	float lod=7;//nivel de detalle
	GLUquadricObj* q;

	//define e inicializa el objeto cuadrico	
	q= gluNewQuadric();	
	gluQuadricNormals(q, GLU_SMOOTH);
	
	//dibuja el cilindro
	glPushMatrix();
		glColor3f(0.75,0.75,0.75);//defino la base en un color gris claro.
		//glTranslatef(0,0,1);
		glRotated(90,0,1,0);//Para que quede sobre el eje z.
		gluCylinder(q,radio_base, radio_tapa, altura,lod, lod);
	glPopMatrix();
	
}
void drawCasco(int lod) {
  glutSolidSphere(1,lod,lod);
}


void drawCube() {
  glColor3f(1,1,1);
  glBegin(GL_LINES);
  glVertex3f(-1,-1,-1); glVertex3f(1,-1,-1);
  glVertex3f(-1,-1,-1); glVertex3f(-1,1,-1);
  glVertex3f(-1,-1,-1); glVertex3f(-1,-1,1);
  glVertex3f(1,1,1); glVertex3f(1,1,-1);
  glVertex3f(1,1,1); glVertex3f(1,-1,1);
  glVertex3f(1,1,1); glVertex3f(-1,1,1);
  glVertex3f(-1,-1,1); glVertex3f(1,-1,1);
  glVertex3f(-1,-1,1); glVertex3f(-1,1,1);
  glVertex3f(-1,1,-1); glVertex3f(-1,1,1);
  glVertex3f(-1,1,-1); glVertex3f(1,1,-1);
  glVertex3f(1,-1,-1); glVertex3f(1,1,-1);
  glVertex3f(1,-1,-1); glVertex3f(1,-1,1);
  glEnd();
  
  // ejes
  glLineWidth(4);
  glBegin(GL_LINES);
  glColor3f(1,0,0); glVertex3d(0,0,0); glVertex3d(1.5,0,0);
  glColor3f(0,1,0); glVertex3d(0,0,0); glVertex3d(0,1.5,0);
  glColor3f(0,0,1); glVertex3d(0,0,0); glVertex3d(0,0,1.5);
  glEnd();
  glLineWidth(2);
  
}

void drawPlanito() {
  // plano para apoyar el auto
  glColor4f(.5,.5,.5,.85);
  glBegin(GL_QUADS);
  glVertex3f(-1,-1,0);
  glVertex3f(-1, 1,0);
  glVertex3f( 1, 1,0);
  glVertex3f( 1,-1,0);
  glEnd();
}


void drawPista() {
  const int w_2=text_w*3/2, h_2=text_h*3/2;
  glEnable(GL_TEXTURE_2D);
  glColor3f(0,0,.7);
  glBegin(GL_QUADS);
  glNormal3f(0,0,1);
  glTexCoord2d(0,3); glVertex2f(-w_2,h_2);
  glTexCoord2d(3,3); glVertex2f(w_2,h_2);
  glTexCoord2d(3,0); glVertex2f(w_2,-h_2);
  glTexCoord2d(0,0); glVertex2f(-w_2,-h_2);
  glEnd();
  glDisable(GL_TEXTURE_2D);
}

void drawObjects() {
  //if (!animado) 
  //{
    //drawCube(); 
	//int v=0;

  //}
    
    
 // else {  // inicio else animado
    
    drawPista();
// @@@@@ Aplicar las transformaciones necesarias para ubicar las partes del 
//       brazo como van.
  
        
  if(contt==0)  {
    
     contt++;
     original=rang;
    
  }else{
    original=original-rang;
  }
    
    
 // float ca=cos(-original);
  
 // float co=sin(-original);
  
  
  //float rang=rang*(M_PI/180);
   
//  float ca=cos(-rang);
//  float co=sin(-rang);
  
  
  float ca=cos(-ang_brazo);//que es esto?
  float co=sin(-ang_brazo);// Y esto?
  
  
  
    
  glPushMatrix(); // rota todo inicio
  
      glTranslatef(0.0,0.0,0.165);
   //   glTranslatef(ax,ay,0.0);
      glRotatef(-aang,0,0,1);
      glRotatef(2,0,1,0);
  
    //     Base
	// La base es una pirame de 0.3x0.3x0.3
    glPushMatrix();
	    //Estas son las transformaciones para colocar la base
  		glTranslatef(0.0,0.0,1.0);//la traslado al inicio de coordenadas. Hay un 
		//translate tambien cuando se dibuja, en el drawBase. No se si esto esta 
		//bien
        glRotatef(-90,0,1,0);//roto para que la punta quede para arriba. Me
		//parece que y apunta hacia el observador.
		//x apunta hacia arriba. Y z hacia la derecha
		glScalef(0.5,0.3,0.3);
		drawBase(false);
	
	//glTranslatef(0.0,0.0,1.0);
	//glRotatef(90,0,1,0);
	
	//drawChasis(false);
    glPopMatrix();
  
    //Qué hace acá?  si desde aca
	glPushMatrix();  ///
 	glRotatef(-ang_completo,0,1,0);  ///
	//glRotatef(-100,0,1,0);
	  
	//  Brazo
      
      
    glPushMatrix();
        glTranslatef(0.0,0.0,1.0);
       	glRotatef(ang_brazo,0,1,0);
		glScalef(1.0,0.3,0.1);
        drawMiembro();	
	glPopMatrix();
      
      
    // Ante Brazo
    glPushMatrix();
        glTranslatef(1.0,0.0,1.0);//Porque usa dos traslate?. 
		glTranslatef(ca,0.0,co);
        glRotatef(ang_brazo_segundo,0,1,0);
		glScalef(1.0,0.3,0.1);
		drawMiembro();
	glPopMatrix();
	 
	  
	glPopMatrix(); //Esto que hace? si se hace 
	  
      
         
glPopMatrix(); //rota todo fin
  
    //bola
	glPushMatrix();
	  glRotatef(angulo,0,0,1);
	  glTranslatef(0.0,0.5,0.0);
	  // glTranslatef(distancia,0.0,0.0);
      glScalef(0.1,0.1,0.1);
      drawCasco(lod);
	glPopMatrix();
  
  
  
  
     
 // if (!animado) drawPlanito(); // referencia de suelo para cuando no esta la pista
  
 // } // fin else animado 
  
}
