#include<iostream>
#include<fstream>
#include<vector>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<stb/stb_image.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include"Texture.h"
#include"shaderClass.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"


const unsigned int width = 800;
const unsigned int height = 800;


// Class to load and animate GIF textures in OpenGL
class AnimatedGifTexture
{
public:
	std::vector<GLuint> textureIDs;
	std::vector<int> frameDelaysMs;
	int totalFrames = 0;
	int width = 0;
	int height = 0;
	int totalDurationMs = 0;

	AnimatedGifTexture(const char* filePath)
	{
		std::ifstream file(filePath, std::ios::binary | std::ios::ate);
		if (!file.is_open())
		{
			std::cout << "Failed to open GIF file: " << filePath << std::endl;
			return;
		}

		std::streamsize size = file.tellg();
		file.seekg(0, std::ios::beg);
		std::vector<unsigned char> buffer(static_cast<size_t>(size));
		if (!file.read((char*)buffer.data(), size))
		{
			std::cout << "Failed to read GIF file data: " << filePath << std::endl;
			return;
		}

		stbi_set_flip_vertically_on_load(true);

		int* delays = nullptr;
		int comp = 0;
		unsigned char* data = stbi_load_gif_from_memory(
			buffer.data(),
			(int)buffer.size(),
			&delays,
			&width,
			&height,
			&totalFrames,
			&comp,
			4
		);

		if (!data || totalFrames <= 0)
		{
			std::cout << "Failed to decode animated GIF: " << filePath << std::endl;
			if (data) stbi_image_free(data);
			if (delays) stbi_image_free(delays);
			return;
		}

		textureIDs.resize(totalFrames);
		frameDelaysMs.resize(totalFrames);
		glGenTextures(totalFrames, textureIDs.data());

		int frameSize = width * height * 4;
		totalDurationMs = 0;

		for (int i = 0; i < totalFrames; i++)
		{
			// stb_image already saves delays[i] in milliseconds (1/1000ths of a second)
			int delayMs = (delays && delays[i] > 0) ? delays[i] : 33;
			frameDelaysMs[i] = delayMs;
			totalDurationMs += delayMs;

			glBindTexture(GL_TEXTURE_2D, textureIDs[i]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RGBA,
				width,
				height,
				0,
				GL_RGBA,
				GL_UNSIGNED_BYTE,
				data + (i * frameSize)
			);
			glGenerateMipmap(GL_TEXTURE_2D);
		}

		glBindTexture(GL_TEXTURE_2D, 0);
		stbi_image_free(data);
		if (delays) stbi_image_free(delays);
	}

	void Bind(double currentTimeSec, GLenum slot = GL_TEXTURE0)
	{
		if (totalFrames == 0) return;

		GLuint texToBind = textureIDs[0];
		if (totalFrames > 1 && totalDurationMs > 0)
		{
			int currentMs = (int)(currentTimeSec * 1000.0) % totalDurationMs;
			int accumMs = 0;
			for (int i = 0; i < totalFrames; i++)
			{
				accumMs += frameDelaysMs[i];
				if (currentMs < accumMs)
				{
					texToBind = textureIDs[i];
					break;
				}
			}
		}

		glActiveTexture(slot);
		glBindTexture(GL_TEXTURE_2D, texToBind);
	}

	void Delete()
	{
		if (!textureIDs.empty())
		{
			glDeleteTextures((GLsizei)textureIDs.size(), textureIDs.data());
			textureIDs.clear();
		}
	}
};


