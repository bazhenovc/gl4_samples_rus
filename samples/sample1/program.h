#if defined( _WIN32 ) || defined( _WIN64 ) || defined( WIN32 ) || defined( WIN64 )
#	define PLATFORM_WINDOWS
#	include <Windows.h>
#endif

#include <string>

enum ETextureType
{
	TEX_DIFFUSE	= 0,
	TEX_NORMAL,
	TEX_HEIGHT,
	TEX_DEPTH,
};


/*
	Helper for program loading, and uniform setting
*/
class Program
{
public:
	struct States
	{
		Matrix4		mvp;
		float		gridScale;
		float		maxTessLevel;
		float		heightScale;
		float		detailLevel;

		States(): gridScale(100.f), maxTessLevel(12.f), heightScale(10.f), detailLevel(500.f) {}
	};

private:
	States		_states;

public:
	States &		getStates()			{ return _states; }
	States const &	getStates()	const	{ return _states; }

	void setConstUniforms(Shader *shader) const;

	void setUniforms(Shader *shader);

	bool load(Shader *&shader, const char *fileName) const;

	void bind(Shader *shader);
};




/*
	System function: set resource directory
*/
bool setResourceDirectory(const char *dirName, int maxSearchDepth = 4)
{
#ifdef PLATFORM_WINDOWS
	std::string	path = dirName;
	for (int i = 0; i < maxSearchDepth; ++i)
	{
		int i_code = ::GetFileAttributes( path.c_str() );
		if ( (i_code != -1) && (FILE_ATTRIBUTE_DIRECTORY & i_code) )
		{
			::SetCurrentDirectory( path.c_str() );
			return true;
		}
		path = "../" + path;
	}
#endif
	return false;
}


template <typename T, size_t I>
inline size_t count_of(const T (&)[I]) {
	return I;
}


void Program::setConstUniforms(Shader *shader) const
{
	shader->setTexture( shader->getLoc("unDiffuseMap"),	TEX_DIFFUSE );
	shader->setTexture( shader->getLoc("unHeightMap"),	TEX_HEIGHT );
	shader->setTexture( shader->getLoc("unNormalMap"),	TEX_NORMAL );
	shader->setTexture( shader->getLoc("unDepthMap"),	TEX_DEPTH );
}

void Program::setUniforms(Shader *shader)
{
	if ( _states.maxTessLevel < 1.f )	_states.maxTessLevel = 1.f;

	shader->setUniformMatrix( "unMVPMatrix",						_states.mvp );
	shader->setUniformFloat( shader->getLoc("unGridScale"),		_states.gridScale );
	shader->setUniformFloat( shader->getLoc("unMaxTessLevel"),	_states.maxTessLevel );
	shader->setUniformFloat( shader->getLoc("unHeightScale"),	_states.heightScale );
	shader->setUniformFloat( shader->getLoc("unDetailLevel"),	_states.detailLevel );
}

bool Program::load(Shader *&shader, const char *fileName) const
{
	if ( !shader )
		shader = new Shader();

	if ( !shader->loadShaders( fileName ) )
		return false;

	setConstUniforms( shader );
	return true;
}

void Program::bind(Shader *shader)
{
	glPatchParameteri( GL_PATCH_VERTICES, shader->getPatchSize() );
	shader->bind();
	setUniforms( shader );
}

