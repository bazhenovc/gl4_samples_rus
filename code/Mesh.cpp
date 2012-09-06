
#include "Mesh.h"
#include <GL/glew.h>
#include <stdio.h>
#include "Vertex.h"
#include <malloc.h>
#include <stdint.h>
#include <vector>

#include <md5/md5load.h>
#include <md5/md5load.cpp>
#include <md5/md5anim.cpp>

#include <assimp/Importer.hpp>
#include <assimp/PostProcess.h>
#include <assimp/Scene.h>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>

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

	indexBuffer->copyData(indices, indexNumber, sizeof ( unsigned int), BU_STREAM);
	vertexBuffer->copyData(vertices, vertexNumber, sizeof ( Vertex), BU_STREAM);

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

	getVertexBuffer()->copyData(quadData, 4, sizeof(Vertex), BU_STATIC);
	getIndexBuffer()->copyData(quadIndices, 6, sizeof(unsigned int), BU_STATIC);

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
	indexBuffer->bind();
	glDrawElements( type, getIndexBuffer()->getSize(),
	               GL_UNSIGNED_INT, 0);
	indexBuffer->unbind();
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
				if ( patchSize == 16 && w % 3 == 0 && h % 3 == 0  )
				{
					for (unsigned int i = 0; i < 4; ++i)
						for (unsigned int j = 0; j < 4; ++j)
							indices.push_back( (h+i-3)*count + w+j-3 );
				}
			}
		}
	}

	bind();

	indexBuffer->copyData( &indices[0], indices.size(), sizeof (indices[0]), GL_STATIC_DRAW );
	vertexBuffer->copyData( &vertices[0], vertices.size(), sizeof (vertices[0]), GL_STATIC_DRAW );

	glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, 0 );
	glEnableVertexAttribArray(0);

	type = GL_PATCHES;

	unbind();
}



static void AssimpInit()
{
	static bool	isAssimpInit = false;

	if ( isAssimpInit )
		return;

	isAssimpInit = true;

	// Create Logger
	Assimp::Logger::LogSeverity severity = Assimp::Logger::VERBOSE;
	Assimp::DefaultLogger::create("",severity, aiDefaultLogStream_STDOUT);
	Assimp::DefaultLogger::create("assimp_log.txt",severity, aiDefaultLogStream_FILE);
	Assimp::DefaultLogger::get()->info("this is my info-call");
}

static Assimp::Importer		assimpImporter;



MultiMesh::MultiMesh()
{
	indexBuffer		= new HardwareBuffer(BT_INDEX);
	vertexBuffer	= new HardwareBuffer(BT_VERTEX);
	
	AssimpInit();
}


MultiMesh::~MultiMesh()
{
	for (size_t i = 0; i < submeshes.size(); ++i) {
		glDeleteVertexArrays( 1, &submeshes[i].vao );
		delete submeshes[i].mtr;
	}
	
	delete vertexBuffer;
	delete indexBuffer;
}


