#ifndef __MS3D_H__
#define __MS3D_H__

#ifdef _MSC_VER
#	pragma pack( push, packing )
#	pragma pack( 1 )
#	define PACK_STRUCT
#elif defined( __GNUC__ )
#	define PACK_STRUCT	__attribute__((packed))
#else
#	error you must unsigned char-align these structures with the appropriate compiler directives
#endif

// File header
struct MS3DHeader
{
	char m_ID[10];
	int m_version;
} PACK_STRUCT;

// BoneVertex information
struct MS3DVertex
{
	unsigned char m_flags;
	float m_vertex[3];
	char BoneID;
	unsigned char m_refCount;
} PACK_STRUCT;

// Triangle information
struct MS3DTriangle
{
	unsigned short m_flags;
	unsigned short m_vertexIndices[3];
	float m_vertexNormals[3][3];
	float m_s[3], m_t[3];
	unsigned char m_smoothingGroup;
	unsigned char m_groupIndex;
} PACK_STRUCT;

// Material information
struct MS3DMaterial
{
    char m_name[32];
    float Ambient[4];
    float Diffuse[4];
    float Specular[4];
    float Emissive[4];
    float Shininess;	// 0.0f - 128.0f
    float m_transparency;	// 0.0f - 1.0f
    unsigned char m_mode;	// 0, 1, 2 is unused now
    char m_texture[128];
    char m_alphamap[128];
} PACK_STRUCT;

//	Joint information
struct MS3DJoint
{
	unsigned char m_flags;
	char m_name[32];
	char m_parentName[32];
	float m_rotation[3];
	float m_translation[3];
	unsigned short m_numRotationKeyframes;
	unsigned short m_numTranslationKeyframes;
} PACK_STRUCT;

// Keyframe data
struct MS3DKeyframe
{
	float m_time;
	float m_parameter[3];
} PACK_STRUCT;

// Default alignment
#ifdef _MSC_VER
#	pragma pack( pop, packing )
#endif

#undef PACK_STRUCT

#endif //