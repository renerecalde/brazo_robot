// Transformaciones (buscar @@@@@ y ver consignas.pdf)

#include <cmath> // atan sqrt
#include <cstdlib> // exit
#include <iostream> // cout
#include <fstream> // file io
#include <iomanip>
#ifdef __APPLE__
# include <OpenGL/gl.h>
# include <GLUT/glut.h>
#else
# include <GL/gl.h>
# include <GL/glut.h>
#endif
using namespace std;

//------------------------------------------------------------
// variables globales y defaults

float aang=0; // orientacion
float ax=0,ay=0; // posicion en la pista
float aacel=0; // acelerador (eje y del mouse)
float aspeed=0; // velocidad actual
float topspeed=60; // velocidad maxima
float rang=0; // direccion de las ruedas delanteras respecto al auto (eje x del mouse) 
float rang2=0; // giro de las ruedas sobre su eje, cuando el auto avanza 
float largo_brazo=2;
float largo_antebrazo=2;
float largo_bace=1;
float angulo=0; //angulo bola
float distancia=1;  //distancia bola
float limite_inferior=1;
float limite_superior=2;
float ang_brazo=0;
float apertura=0;
float ang_completo=0;


int
  w=640,h=480, // tamaño de la ventana
  boton=-1, // boton del mouse clickeado
  xclick,yclick, // x e y cuando clickeo un boton
  lod=10; // nivel de detalle (subdivisiones de lineas y superficies parametricas)
float // luces y colores en float
  lpos[]={2,10,5,0}, // posicion luz, l[4]: 0 => direccional -- 1 => posicional
  escala=100,escala0, // escala de los objetos window/modelo pixeles/unidad
  dist_cam=4, // distancia del ojo al origen de coordenadas en la manipulación
  eye[]={0,0,1}, target[]={0,0,0}, up[]={0,1,0}, // camara, mirando hacia y vertical
  znear=2, zfar=50, //clipping planes cercano y alejado de la camara (en 5 => veo de 3 a -3)
  amy,amy0, // angulo del modelo alrededor del eje y
  ac0=1,rc0, // angulo resp x y distancia al target de la camara al clickear
  sky_color[]={.4,.4,.8,1}; // color del fondo y de la niebla (azul)
//  sky_color[]={0,0,0,0}; // color del fondo y de la niebla (negro)

bool // variables de estado de este programa
  luz_camara=true,  // luz fija a la camara o al espacio
  perspectiva=true, // perspectiva u ortogonal
  animado=true,    // el auto se mueve por la pista
  dibuja=true,      // false si esta minimizado
  relleno=true,     // dibuja relleno o no
  cl_info=true,     // informa por la linea de comandos
  top_view=false,    // vista superior o vista trasera del auto cuando animate=true
  fog=false,
  creado=false,
  mover=false;
   
// tamaño de la pista sobre
int text_w = 200; 
int text_h = 200;
int suma=100;

int counter=0;

short modifiers=0;  // ctrl, alt, shift (de GLUT)
inline short get_modifiers() {return modifiers=(short)glutGetModifiers();}

// temporizador:
static int msecs=50; // milisegundos por frame


//!!!!!!!!!!!!!

// para saber qué teclas hay apretadas cuando se calcula el movimiento del auto
static bool keys[4]; // se modifica Special_cb y SpecialUp_cb, se usa en Idle_cb

static const double R2G=45/atan(1.0);
static const double G2R=atan(1.0)/45;

GLuint texid;

// Load a PPM Image
bool mipmap_ppm(const char *ifile) {
  char dummy; int maxc,wt,ht;
  ifstream fileinput(ifile, ios::binary);
  if (!fileinput.is_open()) { cerr<<"Not found"<<endl; return false; }
  fileinput.get(dummy);
  if (dummy!='P') {cerr<<"Not P6 PPM file"<<endl; return false;}
  fileinput.get(dummy);
  if (dummy!='6') {cerr<<"Not P6 PPM file"<<endl; return false;}
  fileinput.get(dummy);
  dummy=fileinput.peek();
  if (dummy=='#') do {
    fileinput.get(dummy);
  } while (dummy!=10);
  fileinput >> wt >> ht;
  fileinput >> maxc;
  fileinput.get(dummy);
  unsigned char *img=new unsigned char[3*wt*ht];
  fileinput.read((char *)img, 3*wt*ht);
  fileinput.close();
  gluBuild2DMipmaps(GL_TEXTURE_2D, 3, wt, ht,  GL_RGB, GL_UNSIGNED_BYTE, img);
  delete[] img;
  return true;
}

