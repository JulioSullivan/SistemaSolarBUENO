
//glew include
#include <GL/glew.h>

//std includes
#include <string>
#include <iostream>

//glfw include
#include <GLFW/glfw3.h>

// program include
#include "Headers/InputManager.h"
#include "Headers/TimeManager.h"
#include "Headers/Shader.h"

// Texture include
#include "Headers/Texture.h"

#include "Headers\CubemapTexture.h"

// Camera include
#include "Headers/CameraFPS.h"

#include "Headers/Model.h"

// Sphere include
#include "Headers/Sphere.h"

#include "Headers/collision.h"

Sphere sp(1.5, 50, 50, MODEL_MODE::VERTEX_COLOR);
Sphere sp2(1.5, 50, 50, MODEL_MODE::VERTEX_LIGHT_TEXTURE);

Shader lightingShader;
Shader lightingShaderMix;
Shader lampShader;
Shader cubemapShader;
Shader envCubeShader;

Shader particleShader;

Model alien;
Model starship;
Model nanosuit;
Model falcon;

Texture textureDifuse(GL_TEXTURE_2D, "../Textures/container2.png");
Texture textureSpecular(GL_TEXTURE_2D, "../Textures/container2_specular.png");
Texture textureSun(GL_TEXTURE_2D, "../Textures/sun.jpg");
Texture textureEarth(GL_TEXTURE_2D, "../Textures/earth_daymap.jpg");
Texture textureClouds(GL_TEXTURE_2D, "../Textures/earth_clouds.jpg");
Texture textureEarthSpec(GL_TEXTURE_2D, "../Textures/earth_specular.tif");
Texture textureEarthDiff(GL_TEXTURE_2D, "../Textures/earth_nightmap.jpg");
Texture textureMoon(GL_TEXTURE_2D, "../Textures/moon.jpg");
Texture textureMercury(GL_TEXTURE_2D, "../Textures/mercury.jpg");
Texture textureVenus(GL_TEXTURE_2D, "../Textures/venus_atmosphere.jpg");
Texture textureVenusClouds(GL_TEXTURE_2D, "../Textures/venus_surface.jpg");
Texture textureMars(GL_TEXTURE_2D, "../Textures/mars.jpg");
Texture textureJupiter(GL_TEXTURE_2D, "../Textures/jupiter.jpg");
Texture textureSaturn(GL_TEXTURE_2D, "../Textures/saturn.jpg");
Texture textureUranus(GL_TEXTURE_2D, "../Textures/uranus.jpg");
Texture textureNeptune(GL_TEXTURE_2D, "../Textures/neptune.jpg");
Texture texturePluto(GL_TEXTURE_2D, "../Textures/plutomap.jpg");
Texture texturePlutoSpec(GL_TEXTURE_2D, "../Textures/plutobump.jpg");
Texture textureMeteor(GL_TEXTURE_2D, "../Textures/meteor.jpg");
Texture textureMeteorSpec(GL_TEXTURE_2D, "../Textures/meteorSpecular.jpg");

CubemapTexture * cubeMaptexture = new CubemapTexture("../Textures/ame_nebula", "purplenebula_ft.tga", "purplenebula_bk.tga", "purplenebula_up.tga", "purplenebula_dn.tga", "purplenebula_rt.tga", "purplenebula_lf.tga");

GLuint VAO, VBO, EBO;

int screenWidth;
int screenHeight;

GLFWwindow * window;
InputManager inputManager;
double deltaTime;
bool animate = false;
float rotationSpeed;
float orbitSpeed;

float tiempo = 0;
float aumento = 0.0001;

//Para el meteorito
glm::vec3 posicion;
float tamaño, velocidad, rad1, rad2;

// Se definen todos las funciones.
void reshapeCallback(GLFWwindow* Window, int widthRes, int heightRes);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow* window, int button, int state, int mod);
void init(int width, int height, std::string strTitle, bool bFullScreen);
void destroyWindow();
void destroy();
bool processInput(bool continueApplication = true);
glm::mat4 orbit(glm::mat4 matriz, float radio1, float radio2, float velocidad, float t);


