#include"Model.h"

const unsigned int width = 1200;
const unsigned int height = 1200;

// Takes care of the information needed to draw the windows
const unsigned int numWindows = 2;
glm::vec3 positionsWin[numWindows];
float rotationsWin[numWindows];

// Takes care of drawing the windows in the right order
unsigned int orderDraw[numWindows];
float distanceCamera[numWindows];

// Compare function
int compare(const void* a, const void* b)
{
	double diff = distanceCamera[*(int*)b] - distanceCamera[*(int*)a];
	return  (0 < diff) - (diff < 0);
}

int main()
{
	// Initialize GLFW (Window)
	glfwInit();

	// Tell GLFW what version of OpenGL so v(3.3)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Tell GLFW we are using the CORE profile so we only have the modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a GLFWwindow object of 800 by 800 pixels, naming it "Transparency & Blending"
	std::string title = "Transparency & Blending";
	GLFWwindow* window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
	// Error check if the window fails to create
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// Introduce the window into the current context
	glfwMakeContextCurrent(window);

	//Load GLAD so it configures OpenGL
	gladLoadGL();
	// Specify the viewport of OpenGL in the Window Viewport from x = 0, y = 0, to x = 800, y = 800
	glViewport(0, 0, width, height);

	// Generates Shader object using shaders default.vert and default.frag
	Shader shaderProgram("default.vert", "default.frag");
	Shader grassProgram("default.vert", "grass.frag");
	Shader winProgram("default.vert", "windows.frag");

	// Take care of all the light related things
	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);

	shaderProgram.Activate();
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	grassProgram.Activate();
	glUniform4f(glGetUniformLocation(grassProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(grassProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	// Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);
	// Enables Cull Facing
	glEnable(GL_CULL_FACE);
	// Keeps front faces
	glCullFace(GL_FRONT);
	// Uses counter clock-wise standard
	glFrontFace(GL_CCW);
	// Configures the blending function
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Creates camera object
	Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));

	std::string groundPath = "../models/ground/scene.gltf";
	std::string grassPath = "../models/grass/scene.gltf";
	std::string winPath = "../models/windows/scene.gltf";

	// Load in models
	Model ground(groundPath.c_str());
	Model grass(grassPath.c_str());
	Model windows(winPath.c_str());

	// Variables to create periodic event for FPS displaying
	double prevTime = 0.0;
	double crntTime = 0.0;
	double timeDiff;
	// Keeps track of the amount of frames in timeDiff
	unsigned int counter = 0;

	// Use this to disable VSync (not advized)
	//glfwSwapInterval(0);

	// Generates all windows
	for (unsigned int i = 0; i < numWindows; i++)
	{
		positionsWin[i] = glm::vec3
		(
			-15.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (15.0f - (-15.0f)))),
			1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (4.0f - 1.0f))),
			-15.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (15.0f - (-15.0f))))
		);
		rotationsWin[i] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 1.0f));
		orderDraw[i] = i;
	}

	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		// Updates counter and times
		crntTime = glfwGetTime();
		timeDiff = crntTime - prevTime;
		counter++;

		if (timeDiff >= 1.0 / 30.0)
		{
			// Creates new title
			std::string FPS = std::to_string((1.0 / timeDiff) * counter);
			std::string ms = std::to_string((timeDiff / counter) * 1000);
			std::string newTitle = title + " / FPS: " + FPS + " / ms: " + ms;
			glfwSetWindowTitle(window, newTitle.c_str());

			// Resets times and counter
			prevTime = crntTime;
			counter = 0;

			// Use this if you have disabled VSync
			//camera.Inputs(window);
		}

		// Specify the color of the background
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Clean the back buffer and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Handles camera inputs (delete this if you have disabled VSync)
		//camera.Inputs(window);
		// Updates and exports the camera matrix to the Vertex Shader
		camera.updateMatrix(45.0f, 0.1f, 100.0f);

		// Draw the normal model
		ground.Draw(shaderProgram, camera);

		// Disable cull face so that grass and windows have both faces
		glDisable(GL_CULL_FACE);
		grass.Draw(grassProgram, camera);
		// Enable blending for windows
		glEnable(GL_BLEND);
		// Get distance from each window to the camera
		for (unsigned int i = 0; i < numWindows; i++)
		{
			distanceCamera[i] = glm::length(camera.Position - positionsWin[i]);
		}
		// Sort windows by distance from camera
		qsort(orderDraw, numWindows, sizeof(unsigned int), compare);
		// Draw windows
		for (unsigned int i = 0; i < numWindows; i++)
		{
			windows.Draw(winProgram, camera, positionsWin[orderDraw[i]], glm::quat(1.0f, 0.0f, rotationsWin[orderDraw[i]], 0.0f));
		}
		glDisable(GL_BLEND);
		glEnable(GL_CULL_FACE);

		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}

	// Delete all the objects we've created
	shaderProgram.Delete();
	grassProgram.Delete();
	winProgram.Delete();
	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}