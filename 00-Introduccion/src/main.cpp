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
Shader shaderTerrain;

std::shared_ptr<Camera> camera(new ThirdPersonCamera());
float distanceFromPlayer = 6.0f;		// Distancia inicial de la cámara

Sphere skyboxSphere(20, 20);

Box boxCollider;
Sphere sphereCollider(10, 10);
//Cylinder cylinderCollider(10, 10, 1, 1, 1);	// rebanadas, tapas, radio1, radio2, altura

// Models complex instances
// Casa 1
Model casa1Pared1a;
Model casa1Pared1b;
Model casa1Pared1c;
Model casa1Puerta1;
Model casa1Techo1a;
Model casa1Escalera;
Model casa1Pared2a;
Model casa1Pared2b;
Model casa1Pared2c;
Model casa1Puerta2;
Model casa1Ventana1;
Model casa1Ventana2;
Model casa1Techo2a;
Model casa1Techo2b;
Model casa1Techo2c;
Model casa1Pared3a;
Model casa1Pared3b;
Model casa1Pared3c;
Model casa1Puerta3;
Model casa1Techo3a;
Model casa1Door1;
Model casa1Pared4a;
Model casa1Pared4b;
Model casa1Pared4c;
Model casa1Puerta4;
Model casa1Pared5a;
Model casa1Pared6a;
Model casa1TechoA;
Model casa1TechoB;
Model casa1ParedA;
Model casa1ParedB;
Model casa1PuertaA;
Model casa1Door2;
Model casa1Viga1;
Model casa1Viga2;
Model casa1Viga3;
Model casa1Viga4;
Model casa1Viga5a;
Model casa1Viga5b;
Model casa1Viga6;
Model casa1Pilar1;
Model casa1Pilar2;
Model casa1Pilar3;
Model borde1A;
Model borde2A;
Model borde3A;
Model borde4A;
Model borde5A;
Model borde6A;
Model obj1a;
Model obj1b;
Model obj2a;
Model obj2b;
Model obj3a;
Model techoA;
Model techoB;
Model base;
Model puente;
Model techo;
Model bordes;
Model torrePared1;
Model torrePared2;
Model torrePared3;
Model torreEntrada;
Model torrePuerta;
// Casa 2
Model casa2Entrada;
Model casa2ParedA;
Model casa2ParedB;
Model casa2ParedC;
Model casa2ParedD;
Model casa2ParedE;
Model casa2ParedF;
Model casa2ParedG;
Model casa2ParedH;
Model casa2ParedI;
Model casa2TechoA;
Model casa2TechoB;
Model casa2TechoC;
Model casa2VentanaA;
Model casa2VentanaB;
Model casa2VentanaC;
Model casa2VentanaD;
// Casa 3
Model casa3BaseA;
Model casa3BaseB;
Model casa3TechoA;
Model casa3TechoB;
Model casa3VentanaA;
Model casa3PuertaA;
Model casa3PuertaB;
Model casa3PuertaC;

// Modelos animados
// Cat
Model catModelAnimate;
bool stopJump = false;

// Objetos
Model caja;
std::vector<std::pair<glm::vec3, float>> boxPositions = {
	{glm::vec3(32.7, 0, 34.3), 0.0},
	{glm::vec3(32.7, 0, 32.5), 0.0},
	{glm::vec3(32.7, 1.6, 32.9), 0.0},
	{glm::vec3(20.0, 0, 32.8), 0.0},
};
Model key;
Model gem;
std::vector<std::pair<glm::vec3, float>> gemPositions = {
	{glm::vec3(0.0, 0, 5.0), 0.0},
	{glm::vec3(0.0, 0, 10.0), 0.0},
	{glm::vec3(0.0, 0, 15.0), 0.0},
	{glm::vec3(0.0, 0, 20.0), 0.0},
};
Model bigTree;

// Terrain model instance
Terrain terrain(-1,-1,200,32, "../Textures/heightmap.png");

// Texturas
GLuint textureCespedID, textureWallID, textureWindowID, textureHighwayID, textureLandingPadID;
GLuint skyboxTextureID;
GLuint textureRID,textureGID,textureBID,textureBlendMapID;

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

