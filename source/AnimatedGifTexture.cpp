#include "AnimatedGifTexture.h"
#include <iostream>
#include <fstream>
#include <stb/stb_image.h>

AnimatedGifTexture::AnimatedGifTexture(const char* filePath)
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
		// stb_image stores delays[i] in milliseconds (1/1000ths of a second)
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

void AnimatedGifTexture::Bind(double currentTimeSec, GLenum slot)
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

void AnimatedGifTexture::Delete()
{
	if (!textureIDs.empty())
	{
		glDeleteTextures((GLsizei)textureIDs.size(), textureIDs.data());
		textureIDs.clear();
	}
}