//------------------------------------------------------------
// redibuja los objetos
extern void drawObjects();
// Cada vez que hace un redisplay
void Display_cb() { // Este tiene que estar
  if (!dibuja) return;
  
  // borra los buffers de pantalla y z
  glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
  glPolygonMode(GL_FRONT_AND_BACK,relleno?GL_FILL:GL_LINE);
  
  glPushMatrix();
  glLightfv(GL_LIGHT0,GL_POSITION,lpos);  // ubica la luz
  if (animado) {
    if (top_view) 
   //   gluLookAt(ax,ay,15,ax,ay,0,0,1,0);
    
    gluLookAt(0,2,2,0,0,0,0,1,0);
  
      
    else {
      // @@@@@ Ubicar correctamente la camara con gluLookAt, (y ver qué pasa con la luz cuando el auto se meuve)
   
     // gluLookAt(ax,ay,15,ax,ay,0,0,1,0);
      ////
    // gluLookAt(target[0]-0.1,target[1],target[2]+0.1,target[0],target[1],target[2],up[0],up[1],up[2]);
      ////
  //   gluLookAt(ax-dist_cam*cos(-aang*M_PI/180),ay-dist_cam*sin(-aang*M_PI/180),2,ax,ay,0,0,0,1);
   
      gluLookAt(0,3,2,0,0,0,0,0,1);
      
   }
  } else {
    gluLookAt(0,2,2,0,0,0,0,1,0);
   // gluLookAt(dist_cam*eye[0],dist_cam*eye[1],dist_cam*eye[2],0,0,0,up[0],up[1],up[2]);
   // gluLookAt(ax,ay,15,ax,ay,0,0,1,0);
  }
  
  drawObjects();
  glPopMatrix();
  
  glutSwapBuffers();
  
#ifdef _DEBUG
  // chequea errores
  int errornum=glGetError();
  while(errornum!=GL_NO_ERROR){
    if (cl_info){
      if(errornum==GL_INVALID_ENUM)
        cout << "GL_INVALID_ENUM" << endl;
      else if(errornum==GL_INVALID_VALUE)
        cout << "GL_INVALID_VALUE" << endl;
      else if (errornum==GL_INVALID_OPERATION)
        cout << "GL_INVALID_OPERATION" << endl;
      else if (errornum==GL_STACK_OVERFLOW)
        cout << "GL_STACK_OVERFLOW" << endl;
      else if (errornum==GL_STACK_UNDERFLOW)
        cout << "GL_STACK_UNDERFLOW" << endl;
      else if (errornum==GL_OUT_OF_MEMORY)
        cout << "GL_OUT_OF_MEMORY" << endl;
    }
    errornum=glGetError();
  }
#endif // _DEBUG
}

//------------------------------------------------------------
// Regenera la matriz de proyeccion
// cuando cambia algun parametro de la vista
void regen() {
  //  if (cl_info) cout << "regen" << endl;
  if (!dibuja) return;
  
  // matriz de proyeccion
  glMatrixMode(GL_PROJECTION);  glLoadIdentity();
  
  double w0=(double)w/2/escala,h0=(double)h/2/escala; // semiancho y semialto en el target
  
  // frustum, pal eye pero con los z positivos (delante del ojo)
  if (perspectiva){ // perspectiva
    w0*=znear/dist_cam,h0*=znear/dist_cam;  // w0 y h0 en el near
    glFrustum(-w0,w0,-h0,h0,znear,zfar);
  }
  else { // proyeccion ortogonal
    glOrtho(-w0,w0,-h0,h0,znear,zfar);
  }
  
  glMatrixMode(GL_MODELVIEW); glLoadIdentity(); // matriz del modelo->view
  
  glutPostRedisplay(); // avisa que hay que redibujar
}

//------------------------------------------------------------
// Animacion

