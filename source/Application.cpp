#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Texture.h"
#include "AnimatedGifTexture.h"
#include "shaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"

const unsigned int width = 800;
const unsigned int height = 800;

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
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, "Tesseract", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Load GLAD
	gladLoadGL();
	glViewport(0, 0, width, height);

	// Shader Program
	Shader shaderProgram("default.vert", "default.frag");

	// 3D Cube VAO, VBO, EBO
	VAO VAO1;
	VAO1.Bind();
	VBO VBO1(vertices, sizeof(vertices));
	EBO EBO1(indices, sizeof(indices));

	VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
	VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();

	// Background Quad VAO, VBO, EBO
	VAO bgVAO;
	bgVAO.Bind();
	VBO bgVBO(bgVertices, sizeof(bgVertices));
	EBO bgEBO(bgIndices, sizeof(bgIndices));

	bgVAO.LinkAttrib(bgVBO, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
	bgVAO.LinkAttrib(bgVBO, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	bgVAO.LinkAttrib(bgVBO, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	bgVAO.Unbind();
	bgVBO.Unbind();
	bgEBO.Unbind();

	GLuint uniID = glGetUniformLocation(shaderProgram.ID, "scale");

	// Space Background Texture
	Texture spaceTex("Space.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	spaceTex.texUnit(shaderProgram, "tex0", 0);

	// Animated GIF Texture for the Cube
	AnimatedGifTexture cubeGif("Tess.gif");

	// Variables for Rotation & Mouse Interaction
	float autoRotation = 0.0f;
	double prevTime = glfwGetTime();

	bool isDragging = false;
	double prevMouseX = 0.0, prevMouseY = 0.0;
	float mouseRotX = 0.0f;
	float mouseRotY = 0.0f;
	float sensitivity = 0.4f;

	glEnable(GL_DEPTH_TEST);

	// Main Loop
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shaderProgram.Activate();

		int modelLoc = glGetUniformLocation(shaderProgram.ID, "model");
		int viewLoc = glGetUniformLocation(shaderProgram.ID, "view");
		int projLoc = glGetUniformLocation(shaderProgram.ID, "proj");

		glUniform1f(uniID, 0.5f);

		// 1. Draw Space Background Quad
		glDisable(GL_DEPTH_TEST);
		glm::mat4 identity = glm::mat4(1.0f);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(identity));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(identity));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(identity));

		spaceTex.Bind();
		bgVAO.Bind();
		glDrawElements(GL_TRIANGLES, sizeof(bgIndices) / sizeof(int), GL_UNSIGNED_INT, 0);

		// 2. Draw 3D Cube
		glEnable(GL_DEPTH_TEST);

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

		double crntTime = glfwGetTime();
		if (crntTime - prevTime >= 1.0 / 60.0)
		{
			autoRotation += 0.8f;
			prevTime = crntTime;
		}

		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 proj = glm::mat4(1.0f);

		model = glm::rotate(model, glm::radians(mouseRotX), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mouseRotY), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(autoRotation), glm::vec3(0.5f, 1.0f, 0.0f));

		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -2.5f));
		proj = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.0f);

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

		cubeGif.Bind(glfwGetTime(), GL_TEXTURE0);
		VAO1.Bind();
		glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Cleanup
	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	bgVAO.Delete();
	bgVBO.Delete();
	bgEBO.Delete();
	cubeGif.Delete();
	spaceTex.Delete();
	shaderProgram.Delete();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}