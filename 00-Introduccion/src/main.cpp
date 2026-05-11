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
float distanceFromTarget = 7.0f;

Sphere skyboxSphere(20, 20);
Box boxCesped;
Box boxWalls;
Box boxHighway;
Box boxLandingPad;
Sphere esfera1(7, 7);		//Esfera creada como ejemplo

// Models complex instances

// Modelos animados
// Cat
Model catModelAnimate;
bool stopJump = false;

// Terrain model instance
Terrain terrain(-1,-1,200,32, "../Textures/HeightmapP3.png");

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
		"../Textures/mp_bloodvalley/blood-valley_ft.tga",
		"../Textures/mp_bloodvalley/blood-valley_bk.tga",
		"../Textures/mp_bloodvalley/blood-valley_up.tga",
		"../Textures/mp_bloodvalley/blood-valley_dn.tga",
		"../Textures/mp_bloodvalley/blood-valley_rt.tga",
		"../Textures/mp_bloodvalley/blood-valley_lf.tga" };

bool exitApp = false;
int lastMousePosX, offsetX = 0;
int lastMousePosY, offsetY = 0;

// Model matrix definitions
glm::mat4 modelMatrixCat = glm::mat4(1.0f);

int animationCatIndex = 0;

int modelSelected = 0;
bool enableCountSelected = true;

// Variables to animations keyframes
bool saveFrame = false, availableSave = true;
std::ofstream myfile;
std::string fileName = "";
bool record = false;

//Variables de salto
bool isJump = false;	// Salto
float GRAVITY = 4;	// Gravedad
double tmv = 0;			// velocidad
double startTime = 0;	// Tiempo de inicio

double deltaTime;
double currTime, lastTime;

