#define _USE_MATH_DEFINES
#include <cmath>
//glew include
#include <GL/glew.h>

//std includes
#include <string>
#include <iostream>

//glfw include
#include <GLFW/glfw3.h>

// program include
#include "Headers/TimeManager.h"

// Shader include
#include "Headers/Shader.h"

// Model geometric includes
#include "Headers/Sphere.h"
#include "Headers/Cylinder.h"
#include "Headers/Box.h"
#include "Headers/FirstPersonCamera.h"
#include "Headers/ThirdPersonCamera.h"

//GLM include
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Headers/Texture.h"

// Include loader Model class
#include "Headers/Model.h"

#include "Headers/AnimationUtils.h"
#include "Headers/Terrain.h"
#include "Headers/Colisiones.h"

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

int screenWidth;
int screenHeight;

GLFWwindow *window;

Shader shader;
//Shader con skybox
Shader shaderSkybox;
//Shader con multiples luces
Shader shaderMulLighting;
//Shader para el terreno
//Shader shaderTerrain;

std::shared_ptr<Camera> camera(new ThirdPersonCamera());
float distanceFromPlayer = 6.0f;

Sphere skyboxSphere(20, 20);
Box boxCesped;
Box boxWalls;
Box boxHighway;
Box boxLandingPad;
//Sphere esfera1(7, 7);		//Esfera creada como ejemplo

Box boxCollider;
Sphere sphereCollider(10, 10);
Cylinder cylinderCollider(10, 10, 1, 1, 1);	// rebanadas, tapas, radio1, radio2, altura

// Models complex instances
Model scene1;

// Modelos animados
// Cat
Model catModelAnimate;
bool stopJump = false;

// Terrain model instance
//Terrain terrain(-1,-1,200,32, "../Textures/HeightmapP3.png");

GLuint textureCespedID, textureWallID, textureWindowID, textureHighwayID, textureLandingPadID;		// Texturas
GLuint skyboxTextureID;

GLenum types[6] = {
GL_TEXTURE_CUBE_MAP_POSITIVE_X,
GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };

std::string fileNames[6] = { 
		"../Textures/clouds1/clouds1_north.bmp",
		"../Textures/clouds1/clouds1_south.bmp",
		"../Textures/clouds1/clouds1_up.bmp",
		"../Textures/clouds1/clouds1_down.bmp",
		"../Textures/clouds1/clouds1_west.bmp",
		"../Textures/clouds1/clouds1_east.bmp" };

bool exitApp = false;
int lastMousePosX, offsetX = 0;
int lastMousePosY, offsetY = 0;

// Model matrix definitions
glm::mat4 modelMatrixCat = glm::mat4(1.0f);
glm::mat4 modelMatrixCatPrev = glm::mat4(1.0f);  // Posición anterior del cat para resolver colisiones
glm::mat4 modelMatrixScene1 = glm::mat4(1.0f);

int animationCatIndex = 0;

int modelSelected = 0;
bool enableCountSelected = true;

// Variables to animations keyframes
bool saveFrame = false, availableSave = true;
std::ofstream myfile;
std::string fileName = "";
bool record = false;

// Almacenar estado de las variables de los modelos de colisiones
std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4>> collidersOBB;
std::map<std::string, std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4>> collidersSBB;

// Variables de física de cat
float catVelY   = 0.0f;		// Velocidad vertical de cat (+ = sube, - = cae)
float GRAVITY   = 9.8f;		// Aceleración gravitacional
float JUMP_FORCE = 6.0f;	// Impulso inicial del salto
bool catOnGround = false;	// true cuando cat está apoyado sobre un collider

double deltaTime;
double currTime, lastTime;

// Se definen todss las funciones.
void reshapeCallback(GLFWwindow *Window, int widthRes, int heightRes);
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
void mouseCallback(GLFWwindow *window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow *window, int button, int state, int mod);
void mouseScrollCallback(GLFWwindow *window, double xscrollOffset, double yscrollOffset);
void init(int width, int height, std::string strTitle, bool bFullScreen);
void destroy();
bool processInput(bool continueApplication = true);

