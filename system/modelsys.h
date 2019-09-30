#ifndef __MODEL_H__
#define __MODEL_H__
//============================================================================
// Author: Octavian Marius Chincisan 2006
// Zalsoft Inc 1999-2006
//============================================================================

#include "baselib.h"
#include "texman.h"
#include "ms3d.h"


struct BoneVx
{
    int _id;
    V3  _vx;
};

struct Triangle
{
    V3          m_s;
    V3          m_t;  

    V3          m_vertexNormals[3];
	V3          m_vertexIndices;
};

struct Mesh
{
	int m_materialIndex;
	int m_numTriangles;
	int *m_pTriangleIndices;

	Mesh(): m_materialIndex(-1), m_numTriangles(-1), m_pTriangleIndices(NULL) {};
};


struct Material
{
	unsigned int	m_iBpp;					// Image color depth in bits per pixel
	unsigned int	m_iWidth;				// Image width
	unsigned int	m_iHeight;				// Image height

    Htex            h_tex;
	
	int				m_MaskTexID;		// Masking texture ID
	
	BYTE			*m_cData;				// Pixel data
	char			m_strFilename[128];		// File name
	bool			m_bIsTex;				// Is it a texture? (Delete m_cData after loading and binding)
	unsigned int	m_iError;
	CLR			    m_Ambient, 
					m_Diffuse, 
					m_Emissive, 
					m_Specular;

	CLR  			m_Alpha;
	float           m_fShininess;

	Material() 
	{
		m_iBpp = 0;
		m_iWidth = 0;
		m_iHeight = 0;
		m_cData = NULL;
		m_MaskTexID = -1;
		
        m_fShininess = 0;
		strcpy(m_strFilename, "");
		m_iError = 0;
	}

	~Material()
	{
        delete[] (m_cData);
	}
};

//	Animation keyframe information
struct Keyframe
{
	int m_jointIndex;
	float m_time;	// in milliseconds
	V3 m_parameter;
};


struct Joint
{
	V3 m_localRotation;
	V3 m_localTranslation;
	M4 m_absolute, m_relative;

	int m_numRotationKeyframes, m_numTranslationKeyframes;
	Keyframe *m_pTranslationKeyframes;
	Keyframe *m_pRotationKeyframes;

	int m_currentTranslationKeyframe, m_currentRotationKeyframe;
	M4  m_final;

	int m_parent;
};

class Scene;
class Camera;
struct SystemData;
class Model
{
public:
    Model(){d_timer=0;d_timercur=0;};
    ~Model(){};

    BOOL  LoadFile(const TCHAR *sFileName, DWORD flags);
    void  SetJointKeyframe( int jointIndex, 
                             int keyframeIndex, 
                             float time, float* parameter, bool isRotation );

    void Restart();
    void Animate(Scene* pscene, const Camera* pov, const SystemData* psy, int camLeaf, int thisIdx);
    void SetupJoints();
    void Render(const SystemData* psy);

    tstring     _fileName;
    int         m_iNumVertices;
    BoneVx*     m_pVertices;

    int         m_iNumTriangles;
    Triangle*   m_pTriangles;

    int         m_iNumMeshes;
    Mesh*       m_pMeshes;

    int         m_iNumMaterials;
    Material*   m_pMaterials;

    float       m_fTotalTime;

    int         m_iNumJoints;
    Joint*      m_pJoints;

    REAL        d_timer;
    REAL        d_timercur;
};

#endif //
