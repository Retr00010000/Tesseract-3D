#ifndef ANIMATED_GIF_TEXTURE_CLASS_H
#define ANIMATED_GIF_TEXTURE_CLASS_H

#include <glad/glad.h>
#include <vector>

class AnimatedGifTexture
{
public:
	std::vector<GLuint> textureIDs;
	std::vector<int> frameDelaysMs;
	int totalFrames = 0;
	int width = 0;
	int height = 0;
	int totalDurationMs = 0;

	// Constructor: Reads file and generates OpenGL textures
	AnimatedGifTexture(const char* filePath);

	// Binds the active texture frame based on elapsed time
	void Bind(double currentTimeSec, GLenum slot = GL_TEXTURE0);

	// Deletes OpenGL texture buffers
	void Delete();
};

#endif