void MultiMesh::_addSubMesh(void *pscene, unsigned int meshID, std::vector<float> &vertices, std::vector<unsigned int> &indices, const char *texturesPath, const glm::mat4 &m)
{
	const aiScene *	scene = (const aiScene *)pscene;
	const aiMesh *	mesh  = scene->mMeshes[meshID];
	SubMesh			sm;

	sm.count		 = 0;
	sm.mtr			 = NULL;
	sm.name			 = mesh->mName.C_Str();
	sm.offset		 = indices.size();
	sm.indexType	 = GL_UNSIGNED_INT;
	sm.primitiveType = mesh->mPrimitiveTypes;

	_createMaterial( scene->mMaterials[mesh->mMaterialIndex], sm.mtr, texturesPath );

	const bool	has_positions	= mesh->HasPositions();
	const bool	has_texcoords	= mesh->HasTextureCoords(0);
	const bool	has_normals		= mesh->HasNormals();
	const bool	has_tan_binorm	= mesh->HasTangentsAndBitangents();
	const int	stride			= sizeof(float) * (has_positions * 3 + has_texcoords * 2 +
								  has_normals * 3 + has_tan_binorm * 6);
	size_t		vert_offset		= vertices.size();

	vertices.reserve( vertices.size() + mesh->mNumVertices * stride );
	indices.reserve(  indices.size()  + mesh->mNumFaces * 3 );


	for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
	{
		const aiFace *	face = &mesh->mFaces[i];

		for (unsigned int j = 0; j < face->mNumIndices; ++j)
		{
			unsigned int	idx = face->mIndices[j];
			
			indices.push_back( idx );
			sm.count++;
		}
	}
			
			
	for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
	{
		if ( has_positions ) {
			glm::vec4	v = m * glm::vec4( mesh->mVertices[i].x,  mesh->mVertices[i].y, mesh->mVertices[i].z, 1.f );
			vertices.push_back( v.x );
			vertices.push_back( v.y );
			vertices.push_back( v.z );
		}

		if ( has_texcoords ) {
			vertices.push_back( mesh->mTextureCoords[0][i].x );
			vertices.push_back( 1.f - mesh->mTextureCoords[0][i].y );
		}

		if ( has_normals ) {
			glm::vec3	n = glm::inverse( glm::mat3( m ) ) *
							glm::vec3( mesh->mNormals[i].x,  mesh->mNormals[i].y, mesh->mNormals[i].z );
			vertices.push_back( n.x );
			vertices.push_back( n.y );
			vertices.push_back( n.z );
		}

		if ( has_tan_binorm ) {
			vertices.push_back( mesh->mTangents[i].x );
			vertices.push_back( mesh->mTangents[i].y );
			vertices.push_back( mesh->mTangents[i].z );
			vertices.push_back( mesh->mBitangents[i].x );
			vertices.push_back( mesh->mBitangents[i].y );
			vertices.push_back( mesh->mBitangents[i].z );
		}
	}

	glGenVertexArrays( 1, &sm.vao );
	glBindVertexArray( sm.vao );
	vertexBuffer->bind();

	// 0 - position
	if ( has_positions ) {
		glEnableVertexAttribArray( 0 );
		glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, stride, (const void *)(sizeof(float)*vert_offset) );
		vert_offset += 3;
	}

	// 1 - texcoord
	if ( has_texcoords ) {
		glEnableVertexAttribArray( 1 );
		glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, stride, (const void *)(sizeof(float)*vert_offset) );
		vert_offset += 2;
	}

	// 2 - normal
	if ( has_normals ) {
		glEnableVertexAttribArray( 2 );
		glVertexAttribPointer( 2, 3, GL_FLOAT, GL_FALSE, stride, (const void *)(sizeof(float)*vert_offset) );
		vert_offset += 3;
	}
		
	// 3,4 - tangent, binormal
	if ( has_tan_binorm ) {
		glEnableVertexAttribArray( 3 );
		glVertexAttribPointer( 3, 3, GL_FLOAT, GL_FALSE, stride, (const void *)(sizeof(float)*vert_offset) );
		vert_offset += 3;
		glEnableVertexAttribArray( 4 );
		glVertexAttribPointer( 4, 3, GL_FLOAT, GL_FALSE, stride, (const void *)(sizeof(float)*vert_offset) );
		vert_offset += 3;
	}

	glBindVertexArray( 0 );

	submeshes.push_back( sm );
}


void MultiMesh::_createMaterial(void *pmaterial, Material *&mtr, const char *texturesPath)
{
	const aiMaterial *	material = (const aiMaterial *)pmaterial;

	if ( mtr == NULL )
		mtr = new Material();
	
	mtr->createUB( "Material", sizeof(MaterialParams), 0 );

	MaterialParams &	params = mtr->getData<MaterialParams>();

	aiGetMaterialColor( material, AI_MATKEY_COLOR_DIFFUSE,	(aiColor4D *)&params.diffuse );
	aiGetMaterialColor( material, AI_MATKEY_COLOR_SPECULAR,	(aiColor4D *)&params.specular );
	aiGetMaterialColor( material, AI_MATKEY_COLOR_AMBIENT,	(aiColor4D *)&params.ambient );
	aiGetMaterialColor( material, AI_MATKEY_COLOR_EMISSIVE,	(aiColor4D *)&params.emission );
	
	unsigned int	max = 1;
	aiGetMaterialFloatArray( material, AI_MATKEY_SHININESS, &params.shininess, &max );
	max = 1;
	aiGetMaterialFloatArray( material, AI_MATKEY_SHININESS_STRENGTH, &params.strength, &max );
	
	std::string		texPath;

	if ( texturesPath )
	{
		texPath = texturesPath;

		if ( texPath.back() != '\\' && texPath.back() != '/' ) {
			texPath += '/';
		}
	}
	_loadTexture( (void*)material, mtr, 0, texPath );	// diffuse
	_loadTexture( (void*)material, mtr, 1, texPath );	// normal
	_loadTexture( (void*)material, mtr, 2, texPath );	// height
}

