/*
-----------------------------------------------------------------------------
This source file is part of Indigente Game Engine
Indigente - Interactive Digital Entertainment
For the latest info, see http://twiki.im.ufba.br/bin/view/Indigente

Copyright © 2004-2005 Indigente


This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
-----------------------------------------------------------------------------
*/

#include "Bsp.h"
#include "../../Base/TextureArray.h"
#include "../../Render/Drawer.h"
#include "QEntityParser.h"


#include <cstdio>
#include <cmath>

#define 	MAX(X,Y)	X>=Y?X:Y

using namespace Indigente::Graphics::World::Q3Bsp;
using namespace Indigente::Graphics::Base;

const float Bsp::m_kEpsilon;


Bsp::Bsp(){
	m_numOfVerts = 0;			
	m_numOfFaces = 0;			
	m_numOfTextures = 0;		
	m_numOfLightmaps = 0;		
		
	m_numOfNodes = 0;
	m_numOfLeafs = 0 ;
	m_numOfLeafFaces = 0;
	m_numOfPlanes = 0;
		
	m_numOfLeafBrushes = 0;
	m_numOfBrushes = 0;
	m_numOfBrushSides = 0;
			
	m_numOfMeshVerts = 0;         
	m_numOfEntities = 0; 
		
	m_numOfModels = 0;
	m_numOfShaders = 0;
	m_numOfLights = 0;
	
	m_gamma = 5;
	
	m_pVerts = NULL;
	m_pFaces = NULL;
	m_pTextures = NULL;

	m_pNodes = NULL;
	m_pLeafs = NULL;
	m_pPlanes = NULL;
	m_pLeafFaces = NULL;
		
	m_pLeafBrushes = NULL;
	m_pBrushes = NULL;
	m_pBrushSides = NULL;
		
	m_pMeshVerts = NULL;
	m_pEntities = NULL;
		
	m_pModels = NULL;
	m_pShaders = NULL;
	m_pLights = NULL;
		
}

Bsp::~Bsp(){
	if(m_pVerts) delete [] m_pVerts;
	if(m_pFaces) delete [] m_pFaces;
	if(m_pTextures) delete [] m_pTextures;

	if(m_pNodes) delete [] m_pNodes;
	if(m_pLeafs) delete [] m_pLeafs;
	if(m_pPlanes) delete [] m_pPlanes;
	if(m_pLeafFaces) delete [] m_pLeafFaces;
		
	if(m_pLeafBrushes) delete [] m_pLeafBrushes;
	if(m_pBrushes) delete [] m_pBrushes;
	if(m_pBrushSides) delete [] m_pBrushSides;
		
	if(m_pMeshVerts) delete [] m_pMeshVerts;
	if(m_pEntities) delete [] m_pEntities;
		
	if(m_pModels) delete [] m_pModels;
	if(m_pShaders) delete [] m_pShaders;
	if(m_pLights) delete [] m_pLights;
	
	for(unsigned int i=0; i<m_vEntityInfo.size(); i++){
		if(m_vEntityInfo[i]) delete m_vEntityInfo[i];
	}

	for(unsigned int i=0; i<m_vEntityLight.size(); i++){
		if(m_vEntityLight[i]) delete m_vEntityLight[i];
	}

}


string Bsp::findTextureExtension(string strName){
	FILE *fp;
	string filename;
	
	//JPG
	filename = strName;
	filename += ".jpg";
	if((fp = fopen(filename.c_str(), "r"))){
		fclose(fp);
		return filename;
	}

	//TGA	
	filename = strName;
	filename += ".tga";
	if((fp = fopen(filename.c_str(), "r"))){
		fclose(fp);
		return filename;
	}

	//BMP
	filename = strName;
	filename += ".bmp";
	if((fp = fopen(filename.c_str(), "r"))){
		fclose(fp);
		return filename;
	}

	//PNG
	filename = strName;
	filename += ".png";
	if((fp = fopen(filename.c_str(), "r"))){
		fclose(fp);
		return filename;
	}

	//PCX
	filename = strName;
	filename += ".pcx";
	if((fp = fopen(filename.c_str(), "r"))){
		fclose(fp);
		return filename;
	}
	
	return string();	
}




void Bsp::changeGamma(unsigned char *pImage, unsigned int size, float factor){
	for(unsigned int i = 0; i < size / 3; i++, pImage += 3){
		float scale = 1.0f, temp = 0.0f;
		float r = 0, g = 0, b = 0;

		r = (float)pImage[0];
		g = (float)pImage[1];
		b = (float)pImage[2];

		r *= factor / 255.0f;
		g *= factor / 255.0f;
		b *= factor / 255.0f;

		if(r > 1.0f && (temp = (1.0f/r)) < scale) scale=temp;
		if(g > 1.0f && (temp = (1.0f/g)) < scale) scale=temp;
		if(b > 1.0f && (temp = (1.0f/b)) < scale) scale=temp;

		scale*=255.0f;
		r*=scale;   g*=scale;   b*=scale;

		pImage[0] = (unsigned char)r;
		pImage[1] = (unsigned char)g;
		pImage[2] = (unsigned char)b;
	}
	
}

