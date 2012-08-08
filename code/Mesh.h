
#ifndef _MESH_H_
#define _MESH_H_

#include <GL/glew.h>
#include "HardwareBuffer.h"

namespace framework
{

class Mesh
{
public:
	Mesh(void);

	virtual ~Mesh(void);

	void bind(void);

	void unbind(void);

	inline HardwareBuffer* getIndexBuffer(void) {
		return indexBuffer;
	}

	inline HardwareBuffer* getVertexBuffer(void) {
		return vertexBuffer;
	}

	bool fromFile(const char* filename);

	bool toFile(const char* filename);

	void makeQuad();

	void draw();

	void createGrid(unsigned int sideSize, float quadSize, unsigned int patchSize = 3);


private:
	/// VAO
	GLuint vao;

	/// Index buffer
	HardwareBuffer* indexBuffer;

	/// Vertex buffer
	HardwareBuffer* vertexBuffer;

	/// Primitive type
	GLuint type;
};
}

#endif
