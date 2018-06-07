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

Sphere sp(1.5, 50, 50, MODEL_MODE::VERTEX_COLOR);
Sphere sp2(1.5, 50, 50, MODEL_MODE::VERTEX_LIGHT_TEXTURE);

Shader lightingShader;
Shader lightingShaderMix;
Shader lampShader;
Shader cubemapShader;
Shader envCubeShader;

Model modelo1;

Texture textureDifuse(GL_TEXTURE_2D, "../Textures/container2.png");
Texture textureSpecular(GL_TEXTURE_2D, "../Textures/container2_specular.png");
Texture textureSun(GL_TEXTURE_2D, "../Textures/sun.jpg");
Texture textureEarth(GL_TEXTURE_2D, "../Textures/earth_daymap.jpg");
Texture textureClouds(GL_TEXTURE_2D, "../Textures/earth_clouds.jpg");
Texture textureEarthSpec(GL_TEXTURE_2D, "../Textures/earth_specular.tif");
Texture textureEarthDiff(GL_TEXTURE_2D, "../Textures/earth_nightmap.jpg");
Texture textureMercury(GL_TEXTURE_2D, "../Textures/mercury.jpg");
Texture textureVenus(GL_TEXTURE_2D, "../Textures/venus_atmosphere.jpg");
Texture textureVenusClouds(GL_TEXTURE_2D, "../Textures/venus_surface.jpg");
Texture textureMars(GL_TEXTURE_2D, "../Textures/mars.jpg");
Texture textureJupiter(GL_TEXTURE_2D, "../Textures/jupiter.jpg");
Texture textureSaturn(GL_TEXTURE_2D, "../Textures/saturn.jpg");
Texture textureUranus(GL_TEXTURE_2D, "../Textures/uranus.jpg");
Texture textureNeptune(GL_TEXTURE_2D, "../Textures/neptune.jpg");





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



// Se definen todos las funciones.
void reshapeCallback(GLFWwindow* Window, int widthRes, int heightRes);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow* window, int button, int state, int mod);
void init(int width, int height, std::string strTitle, bool bFullScreen);
void destroyWindow();
void destroy();
bool processInput(bool continueApplication = true);

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
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable test depth, must be clear depth buffer bit
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	sp.init();
	sp.load();

	sp2.init();
	sp2.load();

	modelo1.loadModel("../objects/cyborg/cyborg.obj");

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
	textureMercury.load();
	textureVenus.load();
	textureVenusClouds.load();
	textureMars.load();
	textureJupiter.load();
	textureSaturn.load();
	textureUranus.load();
	textureNeptune.load();

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
		if (animate == false)
			animate = true;
		else if (animate == true)
			animate = false;
	}
	else if (key == GLFW_KEY_R && action == GLFW_PRESS)
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

double lastUpdateTime = glfwGetTime(); // last update time
double elapsedTime = lastUpdateTime;   // time elapsed since last update
double frameTime = 0.0f;            // frame time
int frameCount = 0;