void Bsp::createLightmap(unsigned int &texture, BspLightmap *pImageBits, int width, int height){
	Drawer *drawer = Drawer::getInstance();
	
	drawer->genTextures(1, &texture);
	
	drawer->pixelStore(InGE_UNPACK_ALIGNMENT,1);
	
	drawer->bindTexture(InGE_TEXTURE_2D, texture);
	
	this->changeGamma((unsigned char *)pImageBits, width*height*3, m_gamma);
	
	drawer->build2DMipmaps(InGE_TEXTURE_2D, 3, width, height, InGE_RGB, InGE_UNSIGNED_BYTE, pImageBits);
	
	drawer->texParameter(InGE_TEXTURE_2D, InGE_TEXTURE_MAG_FILTER, InGE_LINEAR);
	drawer->texParameter(GL_TEXTURE_2D, InGE_TEXTURE_MIN_FILTER, InGE_LINEAR_MIPMAP_NEAREST);
	drawer->texEnv(InGE_TEXTURE_ENV, InGE_TEXTURE_ENV_MODE, InGE_MODULATE);
	
	
}


int Bsp::findLeaf(V3 &vPos){
	int i = 0;
	float distance = 0.0f;
	
	while(i >=0){
		BspNode &node = m_pNodes[i];
		BspPlane &plane = m_pPlanes[node.plane];
		V3 normal(plane.vNormal);
		
		// Distancia do ponto ao plano. 
		// Para determinaçao do lado do plano em que se encotra o ponto
		distance = ( normal * vPos ) - plane.d;


		if(distance >= 0) i = node.front;
		else i = node.back;
		
	}
	
	return ~i;
}


void Bsp::renderLevel(V3 vPos, Frustum &frustum){
	Drawer *drawer = Drawer::getInstance();
	int indexLeaf = this->findLeaf(vPos);
	
	m_pFrustum = &frustum;
	m_camera = vPos;
	m_cameraCluster = m_pLeafs[indexLeaf].cluster;
	m_facesDrawn.clearAll();


	// Seta o cliente de estado para coordenadas de vertices de textura
	drawer->enableClientState(InGE_VERTEX_ARRAY);
	drawer->enableClientState(InGE_NORMAL_ARRAY);
	//drawer->enableClientState(InGE_COLOR_ARRAY);
	
	//drawer->enableClientState(InGE_TEXTURE_COORD_ARRAY);
	
	// Liga o vetor de textura para primeira passada
	drawer->clientActiveTextureARB(InGE_TEXTURE0_ARB);
	drawer->enableClientState(InGE_TEXTURE_COORD_ARRAY);

	// Liga o vetor de textura para segunda passada	
	drawer->clientActiveTextureARB(InGE_TEXTURE1_ARB);
	drawer->enableClientState(InGE_TEXTURE_COORD_ARRAY);
	
	
	renderTree(0);
	drawer->disableClientState(InGE_VERTEX_ARRAY);
	drawer->disableClientState(InGE_NORMAL_ARRAY);
	drawer->disableClientState(InGE_COLOR_ARRAY);
	drawer->clientActiveTextureARB(InGE_TEXTURE0_ARB);
	drawer->disableClientState(InGE_TEXTURE_COORD_ARRAY);
	drawer->clientActiveTextureARB(InGE_TEXTURE1_ARB);
	drawer->disableClientState(InGE_TEXTURE_COORD_ARRAY);

}



void Bsp::renderTree(int nodeIndex){
	BspNode &node = m_pNodes[nodeIndex];
	
	if(!m_pFrustum->isBoxInFrustum(node.min[0], node.min[1], node.min[2], 
								node.max[0], node.max[1], node.max[2])) return;

	BspPlane &plane	= m_pPlanes[node.plane];
	V3 normal(plane.vNormal);
	
	if(normal * m_camera - plane.d > 0){ 
		if(node.back >= 0) 	renderTree(node.back);
		else renderLeaf(~node.back);

		if(node.front >= 0)	renderTree(node.front);
		else renderLeaf(~node.front);
	}
	else {
		if(node.front >= 0) renderTree(node.front);
		else renderLeaf(~node.front);

		if(node.back >= 0)	renderTree(node.back);
		else renderLeaf(~node.back);
	}
}


