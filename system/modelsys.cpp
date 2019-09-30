//============================================================================
// Author: Octavian Marius Chincisan 2006
// Zalsoft Inc 1999-2006
//============================================================================
#include "modelsys.h"
#include "baseutils.h"
#include "system.h"

BOOL  Model::LoadFile(const TCHAR *sFileName, DWORD flags)
{
    int i;
    FileWrap fw;//(sFileName,"rb");

    if(!fw.Open(sFileName,"rb"))
    {
        return FALSE;
    }
    PathHandler ph(sFileName);
    _fileName = sFileName;

    DWORD fileSize = fw.Getlength();
	BYTE *pBuffer = new BYTE[fileSize];
    fw.Read(pBuffer, fileSize);
    fw.Close();

	const BYTE *pPtr = pBuffer;
	MS3DHeader *pHeader = ( MS3DHeader* )pPtr;
	pPtr += sizeof( MS3DHeader );

	if ( strncmp( pHeader->m_ID, "MS3D000000", 10 ) != 0 )
    {
		return 0; 
    }

	if ( pHeader->m_version < 3 || pHeader->m_version > 4 )
    {
		return false; 
    }

	int nVertices = *( WORD* )pPtr; 
	m_iNumVertices = nVertices;
	m_pVertices = new BoneVx[nVertices];
	pPtr += sizeof( WORD );

	for ( i = 0; i < nVertices; i++ )
	{
		MS3DVertex *pVertex = ( MS3DVertex* )pPtr;
		m_pVertices[i]._id = pVertex->BoneID;
        m_pVertices[i]._vx = pVertex->m_vertex;
		pPtr += sizeof( MS3DVertex );
	}

	int nTriangles = *( WORD* )pPtr;
	m_iNumTriangles = nTriangles;
	m_pTriangles = new Triangle[nTriangles];
	pPtr += sizeof( WORD );

	for ( i = 0; i < nTriangles; i++ )
	{
		MS3DTriangle *pTriangle = ( MS3DTriangle* )pPtr;

		float vertexIndices[3] = { pTriangle->m_vertexIndices[0], 
                                   pTriangle->m_vertexIndices[1], 
                                   pTriangle->m_vertexIndices[2] };

		float t[3] = {  1.0f-pTriangle->m_t[0], 
                        1.0f-pTriangle->m_t[1], 
                        1.0f-pTriangle->m_t[2] };

		m_pTriangles[i].m_vertexNormals[0] = pTriangle->m_vertexNormals[0];
        m_pTriangles[i].m_vertexNormals[1] = pTriangle->m_vertexNormals[1];
        m_pTriangles[i].m_vertexNormals[2] = pTriangle->m_vertexNormals[2];
        m_pTriangles[i].m_s = pTriangle->m_s;
        m_pTriangles[i].m_t = pTriangle->m_t;
		m_pTriangles[i].m_vertexIndices = vertexIndices;
		pPtr += sizeof( MS3DTriangle );
	}

	int nGroups = *( WORD* )pPtr;
	m_iNumMeshes = nGroups;
	m_pMeshes = new Mesh[nGroups];
	pPtr += sizeof( WORD );
	for ( i = 0; i < nGroups; i++ )
	{
		pPtr += sizeof( BYTE );	// flags
		pPtr += 32;				// name

		WORD nTriangles = *( WORD* )pPtr;
		pPtr += sizeof( WORD );
		int *pTriangleIndices = new int[nTriangles];
		for ( int j = 0; j < nTriangles; j++ )
		{
			pTriangleIndices[j] = *( WORD* )pPtr;
			pPtr += sizeof( WORD );
		}

		char materialIndex = *( char* )pPtr;
		pPtr += sizeof( char );
	
		m_pMeshes[i].m_materialIndex = materialIndex;
		m_pMeshes[i].m_numTriangles = nTriangles;
		m_pMeshes[i].m_pTriangleIndices = pTriangleIndices;
	}

	int nMaterials = *( WORD* )pPtr;
	m_iNumMaterials = nMaterials;
	m_pMaterials = new Material[nMaterials];
	pPtr += sizeof( WORD );
    TCHAR pathTemp[256];
	for ( i = 0; i < nMaterials; i++ )
	{
		MS3DMaterial *pMaterial = ( MS3DMaterial* )pPtr;
		m_pMaterials[i].m_Ambient = CLR(pMaterial->Ambient[0], pMaterial->Ambient[1], pMaterial->Ambient[2], pMaterial->Ambient[3]);
		m_pMaterials[i].m_Diffuse = CLR(pMaterial->Diffuse[0], pMaterial->Diffuse[1], pMaterial->Diffuse[2], pMaterial->Diffuse[3]);
		m_pMaterials[i].m_Specular = CLR(pMaterial->Specular[0], pMaterial->Specular[1], pMaterial->Specular[2], pMaterial->Specular[3]);
		m_pMaterials[i].m_Emissive = CLR(pMaterial->Emissive[0], pMaterial->Emissive[1], pMaterial->Emissive[2], pMaterial->Emissive[3]);
		m_pMaterials[i].m_fShininess = pMaterial->Shininess;
		
		if(strlen(pMaterial->m_texture) > 0)
		{
            sprintf(pathTemp,"%s%s",ph.Path(), pMaterial->m_texture + 2);
            m_pMaterials[i].h_tex = PSystem->GetTexMan()->AddTextureFile(pathTemp);
		}
		pPtr += sizeof( MS3DMaterial );
	}


	float animFPS = *( float* )pPtr;
	pPtr += sizeof( float );

	// skip currentTime
	pPtr += sizeof( float );

	int totalFrames = *( int* )pPtr;
	pPtr += sizeof( int );

	m_fTotalTime = totalFrames*1000.0/animFPS;

	m_iNumJoints = *( WORD* )pPtr;
	pPtr += sizeof( WORD );

	if(m_iNumJoints > 0)
	{
		m_pJoints = new Joint[m_iNumJoints];

		struct JointNameListRec
		{
			int m_jointIndex;
			const char *m_pName;
		};

		const BYTE *pTempPtr = pPtr;

		JointNameListRec *pNameList = new JointNameListRec[m_iNumJoints];
		for ( i = 0; i < m_iNumJoints; i++ )
		{
			MS3DJoint *pJoint = ( MS3DJoint* )pTempPtr;
			pTempPtr += sizeof( MS3DJoint );
			pTempPtr += sizeof( MS3DKeyframe )*( pJoint->m_numRotationKeyframes+pJoint->m_numTranslationKeyframes );

			pNameList[i].m_jointIndex = i;
			pNameList[i].m_pName = pJoint->m_name;
		}

		for ( i = 0; i < m_iNumJoints; i++ )
		{
			MS3DJoint *pJoint = ( MS3DJoint* )pPtr;
			pPtr += sizeof( MS3DJoint );

			int j, parentIndex = -1;
			if ( strlen( pJoint->m_parentName ) > 0 )
			{
				for ( j = 0; j < m_iNumJoints; j++ )
				{
					if ( stricmp( pNameList[j].m_pName, pJoint->m_parentName ) == 0 )
					{
						parentIndex = pNameList[j].m_jointIndex;
						break;
					}
				}
				if ( parentIndex == -1 ) {
					cerr << "Unable to find parent bone in MS3D file" << endl;
					return false;
				}
			}

			m_pJoints[i].m_localRotation = pJoint->m_rotation;
			m_pJoints[i].m_localTranslation = pJoint->m_translation;
			m_pJoints[i].m_parent = parentIndex;
			m_pJoints[i].m_numRotationKeyframes = pJoint->m_numRotationKeyframes;
			m_pJoints[i].m_pRotationKeyframes = new Keyframe[pJoint->m_numRotationKeyframes];
			m_pJoints[i].m_numTranslationKeyframes = pJoint->m_numTranslationKeyframes;
			m_pJoints[i].m_pTranslationKeyframes = new Keyframe[pJoint->m_numTranslationKeyframes];

			for ( j = 0; j < pJoint->m_numRotationKeyframes; j++ )
			{
				MS3DKeyframe *pKeyframe = ( MS3DKeyframe* )pPtr;
				pPtr += sizeof( MS3DKeyframe );

				SetJointKeyframe( i, j, pKeyframe->m_time*1000.0f, pKeyframe->m_parameter, true );
			}

			for ( j = 0; j < pJoint->m_numTranslationKeyframes; j++ )
			{
				MS3DKeyframe *pKeyframe = ( MS3DKeyframe* )pPtr;
				pPtr += sizeof( MS3DKeyframe );

				SetJointKeyframe( i, j, pKeyframe->m_time*1000.0f, pKeyframe->m_parameter, false );
			}
		}
		delete[] pNameList;

		SetupJoints();
	//	calculateNormals();
		
		Restart();
	}
	delete[] pBuffer;

	return true;

}