// Implementacion de todas las funciones.
void init(int width, int height, std::string strTitle, bool bFullScreen) {

	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		exit(-1);
	}

	screenWidth = width;
	screenHeight = height;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if (bFullScreen)
		window = glfwCreateWindow(width, height, strTitle.c_str(),
			glfwGetPrimaryMonitor(), nullptr);
	else
		window = glfwCreateWindow(width, height, strTitle.c_str(), nullptr,
			nullptr);

	if (window == nullptr) {
		std::cerr
			<< "Error to create GLFW window, you can try download the last version of your video card that support OpenGL 3.3+"
			<< std::endl;
		destroyWindow();
		exit(-1);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);

	glfwSetWindowSizeCallback(window, reshapeCallback);
	glfwSetWindowSizeCallback(window, reshapeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Init glew
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		std::cerr << "Failed to initialize glew" << std::endl;
		exit(-1);
	}

	glViewport(0, 0, screenWidth, screenHeight);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// Enable test depth, must be clear depth buffer bit
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	sp.init();
	sp.load();

	sp2.init();
	sp2.load();


	alien.loadModel("../objects/SistemaSolar/AlienSistemaSolar.dae");
	starship.loadModel("../objects/Starship2/orbiter bugship.obj");
	//falcon.loadModel("../objects/Starship3/millenium-falcon.obj");
	nanosuit.loadModel("../objects/nanosuit/nanosuit.obj");

	lightingShaderMix.initialize("../Shaders/loadModelLightingMix.vs", "../Shaders/loadModelLightingMix.fs");
	lightingShader.initialize("../Shaders/loadModelLighting.vs", "../Shaders/loadModelLighting.fs");
	lampShader.initialize("../Shaders/lampShader.vs", "../Shaders/lampShader.fs");
	cubemapShader.initialize("../Shaders/cubemapTexture.vs", "../Shaders/cubemapTexture.fs");
	envCubeShader.initialize("../Shaders/envRefCubemapTexture.vs", "../Shaders/envRefCubemapTexture.fs");
	textureDifuse.load();
	textureSun.load();
	textureEarth.load();
	textureClouds.load();
	textureEarthDiff.load();
	textureEarthSpec.load();
	textureMoon.load();
	textureMercury.load();
	textureVenus.load();
	textureVenusClouds.load();
	textureMars.load();
	textureJupiter.load();
	textureSaturn.load();
	textureUranus.load();
	textureNeptune.load();
	texturePluto.load();
	texturePlutoSpec.load();
	textureMeteor.load();
	textureMeteorSpec.load();

	particleShader.initialize("../Shaders/particle.vs", "../Shaders/particle.fs");

	cubeMaptexture->Load();

}

void destroyWindow() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

void destroy() {
	destroyWindow();
	lightingShader.destroy();
	lightingShaderMix.destroy();
	lampShader.destroy();
	cubemapShader.destroy();
	envCubeShader.destroy();
	delete cubeMaptexture;
}