// Se definen todos las funciones.
void reshapeCallback(GLFWwindow *Window, int widthRes, int heightRes);
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
void mouseCallback(GLFWwindow *window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow *window, int button, int state, int mod);
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

	boxCesped.init();
	boxCesped.setShader(&shaderMulLighting);

	boxWalls.init();
	boxWalls.setShader(&shaderMulLighting);

	boxHighway.init();
	boxHighway.setShader(&shaderMulLighting);

	boxLandingPad.init();
	boxLandingPad.setShader(&shaderMulLighting);

	esfera1.init();								//Enviar información de la esfera a la tarjeta de video
	esfera1.setShader(&shaderMulLighting);		//Shader asociado

	// Modelos
	// Cat
	catModelAnimate.loadModel("../models/cat/cat.fbx");
	catModelAnimate.setShader(&shaderMulLighting);

	// Terreno
	terrain.init();
	terrain.setShader(&shaderTerrain);

	camera->setPosition(glm::vec3(0.0, 3.0, 20.0));
	camera->setDistanceFromTarget(distanceFromTarget);
	camera->setSensitivity(1.0);
	
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

	// Definiendo la textura a utilizar
	Texture textureCesped("../Textures/grassy2.png");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	textureCesped.loadImage();
	// Creando la textura con id 1
	glGenTextures(1, &textureCespedID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureCespedID);
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
		std::cout << "Numero de canales :=> " << textureCesped.getChannels() << std::endl;
		glTexImage2D(GL_TEXTURE_2D, 0, textureCesped.getChannels() == 3 ? GL_RGB : GL_RGBA, textureCesped.getWidth(), textureCesped.getHeight(), 0,
		textureCesped.getChannels() == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, textureCesped.getData());
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureCesped.freeImage();

	// Definiendo la textura a utilizar
	Texture textureWall("../Textures/whiteWall.jpg");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	textureWall.loadImage();
	// Creando la textura con id 1
	glGenTextures(1, &textureWallID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureWallID);
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

	// Definiendo la textura a utilizar
	Texture textureWindow("../Textures/ventana.png");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	textureWindow.loadImage();
	// Creando la textura con id 1
	glGenTextures(1, &textureWindowID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureWindowID);
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

	// Definiendo la textura a utilizar
	Texture textureHighway("../Textures/highway.jpg");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	textureHighway.loadImage();
	// Creando la textura con id 1
	glGenTextures(1, &textureHighwayID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureHighwayID);
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
	boxCesped.destroy();
	boxWalls.destroy();
	boxHighway.destroy();
	boxLandingPad.destroy();
	esfera1.destroy();			// Liberar memoria de la tarjeta gráfica

	// Custom objects Delete
	catModelAnimate.destroy();

	// Terrains objects Delete
	terrain.destroy();

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

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset){
	distanceFromTarget -= yoffset;
	camera->setDistanceFromTarget(distanceFromTarget);
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

bool processInput(bool continueApplication) {
	if (exitApp || glfwWindowShouldClose(window) != 0) {
		return false;
	}

	if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		camera->mouseMoveCamera(offsetX, 0.0, deltaTime);
	if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		camera->mouseMoveCamera(0.0, offsetY, deltaTime);

	// Control por mando
	if(glfwJoystickPresent(GLFW_JOYSTICK_1) == GLFW_TRUE){		// Detecta si hay un mando conectado
		std::cout << "Está presente el joystick" << std::endl;
		int axesCount;
		const float * axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);
		std::cout << "El numero de axes es: " << axesCount << std::endl;		// Hay 6 ejes: ejes v y h de los joysticks y los gatillos
		std::cout << "El eje x del joystick izquierdo: " << axes[0]<< std::endl;
		std::cout << "El eje y del joystick izquierdo: " << axes[1]<< std::endl;
		std::cout << "El eje x del joystick derecho: " << axes[2]<< std::endl;
		std::cout << "El eje y del joystick derecho: " << axes[3]<< std::endl;
		std::cout << "El gatillo del joystick izquierdo: " << axes[4]<< std::endl;
		std::cout << "El gatillo del joystick derecho: " << axes[5]<< std::endl;
		// Joystick izquierdo
		if(fabs(axes[0]) > 0.2){
			modelMatrixCat = glm::rotate(modelMatrixCat, /*0.02f*/ -axes[0] * 0.02f, glm::vec3(0, 1, 0));
			animationCatIndex=1;
		}
		if(fabs(axes[1]) > 0.2){
			modelMatrixCat = glm::translate(modelMatrixCat, glm::vec3(0, 0, -axes[1] * 0.02f));
			animationCatIndex=0;
		}
		// Joystick derecho
		if(fabs(axes[2]) > 0.2){
			camera->mouseMoveCamera(axes[2], 0, deltaTime);
		}
		if(fabs(axes[3]) > 0.2){
			camera->mouseMoveCamera(0, axes[3], deltaTime);
		}
		int numerodeBotones;
		const unsigned char * botones = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &numerodeBotones);
		std::cout << "Numero de botones: " << numerodeBotones << std::endl;
		// Botones
		if(botones[0] == GLFW_PRESS && !isJump){		// Saltar con botón A
			startTime = currTime;
			isJump = true;
			tmv = 0;
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
	if (modelSelected == 0 && glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS){
		modelMatrixCat = glm::rotate(modelMatrixCat, 0.02f, glm::vec3(0, 1, 0));
		animationCatIndex = 1;
	} else if (modelSelected == 0 && glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS){
		modelMatrixCat = glm::rotate(modelMatrixCat, -0.02f, glm::vec3(0, 1, 0));
		animationCatIndex = 1;
	}
	if (modelSelected == 0 && glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
		modelMatrixCat = glm::translate(modelMatrixCat, glm::vec3(0.0, 0.0, 0.02));
		animationCatIndex = 1;
	}
	else if (modelSelected == 0 && glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS){
		modelMatrixCat = glm::translate(modelMatrixCat, glm::vec3(0.0, 0.0, -0.02));
		animationCatIndex = 1;
	}

	bool keySpaceStatus = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;		// Detecta si se presiona la barra espaciadora
	if(keySpaceStatus && !isJump){
		isJump = true;
		startTime = currTime;	// Comienza la cuenta del tiempo
		tmv = 0;
	}

	glfwPollEvents();
	return continueApplication;
}

void applicationLoop() {
	bool psi = true;

	glm::vec3 axis;
	glm::vec3 target;
	float angleTarget;

	modelMatrixCat = glm::translate(modelMatrixCat, glm::vec3(0.0f, 0.00, 0.0f));

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
		psi = processInput(true);		//Detectar eventos

		// Variables donde se guardan las matrices de cada articulacion por 1 frame
		
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::perspective(glm::radians(45.0f),
				(float) screenWidth / (float) screenHeight, 0.01f, 100.0f);

		if(modelSelected == 0){
			axis = glm::axis(glm::quat_cast(modelMatrixCat));
			angleTarget = glm::angle(glm::quat_cast(modelMatrixCat));
			target = modelMatrixCat[3];
		}

		if(std::isnan(angleTarget))
			angleTarget = 0.0;
		if(axis.y < 0)
			angleTarget = -angleTarget;
		
		camera->setCameraTarget(target);
		camera->setAngleTarget(angleTarget);
		camera->updateCamera();
		glm::mat4 view = camera->getViewMatrix();

		// Settea la matriz de vista y projection al shader con solo color
		shader.setMatrix4("projection", 1, false, glm::value_ptr(projection));
		shader.setMatrix4("view", 1, false, glm::value_ptr(view));

		// Settea la matriz de vista y projection al shader con skybox
		shaderSkybox.setMatrix4("projection", 1, false,
				glm::value_ptr(projection));
		shaderSkybox.setMatrix4("view", 1, false,
				glm::value_ptr(glm::mat4(glm::mat3(view))));
		// Settea la matriz de vista y projection al shader con multiples luces
		shaderMulLighting.setMatrix4("projection", 1, false,
				glm::value_ptr(projection));
		shaderMulLighting.setMatrix4("view", 1, false,
				glm::value_ptr(view));

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
		shaderTerrain.setInt("spotLightCount", 0);

		/*******************************************
		 * Propiedades PointLights
		 *******************************************/
		shaderMulLighting.setInt("pointLightCount", 0);
		shaderTerrain.setInt("pointLightCount", 0);

		/*******************************************
		 * Terreno
		 *******************************************/
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureCespedID);
		shaderTerrain.setInt("backgroundTexture", 0);
		shaderTerrain.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(40, 40)));
		terrain.setPosition(glm::vec3(100, 0, 100));
		terrain.render();
		shaderTerrain.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(0, 0)));
		glActiveTexture(GL_TEXTURE0);

		/*******************************************
		 * Cesped
		 *******************************************/
		/*glm::mat4 modelCesped = glm::mat4(1.0);
		modelCesped = glm::translate(modelCesped, glm::vec3(0.0, 0.0, 0.0));
		modelCesped = glm::scale(modelCesped, glm::vec3(200.0, 0.001, 200.0));
		// Se activa la textura del agua
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureCespedID);
		shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(200, 200)));
		boxCesped.render(modelCesped);
		shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(0, 0)));
		glBindTexture(GL_TEXTURE_2D, 0);*/

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
		glActiveTexture(GL_TEXTURE0);							//Indicar si tiene textura
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
		esfera1.enableFillMode();

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
		
		/*******************************************
		* Objetos animados por huesos
		*******************************************/
		glm::vec3 ejey = glm::normalize(terrain.getNormalTerrain(modelMatrixCat[3][0], modelMatrixCat[3][2]));
		glm::vec3 ejex = glm::vec3(modelMatrixCat[0]);
		glm::vec3 ejez = glm::normalize(glm::cross(ejex, ejey));
		modelMatrixCat[3].y = terrain.getHeightTerrain(modelMatrixCat[3].x, modelMatrixCat[3].z);
		ejex = glm::normalize(glm::cross(ejey, ejez));
		modelMatrixCat[0] = glm::vec4(ejex, 0.0);
		modelMatrixCat[1] = glm::vec4(ejey, 0.0);
		modelMatrixCat[2] = glm::vec4(ejez, 0.0);
		if(!stopJump){
			modelMatrixCat[3][1] = -GRAVITY * tmv * tmv + 4.0 * tmv + terrain.getHeightTerrain(modelMatrixCat[3][0], modelMatrixCat[3][2]);	// Completando la línea para calcular el tiro parabólico (salto)
			tmv = currTime - startTime;
			if(modelMatrixCat[3][1] < terrain.getHeightTerrain(modelMatrixCat[3].x, modelMatrixCat[3].z)){	// Detectar que está en el suelo (dejar de caer)
				modelMatrixCat[3].y = terrain.getHeightTerrain(modelMatrixCat[3].x, modelMatrixCat[3].z);
				isJump = false;
			}
		}
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

		// Constantes de animaciones
		

		glfwSwapBuffers(window);
	}
}

int main(int argc, char **argv) {
	init(800, 600, "Proyecto Final", false);		//Configuración de la ventana
	applicationLoop();							//Método para mantener la ventana abierta
	destroy();
	return 1;
}