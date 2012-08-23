#ifndef _INC_RENDER_GL_IHARDWARE_BUFFER_GL_HPP
#define _INC_RENDER_GL_IHARDWARE_BUFFER_GL_HPP

#include <GL/glew.h>

namespace framework
{
/**
 Buffer type
 \brief
 This specifies data type in the buffer
 <br> BT_VERTEX for vertex buffers
 <br> BT_INDEX for index buffers
 */
enum BufferType {
    BT_VERTEX = GL_ARRAY_BUFFER,
    BT_INDEX = GL_ELEMENT_ARRAY_BUFFER,
	BT_TEXTURE = GL_TEXTURE_BUFFER,
	BT_UNIFORM = GL_UNIFORM_BUFFER,
};
/**
 This flag determines how do we use our buffer
 \brief
 Choose usage carefully!
 */
enum BufferUsage {
    /// Buffer data will be changed several times
    BU_STATIC = GL_STATIC_DRAW,

    /// Buffer data will be chagned more often (e.g. periodic geometry updates)
    BU_STREAM = GL_STREAM_DRAW,

    /**
     Buffer data is changing rapidly, often every frame.
     \brief
     This flag will tell GL driver to put data into fast AGP
     memory or somewhere else
     */
    BU_DYNAMIC = GL_DYNAMIC_DRAW
};
/**
 Buffer map type.
 \brief
 Buffers have the ability to be 'shadowed' in
 system memory, e.g. creating a copy of existing buffer data in main RAM.
 These are the flags to specify 'mapping' type
 */
enum BufferMap {
    /// Read-only data. No changes allowed
    BM_READONLY = GL_READ_ONLY,

    /**
     Mapped data is write-only. Reading will be impossible, cause
     buffer will always return a pointer to the blank memory instead of
     a memory, associated with buffer contents.
     \note It is possible to write to the new memory while the previous
     one is being used
     */
    BM_WRITEONLY = GL_WRITE_ONLY,

    /**
     Mapped data is read-write, however this may cause performance overhead
     \note
     Try to use another mappings
     */
    BM_READWRITE = GL_READ_WRITE
};
/**
 OpenGL hardware buffer implementation
 */
class HardwareBuffer
{
public:
	/**
	 Constructor
	 \par t Buffer type
	 */
	HardwareBuffer(GLuint t);

	/**
	 Destructor
	 */
	virtual ~HardwareBuffer(void);

	/**
	 Bind buffer
	 \brief
	 Tells the GL driver to use this buffer
	 */
	void bind(void);

	/**
	 Unbind buffer
	 */
	void unbind(void);

	/**
	 Copy a block of data from the system memory to the buffer
	 \brief
	 This method is like a buffer initialization: allocates new data
	 storage in the video memory
	 \par data Pointer to data
	 \par size Data array size
	 \par ptrsize Element pointer size
	 \par usage Usage flag
	 \sa copySubData()
	 */
	void copyData(const void* data, size_t size, size_t ptrsize, GLuint usage);

	/**
	 Copy a block of data from the system memory to the buffers
	 \brief
	 Just overrides existing data in the buffer. Ensure that buffer
	 is big enought to contain everything you want. Also ensure that
	 this buffer is already allocated
	 \par data Pointer to data
	 \par size Data size
	 \par ptrsize Data pointer size
	 \par offset Data offset
	 \sa discard()
	 */
	void copySubData(const void* data, size_t size, size_t ptrsize, size_t offset);

	/**
	 Discard buffer
	 \brief
	 Sets all buffer elements to zero, however keeps allocated storage
	 avaible
	 \note
	 It is possible to change buffer usage with this method
	 \par usage New usage
	 */
	void discard(GLuint usage) {
		_size = 0;
		_ptrsize = 0;
		copyData(0, 0, 0, usage);
	}

	/**
	 Get buffer size
	 \return _size
	 */
	inline size_t getSize(void) const {
		return _size;
	}

	/**
	 Get data pointer size
	 \return _ptrsize
	 */
	inline size_t getPtrSize(void) const {
		return _ptrsize;
	}

	inline GLuint getID() const {
		return _id;
	}

	/**
	 Map buffer
	 \brief
	 Creates a buffer data copy in the system memory, thus allowing
	 directly reading/writing to the buffer data on the GL client.
	 \warning
	 Client-side mapped data could be lost due to some system events
	 \par bm Mapping flag
	 \return A pointer to the shadowed memory
	 \sa unmap()
	 */
	void* map(BufferMap bm);

	/**
	 Unmap buffer
	 \brief
	 Releases shadow copy within the system memory and updates
	 buffer contents if needed
	 \return False on error
	 */
	bool unmap(void);

private:
	/// Buffer type
	GLuint _type;

	/// OpenGL buffer id
	GLuint _id;

	/// Buffer size
	size_t _size;

	/// Pointer size
	size_t _ptrsize;
};
}

#endif
