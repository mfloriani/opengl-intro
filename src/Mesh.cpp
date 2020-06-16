#include "Mesh.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

std::vector<std::string> split(std::string s, std::string d)
{
	std::vector<std::string> result;
	while (true)
	{
		int pos = s.find(d);
		if (pos == -1)
		{
			result.push_back(s);
			break;
		}
		result.push_back(s.substr(0, pos));
		s = s.substr(pos + 1, s.size() - pos - 1);
	}
	return result;
}

Mesh::Mesh(): m_loaded(false)
{
}


Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
}

bool Mesh::LoadOBJ(const std::string& filename)
{
	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> tempVertices;
	std::vector<glm::vec3> tempNormals;
	std::vector<glm::vec2> tempUVs;

	if (filename.find(".obj") != std::string::npos)
	{
		std::ifstream fin(filename, std::ios::in);
		if (!fin)
		{
			std::cerr << "Cannot open " << filename << std::endl;
			return false;
		}

		std::cout << "Loading OBJ file " << filename << " ..." << std::endl;

		std::string lineBuffer;
		while (std::getline(fin, lineBuffer))
		{
			std::stringstream ss(lineBuffer);
			std::string cmd;
			ss >> cmd;

			if (cmd == "v")
			{
				glm::vec3 vertex;
				int dim = 0;
				while (dim < 3 && ss >> vertex[dim]) dim++;

				tempVertices.push_back(vertex);
			}
			else if (cmd == "vt")
			{
				glm::vec2 uv;
				int dim = 0;
				while (dim < 2 && ss >> uv[dim]) dim++;
				
				tempUVs.push_back(uv);
			}
			else if (cmd == "vn")
			{
				glm::vec3 normal;
				int dim = 0;
				while (dim < 3 && ss >> normal[dim]) dim++;

				normal = glm::normalize(normal);
				tempNormals.push_back(normal);
			}
			else if (cmd == "f")
			{
				std::string faceData;
				int vIndex, tIndex, nIndex;

				while (ss >> faceData)
				{
					std::vector<std::string> data = split(faceData, "/");

					// vertex index
					if (data[0].size() > 0)
					{
						sscanf_s(data[0].c_str(), "%d", &vIndex);
						vertexIndices.push_back(vIndex);
					}

					// has texture coord index?
					if (data.size() >= 1)
					{
						if (data[1].size() > 0)
						{
							sscanf_s(data[1].c_str(), "%d", &tIndex);
							uvIndices.push_back(tIndex);
						}
					}

					// has normal index?
					if (data.size() >= 2)
					{
						if (data[2].size() > 0)
						{
							sscanf_s(data[2].c_str(), "%d", &nIndex);
							normalIndices.push_back(nIndex);
						}
					}
				}				
			}
		}
		fin.close();

		// For each vertex of each triangle
		for (unsigned int i = 0; i < vertexIndices.size(); i++)
		{
			Vertex meshVertex;

			if (tempVertices.size() > 0)
			{
				glm::vec3 vertex = tempVertices[vertexIndices[i] - 1];
				meshVertex.position = vertex;
			}

			if (tempNormals.size() > 0)
			{
				glm::vec3 normal = tempNormals[normalIndices[i] - 1];
				meshVertex.normal = normal;
			}

			if (tempUVs.size() > 0)
			{
				glm::vec2 uv = tempUVs[uvIndices[i] - 1];
				meshVertex.texCoord = uv;
			}

			m_vertices.push_back(meshVertex);
		}

		// Create and initialize the buffers
		InitBuffers();

		return (m_loaded = true);
	}

	return false;
}

void Mesh::Draw()
{
	if (!m_loaded) return;

	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLES, 0, m_vertices.size());
	glBindVertexArray(0);
}

void Mesh::InitBuffers()
{
	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), &m_vertices[0], GL_STATIC_DRAW);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
	glEnableVertexAttribArray(0);

	// normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// tex coord
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
}