static GLenum SwitchWrap(aiTextureMapMode mode)
{
	switch ( mode )
	{
		case aiTextureMapMode_Wrap :	return GL_REPEAT;
		case aiTextureMapMode_Clamp :	return GL_CLAMP_TO_EDGE;
		case aiTextureMapMode_Mirror :	return GL_MIRRORED_REPEAT;
		default :						return GL_REPEAT;
	}
}


bool MultiMesh::_loadTexture(void *pmaterial, Material *mtr, int index, const std::string &texturesPath)
{
	struct TexType {
		const char *	uniform;
		aiTextureType	type;
	};
	static const TexType	textureTypes[] = {	{ "unDiffuseMap", aiTextureType_DIFFUSE },
												{ "unNormalMap",  aiTextureType_NORMALS },
												{ "unHeightMap",  aiTextureType_HEIGHT  } };

	Sampler				sampler;
	aiString			texName;
	aiTextureMapMode	mapMode[3]	= { aiTextureMapMode(-1), aiTextureMapMode(-1), aiTextureMapMode(-1) };
	const aiMaterial *	material	= (const aiMaterial *)pmaterial;

	if ( material->GetTexture( textureTypes[index].type, 0, &texName, NULL, NULL, NULL, NULL, mapMode ) == AI_SUCCESS )
	{
		sampler.wrapS		= SwitchWrap( mapMode[0] );
		sampler.wrapT		= SwitchWrap( mapMode[1] );
		sampler.wrapR		= SwitchWrap( mapMode[2] );
		sampler.anisotrophy	= 16;
		sampler.filterMag	= GL_LINEAR;
		sampler.filterMin	= GL_LINEAR_MIPMAP_LINEAR;

		return mtr->addTexture( (texturesPath + texName.C_Str()).c_str(), textureTypes[index].uniform, index, GL_TEXTURE_2D, sampler );
	}
	return false;
}


void MultiMesh::_recursiveLoad(void *pscene, void *pnode, std::vector<float> &vertices, std::vector<unsigned int> &indices, const char *texturesPath, const glm::mat4 &m)
{
	const aiScene *	scne = (const aiScene *)pscene;
	const aiNode *	node = (const aiNode *)pnode;
	const glm::mat4	tr	 = m * glm::transpose(*(glm::mat4 *)&node->mTransformation);

	for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
		_addSubMesh( pscene, node->mMeshes[i], vertices, indices, texturesPath, tr );
	}

	for (unsigned int i = 0; i < node->mNumChildren; ++i) {
		_recursiveLoad( pscene, (void *)node->mChildren[i], vertices, indices, texturesPath, tr );
	}
}


bool MultiMesh::load(const char *filename, const char *texturesPath)
{
	const aiScene* scene = assimpImporter.ReadFile( filename, aiProcessPreset_TargetRealtime_Quality);

	if ( !scene ) {
		printf( "Failed to load mesh: %s\n", filename );
		return false;
	}

	std::vector<float>			vertices;
	std::vector<unsigned int>	indices;

	/*for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
		_addSubMesh( (void *)scene, i, vertices, indices, texturesPath );
	}*/
	_recursiveLoad( (void*)scene, (void*)scene->mRootNode, vertices, indices, texturesPath, glm::mat4() );

	indexBuffer->bind();
	indexBuffer->copyData( &indices[0], indices.size(), sizeof(indices[0]), GL_STATIC_DRAW );
	indexBuffer->unbind();

	vertexBuffer->bind();
	vertexBuffer->copyData( &vertices[0], vertices.size(), sizeof(vertices[0]), GL_STATIC_DRAW );
	vertexBuffer->unbind();

	return true;
}
	

