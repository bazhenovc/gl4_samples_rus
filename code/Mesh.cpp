
#include "Mesh.h"
#include <GL/glew.h>
#include <stdio.h>
#include "Vertex.h"
#include <malloc.h>
#include <stdint.h>
#include <vector>

#define logPrint printf

namespace framework
{
Mesh::Mesh(void)
{
	glGenVertexArrays(1, &vao);
	indexBuffer = new HardwareBuffer(BT_INDEX);
	vertexBuffer = new HardwareBuffer(BT_VERTEX);
	type = GL_TRIANGLES;
}

Mesh::~Mesh(void)
{
	delete vertexBuffer;
	delete indexBuffer;
	glDeleteVertexArrays(1, &vao);
}

void Mesh::bind(void)
{
	glBindVertexArray(vao);
	getIndexBuffer()->bind();
	getVertexBuffer()->bind();
}

void Mesh::unbind(void)
{
	glBindVertexArray(0);
	getVertexBuffer()->unbind();
	getIndexBuffer()->unbind();
}

bool Mesh::fromFile(const char* filename)
{
	size_t unused = 0; /* Damn ungly paranoic warn_unused_result!!! */

	FILE* file = fopen(filename, "rb");
	if (file == NULL) {
		logPrint("Error: cannot read mesh file %s!\n", filename);
		return false;
	}
	// Read and compare magic number
	char magicNumber;
	unused = fread(&magicNumber, sizeof ( char), 1, file);
	if (magicNumber != 0x01) {
		logPrint("Error: invalid magic number for static mesh %s!\n", filename);
		return false;
	}
	// Read vertex info
	uint32_t vertexNumber;
	unused = fread(&vertexNumber, sizeof ( uint32_t), 1, file);
	// Read vertices
	Vertex* vertices = (Vertex*) malloc(vertexNumber
	                                    * sizeof ( Vertex));
	unused = fread(vertices, sizeof ( Vertex), vertexNumber, file);
	// Read indices
	uint32_t indexNumber;
	unused = fread(&indexNumber, sizeof ( uint32_t), 1, file);
	unsigned int* indices = (unsigned int*) malloc(indexNumber
	                        * sizeof ( unsigned int));
	unused = fread(indices, sizeof ( unsigned int), indexNumber, file);

	/*
	box = AABox();

	for (size_t i = 0; i < vertexNumber; i++)
	{
	  box.addPoint(vertices[i].pos[0], vertices[i].pos[1],
	               vertices[i].pos[2]);
	}

	box.repair();
	*/

	bind();
	indexBuffer->bind();
	indexBuffer->copyData(indices, indexNumber, sizeof ( unsigned int), BU_STREAM);
	indexBuffer->unbind();
	// Copy vertices
	vertexBuffer->bind();
	vertexBuffer->copyData(vertices, vertexNumber, sizeof ( Vertex), BU_STREAM);
	vertexBuffer->unbind();

	bind();

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof (Vertex),
	                      (char*) 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, sizeof (Vertex),
	                      (char*) 0 + 12);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof (Vertex),
	                      (char*) 0 + 20);
	glEnableVertexAttribArray(2);

	unbind();

	free(vertices);
	free(indices);

	fclose(file);
	return true;
}

bool Mesh::toFile(const char* filename)
{
	size_t unused = 0;
	bind();
	getVertexBuffer()->bind();
	getIndexBuffer()->bind();

	Vertex* vertices = (Vertex*) getVertexBuffer()->map(BM_READONLY);
	unsigned int* indices = (unsigned int*) getIndexBuffer()->map(BM_READONLY);

	FILE* file = fopen(filename, "wb");
	// Put magical id
	const char magicNumber = 0x01;
	unused = fwrite(&magicNumber, sizeof ( char), 1, file);

	// Put to the file vertex count
	/// TODO: make more portable
	uint32_t vcount = (uint32_t) getVertexBuffer()->getSize();
	unused = fwrite(&vcount, sizeof ( uint32_t), 1, file);

	// Write vertices
	unused = fwrite(vertices, sizeof ( Vertex), getVertexBuffer()->getSize(), file);

	// Put index count
	uint32_t icount = (uint32_t) getIndexBuffer()->getSize();
	unused = fwrite(&icount, sizeof ( uint32_t), 1, file);

	// Write indices
	unused = fwrite(indices, sizeof ( unsigned int), getIndexBuffer()->getSize(), file);

	// Unmap buffers
	getVertexBuffer()->unmap();
	getIndexBuffer()->unmap();

	getIndexBuffer()->unbind();
	getVertexBuffer()->unbind();
	unbind();

	fclose(file);
	return true;
}