void Bsp::renderLeaf(int leafIndex){
	BspLeaf &leaf =  m_pLeafs[leafIndex]; 
	int faceCount = leaf.numOfLeafFaces;
	
	if(!isClusterVisible(m_cameraCluster, leaf.cluster)) return;
	
	
	// Verifica se a folha tem interseccao com o volume de visualizacao
	if(!m_pFrustum->isBoxInFrustum(leaf.min[0], leaf.min[1], leaf.min[2], 
								leaf.max[0], leaf.max[1], leaf.max[2])) return;
	
	
	// Percorre as faces da folha
	while(faceCount--){
		BspLeafFace faceIndex = m_pLeafFaces[leaf.leafFace + faceCount];
		
		if(m_pFaces[faceIndex].type == BILLBOARD) continue;
		else if(m_pFaces[faceIndex].type == BEZIER_PATCH){
			 renderPatch(faceIndex);
		}
		else{
			if(!m_facesDrawn[faceIndex]){
				m_facesDrawn.set(faceIndex);
				renderFace(faceIndex);
			}
		}
	}
}


void Bsp::renderFace(int faceIndex){
	Drawer *drawer = Drawer::getInstance();
	
	BspFace &face = m_pFaces[faceIndex];
	
	m_vShaders[face.textureID]->applyMode();
	
	// Textura
	drawer->activeTextureARB(InGE_TEXTURE0_ARB);
	drawer->enable(InGE_TEXTURE_2D);
	//drawer->bindTexture(InGE_TEXTURE_2D, m_textures[face.textureID]);
	drawer->bindTexture(InGE_TEXTURE_2D, m_vShaders[face.textureID]->getTextureId());
	
	// Lightmap
	drawer->activeTextureARB(InGE_TEXTURE1_ARB);	
	drawer->enable(InGE_TEXTURE_2D);
	drawer->bindTexture(InGE_TEXTURE_2D, m_lightmaps[face.lightmapID]);
	
	// De sizeof(BspVertex) em sizeof(BspVertex) pega 3 floats a partir de vPosition
	drawer->vertexPointer(3, InGE_FLOAT, sizeof(BspVertex), &m_pVerts[face.startVertIndex].vPosition[0]);
	//drawer->colorPointer(4, InGE_BYTE, sizeof(BspVertex), m_pVerts[face.startVertIndex].color);	
	drawer->normalPointer(InGE_FLOAT, sizeof(BspVertex), &m_pVerts[face.startVertIndex].vNormal[0]);


	
	// Cliente de estado de textura
	drawer->clientActiveTextureARB(InGE_TEXTURE0_ARB);
	drawer->texCoordPointer(2, InGE_FLOAT, sizeof(BspVertex), &m_pVerts[face.startVertIndex].vTextureCoord);
	
	// Cliente de estado de lightmaps
	drawer->clientActiveTextureARB(InGE_TEXTURE1_ARB);
	drawer->texCoordPointer(2, InGE_FLOAT, sizeof(BspVertex), &m_pVerts[face.startVertIndex].vLightmapCoord);

	
	
	// drawElements utiliza-se de informacoes contida no vetor m_pMeshVerts
	//para desenhar vertices especificos apontodos pelo vetor. 
	// m_pMeshVerts forma a malha que devera ser renderizada.
	drawer->drawElements(InGE_TRIANGLES, face.numMeshVerts, InGE_UNSIGNED_INT, &m_pMeshVerts[face.meshVertIndex]);
	
}

void Bsp::renderPatch(int faceIndex){
	m_patches[faceIndex].render();
	

}


int Bsp::isClusterVisible(int current, int test){
	if(!m_clusters.pBitsets || current < 0) return 1;
	return ( m_clusters.pBitsets[  current * m_clusters.bytesPerCluster + (test >> 3)  ]   &   (1 << (test & 7) ) ) ;
}




