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
extern float ang_mano;

extern float ang_completo;

extern float ang_brazo_segundo;
static const double PI=4*atan(1.0),R2G=180/M_PI, DOS_PI=2*M_PI, G2R=atan(1.0)/45;

extern float ang_completo_final;
extern float aangulo;//angulo de rotacion final

//Pra el antebrazo
extern bool relleno;
extern bool wire;
extern bool blend;
extern bool smooth;

extern float alfa;//angulo del brazo
extern float apertura_mano;//distancia entre dedos
//==========================================

//===Variables agregadas para este proyecto.

float original=0;

int contt=0;

float ca=0;

float co=0;

int ver=0;
//==========================================
// algunos objetos
//==========================================

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






/*Le puse miembro a cada parte del brazo, puesto que el brazo y el antebrazo 
igual. No se si está bien, ya que cuando haya que usar los angulos no se como va 
quedar funcionan*/

void drawMiembro(bool alt_color=false){
	float radio_base=0.5/6;
	float radio_tapa=0.5/6;
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
	
}//end drawMiembro


void drawMano(bool alt_color=false){
	float radio_base=0.5/6;
	float radio_tapa=0.5/6;
	float altura=1;
	float lod=7;//nivel de detalle
	GLUquadricObj* q;
	
	
	
	//define e inicializa el objeto cuadrico	
	q= gluNewQuadric();	
	gluQuadricNormals(q, GLU_SMOOTH);
	
	//dibujo el cilindro
	glPushMatrix();
		glColor3f(1,0,0);//defino la base en un color gris claro.
		glTranslatef(0,0,-0.5);
		
		gluCylinder(q,radio_base, radio_tapa, altura,lod, lod);//dibujo mano
		
		glRotatef(90,0,1,0);
		gluCylinder(q,radio_base, radio_tapa, altura/2, lod, lod);//dibujo dedo 1
		glTranslatef(-apertura_mano,0,0);
		gluCylinder(q,radio_base, radio_tapa, altura/2, lod, lod);//dibujo dedo 2
	glPopMatrix();
	
	
}



void drawBola(int lod) {
  glutSolidSphere(3,lod,lod);
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
  glColor3f(1,0,0); glVertex3d(0,0,0); glVertex3d(1.5,0,0);//rojo es el eje x
  glColor3f(0,1,0); glVertex3d(0,0,0); glVertex3d(0,1.5,0);// verde es el eje y
  glColor3f(0,0,1); glVertex3d(0,0,0); glVertex3d(0,0,1.5);//azul el eje z
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
    
	drawPista();
	drawPlanito();
	drawCube();
  
	//========= Base
	// La base es una pirame de 0.3x0.3x0.3
	glPushMatrix();
	      //Estas son las transformaciones para colocar la base
	      //glTranslatef(0.0,0.0,1.0);
	      glTranslatef(0.0,0.0,0.0);//la traslado al inicio de coordenadas. Hay un 
		  //translate tambien cuando se dibuja, en el drawBase. No se si esto esta 
	      //bien
	      glRotatef(-90,0,1,0);//roto para que la punta quede para arriba. Me
	 	  //parece que y apunta hacia el observador.
	      //x apunta hacia arriba. Y z hacia la derecha
	      glScalef(0.5,0.3,0.3);
	      drawBase(false);
	glPopMatrix();

	//aca tengo z apuntando hacia arriba
	
    glPushMatrix(); // rota todo inicio
    
	    glRotatef(aangulo,0,0,1);//este rotate se usa para rotar el angulo w
	    //Acá z sigue apuntando para arriba
		 //========= Brazo y Antebrazo
      
          glPushMatrix();
	              //parece que y apunta hacia el observador.
	              //x apunta hacia la izquierda. Y z hacia la derecha
				 glTranslatef(0.0,0.0,1.0);
		         glRotatef(alfa,0,1,0);  
                 drawMiembro();	
				 
				//====dibujo ante brazo	
		         glTranslatef(2,0,0);
		         glRotatef(-2*(alfa),0,1,0);
		         drawMiembro();
				 
				 //===dibujo la mano
				 glTranslatef(2,0,0);
				 drawMano();
	      glPopMatrix();
        
    glPopMatrix(); //rota todo fin
  
        //========= Bola
					
        glPushMatrix();
		     glRotatef(angulo,0,0,1);//Este angulo es el angulo w
		     glTranslatef(distancia,0.0,0.0);//distancia es h
			 glTranslatef(0.0,0.5,0.0);
			 glScalef(0.1,0.1,0.1);
             drawBola(lod);

        glPopMatrix();
  
  
  
}
