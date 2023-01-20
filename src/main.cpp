// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <ctime>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

#include <iostream>
#include <chrono>
#include <thread>
#include <cmath>
#include <vector>

#include "shader/shader.hpp"

#include <string.h>
#include <array>

#include "tetris/tetrisblock.hpp"



template <typename T>
void bufferData(GLuint bufferId, std::vector<T>& data) {
    glBindBuffer(GL_ARRAY_BUFFER, bufferId);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(T), data.data(), GL_STATIC_DRAW);
}

void basicTextureInit(GLuint& texture, int width, int height) {
	glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, width, height);

	// Load image
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

template <typename T>
void setTextureContent(const T* array, GLuint textureID, int width, int height) {
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, array);
}

void drawBuffers(GLuint vertexbuffer, GLuint uvbuffer, std::size_t nVerts) {
	// Game texture drawing
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glDrawArrays(GL_TRIANGLES, 0, nVerts);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

int main( void )
{
	// Initialise GLFW
	if(!glfwInit())
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 768, 768, "Tutorial 04 - Colored Cube", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		exit(-1);
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); 


	// Our VAO
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders(
        "../src/vertex.glsl",
        "../src/fragment.glsl"
    );

	// Use our shader program
	glUseProgram(programID);
	
    std::vector<GLfloat> gameVertexData = {
        0.0, -1.0,
		0.0, 1.0,
		1.0, 1.0,

		0.0, -1.0,
		1.0, 1.0,
		1.0, -1.0
    };

	std::vector<GLfloat> infoVertexData = {
		-1.0, -1.0,
		-1.0, 1.0,
		0.0, 1.0,

		-1.0, -1.0,
		0.0, 1.0,
		0.0, -1.0

	};

	// We will use this for both vertex buffers
	std::vector<GLfloat> uvBufferData = {
		0.0, 0.0,
		0.0, 1.0,
		1.0, 1.0,

		0.0, 0.0,
		1.0, 1.0,
		1.0, 0.0
	};

	// Array for holding the texture pixel colors
	std::array<Color, TETRIS_GRID_N> pixel_colors;
	memset(pixel_colors.data(), 0, sizeof(Color) * TETRIS_GRID_N);

	TetrisPiece p(TetrisPiece::TetrisPieces::T);
	p.setColors(pixel_colors);

	// Info texture
	const int infoX = 100;
	const int infoY = 200;
	std::array<Color, infoX * infoY> info_colors;
	memset(info_colors.data(), 127, sizeof(Color) * infoX * infoY);

	// Game texture draw
	GLuint gamevertexbuffer;
	glGenBuffers(1, &gamevertexbuffer);

	GLuint infovertexbuffer;
	glGenBuffers(1, &infovertexbuffer);

	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);

	// Game textures

	// Info texture start
	GLuint infotexture;
	glActiveTexture(GL_TEXTURE0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	basicTextureInit(infotexture, infoX, infoY);
	setTextureContent(info_colors.data(), infotexture, infoX, infoY);

	// Game texture 
	GLuint gametexture;
	glActiveTexture(GL_TEXTURE1);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	basicTextureInit(gametexture, TETRIS_GRID_X, TETRIS_GRID_Y);
	setTextureContent(pixel_colors.data(), gametexture, TETRIS_GRID_X, TETRIS_GRID_Y);

	GLuint textureUniform = glGetUniformLocation(programID, "gameTexture");

	bufferData(gamevertexbuffer, gameVertexData);
	bufferData(infovertexbuffer, infoVertexData);
	bufferData(uvbuffer, uvBufferData);

	// Some variables
    using time_point = std::chrono::system_clock::time_point;
    long frameDelta = std::floor(1.f / 60.f) * 1000.f;

	long i = 0;

	do{
		if (i % 20 == 0) {
			p.rotate();
			pixel_colors.fill(Color{ 0, 0, 0 });
			p.setColors(pixel_colors);
			setTextureContent(pixel_colors.data(), gametexture, TETRIS_GRID_X, TETRIS_GRID_Y);
		}
		i ++;
		

        time_point frameStart = std::chrono::system_clock::now();
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Draw game info texture
		glUniform1i(textureUniform, 0);
		drawBuffers(infovertexbuffer, uvbuffer, infoVertexData.size());
		
		// Game texture drawing
		glUniform1i(textureUniform, 1);
		drawBuffers(gamevertexbuffer, uvbuffer, gameVertexData.size());

		// End game texture drawing

        time_point frameEnd = std::chrono::system_clock::now();

        long difference = std::chrono::time_point_cast<std::chrono::milliseconds>(frameEnd).time_since_epoch().count();
        difference -= std::chrono::time_point_cast<std::chrono::milliseconds>(frameStart).time_since_epoch().count();

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

        std::this_thread::sleep_for(std::chrono::milliseconds(frameDelta - difference));

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Cleanup VBO and shader
	glDeleteBuffers(1, &gamevertexbuffer);
	glDeleteBuffers(1, &infovertexbuffer);
	glDeleteBuffers(1, &uvbuffer);

	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}