void MultiMesh::setMaterial(size_t submeshIndex, Material *mtr)
{
	if ( submeshIndex < submeshes.size() )
	{
		submeshes[ submeshIndex ].mtr = mtr;
		return;
	}
	assert( false && "index is out of range" );
}


void MultiMesh::setMaterial(const char *submeshName, Material *mtr)
{
	std::string	str( submeshName );

	for (size_t i = 0; i < submeshes.size(); ++i)
	{
		if ( str == submeshes[i].name ) {
			submeshes[i].mtr = mtr;
			return;
		}
	}
	assert( false && "can't find submesh with name" );
}
	

void MultiMesh::draw(bool asPatches)
{
	for (size_t i = 0; i < submeshes.size(); ++i)
	{
		glBindVertexArray( submeshes[i].vao );
		indexBuffer->bind();

		glDrawElements( asPatches ? GL_PATCHES : submeshes[i].primitiveType, submeshes[i].count,
						submeshes[i].indexType, (const void **)(submeshes[i].offset * sizeof(int)) );
		indexBuffer->unbind();
	}
	glBindVertexArray( 0 );
}


void MultiMesh::draw(Shader *shader, bool asPatches)
{
	for (size_t i = 0; i < submeshes.size(); ++i)
	{
		if ( submeshes[i].mtr )
			submeshes[i].mtr->apply( shader );
		
		glBindVertexArray( submeshes[i].vao );
		indexBuffer->bind();

		glDrawElements( asPatches ? GL_PATCHES : submeshes[i].primitiveType, submeshes[i].count,
						submeshes[i].indexType, (const void **)(submeshes[i].offset * sizeof(int)) );
		indexBuffer->unbind();
	}
	glBindVertexArray( 0 );
}





static std::vector< SceletalMesh::Joint >	g_jointBuffer( 1000 );		// static buffer from cache before updating mesh
static std::vector< glm::vec3 >				g_vertexBuffer( 100000 );	// static buffer from cache before loading to VBO



static void getFileName(const char *filename, std::string &name)
{
	size_t	len			= strlen( filename ),
			dot_pos		= len,
			slash_pos	= 0;
	
	for (size_t i = len-1; i < len; --i)
	{
		if ( dot_pos == len && filename[i] == '.' )
			dot_pos = i;

		if ( slash_pos == 0 && (filename[i] == '\\' || filename[i] == '/') ) {
			slash_pos = i+1;
			break;
		}
	}

	name = std::string( filename + slash_pos, filename + dot_pos );
	return;
}


static bool convertBase(const md5_model_t &model, SceletalMesh::TrackInfo &track,
						size_t &frames, std::vector<SceletalMesh::Joint> &joints)
{
	track.name			= "base";
	track.firstFrame	= frames++;
	track.numFrames		= 1;
	track.frameRate		= 1.f;

	// add joints
	for (int i = 0; i < model.num_joints; ++i)
	{
		SceletalMesh::Joint		scjoint;
		md5_joint_t const &		joint	= model.baseSkel[i];

		scjoint.orient	= glm::quat( joint.orient[3], joint.orient[0], joint.orient[1], joint.orient[2] );
		scjoint.pos		= glm::vec3( joint.pos[0], joint.pos[1], joint.pos[2] );
		scjoint.parent	= joint.parent;

		joints.push_back( scjoint );
	}

	return true;
}


static bool convertAnim(const md5_anim_t &anim, const char *filename, SceletalMesh::TrackInfo &track,
						size_t &frames, std::vector<SceletalMesh::Joint> &joints)
{
	getFileName( filename, track.name );

	track.firstFrame	= frames;
	track.numFrames		= 0;
	track.frameRate		= 1.f;

	joints.reserve( joints.size() + anim.num_frames * anim.num_joints );

	// add track frames
	for (int i = 0; i < anim.num_frames; ++i)
	{
		for (int j = 0; j < anim.num_joints; ++j)
		{
			md5_joint_t const &		joint = anim.skelFrames[i][j];
			SceletalMesh::Joint		scjoint;
			
			scjoint.orient	= glm::quat( joint.orient[3], joint.orient[0], joint.orient[1], joint.orient[2] );
			scjoint.pos		= glm::vec3( joint.pos[0], joint.pos[1], joint.pos[2] );
			scjoint.parent	= joint.parent;

			joints.push_back( scjoint );
		}
	}

	frames			+= anim.num_frames;
	track.numFrames	 = anim.num_frames;
	track.frameRate	 = float(anim.frameRate);
	return true;
}