bool Bsp::load(const char *filename){
	FILE *fp;
	int length = 0;

	if(!(fp = fopen(filename, "rb")))
		return false;

	BspHeader header;
	BspLump lumps[kMaxLumps];
	
	//Leitura de cabeçalho
	fread(&header, 1, sizeof(BspHeader), fp);
	if((header.strID[0] != 'I') || (header.strID[1] != 'B') || (header.strID[2] != 'S') || (header.strID[3] != 'P') )
		return false;
	if(header.version != 0x2e)
		return false;
	
	//Leitura de lumps
	fread(lumps, kMaxLumps, sizeof(BspLump), fp);
	
	
	// Leitura de Vertices
	if(lumps[kVertices].length){
		m_numOfVerts 	= lumps[kVertices].length / sizeof(BspVertex);
		m_pVerts 		= new BspVertex [m_numOfVerts];
		fseek(fp, lumps[kVertices].offset, SEEK_SET);
		fread(m_pVerts, m_numOfVerts, sizeof(BspVertex), fp);
	}
	length += lumps[kVertices].length;
		
	// Leitura de Faces
	if(lumps[kFaces].length){
		m_numOfFaces 	= lumps[kFaces].length / sizeof(BspFace);
		m_pFaces 		= new BspFace [m_numOfFaces];
		fseek(fp, lumps[kFaces].offset, SEEK_SET);
		fread(m_pFaces, m_numOfFaces, sizeof(BspFace), fp);
	}
	length += lumps[kFaces].length;

	// Leitura de informacao de textura
	if(lumps[kTextures].length){
		m_numOfTextures = lumps[kTextures].length / sizeof(BspTexture);
		m_pTextures 	= new BspTexture [m_numOfTextures];
		fseek(fp, lumps[kTextures].offset, SEEK_SET);
		fread(m_pTextures, m_numOfTextures, sizeof(BspTexture), fp);

		//TextureArray *textureArray = TextureArray::getInstance();	
		QShaderManager *shaderManager = QShaderManager::getInstance();
		for(int i=0; i< m_numOfTextures; i++){
			QShader *shader = shaderManager->getShader(m_pTextures[i].strName);
			//printf("%s\n", m_pTextures[i].strName);

			if(shader)m_vShaders.push_back(shader);
			else{
				shader = new QShader(m_pTextures[i].strName);
				shaderManager->addShader(shader);
				m_vShaders.push_back(shader);
			}
			
			m_vShaders[i]->loadTextures();
			//m_vShaders[i]->print();
			
			//Acha qual a extensao da textura

			//string filename = this->findTextureExtension(m_pTextures[i].strName);
			
			//if(!filename.empty())  // Armazena em m_texture[i] o id da textura carregada
				//textureArray->loadTexture(filename, &m_textures[i]);

			//printf("%s %s\n", m_pTextures[i].strName,filename.c_str());
		}
		
		//delete [] m_pTextures, m_pTextures = NULL;
	}
	length += lumps[kTextures].length;
	
		
	//Leitura de informação de lightmap
	if(lumps[kLightmaps].length){
		m_numOfLightmaps = lumps[kLightmaps].length / sizeof(BspLightmap);
		BspLightmap *pLightmaps = new BspLightmap[m_numOfLightmaps];
		fseek(fp, lumps[kLightmaps].offset, SEEK_SET);
		fread(pLightmaps, m_numOfLightmaps, sizeof(BspLightmap), fp);
		for(int i=0; i < m_numOfLightmaps; i++){
			this->createLightmap(m_lightmaps[i], &pLightmaps[i], 128,128);
		}
		delete [] pLightmaps;
	}
	length += lumps[kLightmaps].length;

	//Leitura de Nodos 
	if(lumps[kNodes].length){
		m_numOfNodes 	= lumps[kNodes].length / sizeof(BspNode);
		m_pNodes		= new BspNode [m_numOfNodes];
		
		fseek(fp, lumps[kNodes].offset, SEEK_SET);
		fread(m_pNodes, m_numOfNodes, sizeof(BspNode), fp);
	}
	length += lumps[kNodes].length;
	length += sizeof(PhysicalSpace) * m_numOfNodes;
		
	//Leitura das folhas
	if(lumps[kLeafs].length){
		m_numOfLeafs	= lumps[kLeafs].length / sizeof(BspLeaf);
		m_pLeafs		= new BspLeaf [m_numOfLeafs];
		
		fseek(fp, lumps[kLeafs].offset, SEEK_SET);
		fread(m_pLeafs, m_numOfLeafs, sizeof(BspLeaf), fp);
	}
	length += lumps[kLeafs].length;
	length += sizeof(PhysicalSpace) * m_numOfLeafs;
		
	// Leitura de faces da folha
	if(lumps[kLeafFaces].length){
		m_numOfLeafFaces 	= lumps[kLeafFaces].length / sizeof(BspLeafFace);
		m_pLeafFaces		= new BspLeafFace [m_numOfLeafFaces];
		fseek(fp, lumps[kLeafFaces].offset, SEEK_SET);
		fread(m_pLeafFaces, m_numOfLeafFaces, sizeof(BspLeafFace), fp);
	}
	length += lumps[kLeafFaces].length;
		
	// Leitura de planos
	if(lumps[kPlanes].length){
		m_numOfPlanes		= lumps[kPlanes].length / sizeof(BspPlane);
		m_pPlanes 			= new BspPlane [m_numOfPlanes];
		fseek(fp, lumps[kPlanes].offset, SEEK_SET);
		fread(m_pPlanes, m_numOfPlanes, sizeof(BspPlane), fp);
	}
	length += lumps[kPlanes].length;
	
	// Leitura dos Dados de visualizacao
	if(lumps[kVisData].length){
		int size;
		fseek(fp, lumps[kVisData].offset, SEEK_SET);

		fread(&(m_clusters.numOfClusters),       1, sizeof(int), fp);
		fread(&(m_clusters.bytesPerCluster), 1, sizeof(int), fp);
		
		size = m_clusters.numOfClusters * m_clusters.bytesPerCluster;
		
		m_clusters.pBitsets = new unsigned char [size];

		fread(m_clusters.pBitsets, 1, sizeof(unsigned char)*size, fp);
	}
	else m_clusters.pBitsets = NULL;
	
	length += lumps[kVisData].length;

	
	// Leitura dos Brushes
	if(lumps[kBrushes].length){
		m_numOfBrushes 		= lumps[kBrushes].length / sizeof(BspBrush);
		m_pBrushes			= new BspBrush [m_numOfBrushes];
		fseek(fp, lumps[kBrushes].offset, SEEK_SET);
		fread(m_pBrushes, m_numOfBrushes, sizeof(BspBrush), fp);
	}
		
	length += lumps[kBrushes].length;
	
	// Leitura das folhas dos Brushes
	if(lumps[kLeafBrushes].length){
		m_numOfLeafBrushes 	= lumps[kLeafBrushes].length / sizeof(BspLeafBrush);
		m_pLeafBrushes		= new BspLeafBrush[m_numOfLeafBrushes];
		fseek(fp, lumps[kLeafBrushes].offset, SEEK_SET);
		fread(m_pLeafBrushes, m_numOfLeafBrushes, sizeof(BspLeafBrush), fp);
	}
	
	length += lumps[kLeafBrushes].length;
	
	// Leitura do BrushSide
	if(lumps[kBrushSides]. length){
		m_numOfBrushSides	= lumps[kBrushSides]. length / sizeof(BspBrushSide);
		m_pBrushSides		= new BspBrushSide [m_numOfBrushSides];
		
		fseek(fp, lumps[kBrushSides].offset, SEEK_SET);
		fread(m_pBrushSides,m_numOfBrushSides, sizeof(BspBrushSide), fp);
	}
	length += lumps[kBrushSides]. length;
	
	// Leitura de indice para vertices do Mesh
	if(lumps[kMeshVerts].length){
		m_numOfMeshVerts	= lumps[kMeshVerts].length / sizeof(BspMeshVertex);
		m_pMeshVerts		= new BspMeshVertex [m_numOfMeshVerts];
		fseek(fp, lumps[kMeshVerts].offset, SEEK_SET);
		fread(m_pMeshVerts, m_numOfMeshVerts, sizeof(BspMeshVertex),fp );
	}
	length += lumps[kMeshVerts].length;
	
	
	// Leitura de entities
	if(lumps[kEntities].length){
		m_numOfEntities 	= lumps[kEntities].length / sizeof(BspEntity);
		m_pEntities 		= new BspEntity [m_numOfEntities];
		fseek(fp, lumps[kEntities].offset, SEEK_SET);
		fread(m_pEntities, m_numOfEntities, sizeof(BspEntity),fp);
		//fwrite(m_pEntities, m_numOfEntities, sizeof(BspEntity),stdout );
	}
	length += lumps[kEntities].length;

	QEntityParser parser(m_pEntities, m_numOfEntities);
	QEntity *entity =  NULL;
	while((entity = parser.nextEntity()) != NULL){
		if(entity->type == InGE_QENTITY_INFO){
			QEntityInfo *p = (QEntityInfo *) entity;
			m_vEntityInfo.push_back(p);
		}
		else if(entity->type == InGE_QENTITY_LIGHT){
			QEntityLight *p = (QEntityLight *) entity;			
			m_vEntityLight.push_back(p);
		}
	}
	
	
	// Leitura de modelos
	if(lumps[kModels].length){
		m_numOfModels		= lumps[kModels].length / sizeof(BspModel);
		m_pModels			= new BspModel[m_numOfModels];
		fseek(fp, lumps[kModels].offset, SEEK_SET);
		fread(m_pModels, m_numOfModels, sizeof(BspModel), fp);
	}
	length += lumps[kModels].length;
	
	// Leitura de Shaders
	if(lumps[kShaders].length){
		m_numOfShaders		= lumps[kShaders].length / sizeof (BspShader);
		m_pShaders			= new BspShader [m_numOfShaders];
		fseek(fp, lumps[kShaders].offset, SEEK_SET);
		fread(m_pShaders, m_numOfShaders, sizeof(BspShader), fp);
	}
	length += lumps[kShaders].length;
	
	
	// Leitura de Volume light
	if(lumps[kLightVolumes].length){
		m_numOfLights		= lumps[kLightVolumes].length / sizeof(BspLight);
		m_pLights			= new BspLight [m_numOfLights];
		fseek(fp, lumps[kLightVolumes].offset, SEEK_SET);
		fread(m_pLights, m_numOfLights, sizeof(BspLight), fp);
	}
	length += lumps[kLightVolumes].length;
	
	fclose(fp);

	unsigned int j = m_numOfFaces;
	while(j--) {
		if (m_pFaces[j].type == BEZIER_PATCH) {
			m_patches.insert(std::make_pair(j, QBezier()));
			QBezier &bez = m_patches[j];

			bez.setControlSize(m_pFaces[j].size[0], m_pFaces[j].size[1]);
			bez.setTextureID(m_vShaders[m_pFaces[j].textureID]->getTextureId());
			bez.setLightmapID(m_lightmaps[ m_pFaces[j].lightmapID ]);
			
			
			for (int m = 0; m < m_pFaces[j].size[0] * m_pFaces[j].size[1]; ++m)
				bez.addControl(m_pVerts[m_pFaces[j].startVertIndex + m]);

			bez.tesselate(8);
		}
	}

	

	//printf("Alocado %d bytes, %d kbytes, %d mbytes\n", length, (int)length/1024, (int)length/(1024*1024)); 
	m_facesDrawn.resize(m_numOfFaces);
	return true;
}


