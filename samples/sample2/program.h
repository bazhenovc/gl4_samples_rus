#if defined( _WIN32 ) || defined( _WIN64 ) || defined( WIN32 ) || defined( WIN64 )
#	define PLATFORM_WINDOWS
#	include <Windows.h>
#	include <GL/wglew.h>
#else
#	include <gl/glxew.h>
#endif

#include <string>

enum ETextureType
{
	TEX_DIFFUSE	= 0,
	TEX_NORMAL,
	TEX_HEIGHT,
	TEX_DEPTH,
	TEX_TESS_LEVEL,
};


/*
	Helper for program loading, and uniform setting
*/
class Program
{
public:
	struct States
	{
		glm::mat4	mvp;
		float		gridScale;
		float		maxTessLevel;
		float		heightScale;
		float		detailLevel;

		States(): gridScale(10000.f), maxTessLevel(12.f), heightScale(-800.f), detailLevel(6000.f) {}
	};

private:
	States		_states;

public:
	States &		getStates()			{ return _states; }
	States const &	getStates()	const	{ return _states; }

	void setConstUniforms(Shader *shader) const;

	void setUniforms(Shader *shader);

	bool load(Shader *&shader, const char *fileName, const char *preprocessor = NULL) const;

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
#else
#	error TODO: set current directory...
#endif
	return false;
}


#ifdef PLATFORM_WINDOWS
#	define glSwapInterval	wglSwapIntervalEXT
#else
#	define glSwapInterval	glXSwapIntervalEXT
#endif


void Program::setConstUniforms(Shader *shader) const
{
	//shader->bind();
	shader->setTexture( shader->getLoc("unDiffuseMap"),		TEX_DIFFUSE );
	shader->setTexture( shader->getLoc("unHeightMap"),		TEX_HEIGHT );
	shader->setTexture( shader->getLoc("unNormalMap"),		TEX_NORMAL );
	shader->setTexture( shader->getLoc("unDepthMap"),		TEX_DEPTH );
	shader->setTexture( shader->getLoc("unTessLevelMap"),	TEX_TESS_LEVEL );
	//shader->unbind();
}

void Program::setUniforms(Shader *shader)
{
	if ( _states.maxTessLevel < 1.f )	_states.maxTessLevel = 1.f;

	shader->setUniformMatrix( "unMVPMatrix",						 _states.mvp );
	shader->setUniformFloat( shader->getLoc("unGridScale"),		 _states.gridScale );
	shader->setUniformFloat( shader->getLoc("unMaxTessLevel"),	 _states.maxTessLevel );
	shader->setUniformFloat( shader->getLoc("unHeightScale"),	-_states.heightScale );
	shader->setUniformFloat( shader->getLoc("unDetailLevel"),	 _states.detailLevel );
}

bool Program::load(Shader *&shader, const char *fileName, const char *preprocessor) const
{
	if ( !shader )
		shader = new Shader();

	if ( !shader->loadShaders( fileName, preprocessor ) )
		return false;

	//setConstUniforms( shader );
	return true;
}

void Program::bind(Shader *shader)
{
	glPatchParameteri( GL_PATCH_VERTICES, shader->getPatchSize() );
	shader->bind();
	setConstUniforms( shader );
	setUniforms( shader );
}