// Cuando no hay ningún otro evento se invoca   a glutIdleFunc 
// El "framerate" lo determina msec, a menos que la complejidad 
// del modelo (lod) y la no aceleracion por hardware lo bajen
void Idle_cb() {
  static int anterior=glutGet(GLUT_ELAPSED_TIME); // milisegundos desde que arranco
  
  if (msecs!=1){ // esperar msec antes de pasar al próximo cuadro, si msecs es 1 no pierdo tiempo
    int tiempo=glutGet(GLUT_ELAPSED_TIME), lapso=tiempo-anterior;
    if (lapso<msecs) return;
    
    suma+=lapso;
    if (++counter==100) {
      //      cout << "<ms/frame>= " << suma/100.0 << endl;
      counter=suma=0;
    }
    
    
    
    anterior=tiempo;
  }
  
  // aplicar los controles
  if (keys[2]!=keys[3])
    rang=(8*rang+(keys[2]?.5:-.5)*float(10*aspeed+60*(topspeed-aspeed))/topspeed)/9;
  else
    rang=3*rang/4;
  if (keys[1]) aacel=-1;
  else if (keys[0]) aacel=1;
  else aacel=-.2;
  // mover el auto
  aspeed+=aacel*.75-.25;
  if (aspeed<0) aspeed=0;
  else if (aspeed>topspeed) aspeed=topspeed;
  if (animado) {
    ax+=aspeed*cos(aang*G2R)/100;
    ay+=aspeed*sin(aang*G2R)/100;
    // la pista es ciclica
    if (ax<-text_w) ax+=text_w*2;
    else if (ax>text_w) ax-=text_w*2;
    if (ay<-text_h) ay+=text_h*2;
    else if (ay>text_h) ay-=text_h*2;
  } else {
    ax=ay=0;
  }
  aang-=rang*aspeed/150;
  rang2+=aspeed;
    
  if (true){
     
 //   cout<<setprecision(3)<<fixed<<"x:"<<ax<<" y:"<<ay<<" rang:"<<rang<<" rang2:"<<rang2<<" aspeed:"<<aspeed<<" Coseno -rang: "<<cos(-rang)<<" Seno -rang: "<<sin(-rang)<<"\r"<<flush;
    
   // cout<<setprecision(3)<<fixed<<" Angulo: "<<angulo<<"\r"<<flush;
   
  }
    
  glutPostRedisplay();
}

//------------------------------------------------------------
// Maneja cambios de ancho y alto de la ventana
void Reshape_cb(int width, int height){
  h=height; w=width;
  if (cl_info) cout << "reshape: " << w << "x" << h << endl;
  
  if (w==0||h==0) {// minimiza
    dibuja=false; // no dibuja mas
    glutIdleFunc(0); // no llama a cada rato a esa funcion
    return;
  }
  else if (!dibuja&&w&&h){// des-minimiza
    dibuja=true; // ahora si dibuja
    glutIdleFunc(Idle_cb); // registra de nuevo el callback
  }
  
  glViewport(0,0,w,h); // region donde se dibuja
  
  regen(); //regenera la matriz de proyeccion
}

//------------------------------------------------------------
// Movimientos del mouse

// Rotacion del punto (x,y,z) sobre el eje (u,v,w), formula de Rodriguez
void Rotate(float &x, float &y ,float &z, float u, float v, float w, float a) {
  float ux=u*x, uy=u*y, uz=u*z;
  float vx=v*x, vy=v*y, vz=v*z;
  float wx=w*x, wy=w*y, wz=w*z;
  float sa=sin(a), ca=cos(a);
  x=u*(ux+vy+wz)+(x*(v*v+w*w)-u*(vy+wz))*ca+(-wy+vz)*sa;
  y=v*(ux+vy+wz)+(y*(u*u+w*w)-v*(ux+wz))*ca+(wx-uz)*sa;
  z=w*(ux+vy+wz)+(z*(u*u+v*v)-w*(ux+vy))*ca+(-vx+uy)*sa;
}

void Normalize(float x[]) {
  float m=sqrt(x[0]*x[0]+x[1]*x[1]+x[2]*x[2]);
  x[0]/=m; x[1]/=m; x[2]/=m;
}