void reshapeCallback(GLFWwindow* Window, int widthRes, int heightRes) {
	screenWidth = widthRes;
	screenHeight = heightRes;
	glViewport(0, 0, widthRes, heightRes);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	inputManager.keyPressed(inputManager.toApplicationKey(key), deltaTime * 10.0f,
		inputManager.toApplicationState(action));
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		// Animating the planets Orbit around the sun
		// and also their own individual rotation about their own axis
		std::cout << "presionaste P" << std::endl;
		animate = !animate;
	}
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		// Randomize the Size / Orbit Speed Around  the Sun / Rotation Speed about their own axis
		// of all the planet and the sun
		//
		//
		// Rand + 100 to prevent the orbit or rotationSpeed from being 0
		//    Or being too low that it seems to stop
		// 
		// Randomizing the orbitSpeed of planet
		// Capping the orbit Speed at 300, minimum 100
		orbitSpeed = (rand() % 200) + 100;

		// Randomizing the rotationSpeed of planet + sun
		//    upon its own axis
		// Capping the rotation Speed at 300, minimum 100
		rotationSpeed = (rand() % 200) + 100;

		tiempo = 0;
	}

	if (key == GLFW_KEY_UP)
		if (aumento < 0.007)
			aumento += 0.0001;

	if (key == GLFW_KEY_DOWN)
		if (aumento > 0.0)
			aumento -= 0.0001;

	if (key == GLFW_KEY_O && action == GLFW_PRESS) {
		//Obtener la posicion donde se insertará el meteorito
		posicion = glm::vec3(inputManager.getCameraFPS()->Position.x,
			0.0, inputManager.getCameraFPS()->Position.z);
		//Obtener un tamaño aleatorio entre 0.25 y 1.25
		tamaño = ((rand() % 1000) + 250);
		tamaño /= 1000;
		velocidad = ((rand() % 100) + 50);
		velocidad /= 100;
		rad1 = ((rand() % 490) + 60);
		rad2 = ((rand() % 100) + 10);
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	inputManager.mouseMoved(xpos, ypos);
}

void mouseButtonCallback(GLFWwindow* window, int button, int state, int mod) {
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	inputManager.mouseClicked(inputManager.toMouseButtonIndex(button), xpos,
		ypos, inputManager.toApplicationState(state));
}

bool processInput(bool continueApplication) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS
		|| glfwWindowShouldClose(window) != 0) {
		return false;
	}
	TimeManager::Instance().CalculateFrameRate(false);
	deltaTime = TimeManager::Instance().DeltaTime;
	inputManager.do_movement(deltaTime);
	glfwPollEvents();
	return continueApplication;
}

glm::mat4 orbit(glm::mat4 matriz, float radio1, float radio2, float velocidad, float t) {
	/*
	Función para calcular la traslación del planeta implementando la ecuación de una elipse
	Recibe la matriz del modelo del planeta, los radios mayores y menores de la elipse, la velocidad de movimiento y el tiempo
	*/
	return glm::translate(matriz, glm::vec3(cos(t*velocidad)*radio1, 0.0f, sin(t*velocidad)*radio2));
}


double lastUpdateTime = glfwGetTime(); // last update time
double elapsedTime = lastUpdateTime;   // time elapsed since last update
double frameTime = 0.0f;            // frame time
int frameCount = 0;