// Implementacion de todas las funciones.
void init(int width, int height, std::string strTitle, bool bFullScreen) {

	if (!glfwInit()) {		//Iniciar glfw
		std::cerr << "Failed to initialize GLFW" << std::endl;
		exit(-1);
	}

	screenWidth = width;	//Ancho de la ventana
	screenHeight = height;	//Alto de la ventana

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	//Versión de opengl

	if (bFullScreen)		//Validar fullscreen
		window = glfwCreateWindow(width, height, strTitle.c_str(),
				glfwGetPrimaryMonitor(), nullptr);
	else
		window = glfwCreateWindow(width, height, strTitle.c_str(), nullptr,
				nullptr);

	if (window == nullptr) {
		std::cerr
				<< "Error to create GLFW window, you can try download the last version of your video card that support OpenGL 3.3+"
				<< std::endl;
		destroy();
		exit(-1);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);

	glfwSetWindowSizeCallback(window, reshapeCallback);		//Eventos de redimensión de la pantalla
	glfwSetKeyCallback(window, keyCallback);				//Eventos de teclado
	glfwSetCursorPosCallback(window, mouseCallback);		//Eventos de movimiento de mouse
	glfwSetMouseButtonCallback(window, mouseButtonCallback);//Eventos de click
	glfwSetScrollCallback(window, mouseScrollCallback); 	//Eventos de scroll
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Init glew
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		std::cerr << "Failed to initialize glew" << std::endl;
		exit(-1);
	}

	glViewport(0, 0, screenWidth, screenHeight);	//Zona de dibujo de la ventana
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	glEnable(GL_DEPTH_TEST);		//Buffer de profundidad
	glEnable(GL_CULL_FACE);			//Buffer de caras ocultas

	// Inicialización de los shaders
	shader.initialize("../Shaders/colorShader.vs", "../Shaders/colorShader.fs");
	shaderSkybox.initialize("../Shaders/skyBox.vs", "../Shaders/skyBox.fs");
	shaderMulLighting.initialize("../Shaders/iluminacion_textura_animation.vs", "../Shaders/multipleLights.fs");
	//shaderTerrain.initialize("../Shaders/terrain.vs", "../Shaders/terrain.fs");

	// Inicializacion de los objetos.
	skyboxSphere.init();
	skyboxSphere.setShader(&shaderSkybox);
	skyboxSphere.setScale(glm::vec3(20.0f, 20.0f, 20.0f));

	sphereCollider.init();
	sphereCollider.setShader(&shader);		// Shader sin textura
	boxCollider.init();
	boxCollider.setShader(&shader);
	cylinderCollider.init();
	cylinderCollider.setShader(&shader);

	boxCesped.init();
	boxCesped.setShader(&shaderMulLighting);

	boxWalls.init();
	boxWalls.setShader(&shaderMulLighting);

	boxHighway.init();
	boxHighway.setShader(&shaderMulLighting);

	boxLandingPad.init();
	boxLandingPad.setShader(&shaderMulLighting);

	/*esfera1.init();								//Enviar información de la esfera a la tarjeta de video
	esfera1.setShader(&shaderMulLighting);		//Shader asociado
*/
	// Modelos
	// Cat
	catModelAnimate.loadModel("../models/cat/cat.fbx");
	catModelAnimate.setShader(&shaderMulLighting);

	scene1.loadModel("../models/scenes/bed.obj");
	scene1.setShader(&shaderMulLighting);

	// Terreno
	/*terrain.init();
	terrain.setShader(&shaderTerrain);*/

	//camera->setPosition(glm::vec3(0.0, 3.0, 20.0));
	camera->setSensitivity(2.0);						// Establecer sensibilidad
	camera->setDistanceFromTarget(distanceFromPlayer);	// Establecer distancia

	// Carga de texturas para el skybox
	Texture skyboxTexture = Texture("");
	glGenTextures(1, &skyboxTextureID);
	// Tipo de textura CUBE MAP
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextureID);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	for (int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(types); i++) {
		skyboxTexture = Texture(fileNames[i]);
		skyboxTexture.loadImage(true);
		if (skyboxTexture.getData()) {
			glTexImage2D(types[i], 0, skyboxTexture.getChannels() == 3 ? GL_RGB : GL_RGBA, skyboxTexture.getWidth(), skyboxTexture.getHeight(), 0,
			skyboxTexture.getChannels() == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, skyboxTexture.getData());
		} else
			std::cout << "Failed to load texture" << std::endl;
		skyboxTexture.freeImage();
	}

	Texture textureCesped("../Textures/grassy2.png");	// Definiendo la textura a utilizar
	textureCesped.loadImage(); 							// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	glGenTextures(1, &textureCespedID); 				// Creando la textura con id 1
	glBindTexture(GL_TEXTURE_2D, textureCespedID);		// Enlazar esa textura a una tipo de textura de 2D.

	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (textureCesped.getData()) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		//std::cout << "Numero de canales :=> " << textureCesped.getChannels() << std::endl;
		glTexImage2D(GL_TEXTURE_2D, 0, textureCesped.getChannels() == 3 ? GL_RGB : GL_RGBA, textureCesped.getWidth(), textureCesped.getHeight(), 0,
		textureCesped.getChannels() == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, textureCesped.getData());
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureCesped.freeImage();

	
	Texture textureWall("../Textures/whiteWall.jpg");	// Definiendo la textura a utilizar
	textureWall.loadImage(); 							// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	glGenTextures(1, &textureWallID); 					// Creando la textura con id 1
	glBindTexture(GL_TEXTURE_2D, textureWallID); 		// Enlazar esa textura a una tipo de textura de 2D.

	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (textureWall.getData()) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, textureWall.getChannels() == 3 ? GL_RGB : GL_RGBA, textureWall.getWidth(), textureWall.getHeight(), 0,
		textureWall.getChannels() == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, textureWall.getData());
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureWall.freeImage();

	Texture textureWindow("../Textures/ventana.png"); 	// Definiendo la textura a utilizar
	textureWindow.loadImage(); 							// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	glGenTextures(1, &textureWindowID); 				// Creando la textura con id 1
	glBindTexture(GL_TEXTURE_2D, textureWindowID); 		// Enlazar esa textura a una tipo de textura de 2D.

	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (textureWindow.getData()) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, textureWindow.getChannels() == 3 ? GL_RGB : GL_RGBA, textureWindow.getWidth(), textureWindow.getHeight(), 0,
		textureWindow.getChannels() == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, textureWindow.getData());
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureWindow.freeImage();

	Texture textureHighway("../Textures/highway.jpg"); 	// Definiendo la textura a utilizar
	textureHighway.loadImage(); 						// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	glGenTextures(1, &textureHighwayID); 				// Creando la textura con id 1
	glBindTexture(GL_TEXTURE_2D, textureHighwayID); 	// Enlazar esa textura a una tipo de textura de 2D.
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (textureHighway.getData()) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, textureHighway.getChannels() == 3 ? GL_RGB : GL_RGBA, textureHighway.getWidth(), textureHighway.getHeight(), 0,
		textureHighway.getChannels() == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, textureHighway.getData());
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureHighway.freeImage();

	// Definiendo la textura
	Texture textureLandingPad("../Textures/landingPad.jpg");				// Seleccionar textura
	textureLandingPad.loadImage();											// Cargar la textura
	glGenTextures(1, &textureLandingPadID); 								// Creando el id de la textura del landingPad
	glBindTexture(GL_TEXTURE_2D, textureLandingPadID);						// Se enlaza la textura
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// Wrapping en el eje u		// REPEAT: Repetir textura
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);	// Wrapping en el eje v		// CLAMP: Estirar borde hasta la orilla
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);		// Filtering de minimización	// Cuando la textura se aleja	// Linear:
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);		// Filtering de maximimizacion	// Cuando la textura está cerca	   Suaviza la transición de los píxeles
	if(textureLandingPad.getData()){
		// Transferir los datos de la imagen a la tarjeta
		glTexImage2D(GL_TEXTURE_2D, 0, textureLandingPad.getChannels() == 3 ? GL_RGB : GL_RGBA, 
			textureLandingPad.getWidth(), textureLandingPad.getHeight(), 0,
			textureLandingPad.getChannels() == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, 
			textureLandingPad.getData());
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else 
		std::cout << "Fallo la carga de textura" << std::endl;
	textureLandingPad.freeImage(); // Liberamos memoria
}