static void buildFrameJoints(const SceletalMesh::Joint *frame0, const SceletalMesh::Joint *frame1,
							 int numJoints, float time, SceletalMesh::Joint *&result)
{
	result = &g_jointBuffer[0];

	for (int i = 0; i < numJoints; ++i)
	{
		result[i].parent	= frame0[i].parent;

		// linear interpolation
		result[i].pos = frame0[i].pos + time * (frame1[i].pos - frame0[i].pos);
		Quat_slerp( &frame0[i].orient[0], &frame1[i].orient[0], time, &result[i].orient[0] );
	}
}


static void updateMesh( const SceletalMesh::Weight *weights,
						const SceletalMesh::Joint *joints,
						const SceletalMesh::ScVert *vertices, size_t numVerts,
						const unsigned int *indices, size_t numIndices,
						size_t vertOffset, void *&data)
{
	glm::vec3 *	position = &g_vertexBuffer[0];
	data = position;

	for (size_t i = 0; i < numVerts; ++i)
	{
		position[i*2+0] = glm::vec3( 0.f );
		position[i*2+1] = glm::vec3( 0.f );
		
		for (int j = 0; j < vertices[i].count; ++j)
		{
			SceletalMesh::Weight const &	weight = weights[ vertices[i].start + j ];
			SceletalMesh::Joint  const &	joint  = joints[ weight.jointIndex ];

			position[i*2+0] += ( joint.pos + joint.orient * weight.pos ) * weight.bias;
		}
		/*
		for (int j = 0; j < vertices[i].count; ++j)
		{
			SceletalMesh::Weight const &	weight = weights[ vertices[i].start + j ];
			SceletalMesh::Joint  const &	joint  = joints[ weight.jointIndex ];

			glm::vec3	wv;
			Quat_rotatePoint( &joint.orient[0], &weight.pos[0], &wv[0] );

			pos += ( joint.pos + wv ) * weight.bias;
		}*/
	}

	// build normals
	const size_t	num_tris = numIndices / 3;
	for (size_t i = 0; i < num_tris; ++i)
	{
		glm::vec3 const &	v0 = position[ (indices[i*3+0] - vertOffset) * 2 ];
		glm::vec3 const &	v1 = position[ (indices[i*3+1] - vertOffset) * 2 ];
		glm::vec3 const &	v2 = position[ (indices[i*3+2] - vertOffset) * 2 ];
		glm::vec3 &			n0 = position[ (indices[i*3+0] - vertOffset) * 2 + 1 ];
		glm::vec3 &			n1 = position[ (indices[i*3+1] - vertOffset) * 2 + 1 ];
		glm::vec3 &			n2 = position[ (indices[i*3+2] - vertOffset) * 2 + 1 ];

		glm::vec3	norm = glm::cross( v1 - v0, v2 - v0 );

		n0 += norm;
		n1 += norm;
		n2 += norm;
	}

	// normalize
	for (size_t i = 0; i < numVerts; ++i) {
		const glm::vec3 v = position[ i*2 + 1 ];
		position[ i*2 + 1 ] = glm::normalize( v );
	}
}




SceletalMesh::SceletalMesh():
	jointsPerFrame(0), indexBuffer(0), positionBuffer(0),
	texcoordBuffer(0), numFrames(0), vao(0), onAnimationEnd(0)
{
	indexBuffer		= new HardwareBuffer(BT_INDEX);
	positionBuffer	= new HardwareBuffer(BT_VERTEX);
	texcoordBuffer	= new HardwareBuffer(BT_VERTEX);

	glGenVertexArrays( 1, &vao );
}


SceletalMesh::~SceletalMesh()
{
	for (size_t i = 0; i < submeshes.size(); ++i) {
		delete submeshes[i].mtr;
	}

	glDeleteVertexArrays( 1, &vao );
	
	delete positionBuffer;
	delete texcoordBuffer;
	delete indexBuffer;
}