// Vertices coordinates
GLfloat vertices[] =
{ //     COORDINATES        /        COLORS         /    TexCoord   //
	// Front face
	-0.5f, -0.5f,  0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,     0.83f, 0.70f, 0.44f,	1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,     0.83f, 0.70f, 0.44f,	1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 1.0f,

	// Back face
	-0.5f, -0.5f, -0.5f,     0.83f, 0.70f, 0.44f,	1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,     0.83f, 0.70f, 0.44f,	1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 0.0f,

	// Top face
	-0.5f,  0.5f, -0.5f,     0.92f, 0.86f, 0.76f,	0.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,     0.92f, 0.86f, 0.76f,	0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,     0.92f, 0.86f, 0.76f,	1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,     0.92f, 0.86f, 0.76f,	1.0f, 1.0f,

	// Bottom face
	-0.5f, -0.5f, -0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,     0.83f, 0.70f, 0.44f,	1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,     0.83f, 0.70f, 0.44f,	1.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 1.0f,

	// Right face
	 0.5f, -0.5f, -0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,     0.83f, 0.70f, 0.44f,	1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,     0.83f, 0.70f, 0.44f,	1.0f, 0.0f,

	// Left face
	-0.5f, -0.5f, -0.5f,     0.83f, 0.70f, 0.44f,	1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,     0.83f, 0.70f, 0.44f,	1.0f, 1.0f
};

// Indices for vertices order
GLuint indices[] =
{
	// Front
	0, 1, 2,
	0, 2, 3,
	// Back
	4, 5, 6,
	4, 6, 7,
	// Top
	8, 9, 10,
	8, 10, 11,
	// Bottom
	12, 13, 14,
	12, 14, 15,
	// Right
	16, 17, 18,
	16, 18, 19,
	// Left
	20, 21, 22,
	20, 22, 23
};

// Background vertices coordinates
GLfloat bgVertices[] =
{ //     COORDINATES        /        COLORS         /    TexCoord   //
	-1.0f, -1.0f, 0.0f,     1.0f, 1.0f, 1.0f,	    0.0f, 0.0f,
	 1.0f, -1.0f, 0.0f,     1.0f, 1.0f, 1.0f,	    1.0f, 0.0f,
	 1.0f,  1.0f, 0.0f,     1.0f, 1.0f, 1.0f,	    1.0f, 1.0f,
	-1.0f,  1.0f, 0.0f,     1.0f, 1.0f, 1.0f,	    0.0f, 1.0f
};

// Background indices for vertices order
GLuint bgIndices[] =
{
	0, 1, 2,
	0, 2, 3
};