void Mesh::makeQuad()
{
	Vertex quadData[] = {
		{
			{-1.0f, 1.0f, 0.0f}, // Vertex
			{0.0f, 0.0f}, // Texcoord
			{0.0f, 0.0f, 1.0f}, // Normal(not used)
		},
		{
			{1.0f, 1.0f, 0.0f},
			{1.0f, 0.0f},
			{0.0f, 0.0f, 1.0f}
		},
		{
			{-1.0f, -1.0f, 0.0f},
			{0.0f, 1.0f},
			{0.0f, 0.0f, 1.0f}
		},
		{
			{1.0f, -1.0f, 0.0f},
			{1.0f, 1.0f},
			{0.0f, 0.0f, 1.0f}
		}
	};

	unsigned int quadIndices[] = {2, 1, 0, 2, 3, 1};

	bind();
	getVertexBuffer()->bind();
	getVertexBuffer()->copyData(quadData, 4, sizeof(Vertex), BU_STATIC);
	getVertexBuffer()->unbind();

	getIndexBuffer()->bind();
	getIndexBuffer()->copyData(quadIndices, 6, sizeof(unsigned int), BU_STATIC);
	getIndexBuffer()->unbind();

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof (Vertex),
	                      (char*) 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, sizeof (Vertex),
	                      (char*) 0 + 12);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof (Vertex),
	                      (char*) 0 + 20);
	glEnableVertexAttribArray(2);

	unbind();
}

void Mesh::draw()
{
	glBindVertexArray(vao);
	glDrawElements(type, getIndexBuffer()->getSize(),
	               GL_UNSIGNED_INT, 0);
}


void Mesh::createGrid(unsigned int sideSize, float quadSize, unsigned int patchSize)
{
	std::vector<float>				vertices;
	std::vector<unsigned int>		indices;
	unsigned int					count = sideSize + 1;

	vertices.reserve( count * count * 2 );
	indices.reserve( sideSize * sideSize * 4 );

	for (unsigned int h = 0; h < count; ++h)
	{
		for (unsigned int w = 0; w < count; ++w)
		{
			vertices.push_back( w * quadSize );
			vertices.push_back( h * quadSize );

			if ( w != 0 && h != 0 )
			{
				if ( patchSize == 4 )
				{
					indices.push_back( h*count		+ w );
					indices.push_back( h*count		+ w-1 );
					indices.push_back( (h-1)*count	+ w-1 );
					indices.push_back( (h-1)*count	+ w );
				}
				else
				if ( patchSize == 3 )
				{
					indices.push_back( h*count		+ w );
					indices.push_back( h*count		+ w-1 );
					indices.push_back( (h-1)*count	+ w-1 );

					indices.push_back( (h-1)*count	+ w-1 );
					indices.push_back( (h-1)*count	+ w );
					indices.push_back( h*count		+ w );
				}
				else
				if ( patchSize == 16 )
				{
					for (unsigned int i = 0; i < 4; ++i)
						for (unsigned int j = 0; j < 4; ++j)
							indices.push_back( (h+i-3)*count + w+j-3 );
				}
			}
		}
	}

	bind();
	indexBuffer->bind();
	indexBuffer->copyData( &indices[0], indices.size(), sizeof (indices[0]), GL_STATIC_DRAW );
	indexBuffer->unbind();

	vertexBuffer->bind();
	vertexBuffer->copyData( &vertices[0], vertices.size(), sizeof (vertices[0]), GL_STATIC_DRAW );
	vertexBuffer->unbind();

	glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, 0 );
	glEnableVertexAttribArray(0);

	unbind();
}

}