// girar la camara o hacer zoom
void Motion_cb(int xm, int ym){ // drag
  if (modifiers==GLUT_ACTIVE_SHIFT) { // cambio de escala
    escala=escala0*exp((yclick-ym)/100.0);
    regen();
  } else  { // manipulacion
    // @@@@@ Analizar como se giran los vectores eye y up
    float angulox=double(xclick-xm)/100; // respecto a up
    Rotate(eye[0],eye[1],eye[2],up[0],up[1],up[2],angulox);
    float anguloy=double(ym-yclick)/100; // respecto a up vectorial eye
    float // up vectorial eye normalizado
      vx=((up[2]*eye[1])-(up[1]*eye[2])),
      vy=((up[0]*eye[2])-(up[2]*eye[0])),
      vz=((up[1]*eye[0])-(up[0]*eye[1]));
    Rotate(eye[0],eye[1],eye[2],vx,vy,vz,anguloy);
    Rotate(up[0],up[1],up[2],vx,vy,vz,anguloy);
    Normalize(eye); Normalize(up); // mata el error numérico
    xclick=xm; yclick=ym;
    glutPostRedisplay();
  }
}

// Clicks del mouse
// GLUT LEFT BUTTON, GLUT MIDDLE BUTTON, or GLUT RIGHT BUTTON
// The state parameter is either GLUT UP or GLUT DOWN
// glutGetModifiers may be called to determine the state of modifier keys
void Mouse_cb(int button, int state, int x, int y){
  if (button==GLUT_LEFT_BUTTON){
    if (state==GLUT_DOWN) {
      xclick=x; yclick=y;
      boton=button;
      get_modifiers();
      glutMotionFunc(Motion_cb);
      if (modifiers==GLUT_ACTIVE_SHIFT) escala0=escala; // escala      
      else if (modifiers==(GLUT_ACTIVE_ALT|GLUT_ACTIVE_CTRL)) return; // origen textura
      else { // manipulacion
        double yc=eye[1]-target[1],zc=eye[2]-target[2];
        rc0=sqrt(yc*yc+zc*zc); ac0=atan2(yc,zc);
        amy0=amy;
      }
    }
    else if (state==GLUT_UP){
      boton=-1;
      glutMotionFunc(NULL);
    }
  }
}
//------------------------------------------------------------
// Teclado
// Special keys (non-ASCII)
// aca es int key
void Special_cb(int key,int xm=0,int ym=0) {
  if (key==GLUT_KEY_F4){ // alt+f4 => exit
    get_modifiers();
    if (modifiers==GLUT_ACTIVE_ALT)
      exit(EXIT_SUCCESS);
  }
  else if (key==GLUT_KEY_UP) keys[0]=true;
  else if (key==GLUT_KEY_DOWN) keys[1]=true;
  else if (key==GLUT_KEY_RIGHT) keys[2]=true;
  else if (key==GLUT_KEY_LEFT) keys[3]=true;
}

void SpecialUp_cb(int key,int xm=0,int ym=0) {
  if (key==GLUT_KEY_UP) keys[0]=false;
  else if (key==GLUT_KEY_DOWN) keys[1]=false;
  else if (key==GLUT_KEY_RIGHT) keys[2]=false;
  else if (key==GLUT_KEY_LEFT) keys[3]=false;
}



//Función que devuelve un valor random para que la bola aparezca en algun lugar.
int randInRange(int min, int max)
{
  return min + (int) (rand() / (double) (RAND_MAX + 1) * (max - min + 1));
}


