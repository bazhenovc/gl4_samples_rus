/**
 * Copyright 2010 Cyril Bazhenov
 * For legal information see "legal.txt"
 */
#ifndef _TD_VERTEX_H
#define _TD_VERTEX_H

namespace framework
{
#pragma pack(push, 1)
/** A generic datatype used to hold discrete model data in hardware
 * buffers */
typedef struct _Vertex {
	/** Vertex local position */
	float pos[3];
	/** Texture coord */
	float tex[2];
	/** Normal */
	float nrm[3];
	//unsigned char color[4];
} Vertex;
#pragma pack(pop)
}

#endif /* _VERTEX_H */