/**
 * Ray Collision
 * Verifica colisao no movimento pretendido
 */
Q3MoveData &Bsp::checkMoveCollision(V3 start, V3 end){
	m_moveData.startOut = true;
	m_moveData.allSolid = false;
	m_moveData.fraction = 1.0f;
	m_moveData.endPoint = end;	
	m_traceType = InGE_BSP_TRACE_RAY;
	offset = 0.0;
	
	m_startMovePosition = start;
	m_endMovePosition = end;

	if(start == end) return m_moveData;
	// percorre a arvore a partir da raiz
	checkNode(0, 0.0f, 1.0f, start, end);
	
	// Se houve colisao, atualiza os valores
	if(m_moveData.fraction == 1.0f)		m_moveData.endPoint = end;
	else m_moveData.endPoint = start + (end - start)* m_moveData.fraction;
	
	return m_moveData;
}

/**
 * Ray Collision
 * Verifica colisao no movimento pretendido e tentar fazer slide
 */
Q3MoveData &Bsp::checkMoveCollisionAndTrySlide(V3 start, V3 end){
	m_moveData.startOut = true;
	m_moveData.allSolid = false;
	m_moveData.fraction = 1.0f;
	m_moveData.endPoint = end;	
	m_traceType = InGE_BSP_TRACE_RAY;
	offset = 0.0;
	
	m_startMovePosition = start;
	m_endMovePosition = end;

	if(start == end) return m_moveData;
	// percorre a arvore a partir da raiz
	checkNode(0, 0.0f, 1.0f, start, end);
	
	// Se houve colisao, atualiza os valores
	if(m_moveData.fraction == 1.0f)		m_moveData.endPoint = end;
	else{
		V3 slideStartPoint;
		slideStartPoint = m_moveData.endPoint = start + (end - start)* m_moveData.fraction;
		
		V3 v1 = (start - end)* (1.0f - m_moveData.fraction);
		m_moveData.normal.normalize();
		
		V3 v2 =  m_moveData.normal * (v1 * m_moveData.normal);
		m_moveData.endPoint += (v2-v1);
		
		m_moveData.endPoint += m_moveData.normal * 0.0001f;
		checkMoveCollisionAndTrySlide(slideStartPoint, m_moveData.endPoint);
	}
	
	return m_moveData;
}