void destroy() {
	glfwDestroyWindow(window);
	glfwTerminate();
	// --------- IMPORTANTE ----------
	// Eliminar los shader y buffers creados.

	// Shaders Delete
	shader.destroy();
	shaderMulLighting.destroy();
	shaderSkybox.destroy();
	//shaderTerrain.destroy();

	// Basic objects Delete
	skyboxSphere.destroy();
	boxCesped.destroy();
	boxWalls.destroy();
	boxHighway.destroy();
	boxLandingPad.destroy();
	//esfera1.destroy();			// Liberar memoria de la tarjeta gráfica

	// Custom objects Delete
	catModelAnimate.destroy();
	scene1.destroy();

	// Terrains objects Delete
	//terrain.destroy();

	// Textures Delete
	glBindTexture(GL_TEXTURE_2D, 0);
	glDeleteTextures(1, &textureCespedID);
	glDeleteTextures(1, &textureWallID);
	glDeleteTextures(1, &textureWindowID);
	glDeleteTextures(1, &textureHighwayID);
	glDeleteTextures(1, &textureLandingPadID);

	// Cube Maps Delete
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glDeleteTextures(1, &skyboxTextureID);
}

void reshapeCallback(GLFWwindow *Window, int widthRes, int heightRes) {
	screenWidth = widthRes;
	screenHeight = heightRes;
	glViewport(0, 0, widthRes, heightRes);
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action,
		int mode) {
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_ESCAPE:
			exitApp = true;
			break;
		}
	}
}

void mouseCallback(GLFWwindow *window, double xpos, double ypos) {
	offsetX = xpos - lastMousePosX;
	offsetY = ypos - lastMousePosY;
	lastMousePosX = xpos;
	lastMousePosY = ypos;
}

void mouseButtonCallback(GLFWwindow *window, int button, int state, int mod) {
	if (state == GLFW_PRESS) {
		switch (button) {
		case GLFW_MOUSE_BUTTON_RIGHT:
			std::cout << "lastMousePos.y:" << lastMousePosY << std::endl;
			break;
		case GLFW_MOUSE_BUTTON_LEFT:
			std::cout << "lastMousePos.x:" << lastMousePosX << std::endl;
			break;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			std::cout << "lastMousePos.x:" << lastMousePosX << std::endl;
			std::cout << "lastMousePos.y:" << lastMousePosY << std::endl;
			break;
		}
	}
}

void mouseScrollCallback(GLFWwindow *window, double xscrollOffset, double yscrollOffset){
	distanceFromPlayer -= yscrollOffset;
	camera->setDistanceFromTarget(distanceFromPlayer);
}

