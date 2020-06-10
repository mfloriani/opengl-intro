#ifndef __SHADER_PROGRAM_H__
#define __SHADER_PROGRAM_H__

#include <string>
#include <map>
#include "GLEW/glew.h"
#include "glm/glm.hpp"

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
	void SetUniform(const GLchar* name, const glm::vec2& v);
	void SetUniform(const GLchar* name, const glm::vec3& v);
	void SetUniform(const GLchar* name, const glm::vec4& v);

private:
	std::string FileToString(const std::string& filename);
	void CheckCompileErrors(GLuint shader, ShaderType type);
	GLint GetUniformLocation(const GLchar* name);

	GLuint m_handle;
	std::map<std::string, GLint> m_uniformLocations;
};

#endif // !__SHADER_PROGRAM_H__