#include "ShaderProgram.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include "glm/gtc/type_ptr.hpp"

ShaderProgram::ShaderProgram(): m_handle(0)
{

}

ShaderProgram::~ShaderProgram()
{
	glDeleteProgram(m_handle);
}

bool ShaderProgram::LoadShaders(const char* vsFilename, const char* fsFilename)
{
	std::string vsScript = FileToString(vsFilename);
	const GLchar* vertexShader = vsScript.c_str();

	std::string fsScript = FileToString(fsFilename);
	const GLchar* fragmentShader = fsScript.c_str();

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vs, 1, &vertexShader, NULL);
	glShaderSource(fs, 1, &fragmentShader, NULL);

	glCompileShader(vs);
	CheckCompileErrors(vs, VERTEX);
	glCompileShader(fs);
	CheckCompileErrors(fs, FRAGMENT);

	m_handle = glCreateProgram();
	glAttachShader(m_handle, vs);
	glAttachShader(m_handle, fs);
	glLinkProgram(m_handle);
	CheckCompileErrors(m_handle, PROGRAM);
	
	glDeleteShader(vs);
	glDeleteShader(fs);

	m_uniformLocations.clear();

	return true;
}

void ShaderProgram::Use()
{
	if (m_handle > 0)
	{
		glUseProgram(m_handle);
	}
}

std::string ShaderProgram::FileToString(const std::string& filename)
{
	std::stringstream ss;
	std::ifstream file;

	try
	{
		file.open(filename, std::ios::in);
		if (!file.fail())
		{
			ss << file.rdbuf();
		}
		file.close();
	}
	catch (const std::exception&)
	{
		std::cerr << "Error reading shader file" << std::endl;
	}
	return ss.str();
}

void ShaderProgram::CheckCompileErrors(GLuint handle, ShaderType type)
{
	GLint status;
	
	if (type == PROGRAM)
	{
		glGetProgramiv(handle, GL_LINK_STATUS, &status);
		if (!status)
		{
			GLint length = 0;
			glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &length);
			std::string errorLog(length, ' ');
			glGetProgramInfoLog(handle, length, &length, &errorLog[0]);
			std::cerr << "Shader program failed to link" << errorLog << std::endl;
		}
	}
	else
	{
		glGetShaderiv(handle, GL_COMPILE_STATUS, &status);
		if (!status)
		{
			GLint length = 0;
			glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &length);
			std::string errorLog(length, ' ');
			glGetShaderInfoLog(handle, length, &length, &errorLog[0]);
			std::cerr << "Shader failed to compile" << errorLog << std::endl;
		}
	}	
}

void ShaderProgram::SetUniform(const GLchar* name, const glm::vec2& v)
{
	GLint location = GetUniformLocation(name);
	glUniform2f(location, v.x, v.y);
}

void ShaderProgram::SetUniform(const GLchar* name, const glm::vec3& v)
{
	GLint location = GetUniformLocation(name);
	glUniform3f(location, v.x, v.y, v.z);
}

void ShaderProgram::SetUniform(const GLchar* name, const glm::vec4& v)
{
	GLint location = GetUniformLocation(name);
	glUniform4f(location, v.x, v.y, v.z, v.w);
}

void ShaderProgram::SetUniform(const GLchar* name, const glm::mat4& m)
{
	GLint location = GetUniformLocation(name);
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(m));
}

GLint ShaderProgram::GetUniformLocation(const GLchar* name)
{
	std::map<std::string, GLint>::iterator it = m_uniformLocations.find(name);
	if (it == m_uniformLocations.end())
	{
		m_uniformLocations[name] = glGetUniformLocation(m_handle, name);
	}
	return m_uniformLocations[name];
}

GLuint ShaderProgram::GetProgram() const
{
	return m_handle;
}