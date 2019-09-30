#ifndef __AN8FILE_N_HH__
#define  __AN8FILE_N_HH__

#include "baselib.h"
#include "_irender.h"
#include <string>
#include <map>
using namespace std;


//=======================
// an8 sections strutures
struct AN_Header
{
    int         version;    // *10000
    int         buildDate;
};

struct AN_Base
{
    V3 origin;
    Quat orientation;
};


struct AN_Desc
{
    char desc[512];
};

struct AN_Env
{
    REAL    xyzw[4];
    int     framerate;
};


struct AN_TexParams
{
    int blendmode;          // { decal }
    int alphamode;          // { none }
};



struct AN_Colorp
{
    REAL            factor;         //factor
    CLR             rgb;            //rgb
    char            texname[24];
    AN_TexParams    textureparams;

};

struct AN_Surface
{
    CLR             rgb;
    AN_Colorp       diffuse;    //diffuse
    AN_Colorp       specular;   //specular
    AN_Colorp       ambiant;    //ambiant
    AN_Colorp       emissive;   //specular
    REAL            phongsize;

};

struct AN_Material
{
    char            name[24];
    AN_Surface      surface;
    char            texturename[24];
    AN_TexParams    texParams;

};

struct AN_Image
{
    char    name[24];
    char    file[256];
    AN_Base base;
    UV      size;
};

struct AN_FaceRef
{
    int nvexes;
    int flags;
    int material;
    int normal;
    vvector<int> vertices;
    vvector<int> normals;
    vvector<int> texcoords;
   
};

struct AN_Mesh
{
    ~AN_Mesh()
    {
        rfaces.deleteelements();
    }

    char        name[24];
    AN_Base     base;
    AN_Base     pivot;
    char        material[24];
    REAL        smoothangle;

    vvector<string>     matnames;
    vvector<V3>         points;
    vvector<SIZE>       edges;
    vvector<UV>         texcoord;
    vvector<AN_FaceRef*> rfaces;    

};

struct AN_Object
{
    ~AN_Object()
    {
        pmaterials.deleteelements();
        pimages.deleteelements();
    }

    char                    name[32];
    vvector<AN_Material*>   pmaterials;
    vvector<AN_Image*>      pimages;
    AN_Mesh                 mesh;

};

struct AN_point
{
    REAL x,y,z;
};

struct AN_Size
{
    REAL    u,v;
};


struct AN_NamedObject
{
    char            name[24];
    char            name1[24];
    char            material[24];
    vvector<string> weightedby;
};


struct AN_Bone
{
    ~AN_Bone(){
        bones.deleteelements();
    }
    char                name[24];
    int                 length;
    Quat                orientation;
    V3                  dof[4];
    REAL                influence[6];
    vvector<AN_Bone*>   bones;
    AN_NamedObject      nameobj;
};

struct AN_Figure
{
    ~AN_Figure(){
        bones.deleteelements();
    }
    char    name[24];
    vvector<AN_Bone*> bones;
};

struct AN_Track
{
   int frameNo;
   V3  value;
   char unused[24];

};

struct AN_JointAngle
{
    ~AN_JointAngle(){
        tracks.deleteelements();
    }
    char                    bone[24];
    char                    axis;
    vvector<AN_Track*>     tracks;
};

struct AN_Sequence
{
    char            name[24];
    char            figure[24];
    int             frames;
    AN_JointAngle  jangle;
};


class AN8
{
public:
    ~AN8(){reset();}
	//reads the file into ram and prepares for parsing
    int         Load(const char*);
    void        Parse();
    void        ReadTexture();
    void        ReadObject();
    void        ReadFigure();
    void        ReadSequence();
    void        ReadSection(const string&);
    void        ReadMaterial(AN_Object*);
    void        ReadSurface(AN_Material*);
    void        ReadColorMat(AN_Colorp*);
    void        ReadtexParams(AN_TexParams* );
    void        ReadImage(AN_Object* );
    void        ReadBase(AN_Base* );
    void        ReadMesh(AN_Mesh* );
    void        ReadMaterialList(AN_Mesh* );
    void        ReadBone(AN_Bone*);
    void        ReadDof(AN_Bone* pb);
    void        ReadNameObject(AN_Bone* pb);
    void        ReadJointAngle(AN_Sequence* p);

	bool       openChunk(const string&);
	bool       openChunk(const string&, const string&);
	UINT       getInt();
	REAL       getFloat();
	string     getString();
	int        getIntConst();
    CLR        getRGB();
	REAL       getFloatConst();
	UV         getUV();
    SIZE       getSIZE();
	V3         getPoint();
	Quat       getQuat();
    
    void        eat2(const char c);
    void        skipsection();
    void        skip(int i=1);
	string      readIdent();                //reads an anim8or chunk identifier OR brackets to open/close chunks.
	inline char getChar();                  //forwards postion, returns next character (including whitespace)
	inline char getNonWhiteSpaceChar();     //forwards position and returns the next character that is not whitespace
	char        nextChar();                 //deos not forward position, checks ahead
    char        next2Char();
	char        nextNonWhiteSpaceChar();    //does not forward position, just checks ahead

    template <class CBack, class Param>  void EvFiring(CBack cB, Param* p)
    {
        int        count = 0;
        string     sname = readIdent();
        
        (this->*cB)(p,sname,count++);
        
        int        scope = _scope;
        while(1)
        {
            sname = readIdent();
            if(_scope!=scope)
                break;
            if((this->*cB)(p,sname,count++)==0)
                break;
        }

    }

private:
	void reset();
	void pushBlockStack();
    void eat(char&);

private:
    int                     _scope;
    string                  s_data;
    string::const_iterator _cpos;           //current parsing position


    map<string, Htex>       _texAssoc;
    vvector<AN_Object*>     _objects;
    vvector<AN_Sequence*>   _sequences;
};