void Model::SetJointKeyframe( int jointIndex, 
                             int keyframeIndex, 
                             float time, float* parameter, bool isRotation )
{
	if(!( m_iNumJoints > jointIndex))
		return;

	Keyframe& keyframe = isRotation ? m_pJoints[jointIndex].m_pRotationKeyframes[keyframeIndex] :
		m_pJoints[jointIndex].m_pTranslationKeyframes[keyframeIndex];

	keyframe.m_jointIndex = jointIndex;
	keyframe.m_time = time;
	keyframe.m_parameter = parameter;
}


void Model::SetupJoints()
{
	int i;
	for ( i = 0; i < m_iNumJoints; i++ )
	{
		Joint& joint = m_pJoints[i];

        
		joint.m_relative.rotateradians( joint.m_localRotation[0], joint.m_localRotation[1], joint.m_localRotation[2] );
		joint.m_relative.translate( joint.m_localTranslation );
		if ( joint.m_parent != -1 )
		{
			joint.m_absolute = m_pJoints[joint.m_parent].m_absolute;
			joint.m_absolute.multiply(&joint.m_relative);
		}
		else
			joint.m_absolute = joint.m_relative;
	}

	for ( i = 0; i < m_iNumVertices; i++ )
	{
		BoneVx& vertex = m_pVertices[i];

		if ( vertex._id != -1 )
		{
			M4& matrix = m_pJoints[vertex._id].m_absolute;

			matrix.inversetranslatevect( vertex._vx );
			matrix.inverserotatevect( vertex._vx );
		}
	}

	for ( i = 0; i < m_iNumTriangles; i++ ) {
		Triangle& triangle = m_pTriangles[i];
		for ( int j = 0; j < 3; j++ ) {
			BoneVx& vertex = m_pVertices[(const int)triangle.m_vertexIndices[j]];
			if ( vertex._id != -1 ) {
				M4& matrix = m_pJoints[vertex._id].m_absolute;
				matrix.inverserotatevect( triangle.m_vertexNormals[j] );
			}
		}
	}
}