bool SceletalMesh::loadMD5Mesh(const char *filename)
{
	md5load		md5Load;
	md5_model_t	model;

	if ( !md5Load.ReadMD5Model( filename, &model ) ) {
		printf( "Failed to load mesh: %s\n", filename );
		return false;
	}

	size_t	idxSize = 0,
			vertSize = 0,
			weightSize = 0;
	
	for (int i = 0; i < model.num_meshes; ++i)
	{
		md5_mesh_t const &	mesh = model.meshes[i];
		SubMesh				sm;

		getFileName( mesh.shader, sm.name );
		sm.count		 = mesh.num_tris * 3;
		sm.numVerts		 = mesh.num_verts;
		sm.indexType	 = GL_UNSIGNED_INT;
		sm.offset		 = idxSize;
		sm.primitiveType = GL_TRIANGLES;
		idxSize			+= sm.count;
		vertSize		+= mesh.num_verts;
		weightSize		+= mesh.num_weights;

		submeshes.push_back( sm );
	}
	

	std::vector< glm::vec2 >	texcoords;
	
	weights.reserve( weightSize );
	vertices.reserve( vertSize );
	indices.reserve( idxSize );
	texcoords.reserve( vertSize );
	
	weightSize = 0;
	vertSize = 0;

	for (int i = 0; i < model.num_meshes; ++i)
	{
		md5_mesh_t const &	mesh = model.meshes[i];

		for (int j = 0; j < mesh.num_tris; ++j)
		{
			indices.push_back( mesh.triangles[j].index[0] + vertSize );
			indices.push_back( mesh.triangles[j].index[1] + vertSize );
			indices.push_back( mesh.triangles[j].index[2] + vertSize );
		}
		vertSize += mesh.num_verts;

		for (int j = 0; j < mesh.num_verts; ++j)
		{
			ScVert					scvert;
			md5_vertex_t const &	vert = mesh.vertices[j];

			scvert.count	= vert.count;
			scvert.start	= vert.start + weightSize;

			texcoords.push_back( glm::vec2( vert.st[0], vert.st[1] ) );
			vertices.push_back( scvert );
		}
		weightSize += mesh.num_weights;

		for (int j = 0; j < mesh.num_weights; ++j)
		{
			Weight					scweight;
			md5_weight_t const &	weight = mesh.weights[j];
			
			scweight.bias		= weight.bias;
			scweight.jointIndex	= weight.joint;
			scweight.pos		= glm::vec3( weight.pos[0], weight.pos[1], weight.pos[2] );

			weights.push_back( scweight );
		}
	}


	glBindVertexArray( vao );
	
	// position & normal
	positionBuffer->bind();
	positionBuffer->copyData( NULL, vertSize,  sizeof(float)*6,  GL_DYNAMIC_DRAW );
	
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*6, 0 );
	glEnableVertexAttribArray(0);

	glVertexAttribPointer( 2, 3, GL_FLOAT, GL_FALSE, sizeof(float)*6, (const void**)(sizeof(float)*3) );
	glEnableVertexAttribArray(2);
		
	// texcoord
	texcoordBuffer->bind();
	texcoordBuffer->copyData( &texcoords[0], texcoords.size(), sizeof(texcoords[0]),  GL_STATIC_DRAW );
	
	glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 0, 0 );
	glEnableVertexAttribArray(1);

	// indices
	indexBuffer->bind();
	indexBuffer->copyData( &indices[0], indices.size(),  sizeof(indices[0]), GL_STATIC_DRAW );
	
	glBindVertexArray(0);


	animTracks.push_back( TrackInfo() );
	convertBase( model, animTracks.back(), numFrames, joints );
	jointsPerFrame = model.num_joints;

	md5Load.FreeModel( &model );
	return true;
}


bool SceletalMesh::loadMD5Anim(const char *filename)
{
	md5load		md5Load;
	md5_anim_t	anim;

	if ( !md5Load.ReadMD5Anim( filename, &anim ) ) {
		printf( "Failed to load animation: %s\n", filename );
		return false;
	}
	
	assert( anim.num_joints == jointsPerFrame );
	
	animTracks.push_back( TrackInfo() );
	convertAnim( anim, filename, animTracks.back(), numFrames, joints );

	md5Load.FreeAnim( &anim );
	return true;
}


