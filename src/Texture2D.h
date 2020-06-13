#ifndef __TEXTURE2D_H__
#define __TEXTURE2D_H__

#include "GLEW/glew.h"
#include <string>

class Texture2D
{
public:
	Texture2D();
	virtual ~Texture2D();

	bool Load(const std::string &filename, bool genMipMaps = true);
	void Bind(GLuint texunit = 0);

private:
	GLuint m_texture;
};

#endif