void Model::Restart()
{
	for ( int i = 0; i < m_iNumJoints; i++ )
	{
		m_pJoints[i].m_currentRotationKeyframe = m_pJoints[i].m_currentTranslationKeyframe = 0;
		m_pJoints[i].m_final = m_pJoints[i].m_absolute;
	}

	//m_Timer.reset();
}

void Model::Animate(Scene* pscene, 
                    const Camera* pov, 
                    const SystemData* psy, int camLeaf, int thisIdx)
{
    d_timercur = psy->_ticktime - d_timer;
    if(d_timercur > m_fTotalTime)
    {
        d_timer      = psy->_ticktime;
        d_timercur   = 0;
        Restart();
    }
    

	for ( int i = 0; i < m_iNumJoints; i++ )
	{
		float transVec[3];
		M4 transform;
		int frame;
		Joint *pJoint = &m_pJoints[i];

		if ( pJoint->m_numRotationKeyframes == 0 && pJoint->m_numTranslationKeyframes == 0 )
		{
			pJoint->m_final = pJoint->m_absolute;
			continue;
		}

		frame = pJoint->m_currentTranslationKeyframe;
		while ( frame < pJoint->m_numTranslationKeyframes && pJoint->m_pTranslationKeyframes[frame].m_time < d_timercur )
		{
			frame++;
		}
		pJoint->m_currentTranslationKeyframe = frame;

		if ( frame == 0 )
			memcpy( transVec, pJoint->m_pTranslationKeyframes[0].m_parameter, sizeof ( float )*3 );
		else if ( frame == pJoint->m_numTranslationKeyframes )
			memcpy( transVec, pJoint->m_pTranslationKeyframes[frame-1].m_parameter, sizeof ( float )*3 );
		else
		{
			if(!(frame > 0 && frame < pJoint->m_numTranslationKeyframes ))
				return;

			Keyframe curFrame = pJoint->m_pTranslationKeyframes[frame];
			Keyframe prevFrame = pJoint->m_pTranslationKeyframes[frame-1];

			float timeDelta = curFrame.m_time-prevFrame.m_time;
			float interpValue = ( float )(( d_timercur-prevFrame.m_time )/timeDelta );

			transVec[0] = prevFrame.m_parameter[0]+( curFrame.m_parameter[0]-prevFrame.m_parameter[0] )*interpValue;
			transVec[1] = prevFrame.m_parameter[1]+( curFrame.m_parameter[1]-prevFrame.m_parameter[1] )*interpValue;
			transVec[2] = prevFrame.m_parameter[2]+( curFrame.m_parameter[2]-prevFrame.m_parameter[2] )*interpValue; 
		}

		frame = pJoint->m_currentRotationKeyframe;
		while ( frame < pJoint->m_numRotationKeyframes && pJoint->m_pRotationKeyframes[frame].m_time < d_timercur )
		{
			frame++;
		}
		pJoint->m_currentRotationKeyframe = frame;

		if ( frame == 0 )
			transform.rotateradians( pJoint->m_pRotationKeyframes[0].m_parameter[0], pJoint->m_pRotationKeyframes[0].m_parameter[1], pJoint->m_pRotationKeyframes[0].m_parameter[2] );
		else if ( frame == pJoint->m_numRotationKeyframes )
			transform.rotateradians( pJoint->m_pRotationKeyframes[frame-1].m_parameter[0], pJoint->m_pRotationKeyframes[frame-1].m_parameter[1], pJoint->m_pRotationKeyframes[frame-1].m_parameter[2] );
		else
		{
			if(!( frame > 0 && frame < pJoint->m_numRotationKeyframes ))
				return;

			Keyframe curFrame = pJoint->m_pRotationKeyframes[frame];
			Keyframe prevFrame = pJoint->m_pRotationKeyframes[frame-1];
			
			float timeDelta = curFrame.m_time-prevFrame.m_time;
			float interpValue = ( float )(( d_timercur-prevFrame.m_time )/timeDelta );

			if(!( interpValue >= 0 && interpValue <= 1 ))
				return;

#if 0
			CQuaternion qPrev( prevFrame.m_parameter );
			CQuaternion qCur( curFrame.m_parameter );
			CQuaternion qFinal( qPrev, qCur, interpValue );
			MatrixToQuaternion(&qFinal, &transform);
#else
			float rotVec[3];

			rotVec[0] = prevFrame.m_parameter.x+( curFrame.m_parameter.x-prevFrame.m_parameter.x)*interpValue;
			rotVec[1] = prevFrame.m_parameter.y+( curFrame.m_parameter.y-prevFrame.m_parameter.y)*interpValue;
			rotVec[2] = prevFrame.m_parameter.z+( curFrame.m_parameter.z-prevFrame.m_parameter.z)*interpValue;

			transform.rotateradians( rotVec[0], rotVec[1], rotVec[2] );
#endif
		}

		transform.translate( V3(transVec[0], transVec[1], transVec[2]) );
		M4 relativeFinal( pJoint->m_relative );
		relativeFinal.multiply( &transform );

		if ( pJoint->m_parent == -1 )
			pJoint->m_final = relativeFinal;
		else
		{
			pJoint->m_final = m_pJoints[pJoint->m_parent].m_final;
			pJoint->m_final.multiply( &relativeFinal );
		}
	}
}