/**
 * Sphere Collision
 * Verifica colisao no movimento pretendido
 */
Q3MoveData &Bsp::checkMoveCollision(V3 start, V3 end, float radius){
	m_moveData.startOut = true;
	m_moveData.allSolid = false;
	m_moveData.fraction = 1.0f;
	m_moveData.endPoint = end;	
	m_traceType = InGE_BSP_TRACE_SPHERE;
	offset = radius;
	
	m_startMovePosition = start;
	m_endMovePosition = end;

	if(start == end) return m_moveData;
	// percorre a arvore a partir da raiz
	checkNode(0, 0.0f, 1.0f, start, end);
	
	// Se houve colisao, atualiza os valores
	if(m_moveData.fraction == 1.0f)		m_moveData.endPoint = end;
	else m_moveData.endPoint = start + (end - start)* m_moveData.fraction;
	
	return m_moveData;
}


/**
 * Sphere Collision
 * Verifica colisao no movimento pretendido e tentar fazer slide
 */
 
Q3MoveData &Bsp::checkMoveCollisionAndTrySlide(V3 start, V3 end, float radius){
	m_moveData.startOut = true;
	m_moveData.allSolid = false;
	m_moveData.fraction = 1.0f;
	m_moveData.endPoint = end;	
	m_traceType = InGE_BSP_TRACE_SPHERE;
	offset = radius;
	
	m_startMovePosition = start;
	m_endMovePosition = end;

	if(start == end) return m_moveData;
	// percorre a arvore a partir da raiz
	checkNode(0, 0.0f, 1.0f, start, end);
	
	// Se houve colisao, atualiza os valores
	if(m_moveData.fraction == 1.0f)		m_moveData.endPoint = end;
	else{
		V3 slideStartPoint;
		slideStartPoint = m_moveData.endPoint = start + (end - start)* m_moveData.fraction;

		V3 v1 = (start - end)* (1.0f - m_moveData.fraction);
		m_moveData.normal.normalize();

		V3 v2 =  m_moveData.normal * (v1 * m_moveData.normal);
		m_moveData.endPoint += (v2-v1);
	
		m_moveData.endPoint += m_moveData.normal * 0.1f;
		checkMoveCollisionAndTrySlide(slideStartPoint, m_moveData.endPoint, radius);
	}
	
	return m_moveData;
}