////////////////////////////////////////////////////////////////////////////////
//Loader Functions--------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
#if 0
		string an8_filename;

		struct LoadedTextureInfo
		{
			ColorRGB24Texture2DNode *used;
			SurfaceRGB24 *img;
		};

		map<string, LoadedTextureInfo> loaded_textures;
		map<string, list<SceneNode*> > loaded_objects;
		map<string, list<SceneNode*> > loaded_figures;

		void clear()
		{
			//this little loop clears out loaded textures that weren't used
			//or that were used but copied
			for(map<string, LoadedTextureInfo>::iterator i = loaded_textures.begin(); i != loaded_textures.end();i++)
			{
				if(!i->second.used)
					delete i->second.img;
			}

			loaded_textures.clear();
			loaded_objects.clear();
			loaded_figures.clear();
		}

		//sets default options
		void setDefaults()
		{
			use_sphere_primitive = false;
			use_namenodes = false;
			combine_base_nodes = false;
			import_text = false;
			convert_textures = false;
		}

		//simple struct to keep track of material names
		struct Material
		{
			SceneNode *top; //top node in the material string
			SceneNode *bottom; //bottom node in the string
			string name;
		};

		//faces
		struct Face
		{
			UINT material;
			list<UINT> verticies;
			list<UINT> uvs;
		};


		//this class handles all the parsing of the text in an an8 and provides
		//a basic interface to navigate and retrieve s_data

		enum { DECAL = 0, DARKEN, LIGHTEN };
		inline UINT8 blendMode(UINT8 blend_mode, UINT8 base_color, UINT8 texture_color)
		{
			//This function does the blending operation between a texture component
			//and the base color component based on the blend mode.
			//darken "multiplies" the values, which usually makes it darker.
			if(blend_mode == DARKEN)
				return UINT8((REAL(base_color) / 255.0) * (REAL(texture_color) / 255.0) * 255.0);

			//lighten adds the values, and clamps it, which makes it lighter
			if(blend_mode == LIGHTEN)
				return min(255, base_color + texture_color);

			//decal simply replaces the base color with the texture color
			return texture_color;
		}

		inline UINT8 extractAlpha(ColorRGB24 &color)
		{ //extracts the alpha value from a color on a trans map
		 //anim8or apparently averages the tree components
			return UINT8( REAL(color.getRed() + color.getGreen() + color.getBlue()) / 3.0);
		}

		SurfaceRGB24 *loadTexture(AN8 &an8, const string &name)
		{

			map<string, LoadedTextureInfo>::iterator i = loaded_textures.find(name);
			if(i != loaded_textures.end())
				return i->second.img;

			SurfaceRGB24 *color = NULL;

			scene_debug << "Texture name is \"" << name << "\", searching for this texture" << endl;
			scene_debug.indent();

			if(name == "CHECKS")
			{ //checks is a generated texture, not a loaded texture

				color = new SurfaceRGB24(64, 64);
				color->set(ColorRGB24(255, 255, 255));

				//add in 4x4 chunks of color
				for(UINT8 x = 0; x < 64; x++)
					for(UINT8 y = 0; y < 64; y++)
						if((x%8<4) ? (y%8<4) : (y%8>3))color->setPixel(x, y, ColorRGB24(UINT8(REAL(y) / 64.0 * 256.0), UINT8(REAL(x) / 64.0 * 256.0), 0x00));

			} else
			{

				an8.push();
				an8.closeAll();

				if(an8.openChunk("texture", name))
				{
					scene_debug << "Texturename found" << endl;

					an8.getString(); //the name (we already have it), ignored

					bool invert = false;
					if(an8.openChunk("invert"))
					{
						invert = (bool)an8.getInt();
						an8.closeChunk();
						scene_debug << "Texture is inverted (mirrored vertically)" << endl;
					}

					if(an8.openChunk("file"))
					{
						string filename = an8.getString();
						an8.closeChunk();

						//check if path is relative or absolute
						bool relative_path = true;
						for(string::iterator i = filename.begin(); i != filename.end(); i++)
							if(*i == ':')relative_path = false;

						//now we need to add the path of the an8 file to the beggining
						if(relative_path)
						{
							bool past_slash = false;
							char temp[2] = { 0, 0 };
							for(string::reverse_iterator i = an8_filename.rbegin(); i != an8_filename.rend(); i++)
							{
								if((*i == '/') || (*i == '\\'))past_slash = true;
								if(past_slash)
								{
									temp[0] = *i;
									filename = temp + filename;
								}
							}
						}

						//we need forward slashes so we can be linux happy
						for(string::iterator i = filename.begin(); i != filename.end(); i++)if(*i == '\\')*i = '/';

						//attempt to load the texture
						if(isBitmap(filename))
						{
							ColorRGB24Texture2DNode *temp = importBMP(filename, !convert_textures);
							if(!temp)scene_debug << "Error loading \"" << filename << "\"" << endl;
							color = new SurfaceRGB24(*temp->get());
							delete temp;

						} else scene_debug << "Texture \"" << filename << "\" is not a recognized format" << endl;

						//invert if needed
						if(color && invert)color->flipY();

					} else scene_debug << "No file chunk found in this texture" << endl;

					an8.closeChunk();

				} else error_output << "Requested texture \"" << name << "\" not found" << endl;

				an8.pop();

			}

			if(!color)
			{ //load the "texture missing" texture...

				UINT8 size = 64;

				color = new Surface<ColorRGB24>(size, size);

				for(UINT8 x = 0; x < size; x++)
					for(UINT8 y = 0; y < size; y++)
						color->setPixel(x, y, ColorRGB24(UINT8(REAL(x) / REAL(size) * 256.0), UINT8_max - UINT8(REAL(y) / REAL(size) * 256.), 0));

			}

			LoadedTextureInfo temp;
			temp.img = color;
			temp.used = false;
			loaded_textures[name] = temp;

			scene_debug.unindent();
			return color;

		}

		SceneNode *loadBase(AN8 &an8)
		{ //note: may return NULL

			SceneNode *top = NULL;
			SceneNode *bottom = NULL;

			if(an8.openChunk("origin"))
			{
				V3 position = an8.getPoint();
				an8.closeChunk();

				if(position != V3::zero())
				{ //no need for identity case
					ModelviewTranslationNode * pos = new ModelviewTranslationNode;
					pos->set(position);

					top = pos;
					bottom = pos;

					scene_debug << "Position set to " << position.str() << endl;
				}
			}

			if(an8.openChunk("orientation"))
			{
				AN_Quat orientation = an8.getAN_Quat();
				an8.closeChunk();

				if(orientation != AN_Quat::identity())
				{ //no need for identity case

					ModelviewRotationAN_QuatNode * rot = new ModelviewRotationAN_QuatNode;
					rot->set(orientation);

					if(!top)top = rot;
					if(bottom)bottom->addChild(rot);
					bottom = rot;

					scene_debug << "Orientation set to " << orientation.str() << endl;
				}
			}

			if(an8.openChunk("scale"))
			{
				REAL scale = an8.getFloat();
				an8.closeChunk();

				if(scale != 1)
				{ //no need for identity case

					ModelviewScaleNode * scl = new ModelviewScaleNode;
					scl->set(scale);

					if(!top)top = scl;
					if(bottom)bottom->addChild(scl);
					bottom = scl;

					scene_debug << "Scale set to " << scale << endl;
				}
			}

			an8.closeChunk();
			return top;
		}

		SceneNode *defaultMaterial()
		{
			return new ColorRGB24Node(ColorRGB24(178, 178, 178));
		}

		Material loadMaterial(AN8 &an8)
		{
			Material result;
			result.name = an8.getString();

			scene_debug << "Loading material \"" << result.name << "\"" << endl;
			scene_debug.indent();

			UINT8 alpha_base = 255;
			SurfaceRGB24 *alpha_map = NULL;
			byte alpha_blend_mode = DECAL;

			ColorRGB24 diffuse_base(178, 178, 178);;
			SurfaceRGB24 *diffuse_map = NULL;
			byte diffuse_blend_mode = DECAL;

			if(an8.openChunk("surface"))
			{

				if(an8.openChunk("alpha"))
				{
					alpha_base = an8.getInt();
					an8.closeChunk();
					scene_debug << "Base alpha value is " << alpha_base << endl;
				}

				if(an8.openChunk("map"))
				{
					if(an8.openChunk("kind"))
					{
						string type = an8.getString();
						an8.closeChunk();

						if(type == "transparency")
						{
							scene_debug << "Surface has a transparancy map" << endl;
							scene_debug.indent();

							if(an8.openChunk("textureparams"))
							{
								if(an8.openChunk("blendmode"))
								{
									string blend_mode = an8.readIdent();
									an8.closeChunk();

									if(blend_mode == "decal")
									{
										alpha_blend_mode = DECAL;
										scene_debug << "Blend mode is decal" << endl;
									} else if (blend_mode == "darken")
									{
										alpha_blend_mode = DARKEN;
										scene_debug << "Blend mode is darken" << endl;
									} else if (blend_mode == "lighten")
									{
										alpha_blend_mode = LIGHTEN;
										scene_debug << "Blend mode is lighten" << endl;
									}
								}
								an8.closeChunk();
							}

							if(an8.openChunk("texturename"))
							{
								string name = an8.getString();
								an8.closeChunk();
								alpha_map = loadTexture(an8, name);
							}

							scene_debug.unindent();
						}

					}
					an8.closeChunk();
				}

				if(an8.openChunk("diffuse"))
				{
					REAL factor = .7;

					if(an8.openChunk("texturename"))
					{
						string name = an8.getString();
						an8.closeChunk();
						diffuse_map = loadTexture(an8, name);

					}

					if(an8.openChunk("factor"))
					{
						factor= an8.getFloat();
						an8.closeChunk();
						scene_debug << "Diffuse factor is " << factor << endl;

					}

					if(an8.openChunk("rgb"))
					{
						diffuse_base.setRed  (UINT(REAL(an8.getInt()) * factor));
						diffuse_base.setGreen(UINT(REAL(an8.getInt()) * factor));
						diffuse_base.setBlue (UINT(REAL(an8.getInt()) * factor));
						an8.closeChunk();
						scene_debug << "Diffuse base color is " << diffuse_base.str() << endl;
					}

					if(an8.openChunk("textureparams"))
					{
						if(an8.openChunk("blendmode"))
						{
							string blend_mode = an8.readIdent();
							an8.closeChunk();

							if(blend_mode == "decal")
							{
								diffuse_blend_mode = DECAL;
								scene_debug << "Diffuse blend mode is decal" << endl;
							} else if (blend_mode == "darken")
							{
								diffuse_blend_mode = DARKEN;
								scene_debug << "Diffuse blend mode is darken" << endl;
							} else if (blend_mode == "lighten")
							{
								diffuse_blend_mode = LIGHTEN;
								scene_debug << "Diffuse blend mode is lighten" << endl;
							}
						}
						an8.closeChunk();
					}

					an8.closeChunk();
				}

				an8.closeChunk();
			}

			an8.closeChunk();

			//here we attempt to produce the colornode
			//which best matches the anim8or s_data.

			if(!diffuse_map)
			{
				if(!alpha_map)
				{
					if(alpha_base == 255)
					{ //here we create a simple color with no alpha
						result.top = new ColorRGB24Node(diffuse_base);
						result.bottom = result.top;
					} else
					{ //the we create a color with alpha
						result.top = new ColorRGBA32Node(ColorRGBA32(diffuse_base, alpha_base));
						result.bottom = result.top;
					}
				} else
				{ //here we create add uniform color to the alpha map
					SurfaceRGBA32 *img = new SurfaceRGBA32(alpha_map->getWidth(), alpha_map->getHeight());

					ColorRGBA32 *pixel = img->getPixels();
					ColorRGB24 *opixel = alpha_map->getPixels();

					for(UINT m = 0; m < img->numPixels(); m++, pixel++, opixel++)
					{
						pixel->setRed  (diffuse_base.getRed  ());
						pixel->setGreen(diffuse_base.getGreen());
						pixel->setBlue (diffuse_base.getBlue ());
						pixel->setAlpha(blendMode(alpha_blend_mode, alpha_base, extractAlpha(*opixel)));
					}

					result.top = new ColorRGBA32Texture2DNode(img);
					result.bottom = result.top;
				}
			}else
			{ //here we create a color map

				if((!alpha_map) && (diffuse_blend_mode == DECAL) && (alpha_base == 255))
				{//if we have no alpha and the blend mode is decal we can simply use the color map as is

						//search for the loaded texture
						for(map<string, LoadedTextureInfo>::iterator i = loaded_textures.begin();;i++)
						{
							if(i->second.img == diffuse_map)
							{
								if(i->second.used)
									result.top = new ImitationNode(i->second.used);
								else
									result.top = i->second.used = new ColorRGB24Texture2DNode(diffuse_map);

								result.bottom = result.top;

								break;
							}
						}

				} else
				{//we have to do some editing (to put alpha in and/or to do the diffuse blending)

					SurfaceRGB24 *img = new SurfaceRGB24(diffuse_map->getWidth(), diffuse_map->getHeight());

					ColorRGB24 *pixel = img->getPixels();
					ColorRGB24 *opixel = diffuse_map->getPixels();

					for(UINT m = 0; m < img->numPixels(); m++, pixel++, opixel++)
					{
						pixel->setRed  (blendMode(diffuse_blend_mode, diffuse_base.getRed  (), opixel->getRed  ()));
						pixel->setGreen(blendMode(diffuse_blend_mode, diffuse_base.getGreen(), opixel->getGreen()));
						pixel->setBlue (blendMode(diffuse_blend_mode, diffuse_base.getBlue (), opixel->getBlue ()));
					}

					if(!alpha_map)
					{
						if(alpha_base == 255)
						{ //here we use the color map
							result.top = new ColorRGB24Texture2DNode(img);
							result.bottom = result.top;
						} else
						{ //here we add uniform alpha to the color map
							SurfaceRGBA32 *img2 = new SurfaceRGBA32(*img, alpha_base);
							delete img;
							result.top = new ColorRGBA32Texture2DNode(img2);
							result.bottom = result.top;
						}
					} else
					{ //here we combine the color map and the alpha map

						//they must be the same size

						if(img->getWidth() < alpha_map->getWidth())img->scaleX(alpha_map->getWidth());
						if(alpha_map->getWidth() < img->getWidth())alpha_map->scaleX(img->getWidth());
						if(img->getHeight() < alpha_map->getHeight())img->scaleY(alpha_map->getHeight());
						if(alpha_map->getHeight() < img->getHeight())alpha_map->scaleY(img->getHeight());

						SurfaceRGBA32 *combined_img = new SurfaceRGBA32(img->getWidth(), img->getHeight());

						ColorRGB24 *img_pixel = img->getPixels();
						ColorRGB24 *alpha_pixel = alpha_map->getPixels();
						ColorRGBA32 *pixel = combined_img->getPixels();

						for(UINT m = 0; m < img->numPixels(); m++, pixel++, img_pixel++, alpha_pixel++)
						{
							pixel->setRed  (img_pixel->getRed  ());
							pixel->setGreen(img_pixel->getGreen());
							pixel->setBlue (img_pixel->getBlue ());
							pixel->setAlpha(blendMode(alpha_blend_mode, alpha_base, extractAlpha(*alpha_pixel)));
						}

						delete img;

						result.top = new ColorRGBA32Texture2DNode(combined_img);
						result.bottom = result.top;
					}

				}

			}

			scene_debug.unindent();
			return result;
		}

		Material findMaterial(AN8 &an8, const string &material_name, list<Material> &object_list)
		{
			scene_debug << "Searcing for material \"" << material_name << "\"" << endl;
			scene_debug.indent();

			for(list<Material>::const_iterator i = object_list.begin(); i != object_list.end(); i++)
				if(i->name == material_name)
				{
					scene_debug << "Found in object material list" << endl;
					scene_debug.unindent();
					return *i;
				}

			scene_debug << "Not found in object material list, searching global material list" << endl;

			an8.push();
			an8.closeAll();
			if(an8.openChunk("material", material_name))
			{
				Material mat;
				mat = loadMaterial(an8);
				an8.pop();

				object_list.push_back(mat);

				scene_debug << "Found in global material list" << endl;
				scene_debug.unindent();
				return mat;
			}
			an8.pop();

			scene_debug << "Not found in global material list, using default" << endl;

			for(list<Material>::const_iterator i = object_list.begin(); i != object_list.end(); i++)
				if(i->name == " -- default --")
				{
					scene_debug.unindent();
					return *i;
				}

			Material mat;
			mat.top = defaultMaterial();
			mat.bottom = mat.top->findLeaf();
			mat.name = " -- default --";
			object_list.push_back(mat);

			scene_debug.unindent();
			return mat;

		}

		void loadGroup(AN8 &an8, list<Material> &material_list, byte type)
		{ //if material_list is empty it is assume to be an object, otherwise a group within an object

			//type = 0 means this is an object
			//type = 1 means this is a figure
			//type = 2 means this is a group within an object/figure
			//type = 3 means this is a bone within a figure

			string name;
			if(type == 0)
			{
                name = an8.getString();
				scene_debug << "Loading object \"" << name << "\"" << endl;
			}else if(type == 1)
			{
				name = an8.getString();
				scene_debug << "Loading figure \"" << name << "\"" << endl;
			}else if(type == 2)
			{
				scene_debug << "Loading group" << endl;
			}else if(type == 3)
			{
				name = an8.getString();
				scene_debug << "Loading bone \"" << name << "\"" << endl;
			} else
			{
				error_output << "Internal Error: Invalid type passed to Phobic::<unnamed>::loadGroup()" << endl;
				return;
			}


			scene_debug.indent();

			//Materials---------------------------------------------------------
			//object/figures only, not for groups or bones

            if(type < 2)
			{
				while(an8.openChunk("material"))
					material_list.push_back(loadMaterial(an8));
			}

			//Base-------------------------------------------------------------
			//groups only, not for objects, figures, or bones

			static SceneNode *base_accumulate = NULL; //a stack of transforms for each nested group/bone/whatever we are in

			SceneNode *base = NULL; //the base for this group only (gets put in the base_accumulate stack)

			//base_accumulate gets copied for each mesh/sphere/etc that uses the coordinate
			//system shift that base_accumulate. This is so that 2 different meshes/spheres
			//with different materials can have the same base. This could be done a little
			//better, for things that are under the same material, you can stick them under
			//the same base. But I'm too lazy to do that now.

			if(type == 2)
			{
				if(an8.openChunk("base"))
				{
					base = loadBase(an8);
					if(base_accumulate)base_accumulate->findLeaf()->addChild(base);
					else base_accumulate = base;
				}
			}

			//Bone orientation-------------------------------------------
			//bones only (bonebase is also used later)

			SceneNode *bonebase = NULL;

			if((type == 3))
			{

				if(an8.openChunk("orientation"))
				{
					AN_Quat orientation = an8.getAN_Quat();
					an8.closeChunk();
					scene_debug << "Orientation is " << orientation.str() << endl;

					if(orientation != AN_Quat::identity())
					{
						ModelviewRotationAN_QuatNode *q = new ModelviewRotationAN_QuatNode;
						q->set(orientation);
						if(!bonebase)bonebase = q;
						else bonebase->findLeaf()->addChild(q);
					}
				}

				if(bonebase)
				{
					if(base_accumulate)base_accumulate->findLeaf()->addChild(bonebase);
					else base_accumulate = bonebase;
				}

			}

			//Meshes------------------------------------------------------------
			//groups, objects, bones - not figures

			if(type != 1)
			{

				while(an8.openChunk("mesh"))
				{
					scene_debug << "Loading Mesh" << endl;
					scene_debug.indent();

					//MATERIALS
					vector<Material> materiallist; //this holds copys of whats in material_list
					if(an8.openChunk("materiallist"))
					{
						scene_debug << "Reading material list" << endl;
						scene_debug.indent();

						while(an8.openChunk("materialname"))
						{
							string name = an8.getString();
							an8.closeChunk();

							materiallist.push_back(findMaterial(an8, name, material_list));

						}

						scene_debug.unindent();
						an8.closeChunk();

					}

					//LOCAL BASE
					//now for the sphere base
					SceneNode *lbase = NULL;

					if(an8.openChunk("base"))
					{
						scene_debug << "Loading mesh base" << endl;
						scene_debug.indent();

						lbase = loadBase(an8);

						if(lbase)
						{
							if(base_accumulate)base_accumulate->findLeaf()->addChild(lbase);
							else base_accumulate = lbase;
						}

						scene_debug.unindent();
					}

					//POINTS
					vector<V3> vertex_list;
	                if(an8.openChunk("points"))
					{
						scene_debug << "Loading points" << endl;

						for(;;)
						{
							if(an8.nextNonWhiteSpaceChar() == '}')break;
							vertex_list.push_back(an8.getPoint());
							//printf("point %s\n", point.str().c_str());
						}
						an8.closeChunk();
					}

					//UVS
					vector<UV> uv_list;
	                if(an8.openChunk("texcoords"))
					{
						scene_debug << "Loading texcoords" << endl;
						for(;;)
						{
							if(an8.nextNonWhiteSpaceChar() == '}')break;
							uv_list.push_back(an8.getUV() * UV(1, -1));
							//printf("uv %s\n", uv.str().c_str());
						}
						an8.closeChunk();
					}

					//FACES
					list<Face> face_list;

					Face face;
					UINT flags, num_verts;
					bool has_normals, has_uvs;
					char current;
					if(an8.openChunk("faces"))
					{
						scene_debug << "Loading faces" << endl;
						for(;;)
						{
							if(an8.nextNonWhiteSpaceChar() == '}')break;

							num_verts = an8.getInt();
							flags = an8.getInt();
							face.material = an8.getInt();
							face.verticies.clear();
							face.uvs.clear();
							an8.getInt(); //something to do with normals, who knows

							has_normals = getBit(flags, 1);
							has_uvs     = getBit(flags, 2);

							do{ current = an8.getChar(); }while(current != '(');
							for(UINT x = 0; x < num_verts; x++)
							{
								do{ current = an8.getChar(); }while(current != '(');

								face.verticies.push_front(an8.getInt());
								if(has_normals)an8.getInt();
								if(has_uvs)face.uvs.push_front(an8.getInt());

								do{ current = an8.getChar(); }while(current != ')');
							}
							do{ current = an8.getChar(); }while(current != ')');


							face_list.push_back(face);

						}

						an8.closeChunk();

					}

					scene_debug << "Generating meshes" << endl;
					scene_debug.indent();

					//MATERIAL LOOP - here we build a seperate mesh for each material (since meshes cant have multiple materials)
					vector<V3> local_vertex_list;
					vector<UV> local_uv_list;
					list<Face> local_face_list;
					vector<UINT> vertex_index_conversion_list;
					vector<UINT> uv_index_conversion_list;
					Face temp_face;

					for(UINT material_index = 0; material_index < materiallist.size(); material_index++)
					{
						SceneNode *bottom = materiallist[material_index].bottom;

						//now attach the group base if available
						if(base_accumulate)
						{
							scene_debug << "Adding group base" << endl;
							scene_debug.indent();

							SceneNode *temp;
							temp = UtilParser::duplicateTree(base_accumulate);

							//attachment
							bottom->addChild(temp);
							bottom = temp->findLeaf();

							scene_debug.unindent();
						}

						local_vertex_list.clear();
						local_uv_list.clear();
						local_face_list.clear();
						vertex_index_conversion_list.clear();
						uv_index_conversion_list.clear();

	                    for(UINT x = 0; x < vertex_list.size(); x++)vertex_index_conversion_list.push_back(vertex_list.size());
	                    for(UINT x = 0; x < uv_list.size(); x++)uv_index_conversion_list.push_back(uv_list.size());

						for(list<Face>::const_iterator face = face_list.begin(); face != face_list.end(); face++)
						{
							if(face->material == material_index)
							{
								temp_face.verticies.clear();
								temp_face.uvs.clear();

								for(list<UINT>::const_iterator i = face->verticies.begin(); i != face->verticies.end(); i++)
								{
									if(vertex_index_conversion_list[*i] == vertex_list.size())
									{ //this vertex has not been used in the submesh

										temp_face.verticies.push_back(local_vertex_list.size());
										vertex_index_conversion_list[*i] = local_vertex_list.size();

										//add it to the local vertex list
										local_vertex_list.push_back(vertex_list[*i]);

									} else
									{ //this vertex has been used

										temp_face.verticies.push_back(vertex_index_conversion_list[*i]);

									}
								}

								for(list<UINT>::const_iterator i = face->uvs.begin(); i != face->uvs.end(); i++)
								{
									if(uv_index_conversion_list[*i] == uv_list.size())
									{ //this uv has not been used in the submesh

										temp_face.uvs.push_back(local_uv_list.size());
										uv_index_conversion_list[*i] = local_uv_list.size();

										//add it to the local uv list
										local_uv_list.push_back(uv_list[*i]);

									} else
									{ //this uv has been used

										temp_face.uvs.push_back(uv_index_conversion_list[*i]);

									}
								}

								local_face_list.push_back(temp_face);
							}
						}

						//Create the mesh
						if(!local_face_list.empty())
						{

							if(local_uv_list.empty())
							{
								MeshRigidNGon3DNode *sub_mesh = new MeshRigidNGon3DNode;

								sub_mesh->getMesh().allocateVerticies(local_vertex_list.size());
								for(UINT counter = 0; counter < local_vertex_list.size(); counter++)
									sub_mesh->getMesh().setVertex(counter, local_vertex_list[counter]);

								sub_mesh->getMesh().allocateSides(local_face_list.size());
								UINT counter = 0;
								for(list<Face>::const_iterator i = local_face_list.begin(); i != local_face_list.end(); i++, counter++)
									sub_mesh->getMesh().setSide(counter, i->verticies);

								bottom->addChild(sub_mesh);
							} else
							{
								MeshRigidNGon3DTextured2DNode *sub_mesh = new MeshRigidNGon3DTextured2DNode;

								sub_mesh->getMesh().allocateVerticies(local_vertex_list.size());
								for(UINT counter = 0; counter < local_vertex_list.size(); counter++)
									sub_mesh->getMesh().setVertex(counter, local_vertex_list[counter]);

								sub_mesh->getMesh().allocateTextureCoordinates(local_uv_list.size());
								for(UINT counter = 0; counter < local_uv_list.size(); counter++)
									sub_mesh->getMesh().setTextureCoordinate(counter, local_uv_list[counter]);

								sub_mesh->getMesh().allocateSides(local_face_list.size());
								UINT counter = 0;
								for(list<Face>::iterator i = local_face_list.begin(); i != local_face_list.end(); i++, counter++)
								{

									while(i->uvs.size() < i->verticies.size())
									{ //if this face has no uvs

										//FIXME: Check what ani8or does for textured faces for no uvs.
										i->uvs.push_back(0);
									}

									sub_mesh->getMesh().setSide(counter, i->verticies, i->uvs);

								}
								bottom->addChild(sub_mesh);
							}
						}
					}

					scene_debug.unindent();

					if(base_accumulate == lbase)base_accumulate = NULL;
					if(lbase)delete lbase;

					an8.closeChunk();
					scene_debug.unindent();
				}

			}

			//Rectangular Solids------------------------------------------------
			//groups, objects, bones - not figures

			//they are listed as "cubes" in anim8or files but do not neccesarily have the
			//same length along each axis

			if(type != 1)
			{
				while(an8.openChunk("cube"))
				{
					scene_debug << "Loading rectangular solid" << endl;
					scene_debug.indent();

					SceneNode *bottom;

					string material_name(" -- default --");
					if(an8.openChunk("material"))
					{
						material_name = an8.getString();
						an8.closeChunk();

						scene_debug << "Material is \"" << material_name << "\"" << endl;
					} else scene_debug << "Material not present, using default" << endl;
					scene_debug.indent();
					bottom = findMaterial(an8, material_name, material_list).bottom;
					scene_debug.unindent();

					if(base_accumulate)
					{
						SceneNode * temp = UtilParser::duplicateTree(base_accumulate);
						bottom->addChild(temp);
						bottom = temp->findLeaf();
					}

					if(an8.openChunk("base"))
					{
						SceneNode * temp = loadBase(an8);
						bottom->addChild(temp);
						bottom = temp->findLeaf();
					}

					REAL x = 10, y = 10, z = 10; //length along each axis of the rectangular solid
					UINT x_divisions = 1, y_divisions = 1, z_divisions = 1; //number of faces along each axis

					if(an8.openChunk("scale"))
					{
						x = an8.getFloatConst();
						y = an8.getFloatConst();
						z = an8.getFloatConst();
						an8.closeChunk();

						scene_debug << "Size is " << x << "x " << y << "y " << z << endl;
					}else scene_debug << "No size detected, using defaults" << endl;

					if(an8.openChunk("divisions"))
					{
						x_divisions = an8.getInt();
						y_divisions = an8.getInt();
						z_divisions = an8.getInt();
						an8.closeChunk();

						if(x_divisions == 0)x_divisions = 1;
						if(y_divisions == 0)y_divisions = 1;
						if(z_divisions == 0)z_divisions = 1;

						scene_debug << "Number of divisions are " << x_divisions << "x " << y_divisions << "y " << z_divisions << "z" << endl;
					} else scene_debug << "No divisions chunk detected, using defaults" << endl;

					//Now generate the mesh

					MeshRigidNGon3DTextured2DNode *mesh = new MeshRigidNGon3DTextured2DNode;

					//POINTS
					UINT counter = 0;
					mesh->getMesh().allocateVerticies((((x_divisions + 1) * 2) + ((y_divisions - 1) * 2)) * (z_divisions + 1) + ((x_divisions - 1) * (y_divisions - 1) * 2));

					//cross sections along z axis from back to front
					for(UINT zi = 0; zi <= z_divisions; zi++)
					{
						REAL zpos = (REAL(zi) / REAL(z_divisions) -.5f) * z;

						//top (left to right)
						for(UINT xi = 0; xi <= x_divisions; xi++)
							mesh->getMesh().setVertex(counter++, V3((REAL(xi) / REAL(x_divisions) - .5f) * x, .5f * y, zpos));

						//right (top to bottom)
						for(UINT yi = y_divisions; yi >= 1; yi--)
							mesh->getMesh().setVertex(counter++, V3(.5f * x, (REAL(yi-1) / REAL(y_divisions) - .5f) * y, zpos));

						//bottom (right to left)
						for(UINT xi = x_divisions; xi >= 1; xi--)
							mesh->getMesh().setVertex(counter++, V3((REAL(xi-1) / REAL(x_divisions) - .5f) * x, -.5f * y, zpos));

						//left (bottom to top)
						for(UINT yi = 1; yi < y_divisions; yi++)
							mesh->getMesh().setVertex(counter++, V3(-.5f * x, (REAL(yi) / REAL(y_divisions) - .5f) * y, zpos));
					}

					UINT back_points_offset = counter;

					//back
					for(UINT yi = 0; yi < y_divisions - 1; yi++)
						for(UINT xi = 0; xi < x_divisions - 1; xi++)
							mesh->getMesh().setVertex(counter++, V3((REAL(xi + 1) / REAL(x_divisions) - .5f) * x, (REAL(yi + 1) / REAL(y_divisions) - .5f) * y, -.5f * z));

					UINT front_points_offset = counter;

					//front
					for(UINT yi = 0; yi < (y_divisions - 1); yi++)
						for(UINT xi = 0; xi < (x_divisions - 1); xi++)
							mesh->getMesh().setVertex(counter++, V3((REAL(xi + 1) / REAL(x_divisions) - .5f) * x, (REAL(yi + 1) / REAL(y_divisions) - .5f) * y, .5f * z));

					//UVS
					counter = 0;
					mesh->getMesh().allocateTextureCoordinates((x_divisions + 1) * (y_divisions + 1));
					for(UINT yi = 0; yi <= y_divisions; yi++)
						for(UINT xi = 0; xi <= x_divisions; xi++)
							mesh->getMesh().setTextureCoordinate(counter++, UV(REAL(xi) / REAL(x_divisions), -REAL(yi) / REAL(y_divisions)));


					//FACES
					counter = 0;
					UINT polys_per_z_division = (y_divisions * 2) + (x_divisions * 2);
					mesh->getMesh().allocateSides(polys_per_z_division * z_divisions + (x_divisions * y_divisions * 2));

					UINT indicies[4];
					UINT uvindicies[4];

					//z cross sections
					UINT helper;
					UINT side2;
					for(UINT zi = 0; zi < z_divisions; zi++)
					{
						helper = 0;
						side2 = 0;
						for(UINT side = 0; side < polys_per_z_division; side++, side2++)
						{
							indicies[0] = ((side + 1) % polys_per_z_division) + ((zi + 0) * polys_per_z_division);
							indicies[1] = ((side + 0) % polys_per_z_division) + ((zi + 0) * polys_per_z_division);
							indicies[2] = ((side + 0) % polys_per_z_division) + ((zi + 1) * polys_per_z_division);
							indicies[3] = ((side + 1) % polys_per_z_division) + ((zi + 1) * polys_per_z_division);

							if((side == x_divisions) || (side == x_divisions + y_divisions) || (side == x_divisions + y_divisions + x_divisions))
							{
								helper++;
								side2 = 0;
							}

							if(helper == 0)
							{
								uvindicies[0] = side2 + 1;
								uvindicies[1] = side2 + 0;
								uvindicies[2] = side2 + 0;
								uvindicies[3] = side2 + 1;
							}

							if(helper == 1)
							{
								uvindicies[0] = x_divisions + ((y_divisions - (side2 + 1)) * (x_divisions + 1));
								uvindicies[1] = x_divisions + ((y_divisions - (side2 + 0)) * (x_divisions + 1));
								uvindicies[2] = x_divisions + ((y_divisions - (side2 + 0)) * (x_divisions + 1));
								uvindicies[3] = x_divisions + ((y_divisions - (side2 + 1)) * (x_divisions + 1));
							}

							if(helper == 2)
							{
								uvindicies[0] = x_divisions - (side2 + 1) + (y_divisions * (x_divisions + 1));
								uvindicies[1] = x_divisions - (side2 + 0) + (y_divisions * (x_divisions + 1));
								uvindicies[2] = x_divisions - (side2 + 0) + (y_divisions * (x_divisions + 1));
								uvindicies[3] = x_divisions - (side2 + 1) + (y_divisions * (x_divisions + 1));
							}

							if(helper == 3)
							{
								uvindicies[0] = ((side2 + 1) * (x_divisions + 1));
								uvindicies[1] = ((side2 + 0) * (x_divisions + 1));
								uvindicies[2] = ((side2 + 0) * (x_divisions + 1));
								uvindicies[3] = ((side2 + 1) * (x_divisions + 1));
							}

							mesh->getMesh().setSide(counter++, 4, indicies, uvindicies);
						}
					}

					//back
					{
						//middle sections
						for(UINT yi = 0; yi < ((y_divisions > 1) ? y_divisions - 2 : 0); yi++)
						{
							for(UINT xi = 0; xi < ((x_divisions > 1) ? x_divisions - 2 : 0); xi++)
							{
								indicies[3] = back_points_offset + (xi + 0) + ((yi + 0) * (x_divisions - 1));
								indicies[2] = back_points_offset + (xi + 1) + ((yi + 0) * (x_divisions - 1));
								indicies[1] = back_points_offset + (xi + 1) + ((yi + 1) * (x_divisions - 1));
								indicies[0] = back_points_offset + (xi + 0) + ((yi + 1) * (x_divisions - 1));

								uvindicies[3] = (xi + 1) + ((yi + 1) * (x_divisions + 1));
								uvindicies[2] = (xi + 2) + ((yi + 1) * (x_divisions + 1));
								uvindicies[1] = (xi + 2) + ((yi + 2) * (x_divisions + 1));
								uvindicies[0] = (xi + 1) + ((yi + 2) * (x_divisions + 1));

								mesh->getMesh().setSide(counter++, 4, indicies, uvindicies);
							}
						}

						if((x_divisions > 1) && (y_divisions > 1))
						{

							//top middle polygons
							for(UINT xi = 0; xi < x_divisions - 2; xi++)
							{UINT yi = y_divisions - 2;
								indicies[0] = (xi + 1);
								indicies[1] = (xi + 2);
								indicies[2] = back_points_offset + (xi + 1) + ((yi + 0) * (x_divisions - 1));
								indicies[3] = back_points_offset + (xi + 0) + ((yi + 0) * (x_divisions - 1));

								uvindicies[3] = (xi + 1) + ((yi + 1) * (x_divisions + 1));
								uvindicies[2] = (xi + 2) + ((yi + 1) * (x_divisions + 1));
								uvindicies[1] = (xi + 2) + ((yi + 2) * (x_divisions + 1));
								uvindicies[0] = (xi + 1) + ((yi + 2) * (x_divisions + 1));

								mesh->getMesh().setSide(counter++, 4, indicies, uvindicies);
							}

							//right middle polygons
							for(UINT yi = 0; yi < y_divisions - 2; yi++)
							{UINT xi = x_divisions - 2;
								indicies[0] = x_divisions + 1 + (yi + 0);
								indicies[1] = x_divisions + 1 + (yi + 1);
								indicies[2] = back_points_offset + (xi + 0) + (((y_divisions - 2) - (yi + 1)) * (x_divisions - 1));
								indicies[3] = back_points_offset + (xi + 0) + (((y_divisions - 2) - (yi + 0)) * (x_divisions - 1));

								uvindicies[0] = (x_divisions - 0) + ((y_divisions - (yi + 1)) * (x_divisions + 1));
								uvindicies[1] = (x_divisions - 0) + ((y_divisions - (yi + 2)) * (x_divisions + 1));
								uvindicies[2] = (x_divisions - 1) + ((y_divisions - (yi + 2)) * (x_divisions + 1));
								uvindicies[3] = (x_divisions - 1) + ((y_divisions - (yi + 1)) * (x_divisions + 1));

								mesh->getMesh().setSide(counter++, 4, indicies, uvindicies);
							}

							//bottom middle polygons
							for(UINT xi = 0; xi < x_divisions - 2; xi++)
							{UINT yi = 0;
								indicies[0] = x_divisions + y_divisions + 1 + (xi + 0);
								indicies[1] = x_divisions + y_divisions + 1 + (xi + 1);
								indicies[2] = back_points_offset + ((x_divisions - 2) - (xi + 1)) + ((yi + 0) * (x_divisions - 1));
								indicies[3] = back_points_offset + ((x_divisions - 2) - (xi + 0)) + ((yi + 0) * (x_divisions - 1));

								uvindicies[3] = (x_divisions - (xi + 1)) + ((yi + 1) * (x_divisions + 1));
								uvindicies[2] = (x_divisions - (xi + 2)) + ((yi + 1) * (x_divisions + 1));
								uvindicies[1] = (x_divisions - (xi + 2)) + ((yi + 0) * (x_divisions + 1));
								uvindicies[0] = (x_divisions - (xi + 1)) + ((yi + 0) * (x_divisions + 1));

								mesh->getMesh().setSide(counter++, 4, indicies, uvindicies);
							}

							//left middle polygons
							for(UINT yi = 0; yi < y_divisions - 2; yi++)
							{UINT xi = 0;
								indicies[0] = x_divisions + y_divisions + x_divisions + 1 + (yi + 0);
								indicies[1] = x_divisions + y_divisions + x_divisions + 1 + (yi + 1);
								indicies[2] = back_points_offset + (xi + 0) + ((yi + 1) * (x_divisions - 1));
								indicies[3] = back_points_offset + (xi + 0) + ((yi + 0) * (x_divisions - 1));

								uvindicies[0] = (0) + ((yi + 1) * (x_divisions + 1));
								uvindicies[1] = (0) + ((yi + 2) * (x_divisions + 1));
								uvindicies[2] = (1) + ((yi + 2) * (x_divisions + 1));
								uvindicies[3] = (1) + ((yi + 1) * (x_divisions + 1));

								mesh->getMesh().setSide(counter++, 4, indicies, uvindicies);
							}

						}

						//coners
						if((y_divisions > 1) && (x_divisions > 1))
						{ //4 corners

							//top left
							indicies[0] = 0;
							indicies[1] = 1;
							indicies[2] = back_points_offset + (0) + ((y_divisions - 2) * (x_divisions - 1));
							indicies[3] = polys_per_z_division - 1;

							uvindicies[3] = 0 + ((y_divisions - 1) * (x_divisions + 1));
							uvindicies[2] = 1 + ((y_divisions - 1) * (x_divisions + 1));
							uvindicies[1] = 1 + ((y_divisions - 0) * (x_divisions + 1));
							uvindicies[0] = 0 + ((y_divisions - 0) * (x_divisions + 1));

							mesh->getMesh().setSide(counter++, 4, indicies, uvindicies);

							//top right
							indicies[0] = x_divisions - 1;
							indicies[1] = x_divisions;
							indicies[2] = x_divisions + 1;
							indicies[3] = back_points_offset + (x_divisions - 2) + ((y_divisions - 2) * (x_divisions - 1));

							uvindicies[3] = (x_divisions - 1) + ((y_divisions - 1) * (x_divisions + 1));
							uvindicies[2] = (x_divisions - 0) + ((y_divisions - 1) * (x_divisions + 1));
							uvindicies[1] = (x_divisions - 0) + ((y_divisions - 0) * (x_divisions + 1));
							uvindicies[0] = (x_divisions - 1) + ((y_divisions - 0) * (x_divisions + 1));

							mesh->getMesh().setSide(counter++, 4, indicies, uvindicies);

							//bottom right
							indicies[0] = x_divisions + y_divisions - 1;
							indicies[1] = x_divisions + y_divisions;
							indicies[2] = x_divisions + y_divisions + 1;
							indicies[3] = back_points_offset + (x_divisions - 2) + (0 * (x_divisions - 1));

							uvindicies[3] = (x_divisions - 1) + ((1) * (x_divisions + 1));
							uvindicies[2] = (x_divisions - 1) + ((0) * (x_divisions + 1));
							uvindicies[1] = (x_divisions - 0) + ((0) * (x_divisions + 1));
							uvindicies[0] = (x_divisions - 0) + ((1) * (x_divisions + 1));

							mesh->getMesh().setSide(counter++, 4, indicies, uvindicies);

							//bottom left
							indicies[0] = x_divisions + y_divisions + x_divisions - 1;
							indicies[1] = x_divisions + y_divisions + x_divisions;
							indicies[2] = x_divisions + y_divisions + x_divisions + 1;
							indicies[3] = back_points_offset + (0) + (0 * (x_divisions - 1));

							uvindicies[3] = (1) + ((1) * (x_divisions + 1));
							uvindicies[2] = (0) + ((1) * (x_divisions + 1));
							uvindicies[1] = (0) + ((0) * (x_divisions + 1));
							uvindicies[0] = (1) + ((0) * (x_divisions + 1));

							mesh->getMesh().setSide(counter++, 4, indicies, uvindicies);

						}

						if((x_divisions > 1) && (y_divisions == 1))
						{
							for(UINT xi = 0; xi < x_divisions; xi++)
							{

								indicies[0] = xi;
								indicies[1] = xi + 1;
								indicies[2] = polys_per_z_division - xi - 2;
								indicies[3] = polys_per_z_division - xi - 1;

								uvindicies[0] = xi + 0 + x_divisions + 1;
								uvindicies[1] = xi + 1 + x_divisions + 1;
								uvindicies[2] = xi + 1;
								uvindicies[3] = xi + 0;

								mesh->getMesh().setSide(counter++, 4, indicies, uvindicies);

							}
						}

						if((x_divisions == 1) && (y_divisions > 1))
						{
							for(UINT yi = 0; yi < y_divisions; yi++)
							{

								indicies[0] = (yi == 0) ? 0 : (polys_per_z_division - yi);
								indicies[1] = yi + 1;
								indicies[2] = yi + 2;
								indicies[3] = polys_per_z_division - yi - 1;

								uvindicies[0] = (y_divisions - yi - 1) * 2 + 2;
								uvindicies[1] = (y_divisions - yi - 1) * 2 + 3;
								uvindicies[2] = (y_divisions - yi - 1) * 2 + 1;
								uvindicies[3] = (y_divisions - yi - 1) * 2 + 0;

								mesh->getMesh().setSide(counter++, 4, indicies, uvindicies);

							}
						}

						if((x_divisions == 1) && (y_divisions == 1))
						{
							indicies[0] = 0;
							indicies[1] = 1;
							indicies[2] = 2;
							indicies[3] = 3;

							uvindicies[0] = 2;
							uvindicies[1] = 3;
							uvindicies[2] = 1;
							uvindicies[3] = 0;

							mesh->getMesh().setSide(counter++, 4, indicies, uvindicies);
						}

					}

					//front
					{

						UINT front_side_offset = polys_per_z_division * z_divisions;

						//middle sections
						for(UINT yi = 0; yi < ((y_divisions > 1) ? y_divisions - 2 : 0); yi++)
						{
							for(UINT xi = 0; xi < ((x_divisions > 1) ? x_divisions - 2 : 0); xi++)
							{
								indicies[0] = front_points_offset + (xi + 0) + ((yi + 0) * (x_divisions - 1));
								indicies[1] = front_points_offset + (xi + 1) + ((yi + 0) * (x_divisions - 1));
								indicies[2] = front_points_offset + (xi + 1) + ((yi + 1) * (x_divisions - 1));
								indicies[3] = front_points_offset + (xi + 0) + ((yi + 1) * (x_divisions - 1));

								uvindicies[0] = (xi + 1) + ((yi + 1) * (x_divisions + 1));
								uvindicies[1] = (xi + 2) + ((yi + 1) * (x_divisions + 1));
								uvindicies[2] = (xi + 2) + ((yi + 2) * (x_divisions + 1));
								uvindicies[3] = (xi + 1) + ((yi + 2) * (x_divisions + 1));

								mesh->getMesh().setSide(counter++, 4, indicies, uvindicies);
							}
						}

						if((x_divisions > 1) && (y_divisions > 1))
						{

							//top middle polygons
							for(UINT xi = 0; xi < x_divisions - 2; xi++)
							{UINT yi = y_divisions - 2;
								indicies[3] = front_side_offset + (xi + 1);
								indicies[2] = front_side_offset + (xi + 2);
								indicies[1] = front_points_offset + (xi + 1) + ((yi + 0) * (x_divisions - 1));
								indicies[0] = front_points_offset + (xi + 0) + ((yi + 0) * (x_divisions - 1));

								uvindicies[0] = (xi + 1) + ((yi + 1) * (x_divisions + 1));
								uvindicies[1] = (xi + 2) + ((yi + 1) * (x_divisions + 1));
								uvindicies[2] = (xi + 2) + ((yi + 2) * (x_divisions + 1));
								uvindicies[3] = (xi + 1) + ((yi + 2) * (x_divisions + 1));

								mesh->getMesh().setSide(counter++, 4, indicies, uvindicies);
							}

							//right middle polygons
							for(UINT yi = 0; yi < y_divisions - 2; yi++)
							{UINT xi = x_divisions - 2;
								indicies[3] = front_side_offset + x_divisions + 1 + (yi + 0);
								indicies[2] = front_side_offset + x_divisions + 1 + (yi + 1);
								indicies[1] = front_points_offset + (xi + 0) + (((y_divisions - 2) - (yi + 1)) * (x_divisions - 1));
								indicies[0] = front_points_offset + (xi + 0) + (((y_divisions - 2) - (yi + 0)) * (x_divisions - 1));

								uvindicies[3] = (x_divisions - 0) + ((y_divisions - (yi + 1)) * (x_divisions + 1));
								uvindicies[2] = (x_divisions - 0) + ((y_divisions - (yi + 2)) * (x_divisions + 1));
								uvindicies[1] = (x_divisions - 1) + ((y_divisions - (yi + 2)) * (x_divisions + 1));
								uvindicies[0] = (x_divisions - 1) + ((y_divisions - (yi + 1)) * (x_divisions + 1));

								mesh->getMesh().setSide(counter++, 4, indicies, uvindicies);
							}

							//bottom middle polygons
							for(UINT xi = 0; xi < x_divisions - 2; xi++)
							{UINT yi = 0;
								indicies[3] = front_side_offset + x_divisions + y_divisions + 1 + (xi + 0);
								indicies[2] = front_side_offset + x_divisions + y_divisions + 1 + (xi + 1);
								indicies[1] = front_points_offset + ((x_divisions - 2) - (xi + 1)) + ((yi + 0) * (x_divisions - 1));
								indicies[0] = front_points_offset + ((x_divisions - 2) - (xi + 0)) + ((yi + 0) * (x_divisions - 1));

								uvindicies[0] = (x_divisions - (xi + 1)) + ((yi + 1) * (x_divisions + 1));
								uvindicies[1] = (x_divisions - (xi + 2)) + ((yi + 1) * (x_divisions + 1));
								uvindicies[2] = (x_divisions - (xi + 2)) + ((yi + 0) * (x_divisions + 1));
								uvindicies[3] = (x_divisions - (xi + 1)) + ((yi + 0) * (x_divisions + 1));

								mesh->getMesh().setSide(counter++, 4, indicies, uvindicies);
							}

							//left middle polygons
							for(UINT yi = 0; yi < y_divisions - 2; yi++)
							{UINT xi = 0;
								indicies[3] = front_side_offset + x_divisions + y_divisions + x_divisions + 1 + (yi + 0);
								indicies[2] = front_side_offset + x_divisions + y_divisions + x_divisions + 1 + (yi + 1);
								indicies[1] = front_points_offset + (xi + 0) + ((yi + 1) * (x_divisions - 1));
								indicies[0] = front_points_offset + (xi + 0) + ((yi + 0) * (x_divisions - 1));

								uvindicies[3] = (0) + ((yi + 1) * (x_divisions + 1));
								uvindicies[2] = (0) + ((yi + 2) * (x_divisions + 1));
								uvindicies[1] = (1) + ((yi + 2) * (x_divisions + 1));
								uvindicies[0] = (1) + ((yi + 1) * (x_divisions + 1));

								mesh->getMesh().setSide(counter++, 4, indicies, uvindicies);
							}

						}

						//corners
						if((y_divisions > 1) && (x_divisions > 1))
						{
							//top left
							indicies[3] = front_side_offset + 0;
							indicies[2] = front_side_offset + 1;
							indicies[1] = front_points_offset + (0) + ((y_divisions - 2) * (x_divisions - 1));
							indicies[0] = front_side_offset + polys_per_z_division - 1;

							uvindicies[0] = 0 + ((y_divisions - 1) * (x_divisions + 1));
							uvindicies[1] = 1 + ((y_divisions - 1) * (x_divisions + 1));
							uvindicies[2] = 1 + ((y_divisions - 0) * (x_divisions + 1));
							uvindicies[3] = 0 + ((y_divisions - 0) * (x_divisions + 1));

							mesh->getMesh().setSide(counter++, 4, indicies, uvindicies);

							//top right
							indicies[3] = front_side_offset + x_divisions - 1;
							indicies[2] = front_side_offset + x_divisions;
							indicies[1] = front_side_offset + x_divisions + 1;
							indicies[0] = front_points_offset + (x_divisions - 2) + ((y_divisions - 2) * (x_divisions - 1));

							uvindicies[0] = (x_divisions - 1) + ((y_divisions - 1) * (x_divisions + 1));
							uvindicies[1] = (x_divisions - 0) + ((y_divisions - 1) * (x_divisions + 1));
							uvindicies[2] = (x_divisions - 0) + ((y_divisions - 0) * (x_divisions + 1));
							uvindicies[3] = (x_divisions - 1) + ((y_divisions - 0) * (x_divisions + 1));

							mesh->getMesh().setSide(counter++, 4, indicies, uvindicies);

							//bottom right
							indicies[3] = front_side_offset + x_divisions + y_divisions - 1;
							indicies[2] = front_side_offset + x_divisions + y_divisions;
							indicies[1] = front_side_offset + x_divisions + y_divisions + 1;
							indicies[0] = front_points_offset + (x_divisions - 2) + (0 * (x_divisions - 1));

							uvindicies[0] = (x_divisions - 1) + ((1) * (x_divisions + 1));
							uvindicies[1] = (x_divisions - 1) + ((0) * (x_divisions + 1));
							uvindicies[2] = (x_divisions - 0) + ((0) * (x_divisions + 1));
							uvindicies[3] = (x_divisions - 0) + ((1) * (x_divisions + 1));

							mesh->getMesh().setSide(counter++, 4, indicies, uvindicies);

							//bottom left
							indicies[3] = front_side_offset + x_divisions + y_divisions + x_divisions - 1;
							indicies[2] = front_side_offset + x_divisions + y_divisions + x_divisions;
							indicies[1] = front_side_offset + x_divisions + y_divisions + x_divisions + 1;
							indicies[0] = front_points_offset + (0) + (0 * (x_divisions - 1));

							uvindicies[0] = (1) + ((1) * (x_divisions + 1));
							uvindicies[1] = (0) + ((1) * (x_divisions + 1));
							uvindicies[2] = (0) + ((0) * (x_divisions + 1));
							uvindicies[3] = (1) + ((0) * (x_divisions + 1));

							mesh->getMesh().setSide(counter++, 4, indicies, uvindicies);

						}

						if((x_divisions > 1) && (y_divisions == 1))
						{
							for(UINT xi = 0; xi < x_divisions; xi++)
							{

								indicies[3] = front_side_offset + xi;
								indicies[2] = front_side_offset + xi + 1;
								indicies[1] = front_side_offset + (polys_per_z_division - xi - 2);
								indicies[0] = front_side_offset + (polys_per_z_division - xi - 1);

								uvindicies[3] = xi + 0 + x_divisions + 1;
								uvindicies[2] = xi + 1 + x_divisions + 1;
								uvindicies[1] = xi + 1;
								uvindicies[0] = xi + 0;

								mesh->getMesh().setSide(counter++, 4, indicies, uvindicies);

							}
						}

						if((x_divisions == 1) && (y_divisions > 1))
						{

							for(UINT yi = 0; yi < y_divisions; yi++)
							{

								indicies[3] = front_side_offset + ((yi == 0) ? 0 : (polys_per_z_division - yi));
								indicies[2] = front_side_offset + yi + 1;
								indicies[1] = front_side_offset + yi + 2;
								indicies[0] = front_side_offset + (polys_per_z_division - yi - 1);

								uvindicies[0] = (y_divisions - yi - 1) * 2;
								uvindicies[1] = (y_divisions - yi - 1) * 2 + 1;
								uvindicies[2] = (y_divisions - yi - 1) * 2 + 3;
								uvindicies[3] = (y_divisions - yi - 1) * 2 + 2;

								mesh->getMesh().setSide(counter++, 4, indicies, uvindicies);

							}

						}

						if((x_divisions == 1) && (y_divisions == 1))
						{
							indicies[3] = front_side_offset + 0;
							indicies[2] = front_side_offset + 1;
							indicies[1] = front_side_offset + 2;
							indicies[0] = front_side_offset + 3;

							uvindicies[3] = 2;
							uvindicies[2] = 3;
							uvindicies[1] = 1;
							uvindicies[0] = 0;

							mesh->getMesh().setSide(counter++, 4, indicies, uvindicies);
						}
					}

					bottom->addChild(mesh);

					an8.closeChunk();
					scene_debug.unindent();
				}
			}

			//Spheres-----------------------------------------------------------
			//groups, objects, bones - not figures

			if(type != 1)
			{

				while(an8.openChunk("sphere"))
				{
					scene_debug << "Loading Sphere" << endl;
					scene_debug.indent();

					//Get the attach point for the sphere (at the bottom of the proper material)
					SceneNode *bottom = NULL;
					string material_name(" -- default --");
					if(an8.openChunk("material"))
					{
						material_name = an8.getString();
						an8.closeChunk();
					}else scene_debug << "No material specified, using default" << endl;
					bottom = findMaterial(an8, material_name, material_list).bottom;

					//now attach the group base if available
					if(base_accumulate)
					{
						scene_debug << "Adding group base" << endl;
						scene_debug.indent();

						SceneNode *temp;
						temp = UtilParser::duplicateTree(base_accumulate);

						//attachment
						bottom->addChild(temp);
						bottom = temp->findLeaf();

						scene_debug.unindent();
					}

					//now for the sphere base
					if(an8.openChunk("base"))
					{
						scene_debug << "Adding sphere base" << endl;
						scene_debug.indent();

						SceneNode *lbase = loadBase(an8);

						if(lbase)
						{
							bottom->addChild(lbase);
							bottom = lbase->findLeaf();
						}

						scene_debug.unindent();
					}

					REAL radius = 5;
					if(an8.openChunk("diameter"))
					{
						radius = an8.getFloat() / 2.0f;
						an8.closeChunk();

						scene_debug << "Diameter set to " << radius << endl;
					}else scene_debug << "Diameter chunk not found, using default of 10" << endl;

					if(use_sphere_primitive)
					{ //use a sphere node

						SphereNode *sphere = new SphereNode(radius);
						bottom->addChild(sphere);

					} else
					{ //use a mesh

						//defaults
						UINT lon = 12, lat = 8;
						bool geodesic = false;

						//in theory there should be a longlat chunk OR a geodesic
						//chunk.

						if(an8.openChunk("geodesic"))
						{
							lon = an8.getInt();
							an8.closeChunk();
							geodesic = true;
							scene_debug << "Spere is geodesic with " << lon << " divisions" << endl;
						}

						if(an8.openChunk("longlat"))
						{
							lon = an8.getInt();
							lat = an8.getInt();
							an8.closeChunk();
							geodesic = false;
							scene_debug << "Spere is longitude/latidute based with " << lon << " lon and " << lat << " lat" << endl;
						}

						if(geodesic && (lon >= 1))
						{

							//Anim8ors geodesic spheres are based off of a regular
							//octahedron. The number of divisions (stored here in
							//'lon' to few bytes of memory) is the number of times
							//each edge of the octahedron is division. Each division
							//point is then push out from the center to be on the
							//sureface of the sphere.
	/*
							MeshRigidNGon3DNode *sphere = new MeshRigidNGon3DNode;

							//POINTS
							UINT counter = 0;
							//sphere->getMesh.allocateVerticies((lon - 1) * 8 + 12);

							//side 1
							for(UINT y = 0; y <= lon; y++)
							{
								//height
								//for(UINT x = 0; x < y - 2; x

							}


							bottom->addChild(sphere);
	*/
						}

						if((!geodesic) && (lon >= 2) && (lat >= 2))
						{

							//anim8or clamps it to 32 by 32
							if(lon > 32)lon = 32;
							if(lat > 32)lat = 32;

							MeshRigidNGon3DTextured2DNode *sphere = new MeshRigidNGon3DTextured2DNode;

							//POINTS
							UINT counter = 0;
							sphere->getMesh().allocateVerticies((lat - 1) * lon + 2);

							//top and bottom points of the sphere
	                        sphere->getMesh().setVertex(counter++, V3(0, radius, 0));
	                        sphere->getMesh().setVertex((lat - 1) * lon + 1, V3(0, -radius, 0));

							//middle points
							REAL sides = REAL(lat * 2);
							REAL side_size = (2.0 * sides * radius * Math::Rsin(Math::PI / sides)) / sides;
							REAL sum_of_inner_angles = (sides - 2.0) * Math::PI;
							REAL inner_angles = sum_of_inner_angles / sides;
							REAL outer_angles = Math::PI - inner_angles;
							REAL height = radius;

							for(UINT y = 0; y < lat - 1; y++)
							{

								//the different heights are not spaced evenly,
								//they are spaced such that a regular polygon is formed
								height -= Math::Rsin((outer_angles / 2.0) + (REAL(y) * outer_angles)) * side_size;

								//radius of the horizontal cross section at this height
								//from the formula for a sphere (distance formula)
								REAL radius_scaled = Math::Rsqrt((radius * radius) - (height * height));

								for(UINT x = 0; x < lon; x++)
								{//goes around the latitude putting points
									REAL angle = REAL(x) / REAL(lon) * 2.0 * Math::PI;
			                        sphere->getMesh().setVertex(counter++, V3(Math::Rcos(angle) * radius_scaled, height, Math::Rsin(angle) * radius_scaled));
								}


							}

							//UVS
							counter = 0;
							sphere->getMesh().allocateTextureCoordinates((lat + 1) * (lon + 1));
							for(UINT y = 0; y <= lat; y++)
								for(UINT x = 0; x <= lon; x++)
									sphere->getMesh().setTextureCoordinate(counter++, UV(1.0 - (REAL(x) / REAL(lon)), REAL(y) / REAL(lat)));

							//FACES
							counter = 0;
							sphere->getMesh().allocateSides(lon * lat);

							//top/bottom triangles
							UINT indicies[3];
							UINT uvindicies[3];
							for(UINT x = 0; x < lon; x++)
							{
								indicies[0] = 0; //top point index
								indicies[1] = (x + 1) % lon + 1;
								indicies[2] = x + 1;

								uvindicies[0] = (x + 1);
								uvindicies[1] = (x + 1) + (lon + 1);
								uvindicies[2] = (x + 0) + (lon + 1);

								sphere->getMesh().setSide(counter++, 3, indicies, uvindicies);

								indicies[0] = (lat - 1) * lon + 1; //bottom point index
								indicies[1] = (x + 1) + ((lat - 2) * lon);
								indicies[2] = ((x + 1) % lon + 1) + ((lat - 2) * lon);

								uvindicies[0] = (x + 0) + ((lat - 0) * (lon + 1));
								uvindicies[1] = (x + 0) + ((lat - 1) * (lon + 1));
								uvindicies[2] = (x + 1) + ((lat - 1) * (lon + 1));

								sphere->getMesh().setSide(counter++, 3, indicies, uvindicies);

							}

							//middle quads
							UINT qindicies[4];
							UINT quvindicies[4];
							for(UINT y = 0; y < lat - 2; y++)
							{
								for(UINT x = 0; x < lon; x++)
								{

									qindicies[0] = ((x + 0)      ) + ((y + 0) * lon) + 1;
									qindicies[1] = ((x + 1) % lon) + ((y + 0) * lon) + 1;
									qindicies[2] = ((x + 1) % lon) + ((y + 1) * lon) + 1;
									qindicies[3] = ((x + 0)      ) + ((y + 1) * lon) + 1;

									quvindicies[0] = x + 0 + ((y + 1) * (lon + 1));
									quvindicies[1] = x + 1 + ((y + 1) * (lon + 1));
									quvindicies[2] = x + 1 + ((y + 2) * (lon + 1));
									quvindicies[3] = x + 0 + ((y + 2) * (lon + 1));

									sphere->getMesh().setSide(counter++, 4, qindicies, quvindicies);

								}
							}

							//add the mesh to our results list
							bottom->addChild(sphere);

						}

					}

					an8.closeChunk();
					scene_debug.unindent();
				}

			}

			//Cylinders---------------------------------------------------------
			//objects, groups, bones - not figures

			if(type != 1)
			{
				while(an8.openChunk("cylinder"))
				{
					scene_debug << "Loading cylinder" << endl;
					scene_debug.indent();

					string material_name(" -- default --");
					if(an8.openChunk("material"))
					{
						material_name = an8.getString();
						an8.closeChunk();
						scene_debug << "Material is \"" << material_name << "\""<< endl;
					} else scene_debug << "No material present, using default" << endl;

					scene_debug.indent();
					SceneNode *bottom = findMaterial(an8, material_name, material_list).bottom;
					scene_debug.unindent();

					if(base_accumulate)
					{
						SceneNode *temp = UtilParser::duplicateTree(base_accumulate);
						bottom->addChild(temp);
						bottom = temp->findLeaf();
					}

					if(an8.openChunk("base"))
					{
						SceneNode *base = loadBase(an8);

						if(base)
						{
							bottom->addChild(base);
							bottom = base->findLeaf();
						}
					}

					UINT lon = 12, lat = 8;

					if(an8.openChunk("longlat"))
					{
						lon = an8.getInt();
						lat = an8.getInt();
						an8.closeChunk();
						if(lon < 3)lon = 3;
						if(lat < 1)lat = 1;
						scene_debug << "Longitude " << lon << ", latitude " << lat << endl;
					}scene_debug << "No longlat chunk present, using defaults" << endl;

					REAL diameter = 1, top_diameter;

					if(an8.openChunk("diameter"))
					{
						diameter = an8.getFloat();
						an8.closeChunk();

						scene_debug << "Diameter is " << diameter << endl;
					}else scene_debug << "No diameter present, using default (1)" << endl;

					if(an8.openChunk("topdiameter"))
					{
						top_diameter = an8.getFloat();
						an8.closeChunk();

						scene_debug << "Top diameter is " << top_diameter << endl;
					}else
					{
						top_diameter = diameter;
						scene_debug << "No topdiameter present, diameter is uniform" << endl;
					}

					REAL length = 1;

					if(an8.openChunk("length"))
					{
						length = an8.getFloat();
						an8.closeChunk();
						scene_debug << "Length is " << length << endl;
					}else scene_debug << "No length present, using default" << endl;

					bool top_cap = false, bottom_cap = false;

					if(an8.openChunk("capend"))
					{
						an8.closeChunk();
						top_cap = true;
						scene_debug << "Endcap enabled" << endl;
					} else scene_debug << "Endcap disabled" << endl;

					if(an8.openChunk("capstart"))
					{
						an8.closeChunk();
						bottom_cap = true;
						scene_debug << "Startcap enabled" << endl;
					} else scene_debug << "Startcap disabled" << endl;

					MeshRigidNGon3DTextured2DNode *cmesh = new MeshRigidNGon3DTextured2DNode;

					//POINTS
					UINT counter = 0;
					cmesh->getMesh().allocateVerticies(lon * (lat + 1));
					for(UINT h = 0; h <= lat; h++)
						for(UINT a = 0; a < lon; a++)
						{
							REAL angle = (REAL(a) / REAL(lon)) * Math::PI2;
							REAL height_ratio = REAL(h) / REAL(lat);
							REAL local_radius = Math::Rinterpolate(diameter, top_diameter, height_ratio) / 2;
							cmesh->getMesh().setVertex(counter++, V3(Math::Rcos(angle) * local_radius, height_ratio * length, Math::Rsin(angle) * local_radius));
						}

					//UVs
					counter = 0;
					cmesh->getMesh().allocateTextureCoordinates((lon + 1) * (lat + 1));
					for(UINT h = 0; h <= lat; h++)
						for(UINT a = 0; a <= lon; a++)
							cmesh->getMesh().setTextureCoordinate(counter++, UV(1 - (REAL(a) / REAL(lon)), -REAL(h) / REAL(lat)));

					//FACES
					counter = 0;
					cmesh->getMesh().allocateSides(lon * lat + (top_cap ? 1 : 0) + (bottom_cap ? 1 : 0));
					UINT indicies[4];
					UINT uvindicies[4];
					for(UINT h = 0; h < lat; h++)
						for(UINT a = 0; a < lon; a++)
						{
							indicies[0] = ((a + 0) % lon) + ((h + 1) * lon);
							indicies[1] = ((a + 1) % lon) + ((h + 1) * lon);
							indicies[2] = ((a + 1) % lon) + ((h + 0) * lon);
							indicies[3] = ((a + 0) % lon) + ((h + 0) * lon);

							uvindicies[0] = (a + 0) + ((h + 1) * (lon + 1));
							uvindicies[1] = (a + 1) + ((h + 1) * (lon + 1));
							uvindicies[2] = (a + 1) + ((h + 0) * (lon + 1));
							uvindicies[3] = (a + 0) + ((h + 0) * (lon + 1));

							cmesh->getMesh().setSide(counter++, 4, indicies, uvindicies); //vertex indicies and uv indicies are the same
						}

					if(bottom_cap)
					{
						UINT *indicies = new UINT[lon];
						UINT *uvindicies = new UINT[lon];
						for(UINT x = 0; x < lon; x++)
						{//fixme
							indicies[x] = x;//
							uvindicies[x] = x;//lon - x - 1;
						}
						cmesh->getMesh().setSide(counter++, lon, indicies, uvindicies);
						delete[] indicies;
						delete[] uvindicies;
					}

					if(top_cap)
					{
						UINT *indicies = new UINT[lon];
						UINT *uvindicies = new UINT[lon];
						for(UINT x = 0; x < lon; x++)
						{
							indicies[lon - x - 1] = lon * lat + x;
							uvindicies[lon - x - 1] = (lon+1) * lat + x;
						}
						cmesh->getMesh().setSide(counter++, lon, indicies, uvindicies);
						delete[] indicies;
						delete[] uvindicies;
					}

					bottom->addChild(cmesh);

					an8.closeChunk();
					scene_debug.unindent();
				}
			}

			//Named Objects-----------------------------------------------------
			//bones only

			if(type == 3)
			{
				while(an8.openChunk("namedobject"))
				{
					string object_name = an8.getString();
					scene_debug << "Loading namedobject of object \"" << object_name << "\"" << endl;
					scene_debug.indent();

					SceneNode *base = NULL;
					if(an8.openChunk("base"))
					{
						base = loadBase(an8);
						if(base_accumulate)base_accumulate->findLeaf()->addChild(base);
						else base_accumulate = base;
					}

					list<Material> new_list;
					an8.push();
					an8.closeAll();
					if(an8.openChunk("object", object_name))
						loadGroup(an8, new_list, 0);
					an8.pop();

					static UINT unique_number = 0;

					for(list<Material>::iterator i = new_list.begin(); i != new_list.end(); i++)
					{
						//basically we have to change the change the name of each thing to something unique
						//anim8or materials cant have space at the begning (except teh default material)
						//so by putting a space we know we wont conflict with a material in the an8,
						//and by using unique_number we wont conflict with the names generated here.
						i->name = "  unique" + toString(unique_number++) + i->name;

						material_list.push_back(*i);
					}

					if(base_accumulate == base)base_accumulate = NULL;
					if(base)delete base;

					an8.closeChunk();
					scene_debug.unindent();
				}
			}

			//Subgroups---------------------------------------------------------
			//objects, groups, bones, not figures

			if(type != 1)
				while(an8.openChunk("group"))loadGroup(an8, material_list, 2);

			//Bone length------------------------------------------------------
			//for bones only. This comes after the meshes and spheres and stuff
			//because it only applys to sub bones, but not directly attached objects

			if((type == 3) && (name != "root"))
			{   //root bone doesn't need this stuff

				if(an8.openChunk("length"))
				{
					REAL length = an8.getFloat();
					an8.closeChunk();
					scene_debug << "Length is " << length << endl;

					if(length != 0)
					{
						ModelviewTranslationNode *t = new ModelviewTranslationNode;
						t->set(V3(0, length, 0));

						if(!bonebase)
						{
							bonebase = t;
							if(base_accumulate)base_accumulate->findLeaf()->addChild(bonebase);
							else base_accumulate = bonebase;
						}else bonebase->findLeaf()->addChild(t);
					}
				}
			}

			//Sub Bones---------------------------------------------------------
			//figure and bones only
			if((type == 1) || (type == 3))
				while(an8.openChunk("bone"))loadGroup(an8, material_list, 3);

			//Ending stuff for this group/figure/bone/object--------------------

			if(base_accumulate == bonebase)base_accumulate = NULL;
			delete bonebase;

			if(base_accumulate == base)base_accumulate = NULL;
			delete base;

			an8.closeChunk();
			scene_debug.unindent();
		}

		list<SceneNode*> loadObject(AN8 &an8)
		{
			an8.push();
			string name = an8.getString();
			an8.pop();

			scene_debug << "Retrieving object \"" << name << "\"" << endl;
			scene_debug.indent();

			list<SceneNode*> result;

			map<string, list<SceneNode*> >::const_iterator i = loaded_objects.find(name);
			if(i != loaded_objects.end())
			{

				scene_debug << "Object has been loaded" << endl;

				for(list<SceneNode*>::const_iterator i2 = i->second.begin(); i2 != i->second.end(); i2++)
					result.push_back(new ImitationTreeNode(*i2));

			} else
			{

				list<Material> material_list;
				loadGroup(an8, material_list, 0);

				scene_debug << "Object hasn't been loaded" << endl;

				for(list<Material>::const_iterator i = material_list.begin(); i != material_list.end(); i++)
					result.push_back(i->top);

				loaded_objects[name] = result;

			}

			scene_debug.unindent();
			return result;
		}

		list<SceneNode*> loadFigure(AN8 &an8)
		{
			an8.push();
			string name = an8.getString();
			an8.pop();

			scene_debug << "Retrieving figure \"" << name << "\"" << endl;
			scene_debug.indent();

			list<SceneNode*> result;

			map<string, list<SceneNode*> >::const_iterator i = loaded_figures.find(name);
			if(i != loaded_figures.end())
			{

				scene_debug << "Figure has been loaded" << endl;

				for(list<SceneNode*>::const_iterator i2 = i->second.begin(); i2 != i->second.end(); i2++)
					result.push_back(new ImitationTreeNode(*i2));

			} else
			{
				scene_debug << "Figure has not been loaded yet" << endl;

				list<Material> material_list;
				loadGroup(an8, material_list, 1);

				for(list<Material>::const_iterator i = material_list.begin(); i != material_list.end(); i++)
					result.push_back(i->top);

				loaded_figures[name] = result;
			}

			scene_debug.unindent();
			return result;
		}

		list<SceneNode*> loadSceneElement(AN8 &an8, UINT8 type)
		{
			list<SceneNode*> result;

			string name = an8.getString(); //element name

			if(type == 0)scene_debug << "Loading scene element \"" << name << "\"" << endl;
			else if(type == 1)scene_debug << "Loading figure element \"" << name << "\""<< endl;
			else if(type == 2)scene_debug << "Loading null (target) element \"" << name << "\"" << endl;
			else
			{
				error_output << "Invaled type passed to Phobic::<unnamed>::loadSceneElement()" << endl;
				return result;
			}

			scene_debug.indent();

			if(type != 2)
			{ //targets do not refer to an object or figure

				name = an8.getString(); //object/figure name

				if(type == 0)scene_debug << "Element refers to object \"" << name << "\"" << endl;
				if(type == 1)scene_debug << "Element refers to figure \"" << name << "\"" << endl;

			}

			UINT visible = 1; //default visibility
			if(an8.openChunk("visibility"))
			{

				UINT visible = an8.getInt();

				an8.closeChunk();
			}

			if(visible == 1)
			{ //for now, if it's not visible we just dont load it

				SceneNode *bottom = NULL;

				//Location------------------------------------------------------

				if(an8.openChunk("loc"))
				{
					V3 position = an8.getPoint();
					an8.closeChunk();

					if(position != V3::zero())
					{ //no need for the identity case

						scene_debug << "Location set to " << position.str() << endl;

						ModelviewTranslationNode *locnode = new ModelviewTranslationNode;
						locnode->set(position);

						if(bottom)bottom->addChild(locnode);
						else result.push_back(locnode);
						bottom = locnode;
					}

				}

				//Orientation---------------------------------------------------

				if(an8.openChunk("orientation"))
				{
					AN_Quat orientation = an8.getAN_Quat();
					an8.closeChunk();

					if(orientation != AN_Quat::identity())
					{ //no need for the identity case

						scene_debug << "Orientation set to " << orientation.str() << endl;

						ModelviewRotationAN_QuatNode *orinode = new ModelviewRotationAN_QuatNode;
						orinode->set(orientation);

						if(bottom)bottom->addChild(orinode);
						else result.push_back(orinode);
						bottom = orinode;
					}

				}

				//Scale---------------------------------------------------------

				if(an8.openChunk("scale"))
				{ //scale comes after location because we dont want to scale our position
					REAL scale = an8.getFloat();
					an8.closeChunk();

					if(scale != 1)
					{ //no need for the identity case

						scene_debug << "Scale set to " << scale << endl;

						ModelviewScaleNode *scalenode = new ModelviewScaleNode;
						scalenode->set(scale);

						if(bottom)bottom->addChild(scalenode);
						else result.push_back(scalenode);
						bottom = scalenode;
					}

				}

				//Import the object/figure--------------------------------------

				if(type == 0)
				{ //objects
					an8.push();
					an8.closeAll();

					if(an8.openChunk("object", name))
					{

						list<SceneNode *>temp = loadObject(an8);

						if(bottom)bottom->addChild(temp);
						else
							for(list<SceneNode*>::const_iterator i = temp.begin(); i != temp.end(); i++)
								result.push_back(*i);

					}else error_output << "Requested object \"" << name << "\" not found" << endl;

					an8.pop();

				}

				if(type == 1)
				{ //figures

					an8.push();
					an8.closeAll();

					if(an8.openChunk("figure", name))
					{

						list<SceneNode *>temp = loadFigure(an8);

						if(bottom)bottom->addChild(temp);
						else
							for(list<SceneNode*>::const_iterator i = temp.begin(); i != temp.end(); i++)
								result.push_back(*i);

					} else error_output << "Requested figure \"" << name << "\" not found" << endl;

					an8.pop();

				}

				//Sub Object Element--------------------------------------------

				while(an8.openChunk("objectelement"))
				{
					list<SceneNode *>temp = loadSceneElement(an8, 0);

					if(bottom)bottom->addChild(temp);
					else
						for(list<SceneNode*>::const_iterator i = temp.begin(); i != temp.end(); i++)
							result.push_back(*i);
				}

				//Sub Figure Element--------------------------------------------

				while(an8.openChunk("figureelement"))
				{
					list<SceneNode *>temp = loadSceneElement(an8, 1);

					if(bottom)bottom->addChild(temp);
					else
						for(list<SceneNode*>::const_iterator i = temp.begin(); i != temp.end(); i++)
							result.push_back(*i);
				}


				//Sub Null (Target) Element-------------------------------------

				while(an8.openChunk("null"))
				{
					list<SceneNode *>temp = loadSceneElement(an8, 2);

					if(bottom)bottom->addChild(temp);
					else
						for(list<SceneNode*>::const_iterator i = temp.begin(); i != temp.end(); i++)
							result.push_back(*i);
				}

			} else scene_debug << "Element set to non-visible mode, skipping this element" << endl;

			an8.closeChunk();

			scene_debug.unindent();
			return result;

		}

		list<SceneNode*> loadScene(AN8 &an8)
		{
			string name = an8.getString();

			scene_debug << "Loading scene \"" << name << "\"" << endl;
			scene_debug.indent();

			list<SceneNode*> result;

			//Ground Grid-------------------------------------------------------

			//The ground grid is a flat grid of 20 blocks along the x and z axi
			//with alternating light grey and dark grey colors. It is centered
			//around the origin and is 1000 units long on the x and z axis. It
			//has no verticle depth.

			if(an8.openChunk("groundgrid"))
			{
				UINT groundgrid = an8.getInt();
				an8.closeChunk();

				if(groundgrid == 1)
				{ //1 means that there is a ground grid

					scene_debug << "Creating ground grid" << endl;
					scene_debug.indent();

					ColorRGB24Node *lightgrey = new ColorRGB24Node(ColorRGB24(135, 135, 135));
					ColorRGB24Node *darkgrey = new ColorRGB24Node(ColorRGB24(85, 85, 85));

					MeshRigidNGon3DNode *lightsquares = new MeshRigidNGon3DNode;
					MeshRigidNGon3DNode *darksquares = new MeshRigidNGon3DNode;

					lightsquares->getMesh().allocateVerticies(441);
					darksquares->getMesh().allocateVerticies(441);

					UINT lightvertexcount = 0;
					UINT darkvertexcount = 0;
					for(int y = 0; y <= 20; y++)
						for(int x = 0; x <= 20; x++)
						{
							lightsquares->getMesh().setVertex(lightvertexcount++, V3((x - 10) * 50, 0, (y - 10) * 50));
							darksquares->getMesh().setVertex(darkvertexcount++, V3((x - 10) * 50, 0, (y - 10) * 50));
						}

					lightsquares->getMesh().allocateSides(200);
					darksquares->getMesh().allocateSides(200);

					UINT index_list[4];

					lightvertexcount = 0; //face count, reusing the var
					darkvertexcount = 0;

					bool on_off = true;

					for(UINT y = 0; y < 20; y++)
					{
		                on_off = !on_off; //trade off colors each row

						for(UINT x = 0; x < 20; x++)
						{

							index_list[3] = (x + 0) + ((y + 0) * 21);
							index_list[2] = (x + 1) + ((y + 0) * 21);
							index_list[1] = (x + 1) + ((y + 1) * 21);
							index_list[0] = (x + 0) + ((y + 1) * 21);

							if(on_off = !on_off)lightsquares->getMesh().setSide(lightvertexcount++, 4, index_list);
							else darksquares->getMesh().setSide(darkvertexcount++, 4, index_list);

						}
					}

					lightgrey->addChild(lightsquares);
					darkgrey->addChild(darksquares);
					result.push_back(lightgrey);
					result.push_back(darkgrey);

					scene_debug.unindent();
				}

			}

			//Object Element----------------------------------------------------

			while(an8.openChunk("objectelement"))
			{
				list<SceneNode *>temp = loadSceneElement(an8, 0);

				//this is so lame, stl does not have a push/insert/append function that takes stl types...
				for(list<SceneNode*>::const_iterator i = temp.begin(); i != temp.end(); i++)
					result.push_back(*i);

			}

			//Figure Element----------------------------------------------------

			while(an8.openChunk("figureelement"))
			{

				list<SceneNode *>temp = loadSceneElement(an8, 1);

				for(list<SceneNode*>::const_iterator i = temp.begin(); i != temp.end(); i++)
					result.push_back(*i);

			}


			//Null (Target) Element---------------------------------------------

			while(an8.openChunk("null"))
			{
				list<SceneNode *>temp = loadSceneElement(an8, 2);

				for(list<SceneNode*>::const_iterator i = temp.begin(); i != temp.end(); i++)
					result.push_back(*i);
			}

			an8.closeChunk();
			scene_debug.unindent();
			return result;
		}

	} //end of anon namespace