// Casa 1
glm::mat4 modelMatrixCasa1Pared1a = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa1Pared1b = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa1Pared1c = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa1Puerta1 = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa1Techo1a = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa1Escalera = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa1Pared2a = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa1Pared2b = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa1Pared2c = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa1Puerta2 = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa1Ventana1 = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa1Ventana2 = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa1Techo2a = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa1Techo2b = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa1Techo2c = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa1Pared3a = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa1Pared3b = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa1Pared3c = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa1Puerta3 = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa1Techo3a = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa1Door1 = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa1Pared4a = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa1Pared4b = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa1Pared4c = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa1Puerta4 = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa1Pared5a = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa1Pared6a = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa1TechoA = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa1TechoB = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa1ParedA = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa1ParedB = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa1PuertaA = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa1Door2 = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa1Viga1 = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa1Viga2 = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa1Viga3 = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa1Viga4 = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa1Viga5a = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa1Viga5b = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa1Viga6 = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa1Pilar1 = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa1Pilar2 = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa1Pilar3 = glm::mat4(1.0f);
glm::mat4 modelMatrixBorde1A = glm::mat4(1.0f);
glm::mat4 modelMatrixBorde2A = glm::mat4(1.0f);
glm::mat4 modelMatrixBorde3A = glm::mat4(1.0f);
glm::mat4 modelMatrixBorde4A = glm::mat4(1.0f);
glm::mat4 modelMatrixBorde5A = glm::mat4(1.0f);
glm::mat4 modelMatrixBorde6A = glm::mat4(1.0f);
glm::mat4 modelMatrixObj1a = glm::mat4(1.0f);
glm::mat4 modelMatrixObj1b = glm::mat4(1.0f);
glm::mat4 modelMatrixObj2a = glm::mat4(1.0f);
glm::mat4 modelMatrixObj2b = glm::mat4(1.0f);
glm::mat4 modelMatrixObj3a = glm::mat4(1.0f);
glm::mat4 modelMatrixTechoA = glm::mat4(1.0f);
glm::mat4 modelMatrixTechoB = glm::mat4(1.0f);
glm::mat4 modelMatrixBase = glm::mat4(1.0f);
glm::mat4 modelMatrixPuente = glm::mat4(1.0f);
glm::mat4 modelMatrixTecho = glm::mat4(1.0f);
glm::mat4 modelMatrixBordes = glm::mat4(1.0f);
glm::mat4 modelMatrixTorrePared1 = glm::mat4(1.0f);
glm::mat4 modelMatrixTorrePared2 = glm::mat4(1.0f);
glm::mat4 modelMatrixTorrePared3 = glm::mat4(1.0f);
glm::mat4 modelMatrixTorreEntrada = glm::mat4(1.0f);
glm::mat4 modelMatrixTorrePuerta = glm::mat4(1.0f);
// Casa 2
glm::mat4 modelMatrixCasa2Entrada = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa2ParedA = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa2ParedB = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa2ParedC = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa2ParedD = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa2ParedE = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa2ParedF = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa2ParedG = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa2ParedH = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa2ParedI = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa2TechoA = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa2TechoB = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa2TechoC = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa2VentanaA = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa2VentanaB = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa2VentanaC = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa2VentanaD = glm::mat4(1.0f);
// Casa 3
glm::mat4 modelMatrixCasa3BaseA = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa3BaseB = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa3TechoA = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa3TechoB = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa3VentanaA = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa3PuertaA = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa3PuertaB = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa3PuertaC = glm::mat4(1.0f);
// Objetos
glm::mat4 modelMatrixCaja = glm::mat4(1.0f);
glm::mat4 modelMatrixCasa1Key = glm::mat4(1.0f);
glm::mat4 modelMatrixGem = glm::mat4(1.0f);
glm::mat4 modelMatrixBigTree = glm::mat4(1.0f);

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
	shaderTerrain.initialize("../Shaders/terrain.vs", "../Shaders/terrain.fs");

	// Inicializacion de los objetos.
	skyboxSphere.init();
	skyboxSphere.setShader(&shaderSkybox);
	skyboxSphere.setScale(glm::vec3(20.0f, 20.0f, 20.0f));

	sphereCollider.init();
	sphereCollider.setShader(&shader);		// Shader sin textura
	boxCollider.init();
	boxCollider.setShader(&shader);
	//cylinderCollider.init();
	//cylinderCollider.setShader(&shader);

	// Modelos
	// Cat
	catModelAnimate.loadModel("../models/cat/cat3.fbx");
	catModelAnimate.setShader(&shaderMulLighting);

	// Casa1
	casa1Pared1a.loadModel("../models/scenes/casa1/paredes/pared1/pared1a.obj");
	casa1Pared1a.setShader(&shaderMulLighting);
	casa1Pared1b.loadModel("../models/scenes/casa1/paredes/pared1/pared1b.obj");
	casa1Pared1b.setShader(&shaderMulLighting);
	casa1Pared1c.loadModel("../models/scenes/casa1/paredes/pared1/pared1c.obj");
	casa1Pared1c.setShader(&shaderMulLighting);
	casa1Puerta1.loadModel("../models/scenes/casa1/paredes/pared1/puerta1.obj");
	casa1Puerta1.setShader(&shaderMulLighting);
	casa1Escalera.loadModel("../models/scenes/casa1/paredes/pared1/escalera.obj");
	casa1Escalera.setShader(&shaderMulLighting);
	casa1Techo1a.loadModel("../models/scenes/casa1/paredes/pared1/techo1.obj");
	casa1Techo1a.setShader(&shaderMulLighting);
	casa1Pared2a.loadModel("../models/scenes/casa1/paredes/pared2/pared2a.obj");
	casa1Pared2a.setShader(&shaderMulLighting);
	casa1Pared2b.loadModel("../models/scenes/casa1/paredes/pared2/pared2b.obj");
	casa1Pared2b.setShader(&shaderMulLighting);
	casa1Pared2c.loadModel("../models/scenes/casa1/paredes/pared2/pared2c.obj");
	casa1Pared2c.setShader(&shaderMulLighting);
	casa1Puerta2.loadModel("../models/scenes/casa1/paredes/pared2/puerta2.obj");
	casa1Puerta2.setShader(&shaderMulLighting);
	casa1Techo2a.loadModel("../models/scenes/casa1/paredes/pared2/techo1.obj");
	casa1Techo2a.setShader(&shaderMulLighting);
	casa1Techo2b.loadModel("../models/scenes/casa1/paredes/pared2/techo2.obj");
	casa1Techo2b.setShader(&shaderMulLighting);
	casa1Techo2c.loadModel("../models/scenes/casa1/paredes/pared2/techo3.obj");
	casa1Techo2c.setShader(&shaderMulLighting);
	casa1Ventana1.loadModel("../models/scenes/casa1/paredes/pared2/ventana1.obj");
	casa1Ventana1.setShader(&shaderMulLighting);
	casa1Ventana2.loadModel("../models/scenes/casa1/paredes/pared2/ventana2.obj");
	casa1Ventana2.setShader(&shaderMulLighting);
	casa1Pared3a.loadModel("../models/scenes/casa1/paredes/pared3/pared3a.obj");
	casa1Pared3a.setShader(&shaderMulLighting);
	casa1Pared3b.loadModel("../models/scenes/casa1/paredes/pared3/pared3b.obj");
	casa1Pared3b.setShader(&shaderMulLighting);
	casa1Pared3c.loadModel("../models/scenes/casa1/paredes/pared3/pared3c.obj");
	casa1Pared3c.setShader(&shaderMulLighting);
	casa1Puerta3.loadModel("../models/scenes/casa1/paredes/pared3/puerta3.obj");
	casa1Puerta3.setShader(&shaderMulLighting);
	casa1Techo3a.loadModel("../models/scenes/casa1/paredes/pared3/techo3a.obj");
	casa1Techo3a.setShader(&shaderMulLighting);
	casa1Door1.loadModel("../models/scenes/casa1/paredes/pared3/door1.obj");
	casa1Door1.setShader(&shaderMulLighting);
	casa1Pared4a.loadModel("../models/scenes/casa1/paredes/pared4/pared4a.obj");
	casa1Pared4a.setShader(&shaderMulLighting);
	casa1Pared4b.loadModel("../models/scenes/casa1/paredes/pared4/pared4b.obj");
	casa1Pared4b.setShader(&shaderMulLighting);
	casa1Pared4c.loadModel("../models/scenes/casa1/paredes/pared4/pared4c.obj");
	casa1Pared4c.setShader(&shaderMulLighting);
	casa1Puerta4.loadModel("../models/scenes/casa1/paredes/pared4/puerta4.obj");
	casa1Puerta4.setShader(&shaderMulLighting);
	casa1Pared5a.loadModel("../models/scenes/casa1/paredes/paredExtra/pared5a.obj");
	casa1Pared5a.setShader(&shaderMulLighting);
	casa1Pared6a.loadModel("../models/scenes/casa1/paredes/paredExtra/pared6a.obj");
	casa1Pared6a.setShader(&shaderMulLighting);
	casa1ParedA.loadModel("../models/scenes/casa1/interior/paredA.obj");
	casa1ParedA.setShader(&shaderMulLighting);
	casa1ParedB.loadModel("../models/scenes/casa1/interior/paredB.obj");
	casa1ParedB.setShader(&shaderMulLighting);
	casa1TechoA.loadModel("../models/scenes/casa1/interior/techoA.obj");
	casa1TechoA.setShader(&shaderMulLighting);
	casa1TechoB.loadModel("../models/scenes/casa1/interior/techoB.obj");
	casa1TechoB.setShader(&shaderMulLighting);
	casa1PuertaA.loadModel("../models/scenes/casa1/interior/puertaA.obj");
	casa1PuertaA.setShader(&shaderMulLighting);
	casa1Door2.loadModel("../models/scenes/casa1/interior/door2.obj");
	casa1Door2.setShader(&shaderMulLighting);
	casa1Pilar1.loadModel("../models/scenes/casa1/interior/pilar1.obj");
	casa1Pilar1.setShader(&shaderMulLighting);
	casa1Pilar2.loadModel("../models/scenes/casa1/interior/pilar2.obj");
	casa1Pilar2.setShader(&shaderMulLighting);
	casa1Pilar3.loadModel("../models/scenes/casa1/interior/pilar3.obj");
	casa1Pilar3.setShader(&shaderMulLighting);
	casa1Viga1.loadModel("../models/scenes/casa1/interior/viga1.obj");
	casa1Viga1.setShader(&shaderMulLighting);
	casa1Viga2.loadModel("../models/scenes/casa1/interior/viga2.obj");
	casa1Viga2.setShader(&shaderMulLighting);
	casa1Viga3.loadModel("../models/scenes/casa1/interior/viga3.obj");
	casa1Viga3.setShader(&shaderMulLighting);
	casa1Viga4.loadModel("../models/scenes/casa1/interior/viga4.obj");
	casa1Viga4.setShader(&shaderMulLighting);
	casa1Viga5a.loadModel("../models/scenes/casa1/interior/viga5a.obj");
	casa1Viga5a.setShader(&shaderMulLighting);
	casa1Viga5b.loadModel("../models/scenes/casa1/interior/viga5b.obj");
	casa1Viga5b.setShader(&shaderMulLighting);
	casa1Viga6.loadModel("../models/scenes/casa1/interior/viga6.obj");
	casa1Viga6.setShader(&shaderMulLighting);
	borde1A.loadModel("../models/scenes/casa1/techo/borde1A.obj");
	borde1A.setShader(&shaderMulLighting);
	borde2A.loadModel("../models/scenes/casa1/techo/borde2A.obj");
	borde2A.setShader(&shaderMulLighting);
	borde3A.loadModel("../models/scenes/casa1/techo/borde3A.obj");
	borde3A.setShader(&shaderMulLighting);
	borde4A.loadModel("../models/scenes/casa1/techo/borde4A.obj");
	borde4A.setShader(&shaderMulLighting);
	borde5A.loadModel("../models/scenes/casa1/techo/borde5A.obj");
	borde5A.setShader(&shaderMulLighting);
	borde6A.loadModel("../models/scenes/casa1/techo/borde6A.obj");
	borde6A.setShader(&shaderMulLighting);
	obj1a.loadModel("../models/scenes/casa1/techo/obj1a.obj");
	obj1a.setShader(&shaderMulLighting);
	obj1b.loadModel("../models/scenes/casa1/techo/obj1b.obj");
	obj1b.setShader(&shaderMulLighting);
	obj2a.loadModel("../models/scenes/casa1/techo/obj2a.obj");
	obj2a.setShader(&shaderMulLighting);
	obj2b.loadModel("../models/scenes/casa1/techo/obj2b.obj");
	obj2b.setShader(&shaderMulLighting);
	obj3a.loadModel("../models/scenes/casa1/techo/obj3a.obj");
	obj3a.setShader(&shaderMulLighting);
	techoA.loadModel("../models/scenes/casa1/techo/techoA.obj");
	techoA.setShader(&shaderMulLighting);
	techoB.loadModel("../models/scenes/casa1/techo/techoB.obj");
	techoB.setShader(&shaderMulLighting);
	base.loadModel("../models/scenes/casa1/torre/base.obj");
	base.setShader(&shaderMulLighting);
	puente.loadModel("../models/scenes/casa1/torre/puente.obj");
	puente.setShader(&shaderMulLighting);
	techo.loadModel("../models/scenes/casa1/torre/techo.obj");
	techo.setShader(&shaderMulLighting);
	bordes.loadModel("../models/scenes/casa1/torre/bordes.obj");
	bordes.setShader(&shaderMulLighting);
	torrePared1.loadModel("../models/scenes/casa1/torre/pared1.obj");
	torrePared1.setShader(&shaderMulLighting);
	torrePared2.loadModel("../models/scenes/casa1/torre/pared2.obj");
	torrePared2.setShader(&shaderMulLighting);
	torrePared3.loadModel("../models/scenes/casa1/torre/pared3.obj");
	torrePared3.setShader(&shaderMulLighting);
	torreEntrada.loadModel("../models/scenes/casa1/torre/entrada.obj");
	torreEntrada.setShader(&shaderMulLighting);
	torrePuerta.loadModel("../models/scenes/casa1/torre/puertaA.obj");
	torrePuerta.setShader(&shaderMulLighting);
	casa2Entrada.loadModel("../models/scenes/casa2/casa2entrada.obj");
	casa2Entrada.setShader(&shaderMulLighting);
	casa2ParedA.loadModel("../models/scenes/casa2/casa2paredA.obj");
	casa2ParedA.setShader(&shaderMulLighting);
	casa2ParedB.loadModel("../models/scenes/casa2/casa2paredB.obj");
	casa2ParedB.setShader(&shaderMulLighting);
	casa2ParedC.loadModel("../models/scenes/casa2/casa2paredC.obj");
	casa2ParedC.setShader(&shaderMulLighting);
	casa2ParedD.loadModel("../models/scenes/casa2/casa2paredD.obj");
	casa2ParedD.setShader(&shaderMulLighting);
	casa2ParedE.loadModel("../models/scenes/casa2/casa2paredE.obj");
	casa2ParedE.setShader(&shaderMulLighting);
	casa2ParedF.loadModel("../models/scenes/casa2/casa2paredF.obj");
	casa2ParedF.setShader(&shaderMulLighting);
	casa2ParedG.loadModel("../models/scenes/casa2/casa2paredG.obj");
	casa2ParedG.setShader(&shaderMulLighting);
	casa2ParedH.loadModel("../models/scenes/casa2/casa2paredH.obj");
	casa2ParedH.setShader(&shaderMulLighting);
	casa2ParedI.loadModel("../models/scenes/casa2/casa2paredI.obj");
	casa2ParedI.setShader(&shaderMulLighting);
	casa2TechoA.loadModel("../models/scenes/casa2/casa2techoA.obj");
	casa2TechoA.setShader(&shaderMulLighting);
	casa2TechoB.loadModel("../models/scenes/casa2/casa2techoB.obj");
	casa2TechoB.setShader(&shaderMulLighting);
	casa2TechoC.loadModel("../models/scenes/casa2/casa2techoC.obj");
	casa2TechoC.setShader(&shaderMulLighting);
	casa2VentanaA.loadModel("../models/scenes/casa2/casa2ventanaA.obj");
	casa2VentanaA.setShader(&shaderMulLighting);
	casa2VentanaB.loadModel("../models/scenes/casa2/casa2ventanaB.obj");
	casa2VentanaB.setShader(&shaderMulLighting);
	casa2VentanaC.loadModel("../models/scenes/casa2/casa2ventanaC.obj");
	casa2VentanaC.setShader(&shaderMulLighting);
	casa2VentanaD.loadModel("../models/scenes/casa2/casa2ventanaD.obj");
	casa2VentanaD.setShader(&shaderMulLighting);
	casa3BaseA.loadModel("../models/scenes/casa3/casa3baseA.obj");
	casa3BaseA.setShader(&shaderMulLighting);
	casa3BaseB.loadModel("../models/scenes/casa3/casa3baseB.obj");
	casa3BaseB.setShader(&shaderMulLighting);
	casa3TechoA.loadModel("../models/scenes/casa3/casa3techoA.obj");
	casa3TechoA.setShader(&shaderMulLighting);
	casa3TechoB.loadModel("../models/scenes/casa3/casa3techoB.obj");
	casa3TechoB.setShader(&shaderMulLighting);
	casa3VentanaA.loadModel("../models/scenes/casa3/casa3ventanaA.obj");
	casa3VentanaA.setShader(&shaderMulLighting);
	casa3PuertaA.loadModel("../models/scenes/casa3/casa3puertaA.obj");
	casa3PuertaA.setShader(&shaderMulLighting);
	casa3PuertaB.loadModel("../models/scenes/casa3/casa3puertaB.obj");
	casa3PuertaB.setShader(&shaderMulLighting);
	casa3PuertaC.loadModel("../models/scenes/casa3/casa3puertaC.obj");
	casa3PuertaC.setShader(&shaderMulLighting);

	caja.loadModel("../models/scenes/caja.obj");
	caja.setShader(&shaderMulLighting);
	key.loadModel("../models/scenes/casa1/llave.obj");
	key.setShader(&shaderMulLighting);	
	gem.loadModel("../models/scenes/gem.obj");
	gem.setShader(&shaderMulLighting);
	bigTree.loadModel("../models/scenes/tree.obj");
	bigTree.setShader(&shaderMulLighting);

	// Terreno
	terrain.init();
	terrain.setShader(&shaderTerrain);

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

	Texture textureCesped("../Textures/pasto.png");	// Definiendo la textura a utilizar
	textureCesped.loadImage(); 							// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	glGenTextures(1, &textureCespedID); 				// Creando la textura con id 1
	glBindTexture(GL_TEXTURE_2D, textureCespedID);		// Enlazar esa textura a una tipo de textura de 2D.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (textureCesped.getData()) {
	// Transferir los datos de la imagen a la tarjeta
		glTexImage2D(GL_TEXTURE_2D, 0, textureCesped.getChannels() == 3 ? GL_RGB : GL_RGBA, textureCesped.getWidth(), textureCesped.getHeight(), 0,
		textureCesped.getChannels() == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, textureCesped.getData());
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
		std::cout << "Failed to load texture" << std::endl;
	textureCesped.freeImage();

	// Definiendo la textura del nuevo shader
	Texture textureR("../Textures/dirt.png");
	textureR.loadImage(); // Cargar la textura
	glGenTextures(1, &textureRID); // Creando el id de la textura del landingpad
	glBindTexture(GL_TEXTURE_2D, textureRID); // Se enlaza la textura
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Wrapping en el eje u
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // Wrapping en el eje v
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Filtering de minimización
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Filtering de maximimizacion
	if(textureR.getData()){
		// Transferir los datos de la imagen a la tarjeta
		glTexImage2D(GL_TEXTURE_2D, 0, textureR.getChannels() == 3 ? GL_RGB : GL_RGBA, textureR.getWidth(), textureR.getHeight(), 0,
		textureR.getChannels() == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, textureR.getData());
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else 
		std::cout << "Fallo la carga de textura" << std::endl;
	textureR.freeImage(); // Liberamos memoria

	// Definiendo la textura del nuevo shader
	Texture textureG("../Textures/flowers.png");
	textureG.loadImage(); // Cargar la textura
	glGenTextures(1, &textureGID); // Creando el id de la textura del landingpad
	glBindTexture(GL_TEXTURE_2D, textureGID); // Se enlaza la textura
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Wrapping en el eje u
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // Wrapping en el eje v
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Filtering de minimización
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Filtering de maximimizacion
	if(textureG.getData()){
		// Transferir los datos de la imagen a la tarjeta
		glTexImage2D(GL_TEXTURE_2D, 0, textureG.getChannels() == 3 ? GL_RGB : GL_RGBA, textureG.getWidth(), textureG.getHeight(), 0,
		textureG.getChannels() == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, textureG.getData());
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else 
		std::cout << "Fallo la carga de textura" << std::endl;
	textureG.freeImage(); // Liberamos memoria

	// Definiendo la textura del nuevo shader
	Texture textureB("../Textures/grava.png");
	textureB.loadImage(); // Cargar la textura
	glGenTextures(1, &textureBID); // Creando el id de la textura del landingpad
	glBindTexture(GL_TEXTURE_2D, textureBID); // Se enlaza la textura
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Wrapping en el eje u
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // Wrapping en el eje v
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Filtering de minimización
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Filtering de maximimizacion
	if(textureB.getData()){
		// Transferir los datos de la imagen a la tarjeta
		glTexImage2D(GL_TEXTURE_2D, 0, textureB.getChannels() == 3 ? GL_RGB : GL_RGBA, textureB.getWidth(), textureB.getHeight(), 0,
		textureB.getChannels() == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, textureB.getData());
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else 
		std::cout << "Fallo la carga de textura" << std::endl;
	textureB.freeImage(); // Liberamos memoria

	// Definiendo la textura del nuevo shader
	Texture textureBlendMap("../Textures/blendMap.png");
	textureBlendMap.loadImage(); // Cargar la textura
	glGenTextures(1, &textureBlendMapID); // Creando el id de la textura del landingpad
	glBindTexture(GL_TEXTURE_2D, textureBlendMapID); // Se enlaza la textura
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Wrapping en el eje u
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // Wrapping en el eje v
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Filtering de minimización
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Filtering de maximimizacion
	if(textureBlendMap.getData()){
		// Transferir los datos de la imagen a la tarjeta
		glTexImage2D(GL_TEXTURE_2D, 0, textureBlendMap.getChannels() == 3 ? GL_RGB : GL_RGBA, textureBlendMap.getWidth(), textureBlendMap.getHeight(), 0,
		textureBlendMap.getChannels() == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, textureBlendMap.getData());
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else 
		std::cout << "Fallo la carga de textura" << std::endl;
	textureBlendMap.freeImage(); // Liberamos memoria

	
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
	shaderTerrain.destroy();

	// Basic objects Delete
	skyboxSphere.destroy();

	// Custom objects Delete
	catModelAnimate.destroy();

	casa1Pared1a.destroy();
	casa1Pared1b.destroy();
	casa1Pared1c.destroy();
	casa1Puerta1.destroy();
	casa1Techo1a.destroy();
	casa1Escalera.destroy();
	casa1Pared2a.destroy();
	casa1Pared2b.destroy();
	casa1Pared2c.destroy();
	casa1Puerta2.destroy();
	casa1Ventana1.destroy();
	casa1Ventana2.destroy();
	casa1Techo2a.destroy();
	casa1Techo2b.destroy();
	casa1Techo2c.destroy();
	casa1Pared3a.destroy();
	casa1Pared3b.destroy();
	casa1Pared3c.destroy();
	casa1Puerta3.destroy();
	casa1Techo3a.destroy();
	casa1Door1.destroy();
	casa1Pared4a.destroy();
	casa1Pared4b.destroy();
	casa1Pared4c.destroy();
	casa1Puerta4.destroy();
	casa1Pared5a.destroy();
	casa1Pared6a.destroy();
	casa1TechoA.destroy();
	casa1TechoB.destroy();
	casa1ParedA.destroy();
	casa1ParedB.destroy();
	casa1PuertaA.destroy();
	casa1Door2.destroy();
	casa1Viga1.destroy();
	casa1Viga2.destroy();
	casa1Viga3.destroy();
	casa1Viga4.destroy();
	casa1Viga5a.destroy();
	casa1Viga5b.destroy();
	casa1Viga6.destroy();
	casa1Pilar1.destroy();
	casa1Pilar2.destroy();
	casa1Pilar3.destroy();
	borde1A.destroy();
	borde2A.destroy();
	borde3A.destroy();
	borde4A.destroy();
	borde5A.destroy();
	borde6A.destroy();
	obj1a.destroy();
	obj1b.destroy();
	obj2a.destroy();
	obj2b.destroy();
	obj3a.destroy();
	techoA.destroy();
	techoB.destroy();
	//scene1.destroy();
	base.destroy();
	puente.destroy();
	techo.destroy();
	bordes.destroy();
	torrePared1.destroy();
	torrePared2.destroy();
	torrePared3.destroy();
	torreEntrada.destroy();
	torrePuerta.destroy();
	// Casa 2
	casa2Entrada.destroy();
	casa2ParedA.destroy();
	casa2ParedB.destroy();
	casa2ParedC.destroy();
	casa2ParedD.destroy();
	casa2ParedE.destroy();
	casa2ParedF.destroy();
	casa2ParedG.destroy();
	casa2ParedH.destroy();
	casa2ParedI.destroy();
	casa2TechoA.destroy();
	casa2TechoB.destroy();
	casa2TechoC.destroy();
	casa2VentanaA.destroy();
	casa2VentanaB.destroy();
	casa2VentanaC.destroy();
	casa2VentanaD.destroy();
	// Casa 3
	casa3BaseA.destroy();
	casa3BaseB.destroy();
	casa3TechoA.destroy();
	casa3TechoB.destroy();
	casa3VentanaA.destroy();
	casa3PuertaA.destroy();
	casa3PuertaB.destroy();
	casa3PuertaC.destroy();

	// Objetos
	caja.destroy();
	key.destroy();
	gem.destroy();
	bigTree.destroy();

	// Terrains objects Delete
	terrain.destroy();

	// Textures Delete
	glBindTexture(GL_TEXTURE_2D, 0);
	glDeleteTextures(1, &textureCespedID);
	glDeleteTextures(1, &textureWallID);
	glDeleteTextures(1, &textureWindowID);
	glDeleteTextures(1, &textureHighwayID);
	glDeleteTextures(1, &textureLandingPadID);
	glDeleteTextures(1, &textureRID);
	glDeleteTextures(1, &textureGID);
	glDeleteTextures(1, &textureBID);
	glDeleteTextures(1, &textureBlendMapID);

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
		modelMatrixCat = glm::translate(modelMatrixCat, glm::vec3(0.0, 0.0, 0.05));
		animationCatIndex = 1;
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){	// Shift izquierdo para correr
			modelMatrixCat = glm::translate(modelMatrixCat, glm::vec3(0.0, 0.0, 0.1));
			animationCatIndex = 1;
		}
	}
	else if (modelSelected == 0 && glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
		modelMatrixCat = glm::translate(modelMatrixCat, glm::vec3(0.0, 0.0, -0.03));
		animationCatIndex = 1;
	}

	// Saltar: solo se puede si cat está apoyado en un collider
	bool keySpaceStatus = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
	if(keySpaceStatus && catOnGround){
		catVelY = JUMP_FORCE;	// Aplicar impulso hacia arriba
		catOnGround = false;	// Deja el suelo al saltar
	}

	if(!catOnGround && catVelY >= 0.0f){
		animationCatIndex = 5;
	}
	if(!catOnGround && catVelY <= 0.0f){
		animationCatIndex = 3;	// 0 nada	1 camina	2 camina	3 caida 	4 nada		5 salta		6 ?
	}

	glfwPollEvents();
	return continueApplication;
}

void applicationLoop() {
	bool psi = true;

	glm::vec3 target;

	modelMatrixCat = glm::translate(modelMatrixCat, glm::vec3(-70.0f, 0.0f, -70.0f)); 		// Posición inicial del gato
	modelMatrixCat = glm::rotate(modelMatrixCat, glm::radians(180.0f), glm::vec3(0, 1, 0));

	modelMatrixCasa1Pared1a = glm::translate(modelMatrixCasa1Pared1a, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixCasa1Pared1b = glm::translate(modelMatrixCasa1Pared1b, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixCasa1Pared1c = glm::translate(modelMatrixCasa1Pared1c, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixCasa1Puerta1 = glm::translate(modelMatrixCasa1Puerta1, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixCasa1Techo1a = glm::translate(modelMatrixCasa1Techo1a, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixCasa1Escalera = glm::translate(modelMatrixCasa1Escalera, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixCasa1Pared2a = glm::translate(modelMatrixCasa1Pared2a, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixCasa1Pared2b = glm::translate(modelMatrixCasa1Pared2b, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixCasa1Pared2c = glm::translate(modelMatrixCasa1Pared2c, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixCasa1Puerta2 = glm::translate(modelMatrixCasa1Puerta2, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixCasa1Techo2a = glm::translate(modelMatrixCasa1Techo2a, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixCasa1Techo2b = glm::translate(modelMatrixCasa1Techo2b, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixCasa1Techo2c = glm::translate(modelMatrixCasa1Techo2c, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixCasa1Ventana1 = glm::translate(modelMatrixCasa1Ventana1, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixCasa1Ventana2 = glm::translate(modelMatrixCasa1Ventana2, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixCasa1Pared3a = glm::translate(modelMatrixCasa1Pared3a, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixCasa1Pared3b = glm::translate(modelMatrixCasa1Pared3b, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixCasa1Pared3c = glm::translate(modelMatrixCasa1Pared3c, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixCasa1Puerta3 = glm::translate(modelMatrixCasa1Puerta3, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixCasa1Techo3a = glm::translate(modelMatrixCasa1Techo3a, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixCasa1Door1 = glm::translate(modelMatrixCasa1Door1, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixCasa1Pared4a = glm::translate(modelMatrixCasa1Pared4a, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixCasa1Pared4b = glm::translate(modelMatrixCasa1Pared4b, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixCasa1Pared4c = glm::translate(modelMatrixCasa1Pared4c, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixCasa1Puerta4 = glm::translate(modelMatrixCasa1Puerta4, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixCasa1Pared5a = glm::translate(modelMatrixCasa1Pared5a, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixCasa1Pared6a = glm::translate(modelMatrixCasa1Pared6a, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixCasa1TechoA = glm::translate(modelMatrixCasa1TechoA, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixCasa1TechoB = glm::translate(modelMatrixCasa1TechoB, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixCasa1ParedA = glm::translate(modelMatrixCasa1ParedA, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixCasa1ParedB = glm::translate(modelMatrixCasa1ParedB, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixCasa1PuertaA = glm::translate(modelMatrixCasa1PuertaA, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixCasa1Door2 = glm::translate(modelMatrixCasa1Door2, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixCasa1Viga1 = glm::translate(modelMatrixCasa1Viga1, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixCasa1Viga2 = glm::translate(modelMatrixCasa1Viga2, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixCasa1Viga3 = glm::translate(modelMatrixCasa1Viga3, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixCasa1Viga4 = glm::translate(modelMatrixCasa1Viga4, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixCasa1Viga5a = glm::translate(modelMatrixCasa1Viga5a, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixCasa1Viga5b = glm::translate(modelMatrixCasa1Viga5b, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixCasa1Viga6 = glm::translate(modelMatrixCasa1Viga6, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixCasa1Pilar1 = glm::translate(modelMatrixCasa1Pilar1, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixCasa1Pilar2 = glm::translate(modelMatrixCasa1Pilar2, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixCasa1Pilar3 = glm::translate(modelMatrixCasa1Pilar3, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixBorde1A = glm::translate(modelMatrixBorde1A, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixBorde2A = glm::translate(modelMatrixBorde2A, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixBorde3A = glm::translate(modelMatrixBorde3A, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixBorde4A = glm::translate(modelMatrixBorde4A, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixBorde5A = glm::translate(modelMatrixBorde5A, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixBorde6A = glm::translate(modelMatrixBorde6A, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixObj1a = glm::translate(modelMatrixObj1a, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixObj1b = glm::translate(modelMatrixObj1b, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixObj2a = glm::translate(modelMatrixObj2a, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixObj2b = glm::translate(modelMatrixObj2b, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixObj3a = glm::translate(modelMatrixObj3a, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixTechoA = glm::translate(modelMatrixTechoA, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixTechoB = glm::translate(modelMatrixTechoB, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixBase = glm::translate(modelMatrixBase, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixPuente = glm::translate(modelMatrixPuente, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixTecho = glm::translate(modelMatrixTecho, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixBordes = glm::translate(modelMatrixBordes, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixTorrePared1 = glm::translate(modelMatrixTorrePared1, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixTorrePared2 = glm::translate(modelMatrixTorrePared2, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixTorrePared3 = glm::translate(modelMatrixTorrePared3, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixTorreEntrada = glm::translate(modelMatrixTorreEntrada, glm::vec3(28.0f, 0.0f, 23.0f));
	modelMatrixTorrePuerta = glm::translate(modelMatrixTorrePuerta, glm::vec3(28.0f, 0.0f, 23.0f));
	// Casa 2
	modelMatrixCasa2Entrada = glm::translate(modelMatrixCasa2Entrada, glm::vec3(19.0f, 0.0f, 18.0f));
	modelMatrixCasa2ParedA = glm::translate(modelMatrixCasa2ParedA, glm::vec3(19.0f, 0.0f, 18.0f));
	modelMatrixCasa2ParedB = glm::translate(modelMatrixCasa2ParedB, glm::vec3(19.0f, 0.0f, 18.0f));
	modelMatrixCasa2ParedC = glm::translate(modelMatrixCasa2ParedC, glm::vec3(19.0f, 0.0f, 18.0f));
	modelMatrixCasa2ParedD = glm::translate(modelMatrixCasa2ParedD, glm::vec3(19.0f, 0.0f, 18.0f));
	modelMatrixCasa2ParedE = glm::translate(modelMatrixCasa2ParedE, glm::vec3(19.0f, 0.0f, 18.0f));
	modelMatrixCasa2ParedF = glm::translate(modelMatrixCasa2ParedF, glm::vec3(19.0f, 0.0f, 18.0f));
	modelMatrixCasa2ParedG = glm::translate(modelMatrixCasa2ParedG, glm::vec3(19.0f, 0.0f, 18.0f));
	modelMatrixCasa2ParedH = glm::translate(modelMatrixCasa2ParedH, glm::vec3(19.0f, 0.0f, 18.0f));
	modelMatrixCasa2ParedI = glm::translate(modelMatrixCasa2ParedI, glm::vec3(19.0f, 0.0f, 18.0f));
	modelMatrixCasa2TechoA = glm::translate(modelMatrixCasa2TechoA, glm::vec3(19.0f, 0.0f, 18.0f));
	modelMatrixCasa2TechoB = glm::translate(modelMatrixCasa2TechoB, glm::vec3(19.0f, 0.0f, 18.0f));
	modelMatrixCasa2TechoC = glm::translate(modelMatrixCasa2TechoC, glm::vec3(19.0f, 0.0f, 18.0f));
	modelMatrixCasa2VentanaA = glm::translate(modelMatrixCasa2VentanaA, glm::vec3(19.0f, 0.0f, 18.0f));
	modelMatrixCasa2VentanaB = glm::translate(modelMatrixCasa2VentanaB, glm::vec3(19.0f, 0.0f, 18.0f));
	modelMatrixCasa2VentanaC = glm::translate(modelMatrixCasa2VentanaC, glm::vec3(19.0f, 0.0f, 18.0f));
	modelMatrixCasa2VentanaD = glm::translate(modelMatrixCasa2VentanaD, glm::vec3(19.0f, 0.0f, 18.0f));
	// Casa 3
	modelMatrixCasa3BaseA = glm::translate(modelMatrixCasa3BaseA, glm::vec3(19.0f, 0.0f, 18.0f));
	modelMatrixCasa3BaseB = glm::translate(modelMatrixCasa3BaseB, glm::vec3(19.0f, 0.0f, 18.0f));
	modelMatrixCasa3TechoA = glm::translate(modelMatrixCasa3TechoA, glm::vec3(19.0f, 0.0f, 18.0f));
	modelMatrixCasa3TechoB = glm::translate(modelMatrixCasa3TechoB, glm::vec3(19.0f, 0.0f, 18.0f));
	modelMatrixCasa3VentanaA = glm::translate(modelMatrixCasa3VentanaA, glm::vec3(19.0f, 0.0f, 18.0f));
	modelMatrixCasa3PuertaA = glm::translate(modelMatrixCasa3PuertaA, glm::vec3(19.0f, 0.0f, 18.0f));
	modelMatrixCasa3PuertaB = glm::translate(modelMatrixCasa3PuertaB, glm::vec3(19.0f, 0.0f, 18.0f));
	modelMatrixCasa3PuertaC = glm::translate(modelMatrixCasa3PuertaC, glm::vec3(19.0f, 0.0f, 18.0f));
	// Objetos
	modelMatrixCaja = glm::translate(modelMatrixCaja, glm::vec3(27.1f, 1.55f, 53.6f));
	modelMatrixCasa1Key = glm::translate(modelMatrixCasa1Key, glm::vec3(26.5f, 0.0f, 23.0f));
	modelMatrixGem = glm::translate(modelMatrixGem, glm::vec3(0.0f, 0.0f, 0.0f));
	modelMatrixBigTree = glm::translate(modelMatrixBigTree, glm::vec3(-79.0f, 0.0f, -82.0f));
	modelMatrixBigTree = glm::scale(modelMatrixBigTree, glm::vec3(1.5f));
	modelMatrixBigTree = glm::rotate(modelMatrixBigTree, glm::radians(-130.0f), glm::vec3(0, 1, 0));

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
			if(distanceFromPlayer >= 4){
				target = glm::vec3(modelMatrixCat[3]) + glm::vec3(0.0f, 1.5f, 0.0f);	// Altura de la cámara
			}else{
				target = glm::vec3(modelMatrixCat[3]) + glm::vec3(0.0f, 0.5f, 0.0f);	// Altura de la cámara
			}
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

		shaderTerrain.setMatrix4("projection", 1, false, glm::value_ptr(projection));
		shaderTerrain.setMatrix4("view", 1, false, glm::value_ptr(view));

		/*******************************************
		 * Propiedades Luz direccional
		 *******************************************/
		shaderMulLighting.setVectorFloat3("viewPos", glm::value_ptr(camera->getPosition()));
		shaderMulLighting.setVectorFloat3("directionalLight.light.ambient", glm::value_ptr(glm::vec3(0.3, 0.3, 0.3)));
		shaderMulLighting.setVectorFloat3("directionalLight.light.diffuse", glm::value_ptr(glm::vec3(0.7, 0.7, 0.7)));
		shaderMulLighting.setVectorFloat3("directionalLight.light.specular", glm::value_ptr(glm::vec3(0.9, 0.9, 0.9)));
		shaderMulLighting.setVectorFloat3("directionalLight.direction", glm::value_ptr(glm::vec3(-1.0, 0.0, 0.0)));

		shaderTerrain.setVectorFloat3("viewPos", glm::value_ptr(camera->getPosition()));
		shaderTerrain.setVectorFloat3("directionalLight.light.ambient", glm::value_ptr(glm::vec3(0.3, 0.3, 0.3)));
		shaderTerrain.setVectorFloat3("directionalLight.light.diffuse", glm::value_ptr(glm::vec3(0.7, 0.7, 0.7)));
		shaderTerrain.setVectorFloat3("directionalLight.light.specular", glm::value_ptr(glm::vec3(0.9, 0.9, 0.9)));
		shaderTerrain.setVectorFloat3("directionalLight.direction", glm::value_ptr(glm::vec3(-1.0, 0.0, 0.0)));

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
		// Unidad 0: textura base (negro del blendmap)
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureCespedID);
		shaderTerrain.setInt("backgroundTexture", 0);
		// Unidad 1: textura canal R del blendmap (dirt)
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textureRID);
		shaderTerrain.setInt("rTexture", 1);
		// Unidad 2: textura canal G del blendmap (snow)
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, textureGID);
		shaderTerrain.setInt("gTexture", 2);
		// Unidad 3: textura canal B del blendmap (road)
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, textureBID);
		shaderTerrain.setInt("bTexture", 3);
		// Unidad 4: blendmap (controla la mezcla entre texturas)
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, textureBlendMapID);
		shaderTerrain.setInt("blendMapTexture", 4);

		shaderTerrain.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(40, 40)));
		terrain.setPosition(glm::vec3(100, 0, 100));
		terrain.render();
		shaderTerrain.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(0, 0)));
		// Desvincular unidades 1-4 para no interferir con otros shaders
		glActiveTexture(GL_TEXTURE4); glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE3); glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE0);

		/*******************************************ww
		 * Casa
		 *******************************************/
		// Casa1
		// Pared 1A
		glm::mat4 modelMatrixCasa1Pared1aBody = glm::mat4(modelMatrixCasa1Pared1a);
		modelMatrixCasa1Pared1a[3].y = terrain.getHeightTerrain(modelMatrixCasa1Pared1a[3].x, modelMatrixCasa1Pared1a[3].z);
		modelMatrixCasa1Pared1aBody = glm::translate(modelMatrixCasa1Pared1aBody, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixCasa1Pared1aBody = glm::scale(modelMatrixCasa1Pared1aBody, glm::vec3(0.8));
		casa1Pared1a.render(modelMatrixCasa1Pared1aBody);
		// Pared 1B
		glm::mat4 modelMatrixCasa1Pared1bBody = glm::mat4(modelMatrixCasa1Pared1b);
		modelMatrixCasa1Pared1b[3].y = terrain.getHeightTerrain(modelMatrixCasa1Pared1b[3].x, modelMatrixCasa1Pared1b[3].z);
		modelMatrixCasa1Pared1bBody = glm::translate(modelMatrixCasa1Pared1bBody, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixCasa1Pared1bBody = glm::scale(modelMatrixCasa1Pared1bBody, glm::vec3(0.8));
		casa1Pared1b.render(modelMatrixCasa1Pared1bBody);
		// Pared 1C
		glm::mat4 modelMatrixCasa1Pared1cBody = glm::mat4(modelMatrixCasa1Pared1c);
		modelMatrixCasa1Pared1c[3].y = terrain.getHeightTerrain(modelMatrixCasa1Pared1c[3].x, modelMatrixCasa1Pared1c[3].z);
		modelMatrixCasa1Pared1cBody = glm::translate(modelMatrixCasa1Pared1cBody, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixCasa1Pared1cBody = glm::scale(modelMatrixCasa1Pared1cBody, glm::vec3(0.8));
		casa1Pared1c.render(modelMatrixCasa1Pared1cBody);
		// Puerta 1
		glm::mat4 modelMatrixCasa1Puerta1Body = glm::mat4(modelMatrixCasa1Puerta1);
		modelMatrixCasa1Puerta1[3].y = terrain.getHeightTerrain(modelMatrixCasa1Puerta1[3].x, modelMatrixCasa1Puerta1[3].z);
		modelMatrixCasa1Puerta1Body = glm::translate(modelMatrixCasa1Puerta1Body, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixCasa1Puerta1Body = glm::scale(modelMatrixCasa1Puerta1Body, glm::vec3(0.8));
		casa1Puerta1.render(modelMatrixCasa1Puerta1Body);
		// Techo 1A
		glm::mat4 modelMatrixCasa1Techo1aBody = glm::mat4(modelMatrixCasa1Techo1a);
		modelMatrixCasa1Techo1a[3].y = terrain.getHeightTerrain(modelMatrixCasa1Techo1a[3].x, modelMatrixCasa1Techo1a[3].z);
		modelMatrixCasa1Techo1aBody = glm::translate(modelMatrixCasa1Techo1aBody, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixCasa1Techo1aBody = glm::scale(modelMatrixCasa1Techo1aBody, glm::vec3(0.8));
		casa1Techo1a.render(modelMatrixCasa1Techo1aBody);
		// Escalera
		glm::mat4 modelMatrixCasa1EscaleraBody = glm::mat4(modelMatrixCasa1Escalera);
		modelMatrixCasa1Escalera[3].y = terrain.getHeightTerrain(modelMatrixCasa1Escalera[3].x, modelMatrixCasa1Escalera[3].z);
		modelMatrixCasa1EscaleraBody = glm::translate(modelMatrixCasa1EscaleraBody, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixCasa1EscaleraBody = glm::scale(modelMatrixCasa1EscaleraBody, glm::vec3(0.8));
		casa1Escalera.render(modelMatrixCasa1EscaleraBody);
		// Pared 2A
		glm::mat4 modelMatrixCasa1Pared2aBody = glm::mat4(modelMatrixCasa1Pared2a);
		modelMatrixCasa1Pared2a[3].y = terrain.getHeightTerrain(modelMatrixCasa1Pared2a[3].x, modelMatrixCasa1Pared2a[3].z);
		modelMatrixCasa1Pared2aBody = glm::translate(modelMatrixCasa1Pared2aBody, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixCasa1Pared2aBody = glm::scale(modelMatrixCasa1Pared2aBody, glm::vec3(0.8));
		casa1Pared2a.render(modelMatrixCasa1Pared2aBody);
		// Pared 2B
		glm::mat4 modelMatrixCasa1Pared2bBody = glm::mat4(modelMatrixCasa1Pared2b);
		modelMatrixCasa1Pared2b[3].y = terrain.getHeightTerrain(modelMatrixCasa1Pared2b[3].x, modelMatrixCasa1Pared2b[3].z);
		modelMatrixCasa1Pared2bBody = glm::translate(modelMatrixCasa1Pared2bBody, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixCasa1Pared2bBody = glm::scale(modelMatrixCasa1Pared2bBody, glm::vec3(0.8));
		casa1Pared2b.render(modelMatrixCasa1Pared2bBody);
		// Pared 2C
		glm::mat4 modelMatrixCasa1Pared2cBody = glm::mat4(modelMatrixCasa1Pared2c);
		modelMatrixCasa1Pared2c[3].y = terrain.getHeightTerrain(modelMatrixCasa1Pared2c[3].x, modelMatrixCasa1Pared2c[3].z);
		modelMatrixCasa1Pared2cBody = glm::translate(modelMatrixCasa1Pared2cBody, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixCasa1Pared2cBody = glm::scale(modelMatrixCasa1Pared2cBody, glm::vec3(0.8));
		casa1Pared2c.render(modelMatrixCasa1Pared2cBody);
		// Puerta 2
		glm::mat4 modelMatrixCasa1Puerta2Body = glm::mat4(modelMatrixCasa1Puerta2);
		modelMatrixCasa1Puerta2[3].y = terrain.getHeightTerrain(modelMatrixCasa1Puerta2[3].x, modelMatrixCasa1Puerta2[3].z);
		modelMatrixCasa1Puerta2Body = glm::translate(modelMatrixCasa1Puerta2Body, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixCasa1Puerta2Body = glm::scale(modelMatrixCasa1Puerta2Body, glm::vec3(0.8));
		casa1Puerta2.render(modelMatrixCasa1Puerta2Body);
		// Ventana 1
		glm::mat4 modelMatrixCasa1Ventana1Body = glm::mat4(modelMatrixCasa1Ventana1);
		modelMatrixCasa1Ventana1[3].y = terrain.getHeightTerrain(modelMatrixCasa1Ventana1[3].x, modelMatrixCasa1Ventana1[3].z);
		modelMatrixCasa1Ventana1Body = glm::translate(modelMatrixCasa1Ventana1Body, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixCasa1Ventana1Body = glm::scale(modelMatrixCasa1Ventana1Body, glm::vec3(0.8));
		casa1Ventana1.render(modelMatrixCasa1Ventana1Body);
		// Ventana 2
		glm::mat4 modelMatrixCasa1Ventana2Body = glm::mat4(modelMatrixCasa1Ventana2);
		modelMatrixCasa1Ventana2[3].y = terrain.getHeightTerrain(modelMatrixCasa1Ventana2[3].x, modelMatrixCasa1Ventana2[3].z);
		modelMatrixCasa1Ventana2Body = glm::translate(modelMatrixCasa1Ventana2Body, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixCasa1Ventana2Body = glm::scale(modelMatrixCasa1Ventana2Body, glm::vec3(0.8));
		casa1Ventana2.render(modelMatrixCasa1Ventana2Body);
		// Techo 2A
		glm::mat4 modelMatrixCasa1Techo2aBody = glm::mat4(modelMatrixCasa1Techo2a);
		modelMatrixCasa1Techo2a[3].y = terrain.getHeightTerrain(modelMatrixCasa1Techo2a[3].x, modelMatrixCasa1Techo2a[3].z);
		modelMatrixCasa1Techo2aBody = glm::translate(modelMatrixCasa1Techo2aBody, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixCasa1Techo2aBody = glm::scale(modelMatrixCasa1Techo2aBody, glm::vec3(0.8));
		casa1Techo2a.render(modelMatrixCasa1Techo2aBody);
		// Techo 2B
		glm::mat4 modelMatrixCasa1Techo2bBody = glm::mat4(modelMatrixCasa1Techo2b);
		modelMatrixCasa1Techo2b[3].y = terrain.getHeightTerrain(modelMatrixCasa1Techo2b[3].x, modelMatrixCasa1Techo2b[3].z);
		modelMatrixCasa1Techo2bBody = glm::translate(modelMatrixCasa1Techo2bBody, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixCasa1Techo2bBody = glm::scale(modelMatrixCasa1Techo2bBody, glm::vec3(0.8));
		casa1Techo2b.render(modelMatrixCasa1Techo2bBody);
		// Techo 2C
		glm::mat4 modelMatrixCasa1Techo2cBody = glm::mat4(modelMatrixCasa1Techo2c);
		modelMatrixCasa1Techo2c[3].y = terrain.getHeightTerrain(modelMatrixCasa1Techo2c[3].x, modelMatrixCasa1Techo2c[3].z);
		modelMatrixCasa1Techo2cBody = glm::translate(modelMatrixCasa1Techo2cBody, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixCasa1Techo2cBody = glm::scale(modelMatrixCasa1Techo2cBody, glm::vec3(0.8));
		casa1Techo2c.render(modelMatrixCasa1Techo2cBody);
		// Pared 3A
		glm::mat4 modelMatrixCasa1Pared3aBody = glm::mat4(modelMatrixCasa1Pared3a);
		modelMatrixCasa1Pared3a[3].y = terrain.getHeightTerrain(modelMatrixCasa1Pared3a[3].x, modelMatrixCasa1Pared3a[3].z);
		modelMatrixCasa1Pared3aBody = glm::translate(modelMatrixCasa1Pared3aBody, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixCasa1Pared3aBody = glm::scale(modelMatrixCasa1Pared3aBody, glm::vec3(0.8));
		casa1Pared3a.render(modelMatrixCasa1Pared3aBody);
		// Pared 3B
		glm::mat4 modelMatrixCasa1Pared3bBody = glm::mat4(modelMatrixCasa1Pared3b);
		modelMatrixCasa1Pared3b[3].y = terrain.getHeightTerrain(modelMatrixCasa1Pared3b[3].x, modelMatrixCasa1Pared3b[3].z);
		modelMatrixCasa1Pared3bBody = glm::translate(modelMatrixCasa1Pared3bBody, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixCasa1Pared3bBody = glm::scale(modelMatrixCasa1Pared3bBody, glm::vec3(0.8));
		casa1Pared3b.render(modelMatrixCasa1Pared3bBody);
		// Pared 3C
		glm::mat4 modelMatrixCasa1Pared3cBody = glm::mat4(modelMatrixCasa1Pared3c);
		modelMatrixCasa1Pared3c[3].y = terrain.getHeightTerrain(modelMatrixCasa1Pared3c[3].x, modelMatrixCasa1Pared3c[3].z);
		modelMatrixCasa1Pared3cBody = glm::translate(modelMatrixCasa1Pared3cBody, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixCasa1Pared3cBody = glm::scale(modelMatrixCasa1Pared3cBody, glm::vec3(0.8));
		casa1Pared3c.render(modelMatrixCasa1Pared3cBody);
		// Puerta 3
		glm::mat4 modelMatrixCasa1Puerta3Body = glm::mat4(modelMatrixCasa1Puerta3);
		modelMatrixCasa1Puerta3[3].y = terrain.getHeightTerrain(modelMatrixCasa1Puerta3[3].x, modelMatrixCasa1Puerta3[3].z);
		modelMatrixCasa1Puerta3Body = glm::translate(modelMatrixCasa1Puerta3Body, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixCasa1Puerta3Body = glm::scale(modelMatrixCasa1Puerta3Body, glm::vec3(0.8));
		casa1Puerta3.render(modelMatrixCasa1Puerta3Body);
		// Techo 3A
		glm::mat4 modelMatrixCasa1Techo3aBody = glm::mat4(modelMatrixCasa1Techo3a);
		modelMatrixCasa1Techo3a[3].y = terrain.getHeightTerrain(modelMatrixCasa1Techo3a[3].x, modelMatrixCasa1Techo3a[3].z);
		modelMatrixCasa1Techo3aBody = glm::translate(modelMatrixCasa1Techo3aBody, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixCasa1Techo3aBody = glm::scale(modelMatrixCasa1Techo3aBody, glm::vec3(0.8));
		casa1Techo3a.render(modelMatrixCasa1Techo3aBody);
		// Door 1
		glm::mat4 modelMatrixCasa1Door1Body = glm::mat4(modelMatrixCasa1Door1);
		modelMatrixCasa1Door1[3].y = terrain.getHeightTerrain(modelMatrixCasa1Door1[3].x, modelMatrixCasa1Door1[3].z);
		modelMatrixCasa1Door1Body = glm::translate(modelMatrixCasa1Door1Body, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixCasa1Door1Body = glm::scale(modelMatrixCasa1Door1Body, glm::vec3(0.8));
		casa1Door1.render(modelMatrixCasa1Door1Body);
		// Pared 4A
		glm::mat4 modelMatrixCasa1Pared4aBody = glm::mat4(modelMatrixCasa1Pared4a);
		modelMatrixCasa1Pared4a[3].y = terrain.getHeightTerrain(modelMatrixCasa1Pared4a[3].x, modelMatrixCasa1Pared4a[3].z);
		modelMatrixCasa1Pared4aBody = glm::translate(modelMatrixCasa1Pared4aBody, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixCasa1Pared4aBody = glm::scale(modelMatrixCasa1Pared4aBody, glm::vec3(0.8));
		casa1Pared4a.render(modelMatrixCasa1Pared4aBody);
		// Pared 4B
		glm::mat4 modelMatrixCasa1Pared4bBody = glm::mat4(modelMatrixCasa1Pared4b);
		modelMatrixCasa1Pared4b[3].y = terrain.getHeightTerrain(modelMatrixCasa1Pared4b[3].x, modelMatrixCasa1Pared4b[3].z);
		modelMatrixCasa1Pared4bBody = glm::translate(modelMatrixCasa1Pared4bBody, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixCasa1Pared4bBody = glm::scale(modelMatrixCasa1Pared4bBody, glm::vec3(0.8));
		casa1Pared4b.render(modelMatrixCasa1Pared4bBody);
		// Pared 4C
		glm::mat4 modelMatrixCasa1Pared4cBody = glm::mat4(modelMatrixCasa1Pared4c);
		modelMatrixCasa1Pared4c[3].y = terrain.getHeightTerrain(modelMatrixCasa1Pared4c[3].x, modelMatrixCasa1Pared4c[3].z);
		modelMatrixCasa1Pared4cBody = glm::translate(modelMatrixCasa1Pared4cBody, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixCasa1Pared4cBody = glm::scale(modelMatrixCasa1Pared4cBody, glm::vec3(0.8));
		casa1Pared4c.render(modelMatrixCasa1Pared4cBody);
		// Puerta 4
		glm::mat4 modelMatrixCasa1Puerta4Body = glm::mat4(modelMatrixCasa1Puerta4);
		modelMatrixCasa1Puerta4[3].y = terrain.getHeightTerrain(modelMatrixCasa1Puerta4[3].x, modelMatrixCasa1Puerta4[3].z);
		modelMatrixCasa1Puerta4Body = glm::translate(modelMatrixCasa1Puerta4Body, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixCasa1Puerta4Body = glm::scale(modelMatrixCasa1Puerta4Body, glm::vec3(0.8));
		casa1Puerta4.render(modelMatrixCasa1Puerta4Body);
		// Pared 5A
		glm::mat4 modelMatrixCasa1Pared5aBody = glm::mat4(modelMatrixCasa1Pared5a);
		modelMatrixCasa1Pared5a[3].y = terrain.getHeightTerrain(modelMatrixCasa1Pared5a[3].x, modelMatrixCasa1Pared5a[3].z);
		modelMatrixCasa1Pared5aBody = glm::translate(modelMatrixCasa1Pared5aBody, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixCasa1Pared5aBody = glm::scale(modelMatrixCasa1Pared5aBody, glm::vec3(0.8));
		casa1Pared5a.render(modelMatrixCasa1Pared5aBody);
		// Pared 6A
		glm::mat4 modelMatrixCasa1Pared6aBody = glm::mat4(modelMatrixCasa1Pared6a);
		modelMatrixCasa1Pared6a[3].y = terrain.getHeightTerrain(modelMatrixCasa1Pared6a[3].x, modelMatrixCasa1Pared6a[3].z);
		modelMatrixCasa1Pared6aBody = glm::translate(modelMatrixCasa1Pared6aBody, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixCasa1Pared6aBody = glm::scale(modelMatrixCasa1Pared6aBody, glm::vec3(0.8));
		casa1Pared6a.render(modelMatrixCasa1Pared6aBody);
		// Techo A
		glm::mat4 modelMatrixCasa1TechoABody = glm::mat4(modelMatrixCasa1TechoA);
		modelMatrixCasa1TechoA[3].y = terrain.getHeightTerrain(modelMatrixCasa1TechoA[3].x, modelMatrixCasa1TechoA[3].z);
		modelMatrixCasa1TechoABody = glm::translate(modelMatrixCasa1TechoABody, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixCasa1TechoABody = glm::scale(modelMatrixCasa1TechoABody, glm::vec3(0.8));
		casa1TechoA.render(modelMatrixCasa1TechoABody);
		// Techo B
		glm::mat4 modelMatrixCasa1TechoBBody = glm::mat4(modelMatrixCasa1TechoB);
		modelMatrixCasa1TechoB[3].y = terrain.getHeightTerrain(modelMatrixCasa1TechoB[3].x, modelMatrixCasa1TechoB[3].z);
		modelMatrixCasa1TechoBBody = glm::translate(modelMatrixCasa1TechoBBody, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixCasa1TechoBBody = glm::scale(modelMatrixCasa1TechoBBody, glm::vec3(0.8));
		casa1TechoB.render(modelMatrixCasa1TechoBBody);
		// Pared A
		glm::mat4 modelMatrixCasa1ParedABody = glm::mat4(modelMatrixCasa1ParedA);
		modelMatrixCasa1ParedA[3].y = terrain.getHeightTerrain(modelMatrixCasa1ParedA[3].x, modelMatrixCasa1ParedA[3].z);
		modelMatrixCasa1ParedABody = glm::translate(modelMatrixCasa1ParedABody, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixCasa1ParedABody = glm::scale(modelMatrixCasa1ParedABody, glm::vec3(0.8));
		casa1ParedA.render(modelMatrixCasa1ParedABody);
		// Pared B
		glm::mat4 modelMatrixCasa1ParedBBody = glm::mat4(modelMatrixCasa1ParedB);
		modelMatrixCasa1ParedB[3].y = terrain.getHeightTerrain(modelMatrixCasa1ParedB[3].x, modelMatrixCasa1ParedB[3].z);
		modelMatrixCasa1ParedBBody = glm::translate(modelMatrixCasa1ParedBBody, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixCasa1ParedBBody = glm::scale(modelMatrixCasa1ParedBBody, glm::vec3(0.8));
		casa1ParedB.render(modelMatrixCasa1ParedBBody);
		// Puerta A
		glm::mat4 modelMatrixCasa1PuertaABody = glm::mat4(modelMatrixCasa1PuertaA);
		modelMatrixCasa1PuertaA[3].y = terrain.getHeightTerrain(modelMatrixCasa1PuertaA[3].x, modelMatrixCasa1PuertaA[3].z);
		modelMatrixCasa1PuertaABody = glm::translate(modelMatrixCasa1PuertaABody, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixCasa1PuertaABody = glm::scale(modelMatrixCasa1PuertaABody, glm::vec3(0.8));
		casa1PuertaA.render(modelMatrixCasa1PuertaABody);
		// Door 2
		glm::mat4 modelMatrixCasa1Door2Body = glm::mat4(modelMatrixCasa1Door2);
		modelMatrixCasa1Door2[3].y = terrain.getHeightTerrain(modelMatrixCasa1Door2[3].x, modelMatrixCasa1Door2[3].z);
		modelMatrixCasa1Door2Body = glm::translate(modelMatrixCasa1Door2Body, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixCasa1Door2Body = glm::scale(modelMatrixCasa1Door2Body, glm::vec3(0.8));
		casa1Door2.render(modelMatrixCasa1Door2Body);
		// Viga 1
		glm::mat4 modelMatrixCasa1Viga1Body = glm::mat4(modelMatrixCasa1Viga1);
		modelMatrixCasa1Viga1[3].y = terrain.getHeightTerrain(modelMatrixCasa1Viga1[3].x, modelMatrixCasa1Viga1[3].z);
		modelMatrixCasa1Viga1Body = glm::translate(modelMatrixCasa1Viga1Body, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixCasa1Viga1Body = glm::scale(modelMatrixCasa1Viga1Body, glm::vec3(0.8));
		casa1Viga1.render(modelMatrixCasa1Viga1Body);
		// Viga 2
		glm::mat4 modelMatrixCasa1Viga2Body = glm::mat4(modelMatrixCasa1Viga2);
		modelMatrixCasa1Viga2[3].y = terrain.getHeightTerrain(modelMatrixCasa1Viga2[3].x, modelMatrixCasa1Viga2[3].z);
		modelMatrixCasa1Viga2Body = glm::translate(modelMatrixCasa1Viga2Body, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixCasa1Viga2Body = glm::scale(modelMatrixCasa1Viga2Body, glm::vec3(0.8));
		casa1Viga2.render(modelMatrixCasa1Viga2Body);
		// Viga 3
		glm::mat4 modelMatrixCasa1Viga3Body = glm::mat4(modelMatrixCasa1Viga3);
		modelMatrixCasa1Viga3[3].y = terrain.getHeightTerrain(modelMatrixCasa1Viga3[3].x, modelMatrixCasa1Viga3[3].z);
		modelMatrixCasa1Viga3Body = glm::translate(modelMatrixCasa1Viga3Body, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixCasa1Viga3Body = glm::scale(modelMatrixCasa1Viga3Body, glm::vec3(0.8));
		casa1Viga3.render(modelMatrixCasa1Viga3Body);
		// Viga 4
		glm::mat4 modelMatrixCasa1Viga4Body = glm::mat4(modelMatrixCasa1Viga4);
		modelMatrixCasa1Viga4[3].y = terrain.getHeightTerrain(modelMatrixCasa1Viga4[3].x, modelMatrixCasa1Viga4[3].z);
		modelMatrixCasa1Viga4Body = glm::translate(modelMatrixCasa1Viga4Body, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixCasa1Viga4Body = glm::scale(modelMatrixCasa1Viga4Body, glm::vec3(0.8));
		casa1Viga4.render(modelMatrixCasa1Viga4Body);
		// Viga 5A
		glm::mat4 modelMatrixCasa1Viga5aBody = glm::mat4(modelMatrixCasa1Viga5a);
		modelMatrixCasa1Viga5a[3].y = terrain.getHeightTerrain(modelMatrixCasa1Viga5a[3].x, modelMatrixCasa1Viga5a[3].z);
		modelMatrixCasa1Viga5aBody = glm::translate(modelMatrixCasa1Viga5aBody, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixCasa1Viga5aBody = glm::scale(modelMatrixCasa1Viga5aBody, glm::vec3(0.8));
		casa1Viga5a.render(modelMatrixCasa1Viga5aBody);
		// Viga 5B
		glm::mat4 modelMatrixCasa1Viga5bBody = glm::mat4(modelMatrixCasa1Viga5b);
		modelMatrixCasa1Viga5b[3].y = terrain.getHeightTerrain(modelMatrixCasa1Viga5b[3].x, modelMatrixCasa1Viga5b[3].z);
		modelMatrixCasa1Viga5bBody = glm::translate(modelMatrixCasa1Viga5bBody, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixCasa1Viga5bBody = glm::scale(modelMatrixCasa1Viga5bBody, glm::vec3(0.8));
		casa1Viga5b.render(modelMatrixCasa1Viga5bBody);
		// Viga 6
		glm::mat4 modelMatrixCasa1Viga6Body = glm::mat4(modelMatrixCasa1Viga6);
		modelMatrixCasa1Viga6[3].y = terrain.getHeightTerrain(modelMatrixCasa1Viga6[3].x, modelMatrixCasa1Viga6[3].z);
		modelMatrixCasa1Viga6Body = glm::translate(modelMatrixCasa1Viga6Body, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixCasa1Viga6Body = glm::scale(modelMatrixCasa1Viga6Body, glm::vec3(0.8));
		casa1Viga6.render(modelMatrixCasa1Viga6Body);
		// Pilar 1
		glm::mat4 modelMatrixCasa1Pilar1Body = glm::mat4(modelMatrixCasa1Pilar1);
		modelMatrixCasa1Pilar1[3].y = terrain.getHeightTerrain(modelMatrixCasa1Pilar1[3].x, modelMatrixCasa1Pilar1[3].z);
		modelMatrixCasa1Pilar1Body = glm::translate(modelMatrixCasa1Pilar1Body, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixCasa1Pilar1Body = glm::scale(modelMatrixCasa1Pilar1Body, glm::vec3(0.8));
		casa1Pilar1.render(modelMatrixCasa1Pilar1Body);
		// Pilar 2
		glm::mat4 modelMatrixCasa1Pilar2Body = glm::mat4(modelMatrixCasa1Pilar2);
		modelMatrixCasa1Pilar2[3].y = terrain.getHeightTerrain(modelMatrixCasa1Pilar2[3].x, modelMatrixCasa1Pilar2[3].z);
		modelMatrixCasa1Pilar2Body = glm::translate(modelMatrixCasa1Pilar2Body, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixCasa1Pilar2Body = glm::scale(modelMatrixCasa1Pilar2Body, glm::vec3(0.8));
		casa1Pilar2.render(modelMatrixCasa1Pilar2Body);
		// Pilar 3
		glm::mat4 modelMatrixCasa1Pilar3Body = glm::mat4(modelMatrixCasa1Pilar3);
		modelMatrixCasa1Pilar3[3].y = terrain.getHeightTerrain(modelMatrixCasa1Pilar3[3].x, modelMatrixCasa1Pilar3[3].z);
		modelMatrixCasa1Pilar3Body = glm::translate(modelMatrixCasa1Pilar3Body, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixCasa1Pilar3Body = glm::scale(modelMatrixCasa1Pilar3Body, glm::vec3(0.8));
		casa1Pilar3.render(modelMatrixCasa1Pilar3Body);
		// Borde 1A
		glm::mat4 modelMatrixBorde1ABody = glm::mat4(modelMatrixBorde1A);
		modelMatrixBorde1A[3].y = terrain.getHeightTerrain(modelMatrixBorde1A[3].x, modelMatrixBorde1A[3].z);
		modelMatrixBorde1ABody = glm::translate(modelMatrixBorde1ABody, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixBorde1ABody = glm::scale(modelMatrixBorde1ABody, glm::vec3(0.8));
		borde1A.render(modelMatrixBorde1ABody);
		// Borde 2A
		glm::mat4 modelMatrixBorde2ABody = glm::mat4(modelMatrixBorde2A);
		modelMatrixBorde2A[3].y = terrain.getHeightTerrain(modelMatrixBorde2A[3].x, modelMatrixBorde2A[3].z);
		modelMatrixBorde2ABody = glm::translate(modelMatrixBorde2ABody, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixBorde2ABody = glm::scale(modelMatrixBorde2ABody, glm::vec3(0.8));
		borde2A.render(modelMatrixBorde2ABody);
		// Borde 3A
		glm::mat4 modelMatrixBorde3ABody = glm::mat4(modelMatrixBorde3A);
		modelMatrixBorde3A[3].y = terrain.getHeightTerrain(modelMatrixBorde3A[3].x, modelMatrixBorde3A[3].z);
		modelMatrixBorde3ABody = glm::translate(modelMatrixBorde3ABody, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixBorde3ABody = glm::scale(modelMatrixBorde3ABody, glm::vec3(0.8));
		borde3A.render(modelMatrixBorde3ABody);
		// Borde 4A
		glm::mat4 modelMatrixBorde4ABody = glm::mat4(modelMatrixBorde4A);
		modelMatrixBorde4A[3].y = terrain.getHeightTerrain(modelMatrixBorde4A[3].x, modelMatrixBorde4A[3].z);
		modelMatrixBorde4ABody = glm::translate(modelMatrixBorde4ABody, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixBorde4ABody = glm::scale(modelMatrixBorde4ABody, glm::vec3(0.8));
		borde4A.render(modelMatrixBorde4ABody);
		// Borde 5A
		glm::mat4 modelMatrixBorde5ABody = glm::mat4(modelMatrixBorde5A);
		modelMatrixBorde5A[3].y = terrain.getHeightTerrain(modelMatrixBorde5A[3].x, modelMatrixBorde5A[3].z);
		modelMatrixBorde5ABody = glm::translate(modelMatrixBorde5ABody, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixBorde5ABody = glm::scale(modelMatrixBorde5ABody, glm::vec3(0.8));
		borde5A.render(modelMatrixBorde5ABody);
		// Borde 6A
		glm::mat4 modelMatrixBorde6ABody = glm::mat4(modelMatrixBorde6A);
		modelMatrixBorde6A[3].y = terrain.getHeightTerrain(modelMatrixBorde6A[3].x, modelMatrixBorde6A[3].z);
		modelMatrixBorde6ABody = glm::translate(modelMatrixBorde6ABody, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixBorde6ABody = glm::scale(modelMatrixBorde6ABody, glm::vec3(0.8));
		borde6A.render(modelMatrixBorde6ABody);
		// Obj 1A
		glm::mat4 modelMatrixObj1aBody = glm::mat4(modelMatrixObj1a);
		modelMatrixObj1a[3].y = terrain.getHeightTerrain(modelMatrixObj1a[3].x, modelMatrixObj1a[3].z);
		modelMatrixObj1aBody = glm::translate(modelMatrixObj1aBody, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixObj1aBody = glm::scale(modelMatrixObj1aBody, glm::vec3(0.8));
		obj1a.render(modelMatrixObj1aBody);
		// Obj 1B
		glm::mat4 modelMatrixObj1bBody = glm::mat4(modelMatrixObj1b);
		modelMatrixObj1b[3].y = terrain.getHeightTerrain(modelMatrixObj1b[3].x, modelMatrixObj1b[3].z);
		modelMatrixObj1bBody = glm::translate(modelMatrixObj1bBody, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixObj1bBody = glm::scale(modelMatrixObj1bBody, glm::vec3(0.8));
		obj1b.render(modelMatrixObj1bBody);
		// Obj 2A
		glm::mat4 modelMatrixObj2aBody = glm::mat4(modelMatrixObj2a);
		modelMatrixObj2a[3].y = terrain.getHeightTerrain(modelMatrixObj2a[3].x, modelMatrixObj2a[3].z);
		modelMatrixObj2aBody = glm::translate(modelMatrixObj2aBody, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixObj2aBody = glm::scale(modelMatrixObj2aBody, glm::vec3(0.8));
		obj2a.render(modelMatrixObj2aBody);
		// Obj 2B
		glm::mat4 modelMatrixObj2bBody = glm::mat4(modelMatrixObj2b);
		modelMatrixObj2b[3].y = terrain.getHeightTerrain(modelMatrixObj2b[3].x, modelMatrixObj2b[3].z);
		modelMatrixObj2bBody = glm::translate(modelMatrixObj2bBody, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixObj2bBody = glm::scale(modelMatrixObj2bBody, glm::vec3(0.8));
		obj2b.render(modelMatrixObj2bBody);
		// Obj 3A
		glm::mat4 modelMatrixObj3aBody = glm::mat4(modelMatrixObj3a);
		modelMatrixObj3a[3].y = terrain.getHeightTerrain(modelMatrixObj3a[3].x, modelMatrixObj3a[3].z);
		modelMatrixObj3aBody = glm::translate(modelMatrixObj3aBody, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixObj3aBody = glm::scale(modelMatrixObj3aBody, glm::vec3(0.8));
		obj3a.render(modelMatrixObj3aBody);
		// Techo A
		glm::mat4 modelMatrixTechoABody = glm::mat4(modelMatrixTechoA);
		modelMatrixTechoA[3].y = terrain.getHeightTerrain(modelMatrixTechoA[3].x, modelMatrixTechoA[3].z);
		modelMatrixTechoABody = glm::translate(modelMatrixTechoABody, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixTechoABody = glm::scale(modelMatrixTechoABody, glm::vec3(0.8));
		techoA.render(modelMatrixTechoABody);
		// Techo B
		glm::mat4 modelMatrixTechoBBody = glm::mat4(modelMatrixTechoB);
		modelMatrixTechoB[3].y = terrain.getHeightTerrain(modelMatrixTechoB[3].x, modelMatrixTechoB[3].z);
		modelMatrixTechoBBody = glm::translate(modelMatrixTechoBBody, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixTechoBBody = glm::scale(modelMatrixTechoBBody, glm::vec3(0.8));
		techoB.render(modelMatrixTechoBBody);
		// Base
		glm::mat4 modelMatrixBaseBody = glm::mat4(modelMatrixBase);
		modelMatrixBase[3].y = terrain.getHeightTerrain(modelMatrixBase[3].x, modelMatrixBase[3].z);
		modelMatrixBaseBody = glm::translate(modelMatrixBaseBody, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixBaseBody = glm::scale(modelMatrixBaseBody, glm::vec3(0.8));
		base.render(modelMatrixBaseBody);
		// Puente
		glm::mat4 modelMatrixPuenteBody = glm::mat4(modelMatrixPuente);
		modelMatrixPuente[3].y = terrain.getHeightTerrain(modelMatrixPuente[3].x, modelMatrixPuente[3].z);
		modelMatrixPuenteBody = glm::translate(modelMatrixPuenteBody, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixPuenteBody = glm::scale(modelMatrixPuenteBody, glm::vec3(0.8));
		puente.render(modelMatrixPuenteBody);
		// Techo
		glm::mat4 modelMatrixTechoBody = glm::mat4(modelMatrixTecho);
		modelMatrixTecho[3].y = terrain.getHeightTerrain(modelMatrixTecho[3].x, modelMatrixTecho[3].z);
		modelMatrixTechoBody = glm::translate(modelMatrixTechoBody, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixTechoBody = glm::scale(modelMatrixTechoBody, glm::vec3(0.8));
		techo.render(modelMatrixTechoBody);
		// Bordes
		glm::mat4 modelMatrixBordesBody = glm::mat4(modelMatrixBordes);
		modelMatrixBordes[3].y = terrain.getHeightTerrain(modelMatrixBordes[3].x, modelMatrixBordes[3].z);
		modelMatrixBordesBody = glm::translate(modelMatrixBordesBody, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixBordesBody = glm::scale(modelMatrixBordesBody, glm::vec3(0.8));
		bordes.render(modelMatrixBordesBody);
		// Torre Pared 1
		glm::mat4 modelMatrixTorrePared1Body = glm::mat4(modelMatrixTorrePared1);
		modelMatrixTorrePared1[3].y = terrain.getHeightTerrain(modelMatrixTorrePared1[3].x, modelMatrixTorrePared1[3].z);
		modelMatrixTorrePared1Body = glm::translate(modelMatrixTorrePared1Body, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixTorrePared1Body = glm::scale(modelMatrixTorrePared1Body, glm::vec3(0.8));
		torrePared1.render(modelMatrixTorrePared1Body);
		// Torre Pared 2
		glm::mat4 modelMatrixTorrePared2Body = glm::mat4(modelMatrixTorrePared2);
		modelMatrixTorrePared2[3].y = terrain.getHeightTerrain(modelMatrixTorrePared2[3].x, modelMatrixTorrePared2[3].z);
		modelMatrixTorrePared2Body = glm::translate(modelMatrixTorrePared2Body, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixTorrePared2Body = glm::scale(modelMatrixTorrePared2Body, glm::vec3(0.8));
		torrePared2.render(modelMatrixTorrePared2Body);
		// Torre Pared 3
		glm::mat4 modelMatrixTorrePared3Body = glm::mat4(modelMatrixTorrePared3);
		modelMatrixTorrePared3[3].y = terrain.getHeightTerrain(modelMatrixTorrePared3[3].x, modelMatrixTorrePared3[3].z);
		modelMatrixTorrePared3Body = glm::translate(modelMatrixTorrePared3Body, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixTorrePared3Body = glm::scale(modelMatrixTorrePared3Body, glm::vec3(0.8));
		torrePared3.render(modelMatrixTorrePared3Body);
		// Torre Entrada
		glm::mat4 modelMatrixTorreEntradaBody = glm::mat4(modelMatrixTorreEntrada);
		modelMatrixTorreEntrada[3].y = terrain.getHeightTerrain(modelMatrixTorreEntrada[3].x, modelMatrixTorreEntrada[3].z);
		modelMatrixTorreEntradaBody = glm::translate(modelMatrixTorreEntradaBody, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixTorreEntradaBody = glm::scale(modelMatrixTorreEntradaBody, glm::vec3(0.8));
		torreEntrada.render(modelMatrixTorreEntradaBody);
		// Torre Puerta
		glm::mat4 modelMatrixTorrePuertaBody = glm::mat4(modelMatrixTorrePuerta);
		modelMatrixTorrePuerta[3].y = terrain.getHeightTerrain(modelMatrixTorrePuerta[3].x, modelMatrixTorrePuerta[3].z);
		modelMatrixTorrePuertaBody = glm::translate(modelMatrixTorrePuertaBody, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixTorrePuertaBody = glm::scale(modelMatrixTorrePuertaBody, glm::vec3(0.8));
		torrePuerta.render(modelMatrixTorrePuertaBody);
		// Casa 2 
		// Entrada
		glm::mat4 modelMatrixCasa2EntradaBody = glm::mat4(modelMatrixCasa2Entrada);
		modelMatrixCasa2Entrada[3].y = terrain.getHeightTerrain(modelMatrixCasa2Entrada[3].x, modelMatrixCasa2Entrada[3].z);
		modelMatrixCasa2EntradaBody = glm::translate(modelMatrixCasa2EntradaBody, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixCasa2EntradaBody = glm::scale(modelMatrixCasa2EntradaBody, glm::vec3(0.8));
		casa2Entrada.render(modelMatrixCasa2EntradaBody);
		// Pared A
		glm::mat4 modelMatrixCasa2ParedABody = glm::mat4(modelMatrixCasa2ParedA);
		modelMatrixCasa2ParedA[3].y = terrain.getHeightTerrain(modelMatrixCasa2ParedA[3].x, modelMatrixCasa2ParedA[3].z);
		modelMatrixCasa2ParedABody = glm::translate(modelMatrixCasa2ParedABody, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixCasa2ParedABody = glm::scale(modelMatrixCasa2ParedABody, glm::vec3(0.8));
		casa2ParedA.render(modelMatrixCasa2ParedABody);
		// Pared B
		glm::mat4 modelMatrixCasa2ParedBBody = glm::mat4(modelMatrixCasa2ParedB);
		modelMatrixCasa2ParedB[3].y = terrain.getHeightTerrain(modelMatrixCasa2ParedB[3].x, modelMatrixCasa2ParedB[3].z);
		modelMatrixCasa2ParedBBody = glm::scale(modelMatrixCasa2ParedBBody, glm::vec3(0.8));
		casa2ParedB.render(modelMatrixCasa2ParedBBody);
		// Pared C
		glm::mat4 modelMatrixCasa2ParedCBody = glm::mat4(modelMatrixCasa2ParedC);
		modelMatrixCasa2ParedC[3].y = terrain.getHeightTerrain(modelMatrixCasa2ParedC[3].x, modelMatrixCasa2ParedC[3].z);
		modelMatrixCasa2ParedCBody = glm::scale(modelMatrixCasa2ParedCBody, glm::vec3(0.8));
		casa2ParedC.render(modelMatrixCasa2ParedCBody);
		// Pared D
		glm::mat4 modelMatrixCasa2ParedDBody = glm::mat4(modelMatrixCasa2ParedD);
		modelMatrixCasa2ParedD[3].y = terrain.getHeightTerrain(modelMatrixCasa2ParedD[3].x, modelMatrixCasa2ParedD[3].z);
		modelMatrixCasa2ParedDBody = glm::scale(modelMatrixCasa2ParedDBody, glm::vec3(0.8));
		casa2ParedD.render(modelMatrixCasa2ParedDBody);
		// Pared E
		glm::mat4 modelMatrixCasa2ParedEBody = glm::mat4(modelMatrixCasa2ParedE);
		modelMatrixCasa2ParedE[3].y = terrain.getHeightTerrain(modelMatrixCasa2ParedE[3].x, modelMatrixCasa2ParedE[3].z);
		modelMatrixCasa2ParedEBody = glm::scale(modelMatrixCasa2ParedEBody, glm::vec3(0.8));
		casa2ParedE.render(modelMatrixCasa2ParedEBody);
		// Pared F
		glm::mat4 modelMatrixCasa2ParedFBody = glm::mat4(modelMatrixCasa2ParedF);
		modelMatrixCasa2ParedF[3].y = terrain.getHeightTerrain(modelMatrixCasa2ParedF[3].x, modelMatrixCasa2ParedF[3].z);
		modelMatrixCasa2ParedFBody = glm::scale(modelMatrixCasa2ParedFBody, glm::vec3(0.8));
		casa2ParedF.render(modelMatrixCasa2ParedFBody);
		// Pared G
		glm::mat4 modelMatrixCasa2ParedGBody = glm::mat4(modelMatrixCasa2ParedG);
		modelMatrixCasa2ParedG[3].y = terrain.getHeightTerrain(modelMatrixCasa2ParedG[3].x, modelMatrixCasa2ParedG[3].z);
		modelMatrixCasa2ParedGBody = glm::scale(modelMatrixCasa2ParedGBody, glm::vec3(0.8));
		casa2ParedG.render(modelMatrixCasa2ParedGBody);
		// Pared H
		glm::mat4 modelMatrixCasa2ParedHBody = glm::mat4(modelMatrixCasa2ParedH);
		modelMatrixCasa2ParedH[3].y = terrain.getHeightTerrain(modelMatrixCasa2ParedH[3].x, modelMatrixCasa2ParedH[3].z);
		modelMatrixCasa2ParedHBody = glm::scale(modelMatrixCasa2ParedHBody, glm::vec3(0.8));
		casa2ParedH.render(modelMatrixCasa2ParedHBody);
		// Pared I
		glm::mat4 modelMatrixCasa2ParedIBody = glm::mat4(modelMatrixCasa2ParedI);
		modelMatrixCasa2ParedI[3].y = terrain.getHeightTerrain(modelMatrixCasa2ParedI[3].x, modelMatrixCasa2ParedI[3].z);
		modelMatrixCasa2ParedIBody = glm::scale(modelMatrixCasa2ParedIBody, glm::vec3(0.8));
		casa2ParedI.render(modelMatrixCasa2ParedIBody);
		// Techo A
		glm::mat4 modelMatrixCasa2TechoABody = glm::mat4(modelMatrixCasa2TechoA);
		modelMatrixCasa2TechoA[3].y = terrain.getHeightTerrain(modelMatrixCasa2TechoA[3].x, modelMatrixCasa2TechoA[3].z);
		modelMatrixCasa2TechoABody = glm::scale(modelMatrixCasa2TechoABody, glm::vec3(0.8));
		casa2TechoA.render(modelMatrixCasa2TechoABody);
		// Techo B
		glm::mat4 modelMatrixCasa2TechoBBody = glm::mat4(modelMatrixCasa2TechoB);
		modelMatrixCasa2TechoB[3].y = terrain.getHeightTerrain(modelMatrixCasa2TechoB[3].x, modelMatrixCasa2TechoB[3].z);
		modelMatrixCasa2TechoBBody = glm::scale(modelMatrixCasa2TechoBBody, glm::vec3(0.8));
		casa2TechoB.render(modelMatrixCasa2TechoBBody);
		// Techo C
		glm::mat4 modelMatrixCasa2TechoCBody = glm::mat4(modelMatrixCasa2TechoC);
		modelMatrixCasa2TechoC[3].y = terrain.getHeightTerrain(modelMatrixCasa2TechoC[3].x, modelMatrixCasa2TechoC[3].z);
		modelMatrixCasa2TechoCBody = glm::scale(modelMatrixCasa2TechoCBody, glm::vec3(0.8));
		casa2TechoC.render(modelMatrixCasa2TechoCBody);
		// Ventana A
		glm::mat4 modelMatrixCasa2VentanaABody = glm::mat4(modelMatrixCasa2VentanaA);
		modelMatrixCasa2VentanaA[3].y = terrain.getHeightTerrain(modelMatrixCasa2VentanaA[3].x, modelMatrixCasa2VentanaA[3].z);
		modelMatrixCasa2VentanaABody = glm::translate(modelMatrixCasa2VentanaABody, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixCasa2VentanaABody = glm::scale(modelMatrixCasa2VentanaABody, glm::vec3(0.8));
		casa2VentanaA.render(modelMatrixCasa2VentanaABody);
		// Ventana B
		glm::mat4 modelMatrixCasa2VentanaBBody = glm::mat4(modelMatrixCasa2VentanaB);
		modelMatrixCasa2VentanaB[3].y = terrain.getHeightTerrain(modelMatrixCasa2VentanaB[3].x, modelMatrixCasa2VentanaB[3].z);
		modelMatrixCasa2VentanaBBody = glm::translate(modelMatrixCasa2VentanaBBody, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixCasa2VentanaBBody = glm::scale(modelMatrixCasa2VentanaBBody, glm::vec3(0.8));
		casa2VentanaB.render(modelMatrixCasa2VentanaBBody);
		// Ventana C
		glm::mat4 modelMatrixCasa2VentanaCBody = glm::mat4(modelMatrixCasa2VentanaC);
		modelMatrixCasa2VentanaC[3].y = terrain.getHeightTerrain(modelMatrixCasa2VentanaC[3].x, modelMatrixCasa2VentanaC[3].z);
		modelMatrixCasa2VentanaCBody = glm::translate(modelMatrixCasa2VentanaCBody, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixCasa2VentanaCBody = glm::scale(modelMatrixCasa2VentanaCBody, glm::vec3(0.8));
		casa2VentanaC.render(modelMatrixCasa2VentanaCBody);
		// Ventana D
		glm::mat4 modelMatrixCasa2VentanaDBody = glm::mat4(modelMatrixCasa2VentanaD);
		modelMatrixCasa2VentanaD[3].y = terrain.getHeightTerrain(modelMatrixCasa2VentanaD[3].x, modelMatrixCasa2VentanaD[3].z);
		modelMatrixCasa2VentanaDBody = glm::translate(modelMatrixCasa2VentanaDBody, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixCasa2VentanaDBody = glm::scale(modelMatrixCasa2VentanaDBody, glm::vec3(0.8));
		casa2VentanaD.render(modelMatrixCasa2VentanaDBody);
		// Casa 3
		// Base A
		glm::mat4 modelMatrixCasa3BaseABody = glm::mat4(modelMatrixCasa3BaseA);
		modelMatrixCasa3BaseA[3].y = terrain.getHeightTerrain(modelMatrixCasa3BaseA[3].x, modelMatrixCasa3BaseA[3].z);
		modelMatrixCasa3BaseABody = glm::scale(modelMatrixCasa3BaseABody, glm::vec3(0.8));
		casa3BaseA.render(modelMatrixCasa3BaseABody);
		// Base B
		glm::mat4 modelMatrixCasa3BaseBBody = glm::mat4(modelMatrixCasa3BaseB);
		modelMatrixCasa3BaseB[3].y = terrain.getHeightTerrain(modelMatrixCasa3BaseB[3].x, modelMatrixCasa3BaseB[3].z);
		modelMatrixCasa3BaseBBody = glm::scale(modelMatrixCasa3BaseBBody, glm::vec3(0.8));
		casa3BaseB.render(modelMatrixCasa3BaseBBody);
		// Techo A
		glm::mat4 modelMatrixCasa3TechoABody = glm::mat4(modelMatrixCasa3TechoA);
		modelMatrixCasa3TechoA[3].y = terrain.getHeightTerrain(modelMatrixCasa3TechoA[3].x, modelMatrixCasa3TechoA[3].z);
		modelMatrixCasa3TechoABody = glm::scale(modelMatrixCasa3TechoABody, glm::vec3(0.8));
		casa3TechoA.render(modelMatrixCasa3TechoABody);
		// Techo B
		glm::mat4 modelMatrixCasa3TechoBBody = glm::mat4(modelMatrixCasa3TechoB);
		modelMatrixCasa3TechoB[3].y = terrain.getHeightTerrain(modelMatrixCasa3TechoB[3].x, modelMatrixCasa3TechoB[3].z);
		modelMatrixCasa3TechoBBody = glm::scale(modelMatrixCasa3TechoBBody, glm::vec3(0.8));
		casa3TechoB.render(modelMatrixCasa3TechoBBody);
		// Ventana A
		glm::mat4 modelMatrixCasa3VentanaABody = glm::mat4(modelMatrixCasa3VentanaA);
		modelMatrixCasa3VentanaA[3].y = terrain.getHeightTerrain(modelMatrixCasa3VentanaA[3].x, modelMatrixCasa3VentanaA[3].z);
		modelMatrixCasa3VentanaABody = glm::translate(modelMatrixCasa3VentanaABody, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixCasa3VentanaABody = glm::scale(modelMatrixCasa3VentanaABody, glm::vec3(0.8));
		casa3VentanaA.render(modelMatrixCasa3VentanaABody);
		// Puerta A
		glm::mat4 modelMatrixCasa3PuertaABody = glm::mat4(modelMatrixCasa3PuertaA);
		modelMatrixCasa3PuertaA[3].y = terrain.getHeightTerrain(modelMatrixCasa3PuertaA[3].x, modelMatrixCasa3PuertaA[3].z);
		modelMatrixCasa3PuertaABody = glm::translate(modelMatrixCasa3PuertaABody, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixCasa3PuertaABody = glm::scale(modelMatrixCasa3PuertaABody, glm::vec3(0.8));
		casa3PuertaA.render(modelMatrixCasa3PuertaABody);
		// Puerta B
		glm::mat4 modelMatrixCasa3PuertaBBody = glm::mat4(modelMatrixCasa3PuertaB);
		modelMatrixCasa3PuertaB[3].y = terrain.getHeightTerrain(modelMatrixCasa3PuertaB[3].x, modelMatrixCasa3PuertaB[3].z);
		modelMatrixCasa3PuertaBBody = glm::translate(modelMatrixCasa3PuertaBBody, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixCasa3PuertaBBody = glm::scale(modelMatrixCasa3PuertaBBody, glm::vec3(0.8));
		casa3PuertaB.render(modelMatrixCasa3PuertaBBody);
		// Puerta C
		glm::mat4 modelMatrixCasa3PuertaCBody = glm::mat4(modelMatrixCasa3PuertaC);
		modelMatrixCasa3PuertaC[3].y = terrain.getHeightTerrain(modelMatrixCasa3PuertaC[3].x, modelMatrixCasa3PuertaC[3].z);
		modelMatrixCasa3PuertaCBody = glm::translate(modelMatrixCasa3PuertaCBody, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixCasa3PuertaCBody = glm::scale(modelMatrixCasa3PuertaCBody, glm::vec3(0.8));
		casa3PuertaC.render(modelMatrixCasa3PuertaCBody);
		// Objetos
		for(int i = 0; i < boxPositions.size(); i++) {
			glm::vec3 pos = boxPositions[i].first;
			pos.y = terrain.getHeightTerrain(pos.x, pos.z) + pos.y;
			caja.setPosition(pos);
			caja.setScale(glm::vec3(0.8f));
			caja.setOrientation(glm::vec3(0, boxPositions[i].second, 0));
			caja.render();
		}
		for(int i = 0; i < gemPositions.size(); i++) {
			glm::vec3 pos = gemPositions[i].first;
			pos.y = terrain.getHeightTerrain(pos.x, pos.z) + pos.y;
			gem.setPosition(pos);
			gem.setScale(glm::vec3(0.8f));
			gem.setOrientation(glm::vec3(0, gemPositions[i].second, 0));
			gem.render();
		}
		// Llave
		glm::mat4 modelMatrixCasa1KeyBody = glm::mat4(modelMatrixCasa1Key);
		modelMatrixCasa1Key[3].y = terrain.getHeightTerrain(modelMatrixCasa1Key[3].x, modelMatrixCasa1Key[3].z);
		modelMatrixCasa1KeyBody = glm::translate(modelMatrixCasa1KeyBody, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixCasa1KeyBody = glm::scale(modelMatrixCasa1KeyBody, glm::vec3(0.8));
		key.render(modelMatrixCasa1KeyBody);
		// Árbol
		glm::mat4 modelMatrixBigTreeBody = glm::mat4(modelMatrixBigTree);
		modelMatrixBigTree[3].y = terrain.getHeightTerrain(modelMatrixBigTree[3].x, modelMatrixBigTree[3].z);
		modelMatrixBigTreeBody = glm::translate(modelMatrixBigTreeBody, glm::vec3(0.0, 0.0, 0.0));
		modelMatrixBigTreeBody = glm::scale(modelMatrixBigTreeBody, glm::vec3(1.0));
		bigTree.render(modelMatrixBigTreeBody);
		/*******************************************
		* Objetos animados por huesos
		*******************************************/
		// --- Física de Cat ---
		// 0. Resetear flag de suelo al inicio de cada frame
		catOnGround = false;

		// 1. Aplicar gravedad y velocidad vertical
		catVelY -= GRAVITY * (float)deltaTime;
		modelMatrixCat[3].y += catVelY * (float)deltaTime;

		// 2. Comprobar colisión con el terreno como suelo
		//    Obtenemos la altura del terreno en la posición XZ de Cat.
		float terrainHeight = terrain.getHeightTerrain(modelMatrixCat[3][0], modelMatrixCat[3][2]);
		bool catOnTerrain = false;
		if (modelMatrixCat[3].y <= terrainHeight) {
			// Cat toca o está debajo del terreno: asentarlo encima
			modelMatrixCat[3].y = terrainHeight;
			if (catVelY < 0.0f) catVelY = 0.0f;
			catOnGround  = true;
			catOnTerrain = true;
		}

		// 3. Orientar Cat según la superficie donde está apoyado
		//    - Sobre terreno: usar la normal del terreno para inclinar el modelo.
		//    - En el aire o sobre plataforma (OBB): normal vertical (0,1,0).
		glm::vec3 ejeyCat;
		if (catOnTerrain) {
			ejeyCat = glm::normalize(terrain.getNormalTerrain(modelMatrixCat[3][0], modelMatrixCat[3][2]));
		} else {
			ejeyCat = glm::vec3(0.0f, 1.0f, 0.0f);
		}
		glm::vec3 ejexCat = glm::vec3(modelMatrixCat[0]);
		glm::vec3 ejezCat = glm::normalize(glm::cross(ejexCat, ejeyCat));
		ejexCat = glm::normalize(glm::cross(ejeyCat, ejezCat));
		modelMatrixCat[0] = glm::vec4(ejexCat, 0.0);
		modelMatrixCat[1] = glm::vec4(ejeyCat, 0.0);
		modelMatrixCat[2] = glm::vec4(ejezCat, 0.0);

		// Modelo real
		glm::mat4 modelMatrixCatBody = glm::mat4(modelMatrixCat);
		modelMatrixCatBody = glm::scale(modelMatrixCatBody, glm::vec3(0.0002));
		if (animationCatIndex == 5) {
			catModelAnimate.setAnimationLoop(false);
		} else {
			catModelAnimate.setAnimationLoop(true);
		}
		catModelAnimate.setAnimationIndex(animationCatIndex);						// Ligar animación
		catModelAnimate.render(modelMatrixCatBody);
		animationCatIndex = 0;													// Vuelve a idle si no hay input

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
		// Collider del gato
		glm::mat4 modelColliderCat = glm::mat4(modelMatrixCat);
		AbstractModel::OBB colliderCat;
		colliderCat.u = glm::quat_cast(modelColliderCat);
		modelColliderCat = glm::scale(modelColliderCat, glm::vec3(0.02));
		modelColliderCat = glm::translate(modelColliderCat, catModelAnimate.getObb().c);	// Punto central de la caja
		colliderCat.c = modelColliderCat[3];
		colliderCat.e = catModelAnimate.getObb().e * glm::vec3(0.02);
		addOrUpdateColliders(collidersOBB, "cat", colliderCat, modelMatrixCat);

		// Colliders de escenario
		// Casa 1
		// Pared 1A
		glm::mat4 modelColliderCasa1Pared1a = glm::mat4(modelMatrixCasa1Pared1a);			// Modelo
		AbstractModel::OBB colliderCasa1Pared1a;									// Collider
		colliderCasa1Pared1a.u = glm::quat_cast(modelColliderCasa1Pared1a);					// Mat4x4 a quaternion
		modelColliderCasa1Pared1a = glm::scale(modelColliderCasa1Pared1a, glm::vec3(0.8)); // Colocar el escalamiento que tenía el objeto
		modelColliderCasa1Pared1a = glm::translate(modelMatrixCasa1Pared1aBody, casa1Pared1a.getObb().c); 	// Mover al centro de la caja
		colliderCasa1Pared1a.c = modelColliderCasa1Pared1a[3];
		colliderCasa1Pared1a.e = casa1Pared1a.getObb().e * glm::vec3(0.8);	// Escalamiento de 1
		addOrUpdateColliders(collidersOBB, "casa1Pared1a", colliderCasa1Pared1a, modelColliderCasa1Pared1a);
		// Pared 1B
		glm::mat4 modelColliderCasa1Pared1b = glm::mat4(modelMatrixCasa1Pared1b);			// Modelo
		AbstractModel::OBB colliderCasa1Pared1b;									// Collider
		colliderCasa1Pared1b.u = glm::quat_cast(modelColliderCasa1Pared1b);					// Mat4x4 a quaternion
		modelColliderCasa1Pared1b = glm::scale(modelColliderCasa1Pared1b, glm::vec3(0.8)); // Colocar el escalamiento que tenía el objeto
		modelColliderCasa1Pared1b = glm::translate(modelMatrixCasa1Pared1bBody, casa1Pared1b.getObb().c); 	// Mover al centro de la caja
		colliderCasa1Pared1b.c = modelColliderCasa1Pared1b[3];
		colliderCasa1Pared1b.e = casa1Pared1b.getObb().e * glm::vec3(0.8);	// Escalamiento de 1
		addOrUpdateColliders(collidersOBB, "casa1Pared1b", colliderCasa1Pared1b, modelColliderCasa1Pared1b);
		// Pared 1C
		glm::mat4 modelColliderCasa1Pared1c = glm::mat4(modelMatrixCasa1Pared1c);			// Modelo
		AbstractModel::OBB colliderCasa1Pared1c;									// Collider
		colliderCasa1Pared1c.u = glm::quat_cast(modelColliderCasa1Pared1c);					// Mat4x4 a quaternion
		modelColliderCasa1Pared1c = glm::scale(modelColliderCasa1Pared1c, glm::vec3(0.8)); // Colocar el escalamiento que tenía el objeto
		modelColliderCasa1Pared1c = glm::translate(modelMatrixCasa1Pared1cBody, casa1Pared1c.getObb().c); 	// Mover al centro de la caja
		colliderCasa1Pared1c.c = modelColliderCasa1Pared1c[3];
		colliderCasa1Pared1c.e = casa1Pared1c.getObb().e * glm::vec3(0.8);	// Escalamiento de 1
		addOrUpdateColliders(collidersOBB, "casa1Pared1c", colliderCasa1Pared1c, modelColliderCasa1Pared1c);
		// Techo 1A
		glm::mat4 modelColliderCasa1Techo1a = glm::mat4(modelMatrixCasa1Techo1a);			// Modelo
		AbstractModel::OBB colliderCasa1Techo1a;									// Collider
		colliderCasa1Techo1a.u = glm::quat_cast(modelColliderCasa1Techo1a);					// Mat4x4 a quaternion
		modelColliderCasa1Techo1a = glm::scale(modelColliderCasa1Techo1a, glm::vec3(0.8)); // Colocar el escalamiento que tenía el objeto
		modelColliderCasa1Techo1a = glm::translate(modelMatrixCasa1Pared1cBody, casa1Techo1a.getObb().c); 	// Mover al centro de la caja
		colliderCasa1Techo1a.c = modelColliderCasa1Techo1a[3];
		colliderCasa1Techo1a.e = casa1Techo1a.getObb().e * glm::vec3(0.8);	// Escalamiento de 1
		addOrUpdateColliders(collidersOBB, "casa1Techo1a", colliderCasa1Techo1a, modelColliderCasa1Techo1a);
		// Pared 2A
		glm::mat4 modelColliderCasa1Pared2a = glm::mat4(modelMatrixCasa1Pared2a);			// Modelo
		AbstractModel::OBB colliderCasa1Pared2a;									// Collider
		colliderCasa1Pared2a.u = glm::quat_cast(modelColliderCasa1Pared2a);					// Mat4x4 a quaternion
		modelColliderCasa1Pared2a = glm::scale(modelColliderCasa1Pared2a, glm::vec3(0.8)); // Colocar el escalamiento que tenía el objeto
		modelColliderCasa1Pared2a = glm::translate(modelMatrixCasa1Pared2aBody, casa1Pared2a.getObb().c); 	// Mover al centro de la caja
		colliderCasa1Pared2a.c = modelColliderCasa1Pared2a[3];
		colliderCasa1Pared2a.e = casa1Pared2a.getObb().e * glm::vec3(0.8);	// Escalamiento de 1
		addOrUpdateColliders(collidersOBB, "casa1Pared2a", colliderCasa1Pared2a, modelColliderCasa1Pared2a);
		// Pared 2B
		glm::mat4 modelColliderCasa1Pared2b = glm::mat4(modelMatrixCasa1Pared2b);			// Modelo
		AbstractModel::OBB colliderCasa1Pared2b;									// Collider
		colliderCasa1Pared2b.u = glm::quat_cast(modelColliderCasa1Pared2b);					// Mat4x4 a quaternion
		modelColliderCasa1Pared2b = glm::scale(modelColliderCasa1Pared2b, glm::vec3(0.8)); // Colocar el escalamiento que tenía el objeto
		modelColliderCasa1Pared2b = glm::translate(modelMatrixCasa1Pared2bBody, casa1Pared2b.getObb().c); 	// Mover al centro de la caja
		colliderCasa1Pared2b.c = modelColliderCasa1Pared2b[3];
		colliderCasa1Pared2b.e = casa1Pared2b.getObb().e * glm::vec3(0.8);	// Escalamiento de 1
		addOrUpdateColliders(collidersOBB, "casa1Pared2b", colliderCasa1Pared2b, modelColliderCasa1Pared2b);
		// Pared C
		glm::mat4 modelColliderCasa1Pared2c = glm::mat4(modelMatrixCasa1Pared2c);			// Modelo
		AbstractModel::OBB colliderCasa1Pared2c;									// Collider
		colliderCasa1Pared2c.u = glm::quat_cast(modelColliderCasa1Pared2c);					// Mat4x4 a quaternion
		modelColliderCasa1Pared2c = glm::scale(modelColliderCasa1Pared2c, glm::vec3(0.8)); // Colocar el escalamiento que tenía el objeto
		modelColliderCasa1Pared2c = glm::translate(modelMatrixCasa1Pared2cBody, casa1Pared2c.getObb().c); 	// Mover al centro de la caja
		colliderCasa1Pared2c.c = modelColliderCasa1Pared2c[3];
		colliderCasa1Pared2c.e = casa1Pared2c.getObb().e * glm::vec3(0.8);	// Escalamiento de 1
		addOrUpdateColliders(collidersOBB, "casa1Pared2c", colliderCasa1Pared2c, modelColliderCasa1Pared2c);
		// Techo 2A
		glm::mat4 modelColliderCasa1Techo2a = glm::mat4(modelMatrixCasa1Techo2a);			// Modelo
		AbstractModel::OBB colliderCasa1Techo2a;									// Collider
		colliderCasa1Techo2a.u = glm::quat_cast(modelColliderCasa1Techo2a);					// Mat4x4 a quaternion
		modelColliderCasa1Techo2a = glm::scale(modelColliderCasa1Techo2a, glm::vec3(0.8)); // Colocar el escalamiento que tenía el objeto
		modelColliderCasa1Techo2a = glm::translate(modelMatrixCasa1Pared1cBody, casa1Techo2a.getObb().c); 	// Mover al centro de la caja
		colliderCasa1Techo2a.c = modelColliderCasa1Techo2a[3];
		colliderCasa1Techo2a.e = casa1Techo2a.getObb().e * glm::vec3(0.8);	// Escalamiento de 1
		addOrUpdateColliders(collidersOBB, "casa1Techo2a", colliderCasa1Techo2a, modelColliderCasa1Techo2a);
		// Techo 2B
		glm::mat4 modelColliderCasa1Techo2b = glm::mat4(modelMatrixCasa1Techo2b);			// Modelo
		AbstractModel::OBB colliderCasa1Techo2b;									// Collider
		colliderCasa1Techo2b.u = glm::quat_cast(modelColliderCasa1Techo2b);					// Mat4x4 a quaternion
		modelColliderCasa1Techo2b = glm::scale(modelColliderCasa1Techo2b, glm::vec3(0.8)); // Colocar el escalamiento que tenía el objeto
		modelColliderCasa1Techo2b = glm::translate(modelMatrixCasa1Pared1cBody, casa1Techo2b.getObb().c); 	// Mover al centro de la caja
		colliderCasa1Techo2b.c = modelColliderCasa1Techo2b[3];
		colliderCasa1Techo2b.e = casa1Techo2b.getObb().e * glm::vec3(0.8);	// Escalamiento de 1
		addOrUpdateColliders(collidersOBB, "casa1Techo2b", colliderCasa1Techo2b, modelColliderCasa1Techo2b);
		// Techo 2C
		glm::mat4 modelColliderCasa1Techo2c = glm::mat4(modelMatrixCasa1Techo2c);			// Modelo
		AbstractModel::OBB colliderCasa1Techo2c;									// Collider
		colliderCasa1Techo2c.u = glm::quat_cast(modelColliderCasa1Techo2c);					// Mat4x4 a quaternion
		modelColliderCasa1Techo2c = glm::scale(modelColliderCasa1Techo2c, glm::vec3(0.8)); // Colocar el escalamiento que tenía el objeto
		modelColliderCasa1Techo2c = glm::translate(modelMatrixCasa1Pared1cBody, casa1Techo2c.getObb().c); 	// Mover al centro de la caja
		colliderCasa1Techo2c.c = modelColliderCasa1Techo2c[3];
		colliderCasa1Techo2c.e = casa1Techo2c.getObb().e * glm::vec3(0.8);	// Escalamiento de 1
		addOrUpdateColliders(collidersOBB, "casa1Techo2c", colliderCasa1Techo2c, modelColliderCasa1Techo2c);
		// Pared 3A
		glm::mat4 modelColliderCasa1Pared3a = glm::mat4(modelMatrixCasa1Pared3a);
		AbstractModel::OBB colliderCasa1Pared3a;
		colliderCasa1Pared3a.u = glm::quat_cast(modelColliderCasa1Pared3a);
		modelColliderCasa1Pared3a = glm::scale(modelColliderCasa1Pared3a, glm::vec3(0.8));
		modelColliderCasa1Pared3a = glm::translate(modelMatrixCasa1Pared3aBody, casa1Pared3a.getObb().c);
		colliderCasa1Pared3a.c = modelColliderCasa1Pared3a[3];
		colliderCasa1Pared3a.e = casa1Pared3a.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "casa1Pared3a", colliderCasa1Pared3a, modelColliderCasa1Pared3a);
		// Pared 3B
		glm::mat4 modelColliderCasa1Pared3b = glm::mat4(modelMatrixCasa1Pared3b);
		AbstractModel::OBB colliderCasa1Pared3b;
		colliderCasa1Pared3b.u = glm::quat_cast(modelColliderCasa1Pared3b);
		modelColliderCasa1Pared3b = glm::scale(modelColliderCasa1Pared3b, glm::vec3(0.8));
		modelColliderCasa1Pared3b = glm::translate(modelMatrixCasa1Pared3bBody, casa1Pared3b.getObb().c);
		colliderCasa1Pared3b.c = modelColliderCasa1Pared3b[3];
		colliderCasa1Pared3b.e = casa1Pared3b.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "casa1Pared3b", colliderCasa1Pared3b, modelColliderCasa1Pared3b);
		// Pared 3C
		glm::mat4 modelColliderCasa1Pared3c = glm::mat4(modelMatrixCasa1Pared3c);
		AbstractModel::OBB colliderCasa1Pared3c;
		colliderCasa1Pared3c.u = glm::quat_cast(modelColliderCasa1Pared3c);
		modelColliderCasa1Pared3c = glm::scale(modelColliderCasa1Pared3c, glm::vec3(0.8));
		modelColliderCasa1Pared3c = glm::translate(modelMatrixCasa1Pared3cBody, casa1Pared3c.getObb().c);
		colliderCasa1Pared3c.c = modelColliderCasa1Pared3c[3];
		colliderCasa1Pared3c.e = casa1Pared3c.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "casa1Pared3c", colliderCasa1Pared3c, modelColliderCasa1Pared3c);
		// Door 1
		glm::mat4 modelColliderCasa1Door1 = glm::mat4(modelMatrixCasa1Door1);
		AbstractModel::OBB colliderCasa1Door1;
		colliderCasa1Door1.u = glm::quat_cast(modelColliderCasa1Door1);
		modelColliderCasa1Door1 = glm::scale(modelColliderCasa1Door1, glm::vec3(0.8));
		modelColliderCasa1Door1 = glm::translate(modelMatrixCasa1Door1Body, casa1Door1.getObb().c);
		colliderCasa1Door1.c = modelColliderCasa1Door1[3];
		colliderCasa1Door1.e = casa1Door1.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "casa1Door1", colliderCasa1Door1, modelColliderCasa1Door1);
		// Techo 3A
		glm::mat4 modelColliderCasa1Techo3a = glm::mat4(modelMatrixCasa1Techo3a);
		AbstractModel::OBB colliderCasa1Techo3a;
		colliderCasa1Techo3a.u = glm::quat_cast(modelColliderCasa1Techo3a);
		modelColliderCasa1Techo3a = glm::scale(modelColliderCasa1Techo3a, glm::vec3(0.8));
		modelColliderCasa1Techo3a = glm::translate(modelMatrixCasa1Techo3aBody, casa1Techo3a.getObb().c);
		colliderCasa1Techo3a.c = modelColliderCasa1Techo3a[3];
		colliderCasa1Techo3a.e = casa1Techo3a.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "casa1Techo3a", colliderCasa1Techo3a, modelColliderCasa1Techo3a);
		// Pared 4A
		glm::mat4 modelColliderCasa1Pared4a = glm::mat4(modelMatrixCasa1Pared4a);
		AbstractModel::OBB colliderCasa1Pared4a;
		colliderCasa1Pared4a.u = glm::quat_cast(modelColliderCasa1Pared4a);
		modelColliderCasa1Pared4a = glm::scale(modelColliderCasa1Pared4a, glm::vec3(0.8));
		modelColliderCasa1Pared4a = glm::translate(modelMatrixCasa1Pared4aBody, casa1Pared4a.getObb().c);
		colliderCasa1Pared4a.c = modelColliderCasa1Pared4a[3];
		colliderCasa1Pared4a.e = casa1Pared4a.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "casa1Pared4a", colliderCasa1Pared4a, modelColliderCasa1Pared4a);
		// Pared 4B
		glm::mat4 modelColliderCasa1Pared4b = glm::mat4(modelMatrixCasa1Pared4b);
		AbstractModel::OBB colliderCasa1Pared4b;
		colliderCasa1Pared4b.u = glm::quat_cast(modelColliderCasa1Pared4b);
		modelColliderCasa1Pared4b = glm::scale(modelColliderCasa1Pared4b, glm::vec3(0.8));
		modelColliderCasa1Pared4b = glm::translate(modelMatrixCasa1Pared4bBody, casa1Pared4b.getObb().c);
		colliderCasa1Pared4b.c = modelColliderCasa1Pared4b[3];
		colliderCasa1Pared4b.e = casa1Pared4b.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "casa1Pared4b", colliderCasa1Pared4b, modelColliderCasa1Pared4b);
		// Pared 4C
		glm::mat4 modelColliderCasa1Pared4c = glm::mat4(modelMatrixCasa1Pared4c);
		AbstractModel::OBB colliderCasa1Pared4c;
		colliderCasa1Pared4c.u = glm::quat_cast(modelColliderCasa1Pared4c);
		modelColliderCasa1Pared4c = glm::scale(modelColliderCasa1Pared4c, glm::vec3(0.8));
		modelColliderCasa1Pared4c = glm::translate(modelMatrixCasa1Pared4cBody, casa1Pared4c.getObb().c);
		colliderCasa1Pared4c.c = modelColliderCasa1Pared4c[3];
		colliderCasa1Pared4c.e = casa1Pared4c.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "casa1Pared4c", colliderCasa1Pared4c, modelColliderCasa1Pared4c);
		// Pared 5A
		glm::mat4 modelColliderCasa1Pared5a = glm::mat4(modelMatrixCasa1Pared5a);
		AbstractModel::OBB colliderCasa1Pared5a;
		colliderCasa1Pared5a.u = glm::quat_cast(modelColliderCasa1Pared5a);
		modelColliderCasa1Pared5a = glm::scale(modelColliderCasa1Pared5a, glm::vec3(0.8));
		modelColliderCasa1Pared5a = glm::translate(modelMatrixCasa1Pared5aBody, casa1Pared5a.getObb().c);
		colliderCasa1Pared5a.c = modelColliderCasa1Pared5a[3];
		colliderCasa1Pared5a.e = casa1Pared5a.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "casa1Pared5a", colliderCasa1Pared5a, modelColliderCasa1Pared5a);
		// Pared 6A
		glm::mat4 modelColliderCasa1Pared6a = glm::mat4(modelMatrixCasa1Pared6a);
		AbstractModel::OBB colliderCasa1Pared6a;
		colliderCasa1Pared6a.u = glm::quat_cast(modelColliderCasa1Pared6a);
		modelColliderCasa1Pared6a = glm::scale(modelColliderCasa1Pared6a, glm::vec3(0.8));
		modelColliderCasa1Pared6a = glm::translate(modelMatrixCasa1Pared6aBody, casa1Pared6a.getObb().c);
		colliderCasa1Pared6a.c = modelColliderCasa1Pared6a[3];
		colliderCasa1Pared6a.e = casa1Pared6a.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "casa1Pared6a", colliderCasa1Pared6a, modelColliderCasa1Pared6a);
		// Techo A
		glm::mat4 modelColliderCasa1TechoA = glm::mat4(modelMatrixCasa1TechoA);
		AbstractModel::OBB colliderCasa1TechoA;
		colliderCasa1TechoA.u = glm::quat_cast(modelColliderCasa1TechoA);
		modelColliderCasa1TechoA = glm::scale(modelColliderCasa1TechoA, glm::vec3(0.8));
		modelColliderCasa1TechoA = glm::translate(modelMatrixCasa1TechoABody, casa1TechoA.getObb().c);
		colliderCasa1TechoA.c = modelColliderCasa1TechoA[3];
		colliderCasa1TechoA.e = casa1TechoA.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "casa1TechoA", colliderCasa1TechoA, modelColliderCasa1TechoA);
		// Techo B
		glm::mat4 modelColliderCasa1TechoB = glm::mat4(modelMatrixCasa1TechoB);
		AbstractModel::OBB colliderCasa1TechoB;
		colliderCasa1TechoB.u = glm::quat_cast(modelColliderCasa1TechoB);
		modelColliderCasa1TechoB = glm::scale(modelColliderCasa1TechoB, glm::vec3(0.8));
		modelColliderCasa1TechoB = glm::translate(modelMatrixCasa1TechoBBody, casa1TechoB.getObb().c);
		colliderCasa1TechoB.c = modelColliderCasa1TechoB[3];
		colliderCasa1TechoB.e = casa1TechoB.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "casa1TechoB", colliderCasa1TechoB, modelColliderCasa1TechoB);
		// Pared A
		glm::mat4 modelColliderCasa1ParedA = glm::mat4(modelMatrixCasa1ParedA);
		AbstractModel::OBB colliderCasa1ParedA;
		colliderCasa1ParedA.u = glm::quat_cast(modelColliderCasa1ParedA);
		modelColliderCasa1ParedA = glm::scale(modelColliderCasa1ParedA, glm::vec3(0.8));
		modelColliderCasa1ParedA = glm::translate(modelMatrixCasa1ParedABody, casa1ParedA.getObb().c);
		colliderCasa1ParedA.c = modelColliderCasa1ParedA[3];
		colliderCasa1ParedA.e = casa1ParedA.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "casa1ParedA", colliderCasa1ParedA, modelColliderCasa1ParedA);
		// Pared B
		glm::mat4 modelColliderCasa1ParedB = glm::mat4(modelMatrixCasa1ParedB);
		AbstractModel::OBB colliderCasa1ParedB;
		colliderCasa1ParedB.u = glm::quat_cast(modelColliderCasa1ParedB);
		modelColliderCasa1ParedB = glm::scale(modelColliderCasa1ParedB, glm::vec3(0.8));
		modelColliderCasa1ParedB = glm::translate(modelMatrixCasa1ParedBBody, casa1ParedB.getObb().c);
		colliderCasa1ParedB.c = modelColliderCasa1ParedB[3];
		colliderCasa1ParedB.e = casa1ParedB.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "casa1ParedB", colliderCasa1ParedB, modelColliderCasa1ParedB);
		// Door 2
		glm::mat4 modelColliderCasa1Door2 = glm::mat4(modelMatrixCasa1Door2);
		AbstractModel::OBB colliderCasa1Door2;
		colliderCasa1Door2.u = glm::quat_cast(modelColliderCasa1Door2);
		modelColliderCasa1Door2 = glm::scale(modelColliderCasa1Door2, glm::vec3(0.8));
		modelColliderCasa1Door2 = glm::translate(modelMatrixCasa1Door2Body, casa1Door2.getObb().c);
		colliderCasa1Door2.c = modelColliderCasa1Door2[3];
		colliderCasa1Door2.e = casa1Door2.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "casa1Door2", colliderCasa1Door2, modelColliderCasa1Door2);
		// Viga 1
		glm::mat4 modelColliderCasa1Viga1 = glm::mat4(modelMatrixCasa1Viga1);
		AbstractModel::OBB colliderCasa1Viga1;
		colliderCasa1Viga1.u = glm::quat_cast(modelColliderCasa1Viga1);
		modelColliderCasa1Viga1 = glm::scale(modelColliderCasa1Viga1, glm::vec3(0.8));
		modelColliderCasa1Viga1 = glm::translate(modelMatrixCasa1Viga1Body, casa1Viga1.getObb().c);
		colliderCasa1Viga1.c = modelColliderCasa1Viga1[3];
		colliderCasa1Viga1.e = casa1Viga1.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "casa1Viga1", colliderCasa1Viga1, modelColliderCasa1Viga1);
		// Viga 2
		glm::mat4 modelColliderCasa1Viga2 = glm::mat4(modelMatrixCasa1Viga2);
		AbstractModel::OBB colliderCasa1Viga2;
		colliderCasa1Viga2.u = glm::quat_cast(modelColliderCasa1Viga2);
		modelColliderCasa1Viga2 = glm::scale(modelColliderCasa1Viga2, glm::vec3(0.8));
		modelColliderCasa1Viga2 = glm::translate(modelMatrixCasa1Viga2Body, casa1Viga2.getObb().c);
		colliderCasa1Viga2.c = modelColliderCasa1Viga2[3];
		colliderCasa1Viga2.e = casa1Viga2.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "casa1Viga2", colliderCasa1Viga2, modelColliderCasa1Viga2);
		// Viga 3
		glm::mat4 modelColliderCasa1Viga3 = glm::mat4(modelMatrixCasa1Viga3);
		AbstractModel::OBB colliderCasa1Viga3;
		colliderCasa1Viga3.u = glm::quat_cast(modelColliderCasa1Viga3);
		modelColliderCasa1Viga3 = glm::scale(modelColliderCasa1Viga3, glm::vec3(0.8));
		modelColliderCasa1Viga3 = glm::translate(modelMatrixCasa1Viga3Body, casa1Viga3.getObb().c);
		colliderCasa1Viga3.c = modelColliderCasa1Viga3[3];
		colliderCasa1Viga3.e = casa1Viga3.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "casa1Viga3", colliderCasa1Viga3, modelColliderCasa1Viga3);
		// Viga 4
		glm::mat4 modelColliderCasa1Viga4 = glm::mat4(modelMatrixCasa1Viga4);
		AbstractModel::OBB colliderCasa1Viga4;
		colliderCasa1Viga4.u = glm::quat_cast(modelColliderCasa1Viga4);
		modelColliderCasa1Viga4 = glm::scale(modelColliderCasa1Viga4, glm::vec3(0.8));
		modelColliderCasa1Viga4 = glm::translate(modelMatrixCasa1Viga4Body, casa1Viga4.getObb().c);
		colliderCasa1Viga4.c = modelColliderCasa1Viga4[3];
		colliderCasa1Viga4.e = casa1Viga4.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "casa1Viga4", colliderCasa1Viga4, modelColliderCasa1Viga4);
		// Viga 5A
		glm::mat4 modelColliderCasa1Viga5a = glm::mat4(modelMatrixCasa1Viga5a);
		AbstractModel::OBB colliderCasa1Viga5a;
		colliderCasa1Viga5a.u = glm::quat_cast(modelColliderCasa1Viga5a);
		modelColliderCasa1Viga5a = glm::scale(modelColliderCasa1Viga5a, glm::vec3(0.8));
		modelColliderCasa1Viga5a = glm::translate(modelMatrixCasa1Viga5aBody, casa1Viga5a.getObb().c);
		colliderCasa1Viga5a.c = modelColliderCasa1Viga5a[3];
		colliderCasa1Viga5a.e = casa1Viga5a.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "casa1Viga5a", colliderCasa1Viga5a, modelColliderCasa1Viga5a);
		// Viga 5B
		glm::mat4 modelColliderCasa1Viga5b = glm::mat4(modelMatrixCasa1Viga5b);
		AbstractModel::OBB colliderCasa1Viga5b;
		colliderCasa1Viga5b.u = glm::quat_cast(modelColliderCasa1Viga5b);
		modelColliderCasa1Viga5b = glm::scale(modelColliderCasa1Viga5b, glm::vec3(0.8));
		modelColliderCasa1Viga5b = glm::translate(modelMatrixCasa1Viga5bBody, casa1Viga5b.getObb().c);
		colliderCasa1Viga5b.c = modelColliderCasa1Viga5b[3];
		colliderCasa1Viga5b.e = casa1Viga5b.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "casa1Viga5b", colliderCasa1Viga5b, modelColliderCasa1Viga5b);
		// Viga 6
		glm::mat4 modelColliderCasa1Viga6 = glm::mat4(modelMatrixCasa1Viga6);
		AbstractModel::OBB colliderCasa1Viga6;
		colliderCasa1Viga6.u = glm::quat_cast(modelColliderCasa1Viga6);
		modelColliderCasa1Viga6 = glm::scale(modelColliderCasa1Viga6, glm::vec3(0.8));
		modelColliderCasa1Viga6 = glm::translate(modelMatrixCasa1Viga6Body, casa1Viga6.getObb().c);
		colliderCasa1Viga6.c = modelColliderCasa1Viga6[3];
		colliderCasa1Viga6.e = casa1Viga6.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "casa1Viga6", colliderCasa1Viga6, modelColliderCasa1Viga6);
		// Pilar 1
		glm::mat4 modelColliderCasa1Pilar1 = glm::mat4(modelMatrixCasa1Pilar1);
		AbstractModel::OBB colliderCasa1Pilar1;
		colliderCasa1Pilar1.u = glm::quat_cast(modelColliderCasa1Pilar1);
		modelColliderCasa1Pilar1 = glm::scale(modelColliderCasa1Pilar1, glm::vec3(0.8));
		modelColliderCasa1Pilar1 = glm::translate(modelMatrixCasa1Pilar1Body, casa1Pilar1.getObb().c);
		colliderCasa1Pilar1.c = modelColliderCasa1Pilar1[3];
		colliderCasa1Pilar1.e = casa1Pilar1.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "casa1Pilar1", colliderCasa1Pilar1, modelColliderCasa1Pilar1);
		// Pilar 2
		glm::mat4 modelColliderCasa1Pilar2 = glm::mat4(modelMatrixCasa1Pilar2);
		AbstractModel::OBB colliderCasa1Pilar2;
		colliderCasa1Pilar2.u = glm::quat_cast(modelColliderCasa1Pilar2);
		modelColliderCasa1Pilar2 = glm::scale(modelColliderCasa1Pilar2, glm::vec3(0.8));
		modelColliderCasa1Pilar2 = glm::translate(modelMatrixCasa1Pilar2Body, casa1Pilar2.getObb().c);
		colliderCasa1Pilar2.c = modelColliderCasa1Pilar2[3];
		colliderCasa1Pilar2.e = casa1Pilar2.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "casa1Pilar2", colliderCasa1Pilar2, modelColliderCasa1Pilar2);
		// Pilar 3
		glm::mat4 modelColliderCasa1Pilar3 = glm::mat4(modelMatrixCasa1Pilar3);
		AbstractModel::OBB colliderCasa1Pilar3;
		colliderCasa1Pilar3.u = glm::quat_cast(modelColliderCasa1Pilar3);
		modelColliderCasa1Pilar3 = glm::scale(modelColliderCasa1Pilar3, glm::vec3(0.8));
		modelColliderCasa1Pilar3 = glm::translate(modelMatrixCasa1Pilar3Body, casa1Pilar3.getObb().c);
		colliderCasa1Pilar3.c = modelColliderCasa1Pilar3[3];
		colliderCasa1Pilar3.e = casa1Pilar3.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "casa1Pilar3", colliderCasa1Pilar3, modelColliderCasa1Pilar3);
		// Borde 1A
		glm::mat4 modelColliderBorde1A = glm::mat4(modelMatrixBorde1A);
		AbstractModel::OBB colliderBorde1A;
		colliderBorde1A.u = glm::quat_cast(modelColliderBorde1A);
		modelColliderBorde1A = glm::scale(modelColliderBorde1A, glm::vec3(0.8));
		modelColliderBorde1A = glm::translate(modelMatrixBorde1ABody, borde1A.getObb().c);
		colliderBorde1A.c = modelColliderBorde1A[3];
		colliderBorde1A.e = borde1A.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "borde1A", colliderBorde1A, modelColliderBorde1A);
		// Borde 2A
		glm::mat4 modelColliderBorde2A = glm::mat4(modelMatrixBorde2A);
		AbstractModel::OBB colliderBorde2A;
		colliderBorde2A.u = glm::quat_cast(modelColliderBorde2A);
		modelColliderBorde2A = glm::scale(modelColliderBorde2A, glm::vec3(0.8));
		modelColliderBorde2A = glm::translate(modelMatrixBorde2ABody, borde2A.getObb().c);
		colliderBorde2A.c = modelColliderBorde2A[3];
		colliderBorde2A.e = borde2A.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "borde2A", colliderBorde2A, modelColliderBorde2A);
		// Borde 3A
		glm::mat4 modelColliderBorde3A = glm::mat4(modelMatrixBorde3A);
		AbstractModel::OBB colliderBorde3A;
		colliderBorde3A.u = glm::quat_cast(modelColliderBorde3A);
		modelColliderBorde3A = glm::scale(modelColliderBorde3A, glm::vec3(0.8));
		modelColliderBorde3A = glm::translate(modelMatrixBorde3ABody, borde3A.getObb().c);
		colliderBorde3A.c = modelColliderBorde3A[3];
		colliderBorde3A.e = borde3A.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "borde3A", colliderBorde3A, modelColliderBorde3A);
		// Borde 4A
		glm::mat4 modelColliderBorde4A = glm::mat4(modelMatrixBorde4A);
		AbstractModel::OBB colliderBorde4A;
		colliderBorde4A.u = glm::quat_cast(modelColliderBorde4A);
		modelColliderBorde4A = glm::scale(modelColliderBorde4A, glm::vec3(0.8));
		modelColliderBorde4A = glm::translate(modelMatrixBorde4ABody, borde4A.getObb().c);
		colliderBorde4A.c = modelColliderBorde4A[3];
		colliderBorde4A.e = borde4A.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "borde4A", colliderBorde4A, modelColliderBorde4A);
		// Borde 5A
		glm::mat4 modelColliderBorde5A = glm::mat4(modelMatrixBorde5A);
		AbstractModel::OBB colliderBorde5A;
		colliderBorde5A.u = glm::quat_cast(modelColliderBorde5A);
		modelColliderBorde5A = glm::scale(modelColliderBorde5A, glm::vec3(0.8));
		modelColliderBorde5A = glm::translate(modelMatrixBorde5ABody, borde5A.getObb().c);
		colliderBorde5A.c = modelColliderBorde5A[3];
		colliderBorde5A.e = borde5A.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "borde5A", colliderBorde5A, modelColliderBorde5A);
		// Borde 6A
		glm::mat4 modelColliderBorde6A = glm::mat4(modelMatrixBorde6A);
		AbstractModel::OBB colliderBorde6A;
		colliderBorde6A.u = glm::quat_cast(modelColliderBorde6A);
		modelColliderBorde6A = glm::scale(modelColliderBorde6A, glm::vec3(0.8));
		modelColliderBorde6A = glm::translate(modelMatrixBorde6ABody, borde6A.getObb().c);
		colliderBorde6A.c = modelColliderBorde6A[3];
		colliderBorde6A.e = borde6A.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "borde6A", colliderBorde6A, modelColliderBorde6A);
		// Obj 1A
		glm::mat4 modelColliderObj1A = glm::mat4(modelMatrixObj1a);
		AbstractModel::OBB colliderObj1A;
		colliderObj1A.u = glm::quat_cast(modelColliderObj1A);
		modelColliderObj1A = glm::scale(modelColliderObj1A, glm::vec3(0.8));
		modelColliderObj1A = glm::translate(modelMatrixObj1aBody, obj1a.getObb().c);
		colliderObj1A.c = modelColliderObj1A[3];
		colliderObj1A.e = obj1a.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "obj1A", colliderObj1A, modelColliderObj1A);
		// Obj 1B
		glm::mat4 modelColliderObj1B = glm::mat4(modelMatrixObj1b);
		AbstractModel::OBB colliderObj1B;
		colliderObj1B.u = glm::quat_cast(modelColliderObj1B);
		modelColliderObj1B = glm::scale(modelColliderObj1B, glm::vec3(0.8));
		modelColliderObj1B = glm::translate(modelMatrixObj1bBody, obj1b.getObb().c);
		colliderObj1B.c = modelColliderObj1B[3];
		colliderObj1B.e = obj1b.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "obj1B", colliderObj1B, modelColliderObj1B);
		// Obj 2A
		glm::mat4 modelColliderObj2A = glm::mat4(modelMatrixObj2a);
		AbstractModel::OBB colliderObj2A;
		colliderObj2A.u = glm::quat_cast(modelColliderObj2A);
		modelColliderObj2A = glm::scale(modelColliderObj2A, glm::vec3(0.8));
		modelColliderObj2A = glm::translate(modelMatrixObj2aBody, obj2a.getObb().c);
		colliderObj2A.c = modelColliderObj2A[3];
		colliderObj2A.e = obj2a.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "obj2A", colliderObj2A, modelColliderObj2A);
		// Obj 2B
		glm::mat4 modelColliderObj2B = glm::mat4(modelMatrixObj2b);
		AbstractModel::OBB colliderObj2B;
		colliderObj2B.u = glm::quat_cast(modelColliderObj2B);
		modelColliderObj2B = glm::scale(modelColliderObj2B, glm::vec3(0.8));
		modelColliderObj2B = glm::translate(modelMatrixObj2bBody, obj2b.getObb().c);
		colliderObj2B.c = modelColliderObj2B[3];
		colliderObj2B.e = obj2b.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "obj2B", colliderObj2B, modelColliderObj2B);
		// Obj 3A
		glm::mat4 modelColliderObj3A = glm::mat4(modelMatrixObj3a);
		AbstractModel::OBB colliderObj3A;
		colliderObj3A.u = glm::quat_cast(modelColliderObj3A);
		modelColliderObj3A = glm::scale(modelColliderObj3A, glm::vec3(0.8));
		modelColliderObj3A = glm::translate(modelMatrixObj3aBody, obj3a.getObb().c);
		colliderObj3A.c = modelColliderObj3A[3];
		colliderObj3A.e = obj3a.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "obj3A", colliderObj3A, modelColliderObj3A);
		// Techo A
		glm::mat4 modelColliderTechoA = glm::mat4(modelMatrixTechoA);
		AbstractModel::OBB colliderTechoA;
		colliderTechoA.u = glm::quat_cast(modelColliderTechoA);
		modelColliderTechoA = glm::scale(modelColliderTechoA, glm::vec3(0.8));
		modelColliderTechoA = glm::translate(modelMatrixTechoABody, techoA.getObb().c);
		colliderTechoA.c = modelColliderTechoA[3];
		colliderTechoA.e = techoA.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "techoA", colliderTechoA, modelColliderTechoA);
		// Techo A
		glm::mat4 modelColliderTechoB = glm::mat4(modelMatrixTechoB);
		AbstractModel::OBB colliderTechoB;
		colliderTechoB.u = glm::quat_cast(modelColliderTechoB);
		modelColliderTechoB = glm::scale(modelColliderTechoB, glm::vec3(0.8));
		modelColliderTechoB = glm::translate(modelMatrixTechoBBody, techoB.getObb().c);
		colliderTechoB.c = modelColliderTechoB[3];
		colliderTechoB.e = techoB.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "techoB", colliderTechoB, modelColliderTechoB);
		// Base
		glm::mat4 modelColliderBase = glm::mat4(modelMatrixBase);
		AbstractModel::OBB colliderBase;
		colliderBase.u = glm::quat_cast(modelColliderBase);
		modelColliderBase = glm::scale(modelColliderBase, glm::vec3(0.8));
		modelColliderBase = glm::translate(modelMatrixBaseBody, base.getObb().c);
		colliderBase.c = modelColliderBase[3];
		colliderBase.e = base.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "base", colliderBase, modelColliderBase);
		// Puente
		glm::mat4 modelColliderPuente = glm::mat4(modelMatrixPuente);
		AbstractModel::OBB colliderPuente;
		colliderPuente.u = glm::quat_cast(modelColliderPuente);
		modelColliderPuente = glm::scale(modelColliderPuente, glm::vec3(0.8));
		modelColliderPuente = glm::translate(modelMatrixPuenteBody, puente.getObb().c);
		colliderPuente.c = modelColliderPuente[3];
		colliderPuente.e = puente.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "puente", colliderPuente, modelColliderPuente);
		// Techo
		glm::mat4 modelColliderTecho = glm::mat4(modelMatrixTecho);
		AbstractModel::OBB colliderTecho;
		colliderTecho.u = glm::quat_cast(modelColliderTecho);
		modelColliderTecho = glm::scale(modelColliderTecho, glm::vec3(0.8));
		modelColliderTecho = glm::translate(modelMatrixTechoBody, techo.getObb().c);
		colliderTecho.c = modelColliderTecho[3];
		colliderTecho.e = techo.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "techo", colliderTecho, modelColliderTecho);
		// Bordes
		glm::mat4 modelColliderBordes = glm::mat4(modelMatrixBordes);
		AbstractModel::OBB colliderBordes;
		colliderBordes.u = glm::quat_cast(modelColliderBordes);
		modelColliderBordes = glm::scale(modelColliderBordes, glm::vec3(0.8));
		modelColliderBordes = glm::translate(modelMatrixBordesBody, bordes.getObb().c);
		colliderBordes.c = modelColliderBordes[3];
		colliderBordes.e = bordes.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "bordes", colliderBordes, modelColliderBordes);
		// Torre Pared 1
		glm::mat4 modelColliderTorrePared1 = glm::mat4(modelMatrixTorrePared1);
		AbstractModel::OBB colliderTorrePared1;
		colliderTorrePared1.u = glm::quat_cast(modelColliderTorrePared1);
		modelColliderTorrePared1 = glm::scale(modelColliderTorrePared1, glm::vec3(0.8));
		modelColliderTorrePared1 = glm::translate(modelMatrixTorrePared1Body, torrePared1.getObb().c);
		colliderTorrePared1.c = modelColliderTorrePared1[3];
		colliderTorrePared1.e = torrePared1.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "torrePared1", colliderTorrePared1, modelColliderTorrePared1);
		// Torre Pared 2
		glm::mat4 modelColliderTorrePared2 = glm::mat4(modelMatrixTorrePared2);
		AbstractModel::OBB colliderTorrePared2;
		colliderTorrePared2.u = glm::quat_cast(modelColliderTorrePared2);
		modelColliderTorrePared2 = glm::scale(modelColliderTorrePared2, glm::vec3(0.8));
		modelColliderTorrePared2 = glm::translate(modelMatrixTorrePared2Body, torrePared2.getObb().c);
		colliderTorrePared2.c = modelColliderTorrePared2[3];
		colliderTorrePared2.e = torrePared2.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "torrePared2", colliderTorrePared2, modelColliderTorrePared2);
		// Torre Pared 3
		glm::mat4 modelColliderTorrePared3 = glm::mat4(modelMatrixTorrePared3);
		AbstractModel::OBB colliderTorrePared3;
		colliderTorrePared3.u = glm::quat_cast(modelColliderTorrePared3);
		modelColliderTorrePared3 = glm::scale(modelColliderTorrePared3, glm::vec3(0.8));
		modelColliderTorrePared3 = glm::translate(modelMatrixTorrePared3Body, torrePared3.getObb().c);
		colliderTorrePared3.c = modelColliderTorrePared3[3];
		colliderTorrePared3.e = torrePared3.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "torrePared3", colliderTorrePared3, modelColliderTorrePared3);
		// Torre Puerta
		glm::mat4 modelColliderTorrePuerta = glm::mat4(modelMatrixTorrePuerta);
		AbstractModel::OBB colliderTorrePuerta;
		colliderTorrePuerta.u = glm::quat_cast(modelColliderTorrePuerta);
		modelColliderTorrePuerta = glm::scale(modelColliderTorrePuerta, glm::vec3(0.8));
		modelColliderTorrePuerta = glm::translate(modelMatrixTorrePuertaBody, torrePuerta.getObb().c);
		colliderTorrePuerta.c = modelColliderTorrePuerta[3];
		colliderTorrePuerta.e = torrePuerta.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "torrePuerta", colliderTorrePuerta, modelColliderTorrePuerta);
		// Casa 2
		// Pared A
		glm::mat4 modelColliderCasa2ParedA = glm::mat4(modelMatrixCasa2ParedA);
		AbstractModel::OBB colliderCasa2ParedA;
		colliderCasa2ParedA.u = glm::quat_cast(modelColliderCasa2ParedA);
		modelColliderCasa2ParedA = glm::scale(modelColliderCasa2ParedA, glm::vec3(0.8));
		modelColliderCasa2ParedA = glm::translate(modelMatrixCasa2ParedABody, casa2ParedA.getObb().c);
		colliderCasa2ParedA.c = modelColliderCasa2ParedA[3];
		colliderCasa2ParedA.e = casa2ParedA.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "casa2ParedA", colliderCasa2ParedA, modelColliderCasa2ParedA);
		// Pared B
		glm::mat4 modelColliderCasa2ParedB = glm::mat4(modelMatrixCasa2ParedB);
		AbstractModel::OBB colliderCasa2ParedB;
		colliderCasa2ParedB.u = glm::quat_cast(modelColliderCasa2ParedB);
		modelColliderCasa2ParedB = glm::scale(modelColliderCasa2ParedB, glm::vec3(0.8));
		modelColliderCasa2ParedB = glm::translate(modelMatrixCasa2ParedBBody, casa2ParedB.getObb().c);
		colliderCasa2ParedB.c = modelColliderCasa2ParedB[3];
		colliderCasa2ParedB.e = casa2ParedB.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "casa2ParedB", colliderCasa2ParedB, modelColliderCasa2ParedB);
		// Pared C
		glm::mat4 modelColliderCasa2ParedC = glm::mat4(modelMatrixCasa2ParedC);
		AbstractModel::OBB colliderCasa2ParedC;
		colliderCasa2ParedC.u = glm::quat_cast(modelColliderCasa2ParedC);
		modelColliderCasa2ParedC = glm::scale(modelColliderCasa2ParedC, glm::vec3(0.8));
		modelColliderCasa2ParedC = glm::translate(modelMatrixCasa2ParedCBody, casa2ParedC.getObb().c);
		colliderCasa2ParedC.c = modelColliderCasa2ParedC[3];
		colliderCasa2ParedC.e = casa2ParedC.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "casa2ParedC", colliderCasa2ParedC, modelColliderCasa2ParedC);
		// Pared D
		glm::mat4 modelColliderCasa2ParedD = glm::mat4(modelMatrixCasa2ParedD);
		AbstractModel::OBB colliderCasa2ParedD;
		colliderCasa2ParedD.u = glm::quat_cast(modelColliderCasa2ParedD);
		modelColliderCasa2ParedD = glm::scale(modelColliderCasa2ParedD, glm::vec3(0.8));
		modelColliderCasa2ParedD = glm::translate(modelMatrixCasa2ParedDBody, casa2ParedD.getObb().c);
		colliderCasa2ParedD.c = modelColliderCasa2ParedD[3];
		colliderCasa2ParedD.e = casa2ParedD.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "casa2ParedD", colliderCasa2ParedD, modelColliderCasa2ParedD);
		// Pared E
		glm::mat4 modelColliderCasa2ParedE = glm::mat4(modelMatrixCasa2ParedE);
		AbstractModel::OBB colliderCasa2ParedE;
		colliderCasa2ParedE.u = glm::quat_cast(modelColliderCasa2ParedE);
		modelColliderCasa2ParedE = glm::scale(modelColliderCasa2ParedE, glm::vec3(0.8));
		modelColliderCasa2ParedE = glm::translate(modelMatrixCasa2ParedEBody, casa2ParedE.getObb().c);
		colliderCasa2ParedE.c = modelColliderCasa2ParedE[3];
		colliderCasa2ParedE.e = casa2ParedE.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "casa2ParedE", colliderCasa2ParedE, modelColliderCasa2ParedE);
		// Pared F
		glm::mat4 modelColliderCasa2ParedF = glm::mat4(modelMatrixCasa2ParedF);
		AbstractModel::OBB colliderCasa2ParedF;
		colliderCasa2ParedF.u = glm::quat_cast(modelColliderCasa2ParedF);
		modelColliderCasa2ParedF = glm::scale(modelColliderCasa2ParedF, glm::vec3(0.8));
		modelColliderCasa2ParedF = glm::translate(modelMatrixCasa2ParedFBody, casa2ParedF.getObb().c);
		colliderCasa2ParedF.c = modelColliderCasa2ParedF[3];
		colliderCasa2ParedF.e = casa2ParedF.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "casa2ParedF", colliderCasa2ParedF, modelColliderCasa2ParedF);
		// Pared G
		glm::mat4 modelColliderCasa2ParedG = glm::mat4(modelMatrixCasa2ParedG);
		AbstractModel::OBB colliderCasa2ParedG;
		colliderCasa2ParedG.u = glm::quat_cast(modelColliderCasa2ParedG);
		modelColliderCasa2ParedG = glm::scale(modelColliderCasa2ParedG, glm::vec3(0.8));
		modelColliderCasa2ParedG = glm::translate(modelMatrixCasa2ParedGBody, casa2ParedG.getObb().c);
		colliderCasa2ParedG.c = modelColliderCasa2ParedG[3];
		colliderCasa2ParedG.e = casa2ParedG.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "casa2ParedG", colliderCasa2ParedG, modelColliderCasa2ParedG);
		// Pared H
		glm::mat4 modelColliderCasa2ParedH = glm::mat4(modelMatrixCasa2ParedH);
		AbstractModel::OBB colliderCasa2ParedH;
		colliderCasa2ParedH.u = glm::quat_cast(modelColliderCasa2ParedH);
		modelColliderCasa2ParedH = glm::scale(modelColliderCasa2ParedH, glm::vec3(0.8));
		modelColliderCasa2ParedH = glm::translate(modelMatrixCasa2ParedHBody, casa2ParedH.getObb().c);
		colliderCasa2ParedH.c = modelColliderCasa2ParedH[3];
		colliderCasa2ParedH.e = casa2ParedH.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "casa2ParedH", colliderCasa2ParedH, modelColliderCasa2ParedH);
		// Pared I
		glm::mat4 modelColliderCasa2ParedI = glm::mat4(modelMatrixCasa2ParedI);
		AbstractModel::OBB colliderCasa2ParedI;
		colliderCasa2ParedI.u = glm::quat_cast(modelColliderCasa2ParedI);
		modelColliderCasa2ParedI = glm::scale(modelColliderCasa2ParedI, glm::vec3(0.8));
		modelColliderCasa2ParedI = glm::translate(modelMatrixCasa2ParedIBody, casa2ParedI.getObb().c);
		colliderCasa2ParedI.c = modelColliderCasa2ParedI[3];
		colliderCasa2ParedI.e = casa2ParedI.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "casa2ParedI", colliderCasa2ParedI, modelColliderCasa2ParedI);
		// Techo A
		glm::mat4 modelColliderCasa2TechoA = glm::mat4(modelMatrixCasa2TechoA);
		AbstractModel::OBB colliderCasa2TechoA;
		colliderCasa2TechoA.u = glm::quat_cast(modelColliderCasa2TechoA);
		modelColliderCasa2TechoA = glm::scale(modelColliderCasa2TechoA, glm::vec3(0.8));
		modelColliderCasa2TechoA = glm::translate(modelMatrixCasa2TechoABody, casa2TechoA.getObb().c);
		colliderCasa2TechoA.c = modelColliderCasa2TechoA[3];
		colliderCasa2TechoA.e = casa2TechoA.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "casa2TechoA", colliderCasa2TechoA, modelColliderCasa2TechoA);
		// Techo B
		glm::mat4 modelColliderCasa2TechoB = glm::mat4(modelMatrixCasa2TechoB);
		AbstractModel::OBB colliderCasa2TechoB;
		colliderCasa2TechoB.u = glm::quat_cast(modelColliderCasa2TechoB);
		modelColliderCasa2TechoB = glm::scale(modelColliderCasa2TechoB, glm::vec3(0.8));
		modelColliderCasa2TechoB = glm::translate(modelMatrixCasa2TechoBBody, casa2TechoB.getObb().c);
		colliderCasa2TechoB.c = modelColliderCasa2TechoB[3];
		colliderCasa2TechoB.e = casa2TechoB.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "casa2TechoB", colliderCasa2TechoB, modelColliderCasa2TechoB);
		// Techo C
		glm::mat4 modelColliderCasa2TechoC = glm::mat4(modelMatrixCasa2TechoC);
		AbstractModel::OBB colliderCasa2TechoC;
		colliderCasa2TechoC.u = glm::quat_cast(modelColliderCasa2TechoC);
		modelColliderCasa2TechoC = glm::scale(modelColliderCasa2TechoC, glm::vec3(0.8));
		modelColliderCasa2TechoC = glm::translate(modelMatrixCasa2TechoCBody, casa2TechoC.getObb().c);
		colliderCasa2TechoC.c = modelColliderCasa2TechoC[3];
		colliderCasa2TechoC.e = casa2TechoC.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "casa2TechoC", colliderCasa2TechoC, modelColliderCasa2TechoC);
		// Ventana A
		glm::mat4 modelColliderCasa2VentanaA = glm::mat4(modelMatrixCasa2VentanaA);
		AbstractModel::OBB colliderCasa2VentanaA;
		colliderCasa2VentanaA.u = glm::quat_cast(modelColliderCasa2VentanaA);
		modelColliderCasa2VentanaA = glm::scale(modelColliderCasa2VentanaA, glm::vec3(0.8));
		modelColliderCasa2VentanaA = glm::translate(modelMatrixCasa2VentanaABody, casa2VentanaA.getObb().c);
		colliderCasa2VentanaA.c = modelColliderCasa2VentanaA[3];
		colliderCasa2VentanaA.e = casa2VentanaA.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "casa2VentanaA", colliderCasa2VentanaA, modelColliderCasa2VentanaA);
		// Ventana B
		glm::mat4 modelColliderCasa2VentanaB = glm::mat4(modelMatrixCasa2VentanaB);
		AbstractModel::OBB colliderCasa2VentanaB;
		colliderCasa2VentanaB.u = glm::quat_cast(modelColliderCasa2VentanaB);
		modelColliderCasa2VentanaB = glm::scale(modelColliderCasa2VentanaB, glm::vec3(0.8));
		modelColliderCasa2VentanaB = glm::translate(modelMatrixCasa2VentanaBBody, casa2VentanaB.getObb().c);
		colliderCasa2VentanaB.c = modelColliderCasa2VentanaB[3];
		colliderCasa2VentanaB.e = casa2VentanaB.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "casa2VentanaB", colliderCasa2VentanaB, modelColliderCasa2VentanaB);
		// Ventana C
		glm::mat4 modelColliderCasa2VentanaC = glm::mat4(modelMatrixCasa2VentanaC);
		AbstractModel::OBB colliderCasa2VentanaC;
		colliderCasa2VentanaC.u = glm::quat_cast(modelColliderCasa2VentanaC);
		modelColliderCasa2VentanaC = glm::scale(modelColliderCasa2VentanaC, glm::vec3(0.8));
		modelColliderCasa2VentanaC = glm::translate(modelMatrixCasa2VentanaCBody, casa2VentanaC.getObb().c);
		colliderCasa2VentanaC.c = modelColliderCasa2VentanaC[3];
		colliderCasa2VentanaC.e = casa2VentanaC.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "casa2VentanaC", colliderCasa2VentanaC, modelColliderCasa2VentanaC);
		// Ventana D
		glm::mat4 modelColliderCasa2VentanaD = glm::mat4(modelMatrixCasa2VentanaD);
		AbstractModel::OBB colliderCasa2VentanaD;
		colliderCasa2VentanaD.u = glm::quat_cast(modelColliderCasa2VentanaD);
		modelColliderCasa2VentanaD = glm::scale(modelColliderCasa2VentanaD, glm::vec3(0.8));
		modelColliderCasa2VentanaD = glm::translate(modelMatrixCasa2VentanaDBody, casa2VentanaD.getObb().c);
		colliderCasa2VentanaD.c = modelColliderCasa2VentanaD[3];
		colliderCasa2VentanaD.e = casa2VentanaD.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "casa2VentanaD", colliderCasa2VentanaD, modelColliderCasa2VentanaD);
		// Casa 3
		// Base A
		glm::mat4 modelColliderCasa3BaseA = glm::mat4(modelMatrixCasa3BaseA);
		AbstractModel::OBB colliderCasa3BaseA;
		colliderCasa3BaseA.u = glm::quat_cast(modelColliderCasa3BaseA);
		modelColliderCasa3BaseA = glm::scale(modelColliderCasa3BaseA, glm::vec3(0.8));
		modelColliderCasa3BaseA = glm::translate(modelMatrixCasa3BaseABody, casa3BaseA.getObb().c);
		colliderCasa3BaseA.c = modelColliderCasa3BaseA[3];
		colliderCasa3BaseA.e = casa3BaseA.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "casa3BaseA", colliderCasa3BaseA, modelColliderCasa3BaseA);
		// Base B
		glm::mat4 modelColliderCasa3BaseB = glm::mat4(modelMatrixCasa3BaseB);
		AbstractModel::OBB colliderCasa3BaseB;
		colliderCasa3BaseB.u = glm::quat_cast(modelColliderCasa3BaseB);
		modelColliderCasa3BaseB = glm::scale(modelColliderCasa3BaseB, glm::vec3(0.8));
		modelColliderCasa3BaseB = glm::translate(modelMatrixCasa3BaseBBody, casa3BaseB.getObb().c);
		colliderCasa3BaseB.c = modelColliderCasa3BaseB[3];
		colliderCasa3BaseB.e = casa3BaseB.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "casa3BaseB", colliderCasa3BaseB, modelColliderCasa3BaseB);
		// Techo A
		glm::mat4 modelColliderCasa3TechoA = glm::mat4(modelMatrixCasa3TechoA);
		AbstractModel::OBB colliderCasa3TechoA;
		colliderCasa3TechoA.u = glm::quat_cast(modelColliderCasa3TechoA);
		modelColliderCasa3TechoA = glm::scale(modelColliderCasa3TechoA, glm::vec3(0.8));
		modelColliderCasa3TechoA = glm::translate(modelMatrixCasa3TechoABody, casa3TechoA.getObb().c);
		colliderCasa3TechoA.c = modelColliderCasa3TechoA[3];
		colliderCasa3TechoA.e = casa3TechoA.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "casa3TechoA", colliderCasa3TechoA, modelColliderCasa3TechoA);
		// Techo B
		glm::mat4 modelColliderCasa3TechoB = glm::mat4(modelMatrixCasa3TechoB);
		AbstractModel::OBB colliderCasa3TechoB;
		colliderCasa3TechoB.u = glm::quat_cast(modelColliderCasa3TechoB);
		modelColliderCasa3TechoB = glm::scale(modelColliderCasa3TechoB, glm::vec3(0.8));
		modelColliderCasa3TechoB = glm::translate(modelMatrixCasa3TechoBBody, casa3TechoB.getObb().c);
		colliderCasa3TechoB.c = modelColliderCasa3TechoB[3];
		colliderCasa3TechoB.e = casa3TechoB.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "casa3TechoB", colliderCasa3TechoB, modelColliderCasa3TechoB);
		// Ventana A
		glm::mat4 modelColliderCasa3VentanaA = glm::mat4(modelMatrixCasa3VentanaA);
		AbstractModel::OBB colliderCasa3VentanaA;
		colliderCasa3VentanaA.u = glm::quat_cast(modelColliderCasa3VentanaA);
		modelColliderCasa3VentanaA = glm::scale(modelColliderCasa3VentanaA, glm::vec3(0.8));
		modelColliderCasa3VentanaA = glm::translate(modelMatrixCasa3VentanaABody, casa3VentanaA.getObb().c);
		colliderCasa3VentanaA.c = modelColliderCasa3VentanaA[3];
		colliderCasa3VentanaA.e = casa3VentanaA.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "casa3VentanaA", colliderCasa3VentanaA, modelColliderCasa3VentanaA);
		// Puerta A
		glm::mat4 modelColliderCasa3PuertaA = glm::mat4(modelMatrixCasa3PuertaA);
		AbstractModel::OBB colliderCasa3PuertaA;
		colliderCasa3PuertaA.u = glm::quat_cast(modelColliderCasa3PuertaA);
		modelColliderCasa3PuertaA = glm::scale(modelColliderCasa3PuertaA, glm::vec3(0.8));
		modelColliderCasa3PuertaA = glm::translate(modelMatrixCasa3PuertaABody, casa3PuertaA.getObb().c);
		colliderCasa3PuertaA.c = modelColliderCasa3PuertaA[3];
		colliderCasa3PuertaA.e = casa3PuertaA.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "casa3PuertaA", colliderCasa3PuertaA, modelColliderCasa3PuertaA);
		// Puerta B
		glm::mat4 modelColliderCasa3PuertaB = glm::mat4(modelMatrixCasa3PuertaB);
		AbstractModel::OBB colliderCasa3PuertaB;
		colliderCasa3PuertaB.u = glm::quat_cast(modelColliderCasa3PuertaB);
		modelColliderCasa3PuertaB = glm::scale(modelColliderCasa3PuertaB, glm::vec3(0.8));
		modelColliderCasa3PuertaB = glm::translate(modelMatrixCasa3PuertaBBody, casa3PuertaB.getObb().c);
		colliderCasa3PuertaB.c = modelColliderCasa3PuertaB[3];
		colliderCasa3PuertaB.e = casa3PuertaB.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "casa3PuertaB", colliderCasa3PuertaB, modelColliderCasa3PuertaB);
		// Puerta C
		glm::mat4 modelColliderCasa3PuertaC = glm::mat4(modelMatrixCasa3PuertaC);
		AbstractModel::OBB colliderCasa3PuertaC;
		colliderCasa3PuertaC.u = glm::quat_cast(modelColliderCasa3PuertaC);
		modelColliderCasa3PuertaC = glm::scale(modelColliderCasa3PuertaC, glm::vec3(0.8));
		modelColliderCasa3PuertaC = glm::translate(modelMatrixCasa3PuertaCBody, casa3PuertaC.getObb().c);
		colliderCasa3PuertaC.c = modelColliderCasa3PuertaC[3];
		colliderCasa3PuertaC.e = casa3PuertaC.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "casa3PuertaC", colliderCasa3PuertaC, modelColliderCasa3PuertaC);
		// Objetos
		// Collider Cajas
		for(int i = 0; i < boxPositions.size(); i++) {

			glm::vec3 pos = boxPositions[i].first;
			pos.y = terrain.getHeightTerrain(pos.x, pos.z) + pos.y;

			glm::mat4 modelMatrixCaja = glm::mat4(1.0f);
			modelMatrixCaja = glm::translate(modelMatrixCaja, pos);
			modelMatrixCaja = glm::rotate(modelMatrixCaja, glm::radians(boxPositions[i].second), glm::vec3(0.0f, 1.0f, 0.0f));

			glm::mat4 modelMatrixCajaBody = glm::mat4(modelMatrixCaja);
			modelMatrixCajaBody = glm::scale(modelMatrixCajaBody, glm::vec3(0.8));

			glm::mat4 modelColliderCaja = glm::mat4(modelMatrixCaja);
			AbstractModel::OBB colliderCaja;
			colliderCaja.u = glm::quat_cast(modelColliderCaja);
			modelColliderCaja = glm::scale(modelColliderCaja, glm::vec3(0.8));
			modelColliderCaja = glm::translate(modelMatrixCajaBody, caja.getObb().c);
			colliderCaja.c = modelColliderCaja[3];
			colliderCaja.e = caja.getObb().e * glm::vec3(0.8);

			addOrUpdateColliders(
				collidersOBB,
				"caja_" + std::to_string(i),
				colliderCaja,
				modelColliderCaja);
		}
		// Collider gemas
		for(int i = 0; i < gemPositions.size(); i++) {

			glm::vec3 pos = gemPositions[i].first;
			pos.y = terrain.getHeightTerrain(pos.x, pos.z) + pos.y;

			glm::mat4 modelMatrixGem = glm::mat4(1.0f);
			modelMatrixGem = glm::translate(modelMatrixGem, pos);
			modelMatrixGem = glm::rotate(modelMatrixGem, glm::radians(gemPositions[i].second), glm::vec3(0.0f, 1.0f, 0.0f));

			glm::mat4 modelMatrixGemBody = glm::mat4(modelMatrixGem);
			modelMatrixGemBody = glm::scale(modelMatrixGemBody, glm::vec3(0.8));

			glm::mat4 modelColliderGem = glm::mat4(modelMatrixGem);
			AbstractModel::OBB colliderGem;
			colliderGem.u = glm::quat_cast(modelColliderGem);
			modelColliderGem = glm::scale(modelColliderGem, glm::vec3(0.8));
			modelColliderGem = glm::translate(modelMatrixGemBody, gem.getObb().c);
			colliderGem.c = modelColliderGem[3];
			colliderGem.e = gem.getObb().e * glm::vec3(0.8);

			addOrUpdateColliders(
				collidersOBB,
				"gem_" + std::to_string(i),
				colliderGem,
				modelColliderGem);
		}
		// Collider de llave
		glm::mat4 modelColliderCasa1Key = glm::mat4(modelMatrixCasa1Key);
		AbstractModel::OBB colliderCasa1Key;
		colliderCasa1Key.u = glm::quat_cast(modelColliderCasa1Key);
		modelColliderCasa1Key = glm::scale(modelColliderCasa1Key, glm::vec3(0.8));
		modelColliderCasa1Key = glm::translate(modelMatrixCasa1KeyBody, key.getObb().c);
		colliderCasa1Key.c = modelColliderCasa1Key[3];
		colliderCasa1Key.e = key.getObb().e * glm::vec3(0.8);
		addOrUpdateColliders(collidersOBB, "llave", colliderCasa1Key, modelColliderCasa1Key);

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

		// Pruebas de colisión OBB (plataformas y objetos)
		// catOnGround ya pudo activarse por el terreno arriba.
		// El loop de colisiones puede activarlo además si Cat está sobre una plataforma OBB.
		// NOTA: NO reseteamos catOnGround aquí; ya fue establecido por el terreno si corresponde.
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

				// Flags independientes por tipo de colisión detectada en este frame
				bool hasLateralCollision = false;

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
							// Cat viene de arriba: apoyar sobre el objeto
							modelMatrixCat[3].y += overlapY;
							catVelY     = 0.0f;
							catOnGround = true;
						} else {
							// Cat golpea desde abajo: empujar hacia abajo
							modelMatrixCat[3].y -= overlapY;
							if(catVelY > 0.0f) catVelY = 0.0f;
						}
					} else {
						// --- Colisión LATERAL (X o Z es el eje de menor penetración) ---
						// Se acumula; se aplica al final del loop sobre todos los objetos
						hasLateralCollision = true;
					}
				}

				// Revertir XZ si hubo alguna colisión lateral,
				// INDEPENDIENTEMENTE de si también hubo colisión vertical (suelo/plataforma).
				if(hasLateralCollision){
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