bool SceletalMesh::activeTrack(size_t track, size_t frame)
{
	if ( track < animTracks.size() )
	{
		animation.currTrack = track;
		animation.trackTime = 0.f;

		TrackInfo const &	newTrack = animTracks[ track ];

		if ( frame >= 0 )
			animation.currFrame = frame + newTrack.firstFrame;

		animation.currFrame = glm::clamp( animation.currFrame, newTrack.firstFrame,
										  newTrack.firstFrame + newTrack.numFrames );
		return true;
	}
	return false;
}


bool SceletalMesh::activeTrack(const char *name, size_t frame)
{
	for (size_t i = 0; i < animTracks.size(); ++i) {
		if ( animTracks[i].name == name ) {
			return activeTrack( i, frame );
		}
	}
	return false;
}


void SceletalMesh::setCallback(onTrackEndedFunc_t callback)
{
	onAnimationEnd = callback;
}


void SceletalMesh::setMaterial(size_t submeshIndex, Material *mtr)
{
	if ( submeshIndex < submeshes.size() )
	{
		submeshes[ submeshIndex ].mtr = mtr;
		return;
	}
	assert( false && "index is out of range" );
}


void SceletalMesh::setMaterial(const char *submeshName, Material *mtr)
{
	std::string	str( submeshName );

	for (size_t i = 0; i < submeshes.size(); ++i)
	{
		if ( str == submeshes[i].name ) {
			submeshes[i].mtr = mtr;
			return;
		}
	}
	assert( false && "can't find submesh with name" );
}


void SceletalMesh::update(float timeDelta)
{
	// calculate new frame
	animation.trackTime	   += timeDelta;

	TrackInfo const &	track	= animTracks[ animation.currTrack ];
	float				f0		= track.frameRate * animation.trackTime;
	float				f1		= glm::ceil( track.frameRate * animation.trackTime );
	float				interp	= f1 - f0;
	int					lastFr	= animation.currFrame;

	animation.currFrame		= size_t( f1 ) % track.numFrames + track.firstFrame;

	if ( lastFr > animation.currFrame )
	{
		// on track end
		animation.trackTime -= track.frameRate * track.numFrames;

		if ( onAnimationEnd )
		{
			onAnimationEnd( this );
		}
	}
	if ( animation.currFrame >= numFrames ) {
		animation.currFrame = animTracks[ animation.currTrack ].firstFrame;
	}


	// build scelet
	int	const		frame0		= lastFr * jointsPerFrame;
	int	const		frame1		= animation.currFrame * jointsPerFrame;
	const Joint *	p_joints	= &joints[0];
	Joint	 *		result		= NULL;

	assert( frame0 < joints.size() && frame1 < joints.size() );
	buildFrameJoints( p_joints + frame0, p_joints + frame1, jointsPerFrame, interp, result );

	
	// update buffer
	positionBuffer->bind();

	size_t	vertOffset = 0;

	for (size_t i = 0; i < submeshes.size(); ++i)
	{
		void *	position = NULL;
		updateMesh( &weights[0], result, &vertices[vertOffset], submeshes[i].numVerts,
					&indices[submeshes[i].offset], submeshes[i].count, vertOffset, position );

		positionBuffer->copySubData( position, submeshes[i].numVerts, sizeof(float)*6, vertOffset );
		vertOffset += submeshes[i].numVerts;
	}

	positionBuffer->unbind();
}


void SceletalMesh::draw(bool asPatches)
{
	glBindVertexArray( vao );
	indexBuffer->bind();

	for (size_t i = 0; i < submeshes.size(); ++i)
	{
		glDrawElements( asPatches ? GL_PATCHES : submeshes[i].primitiveType, submeshes[i].count,
						submeshes[i].indexType, (const void **)(submeshes[i].offset * sizeof(int)) );
	}

	indexBuffer->unbind();
	glBindVertexArray( 0 );
}


void SceletalMesh::draw(Shader *shader, bool asPatches)
{
	glBindVertexArray( vao );
	indexBuffer->bind();

	for (size_t i = 0; i < submeshes.size(); ++i)
	{
		if ( submeshes[i].mtr )
			submeshes[i].mtr->apply( shader );

		glDrawElements( asPatches ? GL_PATCHES : submeshes[i].primitiveType, submeshes[i].count,
						submeshes[i].indexType, (const void **)(submeshes[i].offset * sizeof(int)) );
	}

	indexBuffer->unbind();
	glBindVertexArray( 0 );
}

}