int main()
{
	// Initialize GLFW
	glfwInit();
	// Tell GLFW what version of OpenGL we are using 
	// In this case we are using OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Tell GLFW we are using the CORE profile
	// So that means we only have the modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a GLFWwindow object of 800 by 800 pixels, naming it "YoutubeOpenGL"
	GLFWwindow* window = glfwCreateWindow(width, height, "YoutubeOpenGL", NULL, NULL);
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
	// Specify the viewport of OpenGL in the Window
	// In this case the viewport goes from x = 0, y = 0, to x = 800, y = 800
	glViewport(0, 0, width, height);



	// Generates Shader object using shaders default.vert and default.frag
	Shader shaderProgram("default.vert", "default.frag");



	// Generates Vertex Array Object for the 3D Cube and binds it
	VAO VAO1;
	VAO1.Bind();

	// Generates Vertex Buffer Object and links it to vertices
	VBO VBO1(vertices, sizeof(vertices));
	// Generates Element Buffer Object and links it to indices
	EBO EBO1(indices, sizeof(indices));

	// Links VBO attributes such as coordinates and colors to VAO
	VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
	VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	// Unbind all to prevent accidentally modifying them
	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();

	// Generates Vertex Array Object for the Background Quad and binds it
	VAO bgVAO;
	bgVAO.Bind();

	// Generates Background Vertex Buffer Object and links it to bgVertices
	VBO bgVBO(bgVertices, sizeof(bgVertices));
	// Generates Background Element Buffer Object and links it to bgIndices
	EBO bgEBO(bgIndices, sizeof(bgIndices));

	// Links Background VBO attributes to bgVAO
	bgVAO.LinkAttrib(bgVBO, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
	bgVAO.LinkAttrib(bgVBO, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	bgVAO.LinkAttrib(bgVBO, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	bgVAO.Unbind();
	bgVBO.Unbind();
	bgEBO.Unbind();

	// Gets ID of uniform called "scale"
	GLuint uniID = glGetUniformLocation(shaderProgram.ID, "scale");

	// Space Background Texture
	Texture spaceTex("Space.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	spaceTex.texUnit(shaderProgram, "tex0", 0);

	// Animated GIF Texture for the 3D Cube
	AnimatedGifTexture cubeGif("Tess.gif");

	// Variables for Rotation & Mouse Interaction
	float autoRotation = 0.0f;
	double prevTime = glfwGetTime();

	bool isDragging = false;
	double prevMouseX = 0.0, prevMouseY = 0.0;
	float mouseRotX = 0.0f;
	float mouseRotY = 0.0f;
	float sensitivity = 0.4f;

	// Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);

	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		// Specify the color of the background
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Clean the back buffer and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Tell OpenGL which Shader Program we want to use
		shaderProgram.Activate();

		// Outputs matrix locations
		int modelLoc = glGetUniformLocation(shaderProgram.ID, "model");
		int viewLoc = glGetUniformLocation(shaderProgram.ID, "view");
		int projLoc = glGetUniformLocation(shaderProgram.ID, "proj");

		// Assigns a value to the uniform; NOTE: Must always be done after activating the Shader Program
		glUniform1f(uniID, 0.5f);

		// 1. Draw Space Background Quad (with depth test disabled so it stays in the background)
		glDisable(GL_DEPTH_TEST);
		glm::mat4 identity = glm::mat4(1.0f);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(identity));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(identity));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(identity));

		spaceTex.Bind();
		bgVAO.Bind();
		glDrawElements(GL_TRIANGLES, sizeof(bgIndices) / sizeof(int), GL_UNSIGNED_INT, 0);

		// 2. Draw 3D Cube (with depth test enabled)
		glEnable(GL_DEPTH_TEST);

		// Handle Mouse Drag Interaction
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		{
			double mouseX, mouseY;
			glfwGetCursorPos(window, &mouseX, &mouseY);

			if (isDragging)
			{
				float deltaX = (float)(mouseX - prevMouseX);
				float deltaY = (float)(mouseY - prevMouseY);

				mouseRotY += deltaX * sensitivity;
				mouseRotX += deltaY * sensitivity;
			}
			else
			{
				isDragging = true;
			}

			prevMouseX = mouseX;
			prevMouseY = mouseY;
		}
		else
		{
			isDragging = false;
		}

		// Simple timer for continuous default spinning motion
		double crntTime = glfwGetTime();
		if (crntTime - prevTime >= 1.0 / 60.0)
		{
			autoRotation += 0.8f;
			prevTime = crntTime;
		}

		// Initializes matrices so they are not the null matrix
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 proj = glm::mat4(1.0f);

		// Apply user mouse orientation offset
		model = glm::rotate(model, glm::radians(mouseRotX), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mouseRotY), glm::vec3(0.0f, 1.0f, 0.0f));

		// Apply the default spinning motion along vec3(0.5f, 1.0f, 0.0f)
		model = glm::rotate(model, glm::radians(autoRotation), glm::vec3(0.5f, 1.0f, 0.0f));

		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -2.5f));
		proj = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.0f);

		// Outputs the matrices into the Vertex Shader
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

		// Binds the active GIF frame texture for the cube
		cubeGif.Bind(glfwGetTime(), GL_TEXTURE0);
		VAO1.Bind();
		// Draw primitives, number of indices, datatype of indices, index of indices
		glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);

		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}



	// Delete all the objects we've created
	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	bgVAO.Delete();
	bgVBO.Delete();
	bgEBO.Delete();
	cubeGif.Delete();
	spaceTex.Delete();
	shaderProgram.Delete();
	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}