////////////////////////////////////////////////////////////////////////////////
//Interface Functions (use these)-----------------------------------------------
////////////////////////////////////////////////////////////////////////////////

	//very basic check to see if a file is an anim8or file
	const bool isAN8(const string &filename)
	{
		//fixme: use AN8 class instead

		ifstream infile(filename.c_str(), ios::in);
		if(!infile.is_open())return false;

		string chunk_name;
		char c;

		do
		{
			infile >> c;
			if(infile.eof())return false;
		} while ((c == ' ') || (c == '\n') || (c == '\r'));

		while((c != ' ') && (c != '\n') && (c != '\r') && (c != '{'))
		{
			chunk_name += c;
			infile >> c;
			if(infile.eof())return false;
		}

		//check to see if we have found an anim8or global chunk
		if(chunk_name == "header")return true;
		if(chunk_name == "object")return true;
		if(chunk_name == "figure")return true;
		if(chunk_name == "sequence")return true;
		if(chunk_name == "scene")return true;

		return false;
	}
/*
	SceneNode * const import_an8_object(const string &filename, const string &object_name)
	{
		scene_debug << "AN8 importer attempting to load \"" << filename << "\" object \"" << object_name << "\"" << endl;
		scene_debug.indent();

		clear();

		AN8 temp;
		temp.loadFile(filename);

		if(!temp.openChunk("object", object_name))
		{ //object not found
			err(5, object_name);
			scene_debug.unindent();

			setDefaults();
			return new GenericNode;
		}

		an8_filename = filename;

		list<SceneNode*> loaded_list = loadObject(temp);

		scene_debug.unindent();

		if(loaded_list.size() == 0)return new GenericNode;
		if(loaded_list.size() == 1)return *(loaded_list.begin());

		//since theres more than one node and we can only return one, then
		//group them as children under one node
		SceneNode *top = new GenericNode;
		top->addChild(loaded_list);

		setDefaults();
		return top;

	}

	SceneNode * const import_an8_scene (const string &filename, const string &scene_name)
	{
		scene_debug << "AN8 importer attempting to load \"" << filename << "\" scene \"" << scene_name << "\"" << endl;
		scene_debug.indent();

		clear();

		AN8 temp;
		temp.loadFile(filename);

		if(!temp.openChunk("scene", scene_name))
		{ //scene not found
			err(6, scene_name);
			scene_debug.unindent();

			setDefaults();
			return new GenericNode;
		}

		an8_filename = filename;

		list<SceneNode*> loaded_list = loadScene(temp);

		scene_debug.unindent();

		if(loaded_list.size() == 0)return new GenericNode;
		if(loaded_list.size() == 1)return *(loaded_list.begin());

		//since theres more than one node and we can only return one, then
		//group them as children under one node
		SceneNode *top = new GenericNode;
		top->addChild(loaded_list);

		setDefaults();
		return top;
	}
*/
	AN8SceneCompiler::AN8SceneCompiler() : SceneCompiler()
	{
	}

	AN8SceneCompiler::~AN8SceneCompiler()
	{
	}

	const FileType AN8SceneCompiler::fileType()const
	{
		return FileType("*.an8", "Anim8or");
	}

	const bool AN8SceneCompiler::isValid(InputStream &input)const
	{
		//fixme: use AN8 class instead

		if(input.empty())return false;

		string chunk_name;
		byte c;

		do
		{
			input >> c;
			if(input.empty())return false;
		} while ((c == ' ') || (c == '\n') || (c == '\r'));

		while((c != ' ') && (c != '\n') && (c != '\r') && (c != '{'))
		{
			chunk_name += c;
			input >> c;
			if(input.empty())return false;
		}

		//check to see if we have found an anim8or global chunk
		if(chunk_name == "header")return true;
		if(chunk_name == "object")return true;
		if(chunk_name == "figure")return true;
		if(chunk_name == "sequence")return true;
		if(chunk_name == "scene")return true;

		return false;
	}

	const list<SceneNode*> AN8SceneCompiler::run(InputStream &input)const
	{
        list<SceneNode*> result;

		scene_debug << "Loading an8" << endl;
		scene_debug.indent();

		clear();

		AN8 temp;
		temp.load(input);

		if(!temp.openChunk("object", "object01"))
		{ //object not found

			error_output << "Requested object \"object01\" not found" << endl;

			scene_debug.unindent();

			return result;
		}

		an8_filename = "";

		result = loadObject(temp);

		return result;
	}

	const bool AN8SceneCompiler::run(OutputStream &output, const list<SceneNode *> &root_nodes)const
	{
		return false;
	}
#endif //0

#endif // HEADER