/**
 * Box Collision
 * Verifica colisao no movimento pretendido
 */
Q3MoveData &Bsp::checkMoveCollision(V3 start, V3 end, V3 min, V3 max){
	m_moveData.startOut = true;
	m_moveData.allSolid = false;
	m_moveData.fraction = 1.0f;
	m_moveData.endPoint = end;	
	m_traceType = InGE_BSP_TRACE_BOX;
	m_traceMin = min;
	m_traceMax = max;
	offset = 0.0f;
	
	m_startMovePosition = start;
	m_endMovePosition = end;
	
	m_extends.setXYZ(MAX(fabs(min[0]), fabs(max[0])), MAX(fabs(min[1]), fabs(max[1])), MAX(fabs(min[2]), fabs(max[2])));

	if(start == end) return m_moveData;
	// percorre a arvore a partir da raiz
	checkNode(0, 0.0f, 1.0f, start, end);
	
	// Se houve colisao, atualiza os valores
	if(m_moveData.fraction == 1.0f)		m_moveData.endPoint = end;
	else m_moveData.endPoint = start + (end - start)* m_moveData.fraction;
	
	return m_moveData;
}

/**
 * Box Collision
 * Verifica colisao no movimento pretendido e tenta fazer slide
 */
Q3MoveData &Bsp::checkMoveCollisionAndTrySlide(V3 start, V3 end, V3 min, V3 max){
	m_moveData.startOut = true;
	m_moveData.allSolid = false;
	m_moveData.fraction = 1.0f;
	m_moveData.endPoint = end;
	m_traceType = InGE_BSP_TRACE_BOX;
	m_traceMin = min;
	m_traceMax = max;
	offset = 0.0f;
	
	m_startMovePosition = start;
	m_endMovePosition = end;
	
	m_extends.setXYZ(MAX(fabs(min[0]), fabs(max[0])), MAX(fabs(min[1]), fabs(max[1])), MAX(fabs(min[2]), fabs(max[2])));

	if(start == end) return m_moveData;
	// percorre a arvore a partir da raiz
	checkNode(0, 0.0f, 1.0f, start, end);
	
	// Se houve colisao, atualiza os valores
	if(m_moveData.fraction == 1.0f)		m_moveData.endPoint = end;
	else {
		V3 slideStartPoint;
		slideStartPoint = m_moveData.endPoint = start + (end - start)* m_moveData.fraction;

		V3 v1 = (start - end)* (1.0f - m_moveData.fraction);
		m_moveData.normal.normalize();

		V3 v2 =  m_moveData.normal * (v1 * m_moveData.normal);
		m_moveData.endPoint += (v2-v1);

		m_moveData.endPoint += m_moveData.normal * 0.0001f;
		checkMoveCollisionAndTrySlide(slideStartPoint, m_moveData.endPoint, min, max);
	}
	
	return m_moveData;
}



/**
 * Percorre a arvore(bsp) para ateh achar as folhas para verificar colisao
 */

