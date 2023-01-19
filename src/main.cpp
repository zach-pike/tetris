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

#define TETRIS_GRID_X (int)(16)
#define TETRIS_GRID_Y (int)(16)

constexpr int TETRIS_GRID_N = TETRIS_GRID_X * TETRIS_GRID_Y;

struct Color {
	std::uint8_t r;
	std::uint8_t g;
	std::uint8_t b;
};

class TetrisPiece {
private:
	std::vector<std::tuple<int, int>> parts = {
		std::tuple(0, 0),
		std::tuple(0, -1),
		std::tuple(-1, 0),
		std::tuple(1, 0),
		std::tuple(0, 1),
	};
public:
	TetrisPiece() {}
	~TetrisPiece() {}

	void rotate() {
		for (auto& part : parts) {
			part = std::tuple(-std::get<1>(part), std::get<0>(part));
		}
	}

	void setColors(std::array<Color, TETRIS_GRID_N>& array) {
		int x = 8;
		int y = 8;

		for (auto part : parts) {
			// Calculate the position
			int xC = x + std::get<0>(part);
			int yC = y + std::get<1>(part);

			int idx = xC + (yC * TETRIS_GRID_X);

			array.at(idx) = Color{ 255, 255, 255 };

			if (idx == 137) std::cout << "ppppp\n";

			std::cout << "X: " << xC << " Y: " << yC << '\n';
		}
	}
};

template <typename T>
void bufferData(GLuint bufferId, std::vector<T>& data) {
    glBindBuffer(GL_ARRAY_BUFFER, bufferId);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(T), data.data(), GL_STATIC_DRAW);
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
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders(
        "../src/vertex.glsl",
        "../src/fragment.glsl"
    );

	glUseProgram(programID);
	
    std::vector<GLfloat> vertexBufferData = {
        -1.0, -1.0,
		-1.0, 1.0,
		1.0, 1.0,

		-1.0, -1.0,
		1.0, 1.0,
		1.0, -1.0
    };
	std::vector<GLfloat> uvBufferData = {
		0.0, 0.0,
		0.0, 1.0,
		1.0, 1.0,

		0.0, 0.0,
		1.0, 1.0,
		0.0, 1.0
	};

	GLuint textureUniform = glGetUniformLocation(programID, "gameTexture");


	std::array<Color, TETRIS_GRID_N> pixel_colors;
	pixel_colors.fill(Color{ 0, 0, 0 });

	pixel_colors[0] = Color{ 127, 127, 127 };

	TetrisPiece p;

	p.setColors(pixel_colors);

	std::cout << (int)pixel_colors[137].r << '\n';
	

	GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, TETRIS_GRID_X, TETRIS_GRID_Y);

	// Load image
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, TETRIS_GRID_X, TETRIS_GRID_Y, GL_RGB, GL_UNSIGNED_BYTE, pixel_colors.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glActiveTexture(GL_TEXTURE0);
    glUniform1i(textureUniform, 0);
	

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	
	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);

	// Buffer the quad
	bufferData(vertexbuffer, vertexBufferData);
	bufferData(uvbuffer, uvBufferData);

	// Some variables
    using time_point = std::chrono::system_clock::time_point;
    long frameDelta = std::floor(1.f / 60.f) * 1000.f;

	do{
        time_point frameStart = std::chrono::system_clock::now();
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glDrawArrays(GL_TRIANGLES, 0, vertexBufferData.size());

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

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
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);

	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}