// Maneja pulsaciones del teclado (ASCII keys)
void Keyboard_cb(unsigned char key,int x=0,int y=0) {
  switch (key){
  case 'a': case 'A':// Agarrar la bola.
	
    mover=!mover;
    if (mover) {
      
		 apertura = sqrt(1+distancia);
		
		 ang_brazo = acos((((largo_brazo * largo_brazo) - (largo_antebrazo * largo_antebrazo) + (apertura * apertura)) / (2 * largo_antebrazo * apertura ) )); 
		
		// ang_brazo = 
		 
		 
		 ang_completo=1/ (sin(distancia/apertura));
		 
		 int diferencia= aang - angulo;
		 
		 float avance_ang_completo =ang_completo /diferencia;
		 
		 float avance_ang_brazo =ang_completo /diferencia;
		 
		 
		// ang_brazo=50;
		 
		//rang=ang_brazo;
		
	//	ang_completo
			
			
		
		if(aang < angulo) {
			
			
			int inicio=aang;
		
						
			
			
			for( int aa=inicio ;aa<=angulo   ;aa++  ) {
							
			//	static int anterior=glutGet(GLUT_ELAPSED_TIME); 
				
				
				aang++;
				regen();
				glutPostRedisplay();
						
				cout<<" paso "<<flush;
				
				/*
				
				if (msecs!=1){ // esperar msec antes de pasar al próximo cuadro, si msecs es 1 no pierdo tiempo
					int tiempo=glutGet(GLUT_ELAPSED_TIME), lapso=tiempo-anterior;
					if (lapso<msecs) return;
					
					suma+=lapso;
					if (++counter==100) {
						//      cout << "<ms/frame>= " << suma/100.0 << endl;
						counter=suma=0;
					}
					anterior=tiempo;
				}
				
				*/
				
			}
			
		}else {
			///////////////
			
			int inicio=aang;
			
			for( int aa=inicio ;aa>=angulo   ;aa--) {
				
				//static int anterior=glutGet(GLUT_ELAPSED_TIME); 
				
				aang--;
			
			    regen();
				glutPostRedisplay();
	
				/*
				
				if (msecs!=1){ // esperar msec antes de pasar al próximo cuadro, si msecs es 1 no pierdo tiempo
					int tiempo=glutGet(GLUT_ELAPSED_TIME), lapso=tiempo-anterior;
					if (lapso<msecs) return;
					
					suma+=lapso;
					if (++counter==100) {
						//      cout << "<ms/frame>= " << suma/100.0 << endl;
						counter=suma=0;
					}
				    anterior=tiempo;
				}
				
				*/
				
			}
			
			//////////////
		}
		
      
    if (cl_info) cout << ((mover)? "Mover" : "No mover") << endl;
    
    }
	  break;	  
  case 'b': case 'B': // Insertar la bola 
    creado=!creado;
   // if (creado) {
      
      angulo =randInRange(0,360);

	  distancia=1+randInRange(0,10)/10;
      
     // glutPostRedisplay();
      
    if (cl_info) cout << ((creado)? "Creado: " : "No creado: ") << endl;
    
  //}
	  break;	  	
  case 'c': case 'C': // Tirar la bola
	  
	  break;	  	 	  
  case 'f': case 'F': // relleno
    relleno=!relleno;
    if (relleno) 
      glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    else
      glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    if (cl_info) cout << ((relleno)? "Relleno" : "Sin Relleno") << endl;
    break;
  case 'i': case 'I': // info
    cl_info=!cl_info;
    cout << ((cl_info)? "Info" : "Sin Info") << endl;
    break;
  case 'j': case 'J': // luz fija a la camara o en el espacio
    luz_camara=!luz_camara;
    if (cl_info)
      cout << "Luz fija " << ((luz_camara)? "a la camara" : "en el espacio") << endl;
    regen();
    break;
  case 'p': case 'P':  // perspectiva
    perspectiva=!perspectiva;
    if (cl_info) cout << ((perspectiva)? "Perspectiva" : "Ortogonal") << endl;
    regen();
    break;
  case 'r': case 'R': // movimiento
    animado=!animado; aspeed=aang=rang=0;
    if (cl_info) cout << ((animado)? "Animado" : "No Animado") << endl;
    break;
  case 't': case 'T': // movimiento
    if (!animado) Keyboard_cb('r',x,y);
    top_view=!top_view;
    if (cl_info) cout << ((top_view)? "Vista Superior" : "Vista Posterior") << endl;
    break;
  case 'n': case 'N':  // niebla
    fog=!fog;
    if (fog) glEnable (GL_FOG); else glDisable(GL_FOG);
    if (cl_info) cout << ((fog)? "Con Niebla" : "Sin Niebla") << endl;
    break;
  case 27: // escape => exit
    get_modifiers();
    if (!modifiers)
      exit(EXIT_SUCCESS);
    break;
  case '+': case '-': // lod
    if (key=='+') lod++; else {lod--; if (lod==0) lod=1;}
    if (cl_info) cout << "Nivel de detalle: "  << lod << endl;
    break;
  }
  regen();
  glutPostRedisplay();
}