void Bsp::checkNode(int nodeIndex, float fStart, float fEnd, V3 start, V3 end){
	if(m_moveData.fraction <= fStart)
		return;
	
	//Se o noh eh folha
	if(nodeIndex<0)	{
		BspLeaf &leaf = m_pLeafs[~nodeIndex];
		
		for (int i = 0; i < leaf.numOfLeafBrushes; i++){
			BspBrush &brush = m_pBrushes[m_pLeafBrushes[leaf.leafBrush + i]];
			// Checa se o brush eh valido e  material pra colisao
			if((brush.numOfBrushSides > 0) && (((m_pTextures[brush.textureID].contents)&1)||((m_pTextures[brush.textureID].contents)&0x10000))){
				checkBrush(brush);
			}
			
		}
		return;
	}
	
	BspNode	&node = m_pNodes[nodeIndex];
	BspPlane &plane = m_pPlanes[node.plane];
	V3  normal(plane.vNormal);
	
	float dS = (normal * start) - plane.d;
	float dE = (normal * end) - plane.d;

	if(m_traceType == InGE_BSP_TRACE_BOX) offset = fabs(m_extends[0]*normal[0])+fabs(m_extends[1]*normal[1])+fabs(m_extends[2]*normal[2]);
	
	// Se ambos os pontos estaum na frente do plano, vai pra filho da frente
	if((dS >= offset) && (dE >= offset)){
		checkNode(node.front, fStart, fEnd, start, end);
	}
	// Se ambos os pontos estaum atras do plano, vai pra filho de tras
	else if((dS < -offset) && (dE < -offset)){
		checkNode(node.back, fStart, fEnd, start, end);	
	}
	// Caso contrario, split e vai pra os dois filhos.
	
	else{
		// Indice do noh para o ponto inicial e final
		int s1, s2;
		// Percentual a ser movido pelo ponto inicial e final
		float f1, f2;
		// Ponto de split
		V3 middle;
		
		// Ajusta Indices e Percentuais
		if(dS < dE){
			s1 = node.back;
			s2 = node.front;
			
			float inverseDiff = 1.0f / (dS - dE);

			f1 = (dS - m_kEpsilon - offset) * inverseDiff;
			f2 = (dS + m_kEpsilon + offset) * inverseDiff;			

		}
		else if(dS > dE){
			s1 = node.front;
			s2 = node.back;
			
			float inverseDiff = 1.0f / (dS - dE);

			f1 = (dS + m_kEpsilon + offset) * inverseDiff;
			f2 = (dS - m_kEpsilon - offset) * inverseDiff;			

		}
		else {
			s1 = node.front;
			s2 = node.back;
			f1 = 1.0f;
			f2 = 0.0f;
		}
	
		// Valida Percentuais
		if(f1 < 0.0f) f1 = 0.0f;
		else if (f1 > 1.0f) f1 = 1.0f;
		if(f2 < 0.0f) f2 = 0.0f;
		else if (f2 > 1.0f) f2 = 1.0f;
		
		// definindo ponto de split a partir do ponto inicial
		middle = start + (end - start) * f1;
		// define o percentual do ponto inicial ateh o ponto de split			
		float middleFraction = fStart + (fEnd - fStart) * f1;
		//checagem no noh do ponto inicial
		checkNode(s1, fStart, middleFraction, start, middle);
		
		// definindo ponto de split a partir do ponto final
		middle =  start + (end - start) * f2;
		// define o percentual do ponto final ateh o ponto de split			
		middleFraction = fStart + (fEnd - fStart) * f2;
		//checagem no noh do ponto final
		checkNode(s2, middleFraction, fEnd, middle, end);

	}		

}


/**
 * Faz checagem de colisao com um Brush especifico
 */
 
void Bsp::checkBrush(BspBrush &brush){
	float fStart= -1.0f;
	float fEnd = 1.0f;
	bool  startOut = false;
	bool  endOut   = false;
	
	// Candidato a vetor normal
	V3 vCandidateToHitNormal; 
	
	// Varre os lados do Brush verificando colisao
	for(int i = 0 ; i < brush.numOfBrushSides; i++){
		BspBrushSide &brushSide = m_pBrushSides[brush.brushSide + i];
		BspPlane &plane = m_pPlanes[brushSide.plane];
		
		V3 normal(plane.vNormal);
		float dS;
		float dE;
		
		
		if(m_traceType == InGE_BSP_TRACE_BOX){ // Box
			V3 offset( normal[0] < 0 ? m_traceMax[0] : m_traceMin[0], 
							normal[1] < 0 ? m_traceMax[1] : m_traceMin[1],
							normal[2] < 0 ? m_traceMax[2] : m_traceMin[2]);
							
			dS = normal * (m_startMovePosition + offset) - plane.d;
			dE = normal * (m_endMovePosition + offset) - plane.d;		
		}
		else { // Ray and Sphere
			dS = normal * m_startMovePosition - plane.d - offset;
			dE = normal * m_endMovePosition - plane.d - offset;		
		}
				
		if(dS > 0) startOut = true;
		if(dE > 0) endOut = true;
		
		// Se os dois pontos estaum fora, nao ha colisao
		if((dS > 0) && (dE > 0)) return; 
		// Se os dois pontos estaum atras, testar com outro plano
		else if((dS <= 0) && (dE <= 0)) continue;
		
		
		// Faz dS tender ao ponto final e dE tender ao inicial
		// se ele se cruzarem, entaum naum houve colisao
		if(dS > dE){
			float fraction = (dS - m_kEpsilon) / (dS - dE);
			if(fraction > fStart){
				fStart = fraction;
				vCandidateToHitNormal = normal;
			}
		}
		else{
			float fraction = (dS + m_kEpsilon) / (dS - dE);
			if(fraction < fEnd)	fEnd = fraction;
		}
	}//for
	
	
	if(!startOut){ // inicia dentro do brush
		m_moveData.startOut = false;
		if(!endOut) m_moveData.allSolid = true;
		return;
	}
	
	// Se houve colisao
	if(fStart < fEnd){
		if((fStart > -1.0f) && (fStart < m_moveData.fraction)){
			if(fStart < 0) fStart = 0;
			m_moveData.fraction = fStart;
			m_moveData.normal = vCandidateToHitNormal;
		}
	}
	
}


vector<QEntityInfo *>& Bsp::getVectorOfEntityInfo(){
	return m_vEntityInfo;
}

vector<QEntityLight *>& Bsp::getVectorOfEntityLight(){
	return m_vEntityLight;	
}

 