bool processInput(bool continueApplication) {
	if (exitApp || glfwWindowShouldClose(window) != 0) {
		return false;
	}

	if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		camera->mouseMoveCamera(offsetX, 0.0, deltaTime);
	if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		camera->mouseMoveCamera(0.0, offsetY, deltaTime);
	
	int numerodeBotones;
	const unsigned char * botones = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &numerodeBotones);
	//std::cout << "Numero de botones: " << numerodeBotones << std::endl;

	// Control por mando
	if(glfwJoystickPresent(GLFW_JOYSTICK_1) == GLFW_TRUE){		// Detecta si hay un mando conectado
		//std::cout << "Está presente el joystick" << std::endl;
		int axesCount;
		const float * axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);
		//std::cout << "El numero de axes es: " << axesCount << std::endl;		// Hay 6 ejes: ejes v y h de los joysticks y los gatillos
		//std::cout << "El eje x del joystick izquierdo: " << axes[0]<< std::endl;
		//std::cout << "El eje y del joystick izquierdo: " << axes[1]<< std::endl;
		//std::cout << "El eje x del joystick derecho: " << axes[2]<< std::endl;
		//std::cout << "El eje y del joystick derecho: " << axes[3]<< std::endl;
		//std::cout << "El gatillo del joystick izquierdo: " << axes[4]<< std::endl;
		//std::cout << "El gatillo del joystick derecho: " << axes[5]<< std::endl;
		// Joystick izquierdo
		if(fabs(axes[0]) > 0.2 && catOnGround){
			modelMatrixCat = glm::rotate(modelMatrixCat, /*0.02f*/ -axes[0] * 0.04f, glm::vec3(0, 1, 0));
			animationCatIndex=1;
		}
		if(fabs(axes[1]) > 0.2){
			modelMatrixCat = glm::translate(modelMatrixCat, glm::vec3(0, 0, -axes[1] * 0.04f));
			animationCatIndex=1;
			if(botones[2] == GLFW_PRESS){	// Correr con X
			modelMatrixCat = glm::translate(modelMatrixCat, glm::vec3(0.0, 0.0, 0.04f));
		}
		}
		// Joystick derecho
		if(fabs(axes[2]) > 0.2){
			camera->mouseMoveCamera(axes[2], 0, deltaTime);
		}
		if(fabs(axes[3]) > 0.2){
			camera->mouseMoveCamera(0, axes[3], deltaTime);
		}
		// Botones
		if(botones[0] == GLFW_PRESS && catOnGround){		// Saltar con botón A
			catVelY = JUMP_FORCE;	// Aplicar impulso hacia arriba
			catOnGround = false;	// Deja el suelo al saltar
		}
	}

	/*if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera->moveFrontCamera(true, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera->moveFrontCamera(false, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera->moveRightCamera(false, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera->moveRightCamera(true, deltaTime);
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		camera->mouseMoveCamera(offsetX, offsetY, deltaTime);*/
	offsetX = 0;
	offsetY = 0;

	// Seleccionar modelo
	if (enableCountSelected && glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS){
		enableCountSelected = false;
		modelSelected++;
		if(modelSelected > 2)
			modelSelected = 0;
		
		std::cout << "modelSelected:" << modelSelected << std::endl;
	}
	else if(glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE)
		enableCountSelected = true;

	// Guardar key frames
	if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS
			&& glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS){
		record = true;
		if(myfile.is_open())
			myfile.close();
		myfile.open(fileName);
	}
	if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE
			&& glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS){
		record = false;
		myfile.close();
		
	}
	if(availableSave && glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS){
		saveFrame = true;
		availableSave = false;
	}if(glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_RELEASE)
		availableSave = true;

	// Controles de cat
	if (modelSelected == 0 && glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && catOnGround){
		modelMatrixCat = glm::rotate(modelMatrixCat, 0.05f, glm::vec3(0, 1, 0));
		animationCatIndex = 1;
	} else if (modelSelected == 0 && glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && catOnGround){
		modelMatrixCat = glm::rotate(modelMatrixCat, -0.05f, glm::vec3(0, 1, 0));
		animationCatIndex = 1;
	}
	if (modelSelected == 0 && glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
		modelMatrixCat = glm::translate(modelMatrixCat, glm::vec3(0.0, 0.0, 0.03));
		animationCatIndex = 1;
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){	// Shift izquierdo para correr
			modelMatrixCat = glm::translate(modelMatrixCat, glm::vec3(0.0, 0.0, 0.02));
			animationCatIndex = 1;
		}
	}
	else if (modelSelected == 0 && glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
		modelMatrixCat = glm::translate(modelMatrixCat, glm::vec3(0.0, 0.0, -0.02));
		animationCatIndex = 1;
	}

	// Saltar: solo se puede si cat está apoyado en un collider
	bool keySpaceStatus = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
	if(keySpaceStatus && catOnGround){
		catVelY = JUMP_FORCE;	// Aplicar impulso hacia arriba
		catOnGround = false;	// Deja el suelo al saltar
	}

	glfwPollEvents();
	return continueApplication;
}

