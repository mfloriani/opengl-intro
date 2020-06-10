#ifndef __SHADER_PROGRAM_H__
#define __SHADER_PROGRAM_H__

#include "GLEW/glew.h"
#include <string>

class ShaderProgram
{
public:
	ShaderProgram();
	~ShaderProgram();

	enum ShaderType
	{
		VERTEX,
		FRAGMENT,
		PROGRAM
	};

	bool LoadShaders(const char* vsFilename, const char* fsFilename);
	void Use();

private:
	std::string FileToString(const std::string& filename);
	void CheckCompileErrors(GLuint shader, ShaderType type);

	GLuint m_handle;
};

#endif // !__SHADER_PROGRAM_H__