//------------------------------------------------------------
// Menu
void Menu_cb(int value)
{
  switch (value){
  //Agarrar bola	  
  case 'a':
	  Keyboard_cb('a');
	  return; 	
  //Insertar bola	  
  case 'b':
	  Keyboard_cb('b');
	  return; 	  
  //Tirar la bola en la canasta	  
  case 'c':
	  Keyboard_cb('c');
	  return; 	  
	  
  case 'n':
    Keyboard_cb('n');
    return;
  case 'f':
    Keyboard_cb('f');
    return;
  case 'i':
    Keyboard_cb('i');
    return;
  case 'p':
    Keyboard_cb('p');
    return;
  case 'r':
    Keyboard_cb('r');
    return;
  case 't':
    Keyboard_cb('t');
    return;
  case '+':
    Keyboard_cb('+');
    return;
  case '-':
    Keyboard_cb('-');
    return;
  case 27: //esc
    exit(EXIT_SUCCESS);
  }
}
//------------------------------------------------------------
// pregunta a OpenGL por el valor de una variable de estado
int integerv(GLenum pname){
  int value;
  glGetIntegerv(pname,&value);
  return value;
}
#define _PRINT_INT_VALUE(pname) #pname << ": " << integerv(pname) <<endl

//------------------------------------------------------------
// Inicializa GLUT y OpenGL
void initialize() {
  // pide z-buffer, color RGB y double buffering
  glutInitDisplayMode(GLUT_DEPTH|GLUT_RGB|GLUT_ALPHA|GLUT_DOUBLE);
  
  glutInitWindowSize(w,h); glutInitWindowPosition(50,50);
  
  glutCreateWindow("Brazo robot, Recalde Menin "); // crea el main window
  
  //declara los callbacks
  //los que no se usan no se declaran
  glutDisplayFunc(Display_cb); // redisplays
  glutReshapeFunc(Reshape_cb); // cambio de alto y ancho
  glutKeyboardFunc(Keyboard_cb); // teclado
  glutSpecialFunc(Special_cb); // teclas especiales (cuando se apretan)
  glutSpecialUpFunc(SpecialUp_cb); // teclas especiales (cuando se sueltan)
  glutMouseFunc(Mouse_cb); // botones picados
  glutIdleFunc(Idle_cb); // registra el callback
  
  // crea el menu
  glutCreateMenu(Menu_cb);
  glutAddMenuEntry("   [a]_Agarrar bola          ", 'a');
  glutAddMenuEntry("   [b]_Insertar bola         ", 'b');
  glutAddMenuEntry("   [c]_Canasta               ", 'c');
  glutAddMenuEntry("   [f]_Caras Rellenas        ", 'f');
  glutAddMenuEntry("   [i]_Info On/Off           ", 'i');
  glutAddMenuEntry("   [p]_Perspectiva/Ortogonal ", 'p');
  glutAddMenuEntry("   [r]_Anima                 ", 'r');
  glutAddMenuEntry("   [t]_Vista Superior        ", 't');
  glutAddMenuEntry("   [n]_Niebla On/Off         ", 'n');
  glutAddMenuEntry("   [+]_+Nivel de Detalle     ", '+');
  glutAddMenuEntry("   [-]_-Nivel de Detalle     ", '-');
  glutAddMenuEntry(" [Esc]_Exit                  ", 27);
  glutAttachMenu(GLUT_RIGHT_BUTTON);
  
  // ========================
  // estado normal del OpenGL
  // ========================
  
  glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LEQUAL); // habilita el z-buffer
  glEnable(GL_NORMALIZE); // para que el scaling no moleste
  glEnable(GL_AUTO_NORMAL); // para nurbs??
  glEnable(GL_POLYGON_OFFSET_FILL); glPolygonOffset (1,1); // coplanaridad
  
  glEnable(GL_BLEND); // para usar transparencias en drawPlanito
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  
  // interpola normales por nodos o una normal por plano
  //  glShadeModel(GL_FLAT);
  
  // color de fondo
  glClearColor(sky_color[0],sky_color[1],sky_color[2],sky_color[3]);
  
  // direccion de los poligonos
  glFrontFace(GL_CCW); glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
  glCullFace(GL_BACK); glDisable(GL_CULL_FACE);// se habilita por objeto (cerrado o abierto)
  
  // define luces
  static const float
    lambient[]={.4f,.4f,.4f,1}, // luz ambiente
    ldiffuse[]={.7f,.7f,.7f,1}, // luz difusa
    lspecular[]={1,1,1,1};      // luz especular
  glLightfv(GL_LIGHT0,GL_AMBIENT,lambient);
  glLightfv(GL_LIGHT0,GL_DIFFUSE,ldiffuse);
  glLightfv(GL_LIGHT0,GL_SPECULAR,lspecular);
  // caras de atras y adelante distintos (1) o iguales (0)
  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,0);
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,0);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHTING);
  
  // material estandar
  static const float
    fambient[]={.4f,.2f,.1f,1},
    fdiffuse[]={.5f,.2f,.1f,.5},
    fspecular[]={1,1,1,1},
    bambient[]={.2f,.4f,.4f,1},
    bdiffuse[]={.2f,.5f,.4f,.5},
    bspecular[]={1,1,1,1};
  static const int
    fshininess=25,
    bshininess=50;
  glMaterialfv(GL_FRONT,GL_AMBIENT,fambient);
  glMaterialfv(GL_FRONT,GL_DIFFUSE,fdiffuse);
  glMaterialfv(GL_FRONT,GL_SPECULAR,fspecular);
  glMateriali(GL_FRONT,GL_SHININESS,fshininess);
  glMaterialfv(GL_BACK,GL_AMBIENT,bambient);
  glMaterialfv(GL_BACK,GL_DIFFUSE,bdiffuse);
  glMaterialfv(GL_BACK,GL_SPECULAR,bspecular);
  glMateriali(GL_BACK,GL_SHININESS,bshininess);
  glEnable(GL_COLOR_MATERIAL);
  
  // textura
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glGenTextures(1, &texid);
  glBindTexture(GL_TEXTURE_2D, texid);
  mipmap_ppm("pista.ppm");
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
  
  // efecto de niebla
  glFogi (GL_FOG_MODE, GL_EXP2);
  glFogfv (GL_FOG_COLOR, sky_color);
  glFogf (GL_FOG_DENSITY, .03);
  if (fog) glEnable(GL_FOG);
  
  // ========================
  // info
  if (cl_info)
    cout << "Vendor:         " << glGetString(GL_VENDOR) << endl
    << "Renderer:       " << glGetString(GL_RENDERER) << endl
    << "GL_Version:     " << glGetString(GL_VERSION) << endl
    << "GL_Extensions:  " << glGetString(GL_EXTENSIONS) << endl
    << "GLU_Version:    " << gluGetString(GLU_VERSION) << endl
    << "GLU_Extensions: " << gluGetString(GLU_EXTENSIONS) << endl
    << _PRINT_INT_VALUE(GL_DOUBLEBUFFER)
    << _PRINT_INT_VALUE(GL_STEREO)
    << _PRINT_INT_VALUE(GL_AUX_BUFFERS)
    << _PRINT_INT_VALUE(GL_RED_BITS)
    << _PRINT_INT_VALUE(GL_GREEN_BITS)
    << _PRINT_INT_VALUE(GL_BLUE_BITS)
    << _PRINT_INT_VALUE(GL_ALPHA_BITS)
    << _PRINT_INT_VALUE(GL_DEPTH_BITS)
    << _PRINT_INT_VALUE(GL_STENCIL_BITS)
    << _PRINT_INT_VALUE(GL_ACCUM_RED_BITS)
    << _PRINT_INT_VALUE(GL_ACCUM_GREEN_BITS)
    << _PRINT_INT_VALUE(GL_ACCUM_BLUE_BITS)
    << _PRINT_INT_VALUE(GL_ACCUM_ALPHA_BITS)
    ;
  // ========================
  
  regen(); // para que setee las matrices antes del 1er draw
}

//------------------------------------------------------------
// main
int main(int argc,char** argv) {
  glutInit(&argc,argv);// inicializa glut
  initialize(); // condiciones iniciales de la ventana y OpenGL
  glutMainLoop(); // entra en loop de reconocimiento de eventos
  return 0;
}