void applicationLoop() {
	bool psi = true;

	glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
	double lastTime = TimeManager::Instance().GetTime();

	SBB planet_sbb[11];
	SBB sbbShip = getSBB(starship.getMeshes());

	animate = true;

	while (psi) {
		psi = processInput(true);
		// This is new, need clear depth buffer bit
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Create camera transformations
		glm::mat4 view = inputManager.getCameraFPS()->GetViewMatrix();
		glm::mat4 projection;
		//MODELO
		glm::mat4 model;

		projection = glm::perspective(45.0f, (float)screenWidth / (float)screenHeight, 0.1f, 5000.0f);

		lightingShader.turnOn();

		GLint viewPosLoc = lightingShader.getUniformLocation("viewPos");
		glUniform3f(viewPosLoc, inputManager.getCameraFPS()->Position.x, inputManager.getCameraFPS()->Position.y,
			inputManager.getCameraFPS()->Position.z);

		// Set material properties
		GLint matDiffuseLoc = lightingShader.getUniformLocation(
			"material.diffuse");
		GLint matSpecularLoc = lightingShader.getUniformLocation(
			"material.specular");
		GLint matShineLoc = lightingShader.getUniformLocation(
			"material.shininess");
		glUniform1i(matDiffuseLoc, 0);
		glUniform1i(matSpecularLoc, 1);
		glUniform1f(matShineLoc, 32.0f);

		// Set lights properties
		GLint lightAmbientLoc = lightingShader.getUniformLocation(
			"light.ambient");
		GLint lightDiffuseLoc = lightingShader.getUniformLocation(
			"light.diffuse");
		GLint lightSpecularLoc = lightingShader.getUniformLocation(
			"light.specular");
		GLint lightPosLoc = lightingShader.getUniformLocation("light.position");
		glUniform3f(lightAmbientLoc, 0.8f, 0.8f, 0.8f);
		glUniform3f(lightDiffuseLoc, 0.6f, 0.6f, 0.6f); // Let's darken the light a bit to fit the scene
		glUniform3f(lightSpecularLoc, 1.0f, 0.5f, 0.0f);
		glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);

		GLfloat timeValue = TimeManager::Instance().GetTime() - lastTime;
		

		/******************** SUN ***********************************/
		// Get the uniform locations
		GLint modelLoc = lightingShader.getUniformLocation("model");
		GLint viewLoc = lightingShader.getUniformLocation("view");
		GLint projLoc = lightingShader.getUniformLocation("projection");


		glm::mat4 sun;
		sun = glm::scale(sun, glm::vec3(35.0, 35.0, 35.0));

		//Rotación sobre el mismos eje Y
		sun = glm::rotate(sun, (float)timeValue * 0.05f,
			glm::vec3(0.0f, 1.0f, 0.0f));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(sun));

		textureSun.bind(GL_TEXTURE0);
		int ambientMapLoc = lightingShader.getUniformLocation("material.ambient");
		int diffuseMapLoc = lightingShader.getUniformLocation("material.diffuse");
		int specularMapLoc = lightingShader.getUniformLocation("material.specular");
		int segundaMapLoc = lightingShader.getUniformLocation("material.segunda");

		glUniform1i(ambientMapLoc, 0);
		glUniform1i(diffuseMapLoc, 5);
		glUniform1i(specularMapLoc, 5);
		glUniform1i(segundaMapLoc, 5);

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		//sbb para el sol
		planet_sbb[0].center = glm::vec3(sun * glm::vec4(0, 0, 0, 1));
		planet_sbb[0].ratio = 1.5 * 35.0;

		sp2.render();
		lightingShader.turnOff();
		/***********************************************************/


		/******************** MOON ***********************************/
		lightingShader.turnOn();


		glm::mat4 moon;

		moon = orbit(moon, 3.0, 3.5, 0.4, tiempo*10);
		moon = orbit(moon, 30.0 * 6.2, 35.0 * 6.2, 0.35, tiempo);

		moon = glm::rotate(moon, (float)timeValue * 0.15f,
			glm::vec3(0.7f, 1.0f, 0.0f));
		moon = glm::scale(moon, glm::vec3(0.08, 0.08, 0.08));


		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(moon));

		textureMoon.bind(GL_TEXTURE0);

		int segundaMoon = lightingShader.getUniformLocation("material.segunda");

		glUniform1i(diffuseMapLoc, 5);
		glUniform1i(ambientMapLoc, 0);
		glUniform1i(specularMapLoc, 5);
		glUniform1i(segundaMoon, 5);

		//sbb para la luna
		planet_sbb[1].center = glm::vec3(moon * glm::vec4(0, 0, 0, 1));
		planet_sbb[1].ratio = 1.5 * 0.08;

		sp2.render();
		lightingShader.turnOff();
		/***********************************************************/


		/**********************  STARSHIP CAMERA *************************/
		lightingShader.turnOn();

		glm::mat4 ship;
		ship = glm::translate(ship, glm::vec3(inputManager.getCameraFPS()->Position.x, inputManager.getCameraFPS()->Position.y,
			inputManager.getCameraFPS()->Position.z));
		ship = glm::rotate(ship, 3.1416f , glm::vec3(0.0f, 1.0f, 0.0f));
		ship = glm::rotate(ship, glm::radians(inputManager.getCameraFPS()->Yaw), glm::vec3(0.0f, -1.0f, 0.0f));
		ship = glm::rotate(ship, glm::radians(inputManager.getCameraFPS()->Pitch), glm::vec3(0.0f, 0.0f, -1.0f));
		ship = glm::translate(ship, glm::vec3(-1.0f, -0.5f, 0.0f));
		//ship = glm::rotate(ship, glm::radians(inputManager.getCameraFPS()->Yaw), glm::vec3(0.0f, -1.0f, 0.0f));
		ship = glm::rotate(ship, glm::radians(inputManager.getCameraFPS()->Pitch), glm::vec3(0.0f, 0.0f, -1.0f));
		ship = glm::scale(ship, glm::vec3(0.001f, 0.001f, 0.001f));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(ship));
		starship.render(&lightingShader);

		SBB sbbShipTest;
		sbbShipTest.center = glm::vec3(ship * glm::vec4(0, 0, 0, 1));
		sbbShipTest.ratio = sbbShip.ratio * 0.001f;

		lightingShader.turnOff();
		/***************************************************************/

		/**********************  PREUBA *************************/
		lightingShader.turnOn();

		glm::mat4 model2;
		model2 = glm::translate(model2, glm::vec3(3.0f, -4.0f, -1.0f));
		model2 = glm::scale(model2, glm::vec3(0.8f, 0.8f, 0.8f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model2));
		falcon.render(&lightingShader);

		lightingShader.turnOff();
		/***************************************************************/

		/******************** MERCURY ***********************************/
		lightingShader.turnOn();

		glm::mat4 mercury;
		mercury = orbit(mercury, 10.0 * 6.0, 15.0 * 6.0, 0.5, tiempo);
		mercury = glm::rotate(mercury, (float)timeValue * 0.1f,
			glm::vec3(0.0f, 1.0f, 0.0f));
		mercury = glm::scale(mercury, glm::vec3(0.244, 0.244, 0.244));


		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mercury));

		textureMercury.bind(GL_TEXTURE0);
		ambientMapLoc = lightingShader.getUniformLocation("material.ambient");
		glUniform1i(ambientMapLoc, 0);

		//sbb para mercurio
		planet_sbb[2].center = glm::vec3(mercury * glm::vec4(0, 0, 0, 1));
		planet_sbb[2].ratio = 1.5 * 0.244;

		sp2.render();
		lightingShader.turnOff();
		/***********************************************************/
				
		/******************** Venus ***********************************/
		lightingShader.turnOn();
		glm::mat4 venus;
		venus = orbit(venus, 20.0 * 6.1, 24.0 * 6.1, 0.42, tiempo);
		venus = glm::rotate(venus, (float)timeValue * 0.3f,
			glm::vec3(0.03f, -1.0f, 0.0f));
		venus = glm::scale(venus, glm::vec3(0.605, 0.605, 0.605));


		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(venus));

		textureVenus.bind(GL_TEXTURE0);
		textureVenusClouds.bind(GL_TEXTURE1);

		ambientMapLoc = lightingShader.getUniformLocation("material.ambient");
		int venusClouds = lightingShader.getUniformLocation("material.ambient");
		glUniform1i(ambientMapLoc, 0);
		glUniform1i(venusClouds, 1);

		//sbb para venus
		planet_sbb[3].center = glm::vec3(venus * glm::vec4(0, 0, 0, 1));
		planet_sbb[3].ratio = 1.5 * 0.605;

		sp2.render();
		lightingShader.turnOff();
		/***********************************************************/

		/******************** EARTH ***********************************/
		lightingShader.turnOn();
		glm::mat4 earth;

		earth = orbit(earth, 30.0 * 6.2, 35.0 * 6.2, 0.35, tiempo);
		earth = glm::rotate(earth, (float)timeValue * 0.15f,
			glm::vec3(0.7f, 1.0f, 0.0f));
		earth = glm::scale(earth, glm::vec3(0.637, 0.637, 0.637));


		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(earth));

		textureEarth.bind(GL_TEXTURE0);
		textureEarthDiff.bind(GL_TEXTURE1);
		textureEarthSpec.bind(GL_TEXTURE2);
		textureClouds.bind(GL_TEXTURE3);

		ambientMapLoc = lightingShader.getUniformLocation("material.ambient");
		diffuseMapLoc = lightingShader.getUniformLocation("material.diffuse");
		specularMapLoc = lightingShader.getUniformLocation("material.specular");

		int clouds = lightingShader.getUniformLocation("material.segunda");

		glUniform1i(diffuseMapLoc, 0);
		glUniform1i(ambientMapLoc, 1);
		glUniform1i(specularMapLoc, 2);
		glUniform1i(clouds, 3);

		//sbb para la tierra
		planet_sbb[4].center = glm::vec3(earth * glm::vec4(0, 0, 0, 1));
		planet_sbb[4].ratio = 1.5 * 0.637;

		sp2.render();
		lightingShader.turnOff();
		/***********************************************************/


		/******************** MARS ***********************************/
		lightingShader.turnOn();
		glm::mat4 mars;

		mars = orbit(mars, 42.0 * 6.3, 45.0 * 6.3, 0.3, tiempo);
		mars = glm::rotate(mars, (float)timeValue * 0.1f,
			glm::vec3(0.0f, 1.0f, 0.0f));
		mars = glm::scale(mars, glm::vec3(0.339, 0.339, 0.339));


		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mars));

		textureMars.bind(GL_TEXTURE0);
		ambientMapLoc = lightingShader.getUniformLocation("material.ambient");

		glUniform1i(ambientMapLoc, 0);
		glUniform1i(diffuseMapLoc, 5);
		glUniform1i(specularMapLoc, 5);
		glUniform1i(clouds, 5);

		//sbb para marte
		planet_sbb[5].center = glm::vec3(mars * glm::vec4(0, 0, 0, 1));
		planet_sbb[5].ratio = 1.5 * 0.339;

		sp2.render();


		lightingShader.turnOff();
		/***********************************************************/

		/******************** JUPITER ***********************************/
		lightingShader.turnOn();

		glm::mat4 jupiter;

		jupiter = orbit(jupiter, 50.5 * 6.4, 54.4 * 6.4, 0.27, tiempo);
		jupiter = glm::rotate(jupiter, (float)timeValue * 0.15f,
			glm::vec3(0.0f, 1.0f, 0.0f));
		jupiter = glm::scale(jupiter, glm::vec3(7.15, 7.15, 7.15));


		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(jupiter));

		textureJupiter.bind(GL_TEXTURE0);
		ambientMapLoc = lightingShader.getUniformLocation("material.ambient");
		glUniform1i(ambientMapLoc, 0);

		//sbb para jupiter
		planet_sbb[6].center = glm::vec3(jupiter * glm::vec4(0, 0, 0, 1));
		planet_sbb[6].ratio = 1.5 * 7.15;

		sp2.render();
		//lightingShader.turnOff();

		glm::mat4 model1;
		model1 = orbit(model1, 50.5 * 6.4, 54.4 * 6.4, 0.27, tiempo);
		model1 = glm::rotate(model1, (float)timeValue * 0.15f,
			glm::vec3(0.0f, 1.0f, 0.0f));
		model1 = glm::scale(model1, glm::vec3(0.8, 0.8, 0.8));
		model1 = glm::translate(model1, glm::vec3(0.0, 13.0, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model1));
		alien.render(&lightingShader);
		lightingShader.turnOff();
		/***********************************************************/

		/******************** SATURN ***********************************/
		lightingShader.turnOn();

		glm::mat4 saturn;

		saturn = orbit(saturn, 60.0 * 6.5, 63.8 * 6.5 , 0.20, tiempo);
		saturn = glm::rotate(saturn, (float)timeValue * 0.4f,
			glm::vec3(0.0f, 1.0f, 0.0f));
		saturn = glm::scale(saturn, glm::vec3(6.02, 6.02, 6.02));


		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(saturn));

		textureSaturn.bind(GL_TEXTURE0);
		ambientMapLoc = lightingShader.getUniformLocation("material.ambient");
		glUniform1i(ambientMapLoc, 0);

		//sbb para saturno
		planet_sbb[7].center = glm::vec3(saturn * glm::vec4(0, 0, 0, 1));
		planet_sbb[7].ratio = 1.5 * 6.02;

		sp2.render();
		lightingShader.turnOff();
		/***********************************************************/


		/******************** URANUS ***********************************/
		lightingShader.turnOn();

		glm::mat4 uranus;

		uranus = orbit(uranus, 70.0 * 6.6, 75.0 * 6.6, 0.15, tiempo);
		uranus = glm::rotate(uranus, (float)timeValue * 0.3f,
			glm::vec3(0.0f, 1.0f, 0.0f));
		uranus = glm::scale(uranus, glm::vec3(5.11, 5.11, 5.11));


		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(uranus));

		textureUranus.bind(GL_TEXTURE0);
		ambientMapLoc = lightingShader.getUniformLocation("material.ambient");
		glUniform1i(ambientMapLoc, 0);

		//sbb para urano
		planet_sbb[8].center = glm::vec3(uranus * glm::vec4(0, 0, 0, 1));
		planet_sbb[8].ratio = 1.5 * 5.11;

		sp2.render();
		lightingShader.turnOff();
		/***********************************************************/

		/******************** NEPTUNE ***********************************/
		lightingShader.turnOn();
		glm::mat4 neptune;

		neptune = orbit(neptune, 80.5 * 6.7, 93.8 * 6.7, 0.1, tiempo);
		neptune = glm::rotate(neptune, (float)timeValue * 0.1f,
			glm::vec3(0.0f, 1.0f, 0.0f));
		neptune = glm::scale(neptune, glm::vec3(4.95, 4.95, 4.95));


		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(neptune));

		textureNeptune.bind(GL_TEXTURE0);
		ambientMapLoc = lightingShader.getUniformLocation("material.ambient");
		glUniform1i(ambientMapLoc, 0);

		//sbb para neptuno
		planet_sbb[9].center = glm::vec3(neptune * glm::vec4(0, 0, 0, 1));
		planet_sbb[9].ratio = 1.5 * 4.95;

		sp2.render();
		lightingShader.turnOff();
		/***********************************************************/

		/******************** PLUTO ***********************************/
		lightingShader.turnOn();

		glm::mat4 pluto;
		pluto = orbit(pluto, 80.5 * 6.9, 93.8 * 6.9, 0.1, tiempo);
		pluto = glm::rotate(pluto, (float)timeValue * 0.1f,
			glm::vec3(0.0f, 1.0f, 0.0f));
		pluto = glm::scale(pluto, glm::vec3(0.232, 0.232, 0.232));


		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(pluto));

		texturePluto.bind(GL_TEXTURE0);
		texturePlutoSpec.bind(GL_TEXTURE1);

		ambientMapLoc = lightingShader.getUniformLocation("material.ambient");
		specularMapLoc = lightingShader.getUniformLocation("material.specular");

		glUniform1i(ambientMapLoc, 0);
		glUniform1i(specularMapLoc, 1);

		//sbb para plutón
		planet_sbb[10].center = glm::vec3(pluto * glm::vec4(0, 0, 0, 1));
		planet_sbb[10].ratio = 1.5 * 0.232;

		sp2.render();
		lightingShader.turnOff();
		/***********************************************************/


		/******************** METEORITE *********************************/
		lightingShader.turnOn();
		glm::mat4 meteor;
		
		meteor = orbit(meteor, rad1, rad2, velocidad, tiempo);
		meteor = glm::translate(meteor, posicion);
		meteor = glm::rotate(meteor, tiempo, glm::vec3(0.0f, 1.0f, 0.0f));
		meteor = glm::rotate(meteor, (float) glm::radians(-90.0),
			glm::vec3(0.0f, 0.0f, 1.0f));
		meteor = glm::scale(meteor, glm::vec3(tamaño, tamaño, tamaño));


		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(meteor));

		textureMeteor.bind(GL_TEXTURE0);
		textureMeteorSpec.bind(GL_TEXTURE1);
		ambientMapLoc = lightingShader.getUniformLocation("material.ambient");
		specularMapLoc = lightingShader.getUniformLocation("material.specular");

		glUniform1i(ambientMapLoc, 0);
		glUniform1i(specularMapLoc, 5);

		//sbb para meteoro
		SBB sbbMeteor;
		sbbMeteor.center = glm::vec3(meteor * glm::vec4(0, 0, 0, 1));
		sbbMeteor.ratio = 1.5 * tamaño;

		sp2.render();


		lightingShader.turnOff();
		/***********************************************************/

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		particleShader.turnOn();
		GLint modViewProjLoc = particleShader.getUniformLocation("MVP");
		GLint timeLoc = particleShader.getUniformLocation("time");
		glUniform1f(timeLoc, timeValue);
		glUniformMatrix4fv(modViewProjLoc, 1, GL_FALSE, glm::value_ptr(projection*model*view*meteor));
		glDrawArrays(GL_POINTS, 0, 1000);

		particleShader.turnOff();


		/*************************** LIGHT *************************/
		lampShader.turnOn();

		glm::mat4 light;
		light = glm::translate(glm::mat4(), lightPos);
		light = glm::scale(light, glm::vec3(0.4, 0.4, 0.4));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(light));
		//sp.render();
		lampShader.turnOff();
		/*************************************************************/

		/************************** SKYBOX **************************/
		cubemapShader.turnOn();

		GLint oldCullFaceMode;
		GLint oldDepthFuncMode;

		glGetIntegerv(GL_CULL_FACE_MODE, &oldCullFaceMode);
		glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFuncMode);

		viewLoc = cubemapShader.getUniformLocation("view");
		projLoc = cubemapShader.getUniformLocation("projection");
		modelLoc = cubemapShader.getUniformLocation("model");

		view = glm::mat3(inputManager.getCameraFPS()->GetViewMatrix());
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glm::mat4 cubeModel;
		cubeModel = glm::scale(cubeModel, glm::vec3(2000.0f, 2000.0f, 2000.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(cubeModel));

		cubeMaptexture->Bind(GL_TEXTURE0);
		GLuint cubeTextureId = cubemapShader.getUniformLocation("skybox");
		glUniform1f(cubeTextureId, 0);

		glCullFace(GL_FRONT);
		glDepthFunc(GL_LEQUAL);
		sp2.render();
		glCullFace(oldCullFaceMode);
		glDepthFunc(oldDepthFuncMode);

		cubemapShader.turnOff();
		/****************************************************************/

		
		if (animate)
			tiempo += aumento;

		//Checar colisiones entre todos los planetas
		for (int i = 0; i < 11; i++) {
			if (testSphereSphereIntersection(planet_sbb[i], sbbShipTest))
				std::cout << "Model collision:" << i << " & Starship" << std::endl;
			if (testSphereSphereIntersection(planet_sbb[i], sbbMeteor))
				std::cout << "Model collision:" << i << " & Meteor" << std::endl;
			for (int j = i + 1; j < 11; j++)
				if (testSphereSphereIntersection(planet_sbb[i], planet_sbb[j]))
					std::cout << "Planet collision:" << i << " & " << j << std::endl;
		}

		glfwSwapBuffers(window);
	}
}

int main(int argc, char ** argv) {
	init(800, 700, "Window GLFW", false);
	applicationLoop();
	destroy();
	return 1;
}
	