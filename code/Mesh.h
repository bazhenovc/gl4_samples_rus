
#ifndef _MESH_H_
#define _MESH_H_

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <vector>
#include "HardwareBuffer.h"
#include "Texture.h"
#include "Material.h"

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



class MultiMesh
{
public:
	struct MaterialParams
	{
		glm::vec4	diffuse,
					specular,
					ambient,
					emission;
		float		shininess,
					strength;
		int			_align[2];
	};

private:
	struct SubMesh
	{
		GLuint		vao;
		Material *	mtr;
		std::string	name;
		size_t		count;
		size_t		offset;		// in index buffer
		GLenum		indexType;
		GLenum		primitiveType;

		SubMesh(const char *n = ""):
			name(n), mtr(0), count(0), offset(0), indexType(GL_UNSIGNED_SHORT),
			primitiveType(GL_TRIANGLES), vao(0) {}
	};

	
	std::vector< SubMesh >		submeshes;
	HardwareBuffer	*			indexBuffer,
					*			vertexBuffer;
	
	void _recursiveLoad(void *scene, void *node, std::vector<float> &vertices, std::vector<unsigned int> &indices, const char *texturesPath, const glm::mat4 &m);
	void _addSubMesh(void *scene, unsigned int meshID, std::vector<float> &vertices, std::vector<unsigned int> &indices, const char *texturesPath, const glm::mat4 &m);
	void _createMaterial(void *material, Material *&mtr, const char *texturesPath);
	bool _loadTexture(void *material, Material *mtr, int index, const std::string &texturesPath);

public:
	MultiMesh();
	~MultiMesh();

	bool load(const char *filename, const char *texturesPath = 0);
	
	void setMaterial(size_t submeshIndex, Material *mtr);
	void setMaterial(const char *submeshName, Material *mtr);
	
	void draw(bool asPatches = false);
	void draw(Shader *shader, bool asPatches = false);
	
	size_t		getNumSubmeshes()		const	{ return submeshes.size(); }
	Material *	getMaterial(size_t i)	const	{ return submeshes[i].mtr; }
};



class SceletalMesh
{
public:
	struct Joint
	{
		glm::quat		orient;
		glm::vec3		pos;
		int				parent;
	};

	struct TrackInfo
	{
		std::string		name;
		size_t			firstFrame,
						numFrames;
		float			frameRate;
	};

	struct Weight
	{
		glm::vec3		pos;
		float			bias;
		int				jointIndex;
	};

	struct ScVert
	{
		unsigned int	start,
						count;
	};

	typedef void (*onTrackEndedFunc_t)(SceletalMesh * mesh);
	
private:
	struct SubMesh
	{
		Material *	mtr;
		std::string	name;
		size_t		count,
					numVerts;
		size_t		offset;		// in index buffer
		GLenum		indexType;
		GLenum		primitiveType;

		SubMesh(const char *n = ""):
			name(n), mtr(0), count(0), offset(0), indexType(GL_UNSIGNED_SHORT),
			primitiveType(GL_TRIANGLES), numVerts(0) {}
	};

	struct Animation
	{
		size_t		currFrame,
					currTrack;
		float		trackTime;

		Animation(): currFrame(0), currTrack(0), trackTime(0.f) {}
	};


	std::vector< SubMesh >		submeshes;
	std::vector< Joint >		joints;
	std::vector< TrackInfo >	animTracks;
	std::vector< Weight >		weights;
	std::vector< ScVert >		vertices;
	std::vector<unsigned int>	indices;
	HardwareBuffer	*			indexBuffer,
					*			positionBuffer,
					*			texcoordBuffer;
	onTrackEndedFunc_t			onAnimationEnd;
	Animation					animation;
	GLuint						vao;
	size_t						numFrames;
	unsigned int				jointsPerFrame;

public:
	SceletalMesh();
	~SceletalMesh();

	bool loadMD5Mesh(const char *filename);
	bool loadMD5Anim(const char *filename);

	bool activeTrack(size_t track, size_t frame = -1);
	bool activeTrack(const char *name, size_t frame = -1);

	void setCallback(onTrackEndedFunc_t callback);

	void setMaterial(size_t submeshIndex, Material *mtr);
	void setMaterial(const char *submeshName, Material *mtr);

	void update(float timeDelta);

	void draw(bool asPatches = false);
	void draw(Shader *shader, bool asPatches = false);

	std::vector< TrackInfo > const &	getTrackInfo()	const	{ return animTracks; }

	size_t	getNumSubmeshes()		const	{ return submeshes.size(); }
	size_t	getCurrentTrack()		const	{ return animation.currTrack; }
	size_t	getCurrentFrame()		const	{ return animation.currFrame; }
	size_t	getCurrentTrackFrame()	const	{ return animation.currFrame - animTracks[ animation.currTrack ].firstFrame; }
};

}

#endif