void Model::Render(const SystemData* psy)
{
    /*
    Irender* pR = PSystem->Render();

    pR->Push();
    for ( int i = 0; i < m_iNumMeshes; i++ )
    {
	    int materialIndex = m_pMeshes[i].m_materialIndex;
    		
	    if(materialIndex >= 0)
        {
            pR->BindTex1(&m_pMaterials[materialIndex].h_tex);
        }

        pR->Render(GL_TRIANGLES, 
                  
            
                  m_pMeshes[i].m_numTriangles)
    	
        glBegin(GL_TRIANGLES)

    	
		    for ( int j = 0; j < m_pMeshes[i].m_numTriangles; j++ )
		    {
			    int triangleIndex = m_pMeshes[i].m_pTriangleIndices[j];
			    tTriangle* pTri = &m_pTriangles[triangleIndex];
    			
			    for ( int k = 0; k < 3; k++ )
			    {
				    int index = pTri->m_vertexIndices[k];
    				
				    // { NEW }
				    if ( m_pVertices[index].m_boneID == -1 )
				    {
					    // same as before
					    glTexCoord2f( pTri->m_s[k], pTri->m_t[k] );
					    glNormal3fv( pTri->m_vertexNormals[k] );
					    glVertex3fv( m_pVertices[index].m_Pos );
				    }
				    else
				    {
						    // rotate according to transformation matrix
					    CMatrix& final = m_pJoints[m_pVertices[index].m_boneID].m_final;

					    glTexCoord2f( pTri->m_s[k], pTri->m_t[k] );

					    CVector newNormal( pTri->m_vertexNormals[k][0], pTri->m_vertexNormals[k][1], pTri->m_vertexNormals[k][2], 1 );
					    newNormal.Transform3( &final );
					    newNormal.Normalize();
					    glNormal3fv( newNormal.m_pos);

					    CVector newVertex( m_pVertices[index].m_Pos[0], m_pVertices[index].m_Pos[1], m_pVertices[index].m_Pos[2] );
					    newVertex.Transform( &final );
					    glVertex3fv( newVertex.m_pos);
				    }
			    }
		    }
    	
	    glEnd();
    }
    glPopMatrix();
    */

}