void applicationLoop() {
		bool psi = true;

		glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
		double lastTime = TimeManager::Instance().GetTime();

		while (psi) {
			psi = processInput(true);
			// This is new, need clear depth buffer bit
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Create camera transformations
			glm::mat4 view = inputManager.getCameraFPS()->GetViewMatrix();
			glm::mat4 projection;
			//MODELO
			glm::mat4 model;

			projection = glm::perspective(45.0f, (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);

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
			sun = glm::scale(sun, glm::vec3(1.0, 1.0, 1.0));
			
			//Rotación sobre el mismos eje Y
			sun = glm::rotate(sun, (float)timeValue * 0.05f,
				glm::vec3(0.0f, 1.0f, 0.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(sun));

			textureSun.bind(GL_TEXTURE0);
			int ambientMapLoc = lightingShader.getUniformLocation("material.ambient");
			glUniform1i(ambientMapLoc, 0);
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


			sp2.render();
			lightingShader.turnOff();
			/***********************************************************/

			/******************** MERCURY ***********************************/
			lightingShader.turnOn();

			// Get the uniform locations
			modelLoc = lightingShader.getUniformLocation("model");
			viewLoc = lightingShader.getUniformLocation("view");
			projLoc = lightingShader.getUniformLocation("projection");

			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

			glm::mat4 mercury;
			mercury = glm::scale(mercury, glm::vec3(0.5, 0.5, 0.5));
			mercury = glm::translate(mercury, glm::vec3(10.0f, 0.0f, 10.0f));
			mercury = glm::rotate(mercury, (float)timeValue * 0.1f,
				glm::vec3(0.0f, 1.0f, 0.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mercury));

			textureMercury.bind(GL_TEXTURE0);
			ambientMapLoc = lightingShader.getUniformLocation("material.ambient");
			glUniform1i(ambientMapLoc, 0);

			sp2.render();
			lightingShader.turnOff();
		/***********************************************************/

		/******************** Venus ***********************************/
		lightingShader.turnOn();

		// Get the uniform locations
		modelLoc = lightingShader.getUniformLocation("model");
		viewLoc = lightingShader.getUniformLocation("view");
		projLoc = lightingShader.getUniformLocation("projection");

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glm::mat4 venus;
		venus = glm::scale(venus, glm::vec3(0.5, 0.5, 0.5));
		venus = glm::translate(venus, glm::vec3(20.0f, 0.0f, 20.0f));
		venus = glm::rotate(venus, (float)timeValue * 0.3f,
			glm::vec3(0.03f, -1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(venus));

		textureVenus.bind(GL_TEXTURE0);
		textureVenusClouds.bind(GL_TEXTURE1);

		ambientMapLoc = lightingShader.getUniformLocation("material.ambient");
		int venusClouds = lightingShader.getUniformLocation("material.ambient");
		glUniform1i(ambientMapLoc, 0);
		glUniform1i(ambientMapLoc, 1);


		sp2.render();
		lightingShader.turnOff();
		/***********************************************************/

		/******************** EARTH ***********************************/
		lightingShader.turnOn();

		// Get the uniform locations
		modelLoc = lightingShader.getUniformLocation("model");
		viewLoc = lightingShader.getUniformLocation("view");
		projLoc = lightingShader.getUniformLocation("projection");

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glm::mat4 earth;
		earth = glm::scale(earth, glm::vec3(0.5, 0.5, 0.5));
		earth = glm::translate(earth, glm::vec3(30.0f,0.0f,30.0f));
		earth = glm::rotate(earth, (float)timeValue * 0.15f,
			glm::vec3(0.7f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(earth));

		textureEarth.bind(GL_TEXTURE0);
		textureEarthDiff.bind(GL_TEXTURE1);
		textureEarthSpec.bind(GL_TEXTURE2);
		textureClouds.bind(GL_TEXTURE3);

		ambientMapLoc = lightingShader.getUniformLocation("material.ambient");
		int diffuseMapLoc = lightingShader.getUniformLocation("material.diffuse");
		int specularMapLoc = lightingShader.getUniformLocation("material.specular");
		
		int clouds = lightingShader.getUniformLocation("material.segunda");

		glUniform1i(diffuseMapLoc, 0);
		glUniform1i(ambientMapLoc, 1);
		glUniform1i(specularMapLoc, 2);
		glUniform1i(clouds, 3);


		sp2.render();
		lightingShader.turnOff();
		/***********************************************************/

		/******************** MARS ***********************************/
		lightingShader.turnOn();

		// Get the uniform locations
		modelLoc = lightingShader.getUniformLocation("model");
		viewLoc = lightingShader.getUniformLocation("view");
		projLoc = lightingShader.getUniformLocation("projection");

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glm::mat4 mars;
		mars = glm::scale(mars, glm::vec3(0.5, 0.5, 0.5));
		mars = glm::translate(mars, glm::vec3(40.0f, 0.0f, 40.0f));
		mars = glm::rotate(mars, (float)timeValue * 0.1f,
			glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mars));

		textureMars.bind(GL_TEXTURE0);
		ambientMapLoc = lightingShader.getUniformLocation("material.ambient");
		glUniform1i(ambientMapLoc, 0);

		sp2.render();
		lightingShader.turnOff();
		/***********************************************************/

		/******************** JUPITER ***********************************/
		lightingShader.turnOn();

		// Get the uniform locations
		modelLoc = lightingShader.getUniformLocation("model");
		viewLoc = lightingShader.getUniformLocation("view");
		projLoc = lightingShader.getUniformLocation("projection");

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glm::mat4 jupiter;
		jupiter = glm::scale(jupiter, glm::vec3(0.5, 0.5, 0.5));
		jupiter = glm::translate(jupiter, glm::vec3(50.0f, 0.0f, 50.0f));
		jupiter = glm::rotate(jupiter, (float)timeValue * 0.15f,
			glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(jupiter));

		textureJupiter.bind(GL_TEXTURE0);
		ambientMapLoc = lightingShader.getUniformLocation("material.ambient");
		glUniform1i(ambientMapLoc, 0);

		sp2.render();
		lightingShader.turnOff();
		/***********************************************************/

		/******************** SATURN ***********************************/
		lightingShader.turnOn();

		// Get the uniform locations
		modelLoc = lightingShader.getUniformLocation("model");
		viewLoc = lightingShader.getUniformLocation("view");
		projLoc = lightingShader.getUniformLocation("projection");

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glm::mat4 saturn;
		saturn = glm::scale(saturn, glm::vec3(0.5, 0.5, 0.5));
		saturn = glm::translate(saturn, glm::vec3(60.0f, 0.0f, 60.0f));
		saturn = glm::rotate(saturn, (float)timeValue * 0.4f,
			glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(saturn));

		textureSaturn.bind(GL_TEXTURE0);
		ambientMapLoc = lightingShader.getUniformLocation("material.ambient");
		glUniform1i(ambientMapLoc, 0);

		sp2.render();
		lightingShader.turnOff();
		/***********************************************************/


		/******************** URANUS ***********************************/
		lightingShader.turnOn();

		// Get the uniform locations
		modelLoc = lightingShader.getUniformLocation("model");
		viewLoc = lightingShader.getUniformLocation("view");
		projLoc = lightingShader.getUniformLocation("projection");

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glm::mat4 uranus;
		uranus = glm::scale(uranus, glm::vec3(0.5, 0.5, 0.5));
		uranus = glm::translate(uranus, glm::vec3(70.0f, 0.0f, 70.0f));
		uranus = glm::rotate(uranus, (float)timeValue * 0.3f,
			glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(uranus));

		textureUranus.bind(GL_TEXTURE0);
		ambientMapLoc = lightingShader.getUniformLocation("material.ambient");
		glUniform1i(ambientMapLoc, 0);

		sp2.render();
		lightingShader.turnOff();
		/***********************************************************/

		/******************** NEPTUNE ***********************************/
		lightingShader.turnOn();

		// Get the uniform locations
		modelLoc = lightingShader.getUniformLocation("model");
		viewLoc = lightingShader.getUniformLocation("view");
		projLoc = lightingShader.getUniformLocation("projection");

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glm::mat4 neptune;
		neptune = glm::scale(neptune, glm::vec3(0.5, 0.5, 0.5));
		neptune = glm::translate(neptune, glm::vec3(80.0f, 0.0f, 80.0f));
		neptune = glm::rotate(neptune, (float)timeValue * 0.1f,
			glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(neptune));

		textureNeptune.bind(GL_TEXTURE0);
		ambientMapLoc = lightingShader.getUniformLocation("material.ambient");
		glUniform1i(ambientMapLoc, 0);

		sp2.render();
		lightingShader.turnOff();
		/***********************************************************/


		/*************************** LIGHT *************************/
		lampShader.turnOn();
		// Create transformations
		modelLoc = lampShader.getUniformLocation("model");
		viewLoc = lampShader.getUniformLocation("view");
		projLoc = lampShader.getUniformLocation("projection");
		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glm::mat4 light;
		light = glm::translate(glm::mat4(), lightPos);
		light = glm::scale(light, glm::vec3(0.4, 0.4, 0.4));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(light));
		sp.render();
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
		cubeModel = glm::scale(cubeModel, glm::vec3(20.0f, 20.0f, 20.0f));
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


		/*********************** MODELO *********************************
		envCubeShader.turnOn();
		
		view = inputManager.getCameraFPS()->GetViewMatrix();
		viewLoc = envCubeShader.getUniformLocation("view");
		projLoc = envCubeShader.getUniformLocation("projection");
		modelLoc = envCubeShader.getUniformLocation("model");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		cubeMaptexture->Bind(GL_TEXTURE0);
		cubeTextureId = envCubeShader.getUniformLocation("skybox");
		glUniform1f(cubeTextureId, 0);

		//viewPosLoc = envCubeShader.getUniformLocation("viewPos");
		//glUniform3fv(viewPosLoc,1, glm::value_ptr(inputManager.getCameraFPS()->Position));
		//sp2.render();
		//modelo1.render(&envCubeShader);

		envCubeShader.turnOff();
		********************************************************************/
		glfwSwapBuffers(window);
	}
}

int main(int argc, char ** argv) {
	init(800, 700, "Window GLFW", false);
	applicationLoop();
	destroy();
	return 1;
}

