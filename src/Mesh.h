#ifndef __MESH_H__
#define __MESH_H__

#include <vector>
#include <string>

#include "GLEW/glew.h"
#include "glm/glm.hpp"

struct Vertex
{
	glm::vec3 position;
	glm::vec2 texCoord;
};

class Mesh
{
public:
	Mesh();
	~Mesh();

	bool LoadOBJ(const std::string& filename);
	void Draw();

private:
	void InitBuffers();

	bool m_loaded;
	std::vector<Vertex> m_vertices;
	GLuint m_vbo;
	GLuint m_vao;
};

#endif // !__MESH_H__