void applicationLoop() {
	bool psi = true;

	glm::vec3 target;

	modelMatrixCat = glm::translate(modelMatrixCat, glm::vec3(0.0f, 0.0f, 0.0f));
	modelMatrixScene1 = glm::translate(modelMatrixScene1, glm::vec3(0.0f, 0.0f, 0.0f));
	modelMatrixScene1 = glm::rotate(modelMatrixScene1, 0.0f, glm::vec3(0, 1, 0));

	lastTime = TimeManager::Instance().GetTime();

	while (psi) {
		currTime = TimeManager::Instance().GetTime();
		if(currTime - lastTime < 0.016666667){		//Frame rate (60 fps)
			glfwPollEvents();
			continue;
		}
		lastTime = currTime;
		TimeManager::Instance().CalculateFrameRate(true);
		deltaTime = TimeManager::Instance().DeltaTime;

		// Guardar posición anterior antes de procesar el input
		modelMatrixCatPrev = modelMatrixCat;

		psi = processInput(true);		//Detectar eventos

		std::map<std::string, bool> collisionDetection;

		// Variables donde se guardan las matrices de cada articulacion por 1 frame
		
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::perspective(glm::radians(45.0f),
				(float) screenWidth / (float) screenHeight, 0.01f, 100.0f);

		if(modelSelected == 0){
			target = glm::vec3(modelMatrixCat[3]) + glm::vec3(0.0f, 1.5f, 0.0f);
		}

		camera->setCameraTarget(target);
		camera->updateCamera();
		glm::mat4 view = camera->getViewMatrix();

		// Settea la matriz de vista y projection al shader con solo color
		shader.setMatrix4("projection", 1, false, glm::value_ptr(projection));
		shader.setMatrix4("view", 1, false, glm::value_ptr(view));

		// Settea la matriz de vista y projection al shader con skybox
		shaderSkybox.setMatrix4("projection", 1, false, glm::value_ptr(projection));
		shaderSkybox.setMatrix4("view", 1, false, glm::value_ptr(glm::mat4(glm::mat3(view))));

		// Settea la matriz de vista y projection al shader con multiples luces
		shaderMulLighting.setMatrix4("projection", 1, false, glm::value_ptr(projection));
		shaderMulLighting.setMatrix4("view", 1, false, glm::value_ptr(view));

		/*shaderTerrain.setMatrix4("projection", 1, false, glm::value_ptr(projection));
		shaderTerrain.setMatrix4("view", 1, false, glm::value_ptr(view));*/

		/*******************************************
		 * Propiedades Luz direccional
		 *******************************************/
		shaderMulLighting.setVectorFloat3("viewPos", glm::value_ptr(camera->getPosition()));
		shaderMulLighting.setVectorFloat3("directionalLight.light.ambient", glm::value_ptr(glm::vec3(0.3, 0.3, 0.3)));
		shaderMulLighting.setVectorFloat3("directionalLight.light.diffuse", glm::value_ptr(glm::vec3(0.7, 0.7, 0.7)));
		shaderMulLighting.setVectorFloat3("directionalLight.light.specular", glm::value_ptr(glm::vec3(0.9, 0.9, 0.9)));
		shaderMulLighting.setVectorFloat3("directionalLight.direction", glm::value_ptr(glm::vec3(-1.0, 0.0, 0.0)));

		/*shaderTerrain.setVectorFloat3("viewPos", glm::value_ptr(camera->getPosition()));
		shaderTerrain.setVectorFloat3("directionalLight.light.ambient", glm::value_ptr(glm::vec3(0.3, 0.3, 0.3)));
		shaderTerrain.setVectorFloat3("directionalLight.light.diffuse", glm::value_ptr(glm::vec3(0.7, 0.7, 0.7)));
		shaderTerrain.setVectorFloat3("directionalLight.light.specular", glm::value_ptr(glm::vec3(0.9, 0.9, 0.9)));
		shaderTerrain.setVectorFloat3("directionalLight.direction", glm::value_ptr(glm::vec3(-1.0, 0.0, 0.0)));*/

		/*******************************************
		 * Propiedades SpotLights
		 *******************************************/
		shaderMulLighting.setInt("spotLightCount", 0);
		//shaderTerrain.setInt("spotLightCount", 0);

		/*******************************************
		 * Propiedades PointLights
		 *******************************************/
		shaderMulLighting.setInt("pointLightCount", 0);
		//shaderTerrain.setInt("pointLightCount", 0);

		/*******************************************
		 * Terreno
		 *******************************************/
		/*glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureCespedID);
		shaderTerrain.setInt("backgroundTexture", 0);
		shaderTerrain.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(40, 40)));
		terrain.setPosition(glm::vec3(100, 0, 100));
		terrain.render();
		shaderTerrain.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(0, 0)));
		glActiveTexture(GL_TEXTURE0);*/

		/*******************************************
		 * Cesped
		 *******************************************/
		glm::mat4 modelCesped = glm::mat4(1.0);
		modelCesped = glm::translate(modelCesped, glm::vec3(0.0, 0.0, 0.0));
		modelCesped = glm::scale(modelCesped, glm::vec3(200.0, 0.001, 200.0));
		// Se activa la textura del agua
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureCespedID);
		shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(200, 200)));
		boxCesped.render(modelCesped);
		shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(0, 0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		/*******************************************ww
		 * Casa
		 *******************************************/
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureWallID);
		// Frontal wall
		boxWalls.setScale(glm::vec3(10.0, 5.0, 0.05));
		boxWalls.setPosition(glm::vec3(-10.0, 2.5, 0.0));
		boxWalls.setOrientation(glm::vec3(0.0, 0.0, 0.0));
		boxWalls.render();
		// Left wall
		boxWalls.setScale(glm::vec3(10.0, 5.0, 0.05));
		boxWalls.setPosition(glm::vec3(-15.0, 2.5, -5.0));
		boxWalls.setOrientation(glm::vec3(0.0, 90.0, 0.0));
		boxWalls.render();
		// Right walls
		// First wall
		boxWalls.setScale(glm::vec3(3.75, 5.0, 0.05));
		boxWalls.setPosition(glm::vec3(-5.0, 2.5, -1.875));
		boxWalls.setOrientation(glm::vec3(0.0, 90.0, 0.0));
		boxWalls.render();
		// Second wall
		boxWalls.setScale(glm::vec3(3.75, 5.0, 0.05));
		boxWalls.setPosition(glm::vec3(-5.0, 2.5, -8.125));
		boxWalls.setOrientation(glm::vec3(0.0, 90.0, 0.0));
		boxWalls.render();
		// Third wall
		boxWalls.setScale(glm::vec3(2.5, 1.25, 0.05));
		boxWalls.setPosition(glm::vec3(-5.0, 4.375, -5.0));
		boxWalls.setOrientation(glm::vec3(0.0, 90.0, 0.0));
		boxWalls.render();
		// Four wall
		boxWalls.setScale(glm::vec3(2.5, 1.25, 0.05));
		boxWalls.setPosition(glm::vec3(-5.0, 0.625, -5.0));
		boxWalls.setOrientation(glm::vec3(0.0, 90.0, 0.0));
		boxWalls.render();
		// Back wall
		boxWalls.setScale(glm::vec3(10.0, 5.0, 0.05));
		boxWalls.setPosition(glm::vec3(-10.0, 2.5, -10.0));
		boxWalls.setOrientation(glm::vec3(0.0, 0.0, 0.0));
		boxWalls.render();
		// Ceilling
		boxWalls.setScale(glm::vec3(12.0, 12.0, 0.05));
		boxWalls.setPosition(glm::vec3(-10.0, 5, -5.0));
		boxWalls.setOrientation(glm::vec3(90.0, 0.0, 0.0));
		boxWalls.render();
		// Window
		glBindTexture(GL_TEXTURE_2D, textureWindowID);
		boxWalls.setScale(glm::vec3(2.5, 2.5, 0.05));
		boxWalls.setPosition(glm::vec3(-5.0, 2.5, -5.0));
		boxWalls.setOrientation(glm::vec3(0.0, 90.0, 0.0));
		boxWalls.render();
		// Highway 0
		glBindTexture(GL_TEXTURE_2D, textureHighwayID);
		boxHighway.setScale(glm::vec3(40.0, 0.05, 10.0));
		boxHighway.setPosition(glm::vec3(0.0, 0.05, 10.0));
		boxHighway.setOrientation(glm::vec3(0.0, 0.0, 0.0));
		boxHighway.render();
		// Highway 1
		boxHighway.setScale(glm::vec3(80.0, 0.05, 10.0));
		boxHighway.setPosition(glm::vec3(25.0, 0.05, 0.0));
		boxHighway.setOrientation(glm::vec3(0.0, 90.0, 0.0));
		shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2, 1)));
		boxHighway.render();
		shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(0, 0)));
		// Highway 2
		boxHighway.setScale(glm::vec3(80.0, 0.05, 10.0));
		boxHighway.setPosition(glm::vec3(-25.0, 0.05, 0.0));
		boxHighway.setOrientation(glm::vec3(0.0, 90.0, 0.0));
		shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2, 1)));
		boxHighway.render();
		shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(0, 0)));
		// Highway 3
		boxHighway.setScale(glm::vec3(40.0, 0.05, 10.0));
		boxHighway.setPosition(glm::vec3(0.0, 0.05, -35.0));
		boxHighway.setOrientation(glm::vec3(0.0, 0.0, 0.0));
		boxHighway.render();

		/*******************************************
		 * Esfera 1
		*********************************************/
		/*glActiveTexture(GL_TEXTURE0);							//Indicar si tiene textura
		glBindTexture(GL_TEXTURE_2D, textureHighwayID);			//Textura para la esfera
		shaderMulLighting.setInt("texture1", 0);				//Unidad de textura
		esfera1.setScale(glm::vec3(3.0, 3.0, 3.0));				//Escalamiento de la esfera
		esfera1.setPosition(glm::vec3(3.0f, 2.0f, -15.0f));		//Posición de la esfera
		esfera1.render();										//Render de la esfera

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureWallID);
		shaderMulLighting.setInt("texture1", 0);
		esfera1.setScale(glm::vec3(7.0, 7.0, 7.0));
		esfera1.setPosition(glm::vec3(3.0f, 2.0f, 10.0f));		//Posición de la esfera2
		esfera1.enableWireMode();								//Visualizar la malla del modelo
		esfera1.render();
		esfera1.enableFillMode();

		glActiveTexture(GL_TEXTURE0);							
		glBindTexture(GL_TEXTURE_2D, textureWindowID);			
		shaderMulLighting.setInt("texture1", 0);				
		esfera1.setScale(glm::vec3(1.0, 2.0, 3.0));			
		esfera1.setPosition(glm::vec3(0.0f, 6.0f, -5.0f));		
		esfera1.render();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureWallID);
		shaderMulLighting.setInt("texture1", 0);
		esfera1.setScale(glm::vec3(2.0, 2.0, 2.0));
		esfera1.setPosition(glm::vec3(3.0f, 2.0f, 5.0f));
		esfera1.enableWireMode();
		esfera1.render();
		esfera1.enableFillMode();*/

		/******************************************
		 * Landing pad
		*******************************************/
		boxLandingPad.setScale(glm::vec3(10.0f, 0.05f, 10.0f));
		boxLandingPad.setPosition(glm::vec3(15.0f, 0.05f, -15.0f));
		boxLandingPad.setOrientation(glm::vec3(0.0f, 0.0f, 0.0f));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureLandingPadID);
		shaderMulLighting.setInt("texture1", 0);
		shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(3.0, 4.0)));	// Wrapping eje u y eje v
		boxLandingPad.render();
		shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(1.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		/*******************************************
		 * Custom objects obj
		 *******************************************/
		glm::mat4 modelMatrixScene1Body = glm::mat4(modelMatrixScene1);
		//modelMatrixScene1[3].y = terrain.getHeightTerrain(modelMatrixScene1[3].x, modelMatrixScene1[3].z);
		modelMatrixScene1Body = glm::translate(modelMatrixScene1Body, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixScene1Body = glm::scale(modelMatrixScene1Body, glm::vec3(0.02));
		scene1.render(modelMatrixScene1Body);
		
		/*******************************************
		* Objetos animados por huesos
		*******************************************/
		// Gravedad continua: siempre se acumula velocidad hacia abajo
		// catOnGround se resolverá en el bloque de colisiones al final del frame.
		// Aplicamos la velocidad vertical al modelo (deltaTime en segundos)
		catVelY -= GRAVITY * (float)deltaTime;
		modelMatrixCat[3].y += catVelY * (float)deltaTime;

		// Modelo real
		glm::mat4 modelMatrixCatBody = glm::mat4(modelMatrixCat);
		modelMatrixCatBody = glm::scale(modelMatrixCatBody, glm::vec3(0.0002));
		catModelAnimate.setAnimationIndex(animationCatIndex);									// Ligar animación
		catModelAnimate.render(modelMatrixCatBody);
		animationCatIndex = 0;																// Vuelve a idle si no hay input

		/*******************************************
		 * Skybox
		 *******************************************/
		GLint oldCullFaceMode;
		GLint oldDepthFuncMode;
		// deshabilita el modo del recorte de caras ocultas para ver las esfera desde adentro
		glGetIntegerv(GL_CULL_FACE_MODE, &oldCullFaceMode);
		glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFuncMode);
		shaderSkybox.setFloat("skybox", 0);
		glCullFace(GL_FRONT);
		glDepthFunc(GL_LEQUAL);
		glActiveTexture(GL_TEXTURE0);
		skyboxSphere.render();
		glCullFace(oldCullFaceMode);
		glDepthFunc(oldDepthFuncMode);

		/*******************************************
		 * Colliders
		 *******************************************/
		// Collider del suelo — se construye con los mismos valores del render
		// El céspede está en (0,0,0) escalado a (200, 0.001, 200).
		// El OBB unit del boxCesped tiene half-extents (0.5, 0.5, 0.5),
		// así que los half-extents reales son escala * 0.5 para cada eje.
		AbstractModel::OBB colliderFloor;
		colliderFloor.u  = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);   // Sin rotación
		colliderFloor.c  = glm::vec3(0.0f, 0.0f, 0.0f);          // Centro del box renderizado
		colliderFloor.e  = glm::vec3(100.0f, 0.0005f, 100.0f);   // 200*0.5, 0.001*0.5, 200*0.5
		glm::mat4 modelColliderFloor = glm::translate(glm::mat4(1.0f), colliderFloor.c);
		addOrUpdateColliders(collidersOBB, "floor", colliderFloor, modelColliderFloor);

		// Collider del gato
		glm::mat4 modelColliderCat = glm::mat4(modelMatrixCat);
		AbstractModel::OBB colliderCat;
		colliderCat.u = glm::quat_cast(modelColliderCat);
		modelColliderCat = glm::scale(modelColliderCat, glm::vec3(0.02));
		modelColliderCat = glm::translate(modelColliderCat, catModelAnimate.getObb().c);	// Punto central de la caja
		colliderCat.c = modelColliderCat[3];
		colliderCat.e = catModelAnimate.getObb().e * glm::vec3(0.02);
		addOrUpdateColliders(collidersOBB, "cat", colliderCat, modelMatrixCat);

		glm::mat4 modelColliderScene1 = glm::mat4(modelMatrixScene1);			// Modelo de la nave
		AbstractModel::OBB colliderScene1;										// Collider de la nave
		colliderScene1.u = glm::quat_cast(modelColliderScene1);					// Mat4x4 a quaternion
		modelColliderScene1 = glm::scale(modelColliderScene1, glm::vec3(0.02)); // Colocar el escalamiento que tenía el objeto
		modelColliderScene1 = glm::translate(modelMatrixScene1Body, scene1.getObb().c); 	// Mover al centro de la caja
		//modelColliderScene1 = glm::rotate(modelColliderScene1, 45.0f, glm::vec3(0, 1, 0));
		colliderScene1.c = modelColliderScene1[3];
		colliderScene1.e = scene1.getObb().e * glm::vec3(0.02);	// Escalamiento de 1
		addOrUpdateColliders(collidersOBB, "scene1", colliderScene1, modelColliderScene1);

		// Render de los colliders
		for(auto it = collidersSBB.begin(); it != collidersSBB.end(); it++){
			glm::mat4 matrixCollider = glm::translate(glm::mat4(1.0), std::get<0>(it->second).c);
			matrixCollider = glm::scale(matrixCollider, glm::vec3(std::get<0>(it->second).ratio*2));
			sphereCollider.setColor(glm::vec4(1, 1, 1, 1));
			sphereCollider.enableWireMode();
			sphereCollider.render(matrixCollider);
		};
		for(auto it = collidersOBB.begin(); it != collidersOBB.end(); it++){
			glm::mat4 matrixCollider = glm::translate(glm::mat4(1.0), std::get<0>(it->second).c);
			matrixCollider = matrixCollider * glm::mat4(std::get<0>(it->second).u);	// Rotación
			matrixCollider = glm::scale(matrixCollider, std::get<0>(it->second).e * 2.0f);
			boxCollider.setColor(glm::vec4(1));
			boxCollider.enableWireMode();
			boxCollider.render(matrixCollider);
		};

		// Pruebas de colisión
		// catOnGround se resetea cada frame; el bloque de resolución lo activa
		// si detecta que cat está apoyado encima de otro collider.
		catOnGround = false;
		for(auto it = collidersOBB.begin(); it != collidersOBB.end(); it++){
			bool isCollition = false;
			for(auto jt = collidersOBB.begin(); jt != collidersOBB.end(); jt++){
				if(it != jt && testOBBOBB(std::get<0>(it->second), std::get<0>(jt->second))){
					isCollition = true;
				};
			};
			addOrUpdateCollisionDetection(collisionDetection, it->first, isCollition);

			// Resolución de colisión para cat
			if(it->first == "cat" && isCollition){
				AbstractModel::OBB& catOBB = std::get<0>(it->second);
				bool resolvedAsTop = false;

				for(auto jt = collidersOBB.begin(); jt != collidersOBB.end(); jt++){
					if(jt->first == "cat") continue;
					if(!testOBBOBB(catOBB, std::get<0>(jt->second))) continue;

					AbstractModel::OBB& otherOBB = std::get<0>(jt->second);

					// Calcular penetración (overlap) en cada eje (aproximación AABB)
					float overlapX = (catOBB.e.x + otherOBB.e.x) - glm::abs(catOBB.c.x - otherOBB.c.x);
					float overlapY = (catOBB.e.y + otherOBB.e.y) - glm::abs(catOBB.c.y - otherOBB.c.y);
					float overlapZ = (catOBB.e.z + otherOBB.e.z) - glm::abs(catOBB.c.z - otherOBB.c.z);

					// Sin penetración real en algún eje → ignorar
					if(overlapX <= 0.0f || overlapY <= 0.0f || overlapZ <= 0.0f) continue;

					// El eje con MENOR penetración determina el tipo de colisión (SAT)
					if(overlapY < overlapX && overlapY < overlapZ){
						// --- Colisión VERTICAL (Y es el eje de menor penetración) ---
						if(catOBB.c.y > otherOBB.c.y){
							// Cat viene de arriba: empujar hacia arriba
							modelMatrixCat[3].y += overlapY;
							catVelY     = 0.0f;
							catOnGround = true;
							resolvedAsTop = true;
						} else {
							// Cat golpea desde abajo: empujar hacia abajo
							modelMatrixCat[3].y -= overlapY;
							if(catVelY > 0.0f) catVelY = 0.0f;
						}
					}
					// Si X o Z es el eje menor → colisión LATERAL
					// No se toca Y; se marca para revertir XZ abajo
				}

				// --- Colisión lateral: revertir solo X y Z, sin modificar Y ---
				if(!resolvedAsTop){
					modelMatrixCat[3].x = modelMatrixCatPrev[3].x;
					modelMatrixCat[3].z = modelMatrixCatPrev[3].z;
				}
			}
		};
		
		glfwSwapBuffers(window);
	}
}

int main(int argc, char **argv) {
	init(800, 600, "Proyecto Final", false);		//Configuración de la ventana
	applicationLoop();							//Método para mantener la ventana abierta
	destroy();
	return 1;
}