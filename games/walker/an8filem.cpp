
#include "baseutils.h"
#include "an8filem.h"
#include "scene.h"

void AN8::eat(char& current)
{
	do
	{
		current = getChar();
	}while(current != '(');
}

int    AN8::Load(const TCHAR* fName)
{
    CDirChange cdd(0); 
    FileWrap f;
    
    char    c_curent        = 0;
    char    c_next          = 0;
    bool    inside_quote    = 0;
	bool    inside_comment  = 0;


    if(f.Open(fName,"rb"))
    {
        TCHAR   line[256];
        
        while(!f.IsEof())
        {
            if(f.ReadLine(line, 255))
            {
                char* pStart = line;

                c_next = *pStart;
                while(*pStart)
                {
                    c_curent = c_next;
                    c_next   = *(++pStart);

                    if(c_curent == '\r')c_curent = ' ';
		            if(c_curent == '\n')c_curent = ' ';

				            //quotes (start and end)
                    if((c_curent == '\"') && (!inside_comment))
                        inside_quote = !inside_quote;

				            //comment start
		            if((c_curent == '/') && (c_next == '*') && (!inside_quote))
		            {
			            if(*(s_data.rbegin())!= ' ')
                            s_data += ' '; //comments are treated as whitespace rather than nonspace
			            inside_comment = true;
		            }

		            if(!inside_comment)
		            {
			            if(c_curent != ' ')
                            s_data += c_curent;
			            else if((*(s_data.rbegin())!= ' ') || (inside_quote))
                            s_data += ' '; //dont do multiple whitespaces unless in a string
		            }

		            //comment end
		            if((c_curent == '*') && (c_next == '/') && (!inside_quote))
		            {
			            inside_comment = false;
			            c_curent = c_next; //skip ahead
                        c_next   = *(++pStart);
		            }
                }

            }
        }
        f.Close();
        this->Parse();
        return 0;
    }
    return -1;
}

void AN8::Parse()
{
	string current;
    string previous;
    _scope = 0;

    _cpos = s_data.begin();
    for(;_cpos != s_data.end();)
	{
		current = readIdent();
        if(current=="{")     
        {
            if(1==_scope)
            {
                this->ReadSection(previous);
            }
        }
        previous = current;
    }
}

void AN8::ReadSection(const string& rootsect)
{
    if(rootsect == "texture")
        this->ReadTexture();
    else if(rootsect == "object")
        this->ReadObject();
    else if(rootsect == "figure")
        this->ReadFigure();
    else if(rootsect == "sequence")
        this->ReadSequence();
}

void AN8::ReadSequence()
{
    string          sname;
    AN_Sequence*    p = new AN_Sequence();
    
    strcpy(p->name,  getString().c_str());
    TRACEX("sequence: %s\r\n", p->name);
    int     scope = _scope;
    char    current = nextChar();
    
    while(_scope==scope)
    {
        sname = readIdent();
        if(_scope!=scope)
            break;
        if(sname=="figure")
        {
            skip();
            strcpy(p->figure,this->getString().c_str());
            skip();
        }
        else if(sname=="frames")
        {
            skip();
            p->frames = this->getIntConst();
            skip();
        }
        else if(sname=="jointangle")
        {
            this->ReadJointAngle(p);
        }
        else if(sname!=" ")
        {
            skipsection();
        }
    }

    _sequences << p;
}

void  AN8::ReadJointAngle(AN_Sequence* p)
{
    strcpy(p->jangle.bone,this->getString().c_str());
    p->jangle.axis = this->getString()[0];
    
    eat2('{');

    int         scope = _scope;
    while(readIdent() == "floatkey")
    {
        AN_Track* pt = new AN_Track();
        
        pt->frameNo = this->getIntConst();
        pt->value   = this->getPoint();
        this->getString();
        p->jangle.tracks << pt;
        eat2('}');
    }

}

void  AN8::ReadFigure()
{
    string      sname;
    AN_Figure*  pf = new AN_Figure();
    
    strcpy(pf->name,  getString().c_str());
    TRACEX("figure: %s\r\n", pf->name);
    int         scope = _scope;
    
    char    current = nextChar();
    
    while(_scope==scope)
    {
        sname = readIdent();
        if(_scope!=scope)
            break;

        if(sname=="bone")
        {
            AN_Bone* pb =  new AN_Bone();
            skip();
            this->ReadBone(pb);
            pf->bones << pb;
        }
    }
}

void  AN8::ReadBone(AN_Bone* pb)
{
    string     sname;
    

    strcpy(pb->name,  getString().c_str());
    TRACEX("bone: %s\r\n", pb->name);
    int     scope = _scope;
    char    current = nextChar();
    
    while(_scope==scope)
    {
        sname = readIdent();
        if(_scope!=scope)
            break;

        if(sname=="bone")
        {
            AN_Bone* pbc = new AN_Bone();
            skip();
            this->ReadBone(pbc);
            pb->bones << pbc;
        }
        else if(sname=="length")
        {
            skip(1);
            pb->length = this->getIntConst();
            skip(1);
        }
        else if(sname=="orientation")
        {
            skip(1);
            pb->orientation = this->getQuat();
            skip(1);
        }
        else if(sname=="dof")
        {
            this->ReadDof(pb);
        }
        else if(sname=="influence")
        {
            skip();
            for(int i=0;i<6;i++)
                pb->influence[i] = this->getFloatConst();
            skip();
        }
        else if(sname=="namedobject")
        {
            skip();
            this->ReadNameObject(pb);
        }
        else if(sname!=" ")
        {
            skipsection();
        }

    }
}


void   AN8::ReadNameObject(AN_Bone* pb)
{
    string     sname;
    
    
    strcpy(pb->nameobj.name,  getString().c_str());
    int        scope = _scope;
    
    while(_scope==scope)
    {
        sname = readIdent();
        if(_scope!=scope)
            break;
        if(sname=="name")
        {
            skip();
            strcpy(pb->nameobj.name1, this->getString().c_str());
            skip();
        }
        else if(sname == "material")
        {
            skip();
            strcpy(pb->nameobj.material, this->getString().c_str());
            skip();
        }
        else if(sname == "weightedby")
        {
            skip();
            pb->nameobj.weightedby << this->getString();
            skip();
        }
        else if(sname!=" ")
        {
            skipsection();
        }
    }
}

void  AN8::ReadDof(AN_Bone* pb)
{
    skip(1);
    int         scope = _scope;
    string      sname;
    
    while(1)
    {
        sname = readIdent();
        if(_scope!=scope)
            break;
        else if(sname=="X"){
            pb->dof[0] = this->getPoint();
        }
        else if(sname=="Y"){
            pb->dof[1] = this->getPoint();
        }
        else if(sname=="Z"){
            pb->dof[2] = this->getPoint();
        }
        else if(sname!=" ")
        {
            skipsection();
        }
    }
}

void  AN8::ReadObject()
{
    string      sname;
    AN_Object*  po = new AN_Object();
    
    strcpy(po->name,  getString().c_str());
    int         scope = _scope;
    
    char    current = nextChar();
    
    while(_scope==scope)
    {
        sname = readIdent();
        if(_scope!=scope)
            break;

        if(sname == "material")
        {
            this->ReadMaterial(po);
        }
        else if(sname == "image")
        {
            this->ReadImage(po);
        }
        else if(sname == "mesh")
        {
            this->ReadMesh(&po->mesh);
        }
    }
    _objects << po;
}

void  AN8::ReadMesh(AN_Mesh* m)
{
    string      sname  = readIdent();
    int         scope = _scope;

    while(_scope==scope)
    {
        sname = readIdent();
        if(_scope!=scope)
            break;
        if(sname=="name")
        {
            skip(1);
            strcpy(m->name, readIdent().c_str());
            skip(1);
        }
        else if(sname=="base")
        {
            this->ReadBase(&m->base);
        }
        else if(sname=="pivot")
        {
            this->ReadBase(&m->pivot);
        }
        else if(sname=="material")
        {
            skip(1);
            strcpy(m->material, this->getString().c_str());
            skip(1);
        }
        else if(sname=="smoothangle")
        {
            skip(1);
            m->smoothangle = this->getFloatConst();
            skip(1);
        }
        else if(sname=="materiallist")
        {
            this->ReadMaterialList(m);
        }
        else if(sname=="points")
        {
            skip(1);
            while(1)
            {
                if(nextNonWhiteSpaceChar() == '}')
                    break;
                m->points << this->getPoint();
                this->getChar();
            }
            skip(1);
        }
        else if(sname=="edges")
        {
            skip(1);
            while(1)
            {
                if(nextNonWhiteSpaceChar() == '}')
                    break;
                m->edges << this->getSIZE();
                this->getChar();
            }
            skip(1);

        }
        else if(sname=="texcoords")
        {
            skip(1);
            while(1)
            {
                if(nextNonWhiteSpaceChar() == '}')
                    break;
                m->texcoord << this->getUV();
                this->getChar();
            }
            skip(1);
        }
        else if(sname=="faces")
        {
            
            while(1)
            {
                if(nextNonWhiteSpaceChar() == '}')
                    break;
                
                AN_FaceRef*  fr = new AN_FaceRef();

                fr->nvexes  = this->getInt();
                fr->flags     = this->getInt();
                fr->material  = this->getInt();
                fr->normal    = this->getInt();
                int bhas_n  = fr->flags & 0x1;
                int bhas_uv = fr->flags & 0x4;

                eat2('(');
				for(UINT x = 0; x < fr->nvexes; x++)
				{
                    eat2('(');

                    fr->vertices << getInt();
					if(bhas_n)
                        fr->normals << getInt();
					if(bhas_uv)
                        fr->texcoords << getInt();

                    eat2(')');
				}
                eat2(')');
                m->rfaces << fr;
            }
            skip(1);
        }
        else if(sname!=" ")
        {
            skipsection();
        }
    }
}

void  AN8::ReadMaterialList(AN_Mesh* pi)
{
    string      sname  = readIdent();
    int         scope = _scope;
    while(_scope==scope)
    {
        sname = readIdent();
        if(_scope!=scope)
            break;
        if(sname == "materialname")
        {
            skip(1);
            pi->matnames << this->getString();
            skip(1);
        }
        else if(sname!=" ")
        {
            skipsection();
        }
    }    
}

void  AN8::ReadImage(AN_Object* po)
{
    string      sname  = readIdent();
    int         scope = _scope;
    AN_Image    *pi = new AN_Image();

    while(_scope==scope)
    {
        sname = readIdent();
        if(_scope!=scope)
            break;
        if(sname=="name")
        {
            skip(1);
            strcpy(pi->name, this->getString().c_str());
            skip(1);
        }
        else if(sname=="file")
        {
            skip(1);
            strcpy(pi->file, getString().c_str());
            skip(1);
        }
        else if(sname=="base")
        {
            this->ReadBase(&pi->base);
        }
        else if(sname=="size")
        {
            skip(1);
            pi->size = this->getUV();
            skip(1);
        }
        else if(sname!=" ")
        {
            skipsection();
        }

    }

    po->pimages << pi;
}

void  AN8::ReadBase(AN_Base* b)
{
    string      sname  = readIdent();
    int         scope = _scope;
    while(_scope==scope)
    {
        sname = readIdent();
        if(_scope!=scope)
            break;
        if(sname == "origin")
        {
            skip(1);
            b->origin = this->getPoint();
            skip(1);
        }
        else if(sname == "orientation")
        {
            skip(1);
            b->orientation = this->getQuat();
            skip(1);
        }
        else if(sname!=" ")
        {
            skipsection();
        }
    }
}

void  AN8::ReadMaterial(AN_Object* o)
{
    AN_Material* pm = new AN_Material();
    string       sname = this->readIdent();

    char    current = nextChar();
    sname = this->readIdent();
    strcpy(pm->name, sname.c_str());
    int         scope = _scope;
    while(_scope==scope)
    {
        sname = readIdent();
        if(sname == "surface")
        {
            this->ReadSurface(pm);
        }
    }
    o->pmaterials << pm;
}

void    AN8::ReadSurface(AN_Material* pm)
{
    string      sname  = readIdent();
    int         scope = _scope;
    while(_scope==scope)
    {
        sname = readIdent();
        if(_scope!=scope)
            break;

        if(sname == "rgb")
        {
            skip(1);
            pm->surface.rgb = getRGB();
            skip(1);
        }
        else if(sname == "ambiant")
        {
            this->ReadColorMat(&pm->surface.ambiant);
        }
        else if(sname == "diffuse")
        {
            this->ReadColorMat(&pm->surface.diffuse);
        }
        else if(sname == "specular")
        {
            this->ReadColorMat(&pm->surface.specular);
        }
        else if(sname == "emissive")
        {
            this->ReadColorMat(&pm->surface.emissive);
        }
        else if(sname == "phongsize")
        {
            skip(1);
            pm->surface.phongsize = getFloatConst();
            skip(1);
        }
        else if(sname!=" ")
        {
            skipsection();
        }
    }
}
/*
  diffuse {
        rgb { 249 212 166 }
        factor { 0.70000 }
        texturename { "ear" }
        textureparams {
          blendmode { darken }
          alphamode { none }
        }
      }
*/
void AN8::ReadColorMat(AN_Colorp* c)
{
    string      sname  = readIdent();
    int         scope = _scope;
    while(_scope==scope)
    {
        sname = readIdent();
        if(_scope!=scope)
            break;
       
        if(sname == "rgb")
        {
            skip(1);
            c->rgb = this->getRGB();
            skip(1);
        }
        else if(sname == "factor")
        {
            skip(1);
            c->factor = this->getFloatConst();
            skip(1);
            
        }
        else if(sname == "texturename")
        {
            skip(1);
            strcpy(c->texname, this->getString().c_str());
            skip(1);
        }
        else if(sname == "textureparams")
        {
            this->ReadtexParams(&c->textureparams);
        }
        else if(sname != " ")
            skipsection();
    }
}

/*
        textureparams {
          blendmode { darken }
          alphamode { none }
        }

*/
void AN8::ReadtexParams(AN_TexParams* t)
{
    string      sname  = readIdent();
    int         scope = _scope;
    while(1)
    {
        sname = readIdent();
        if(_scope!=scope)
            break;
        if(sname=="blendmode")
        {
            skip(1);
            string bm = this->readIdent();
            if(bm=="decal")
                t->blendmode = 1;
            else
                t->blendmode = 0;
            skip(1);
        }
        else if(sname=="alphamode")
        {
            skip(1);
            string bm = this->readIdent();
            if(bm=="decal")
                t->alphamode = 1;
            else
                t->alphamode = 0;
            skip(1);
        }
        else if(sname!=" ")
            skipsection();
    }
}

/*
texture { "areyegreen" 
    file { "D:\\capturetf\\data\\char\\Alex\\areyegreen.bmp" }
}
*/
void AN8::ReadTexture()
{
    //possible search 
    string  tname = readIdent();
    string  name;
    do
    {
        name = readIdent();
        if(name == "file")
        {
             string filename = getString();

             //
             //  tname    = texture name on witch is refered from other objects
             //  filename = filename to load for the texture
             //

             Htex* itex = Psys->GenTexFile(filename.c_str(), 0);
             if(itex->hTex == 0)
             {
                 char file[256];
                 char dontcare[256];
                 char path[256];
                 char ext[256];

                _getcwd(path, _MAX_PATH); // test of cwd

                 strcpy(path, filename.c_str());
                 _splitpath(path, dontcare,dontcare,file,ext);
                 strcat(file,ext);

                 itex = Psys->GenTexFile(file, 0);
             }
            _texAssoc[tname] = *itex;
        }
    }while(_scope!=0);

}


UINT AN8::getInt()
{
	//ints are a series of 0 to 9s

	UINT result = 0;
	char current;
	bool started = false;

	for(;;)
	{

		current = nextChar(); //look ahead

		if(current == 0)return result; //end of file

		if((current >= '0') && (current <= '9'))started = true;
		else if(started)break;

		current = getChar(); //get the char we looked ahead to and forward the _position

		if(started)
		{
			result *= 10; //now that there 1s another digit, each previous digit means 10 times more (in decimal)
			result += current - '0'; //now add the last digit
		}

	}

	return result;
}

REAL AN8::getFloat()
{
	char current;
	REAL result = 0;
	bool started = false;
	REAL place = 0;

	for(;;)
	{

		current = nextChar();

		if(current == 0)return result; //end of file

		if(((current >= '0') && (current <= '9')) || ((current == '.') && (place == 0)))started = true;
		else if(started)break;

		current = getChar();

		if(started)
		{
			if(current == '.')place = 1;
			else
			{

				if(place)
				{ //past period
					place *= 10; //10ths, 100th, etc
					result += REAL(current - '0') / place;
				} else
				{ //not past period
					result *= 10; //same as int
					result += current - '0';
				}
			}
		}
	}

	return result;
}

string AN8::getString()
{
	//strings look like "somedata" and may contain spaces or escape chars

	char current;
	string result;

	//first we need to find the first character of a string which is "
	for(current = getChar();current != '\"';current = getChar())
    {
		if(current == 0)
		{ 
			printf("Unexpected end of s_data (expecting a string)\n");
			return "";
		}
    }

	//now just loop and add stuff to result until we come across the closing "
	for(;;)
	{
		current = getChar();
		if(current == '\\')
            current = getChar(); //escape characters
		else if(current == '\"')
            break; //" denotes the end of the string unless escaped

		if(current == 0)
		{ //end of file
			printf("Unexpected end of s_data in a string\n");
			return "";
		}

		result += current;

	}

	return result;
}

int AN8::getIntConst()
{
	//int consts are a series of 0 to 9s with optional + or - at the begining

	int result = 0;
	char current;
	bool started = false;
	bool positive = true;

	for(;;)
	{

		current = nextChar(); //look ahead

		if(current == 0)return result; //end of file

		if(((current >= '0') && (current <= '9')) || ( ((current == '+') || (current == '-')) && (started == false) ))started = true;
		else if(started)break;

		current = getChar(); //get the char we looked ahead to and forward the _position

		if(started)
		{
			if(current == '+'); //default cause is positive anyways
			else if(current == '-')positive = false;

			else
			{ //current is a number
				result *= 10; //now that theres another digit each previous digit means 10 times more (in decimal)
				result += current - '0'; //now add the last digit
			}
		}

	}

	if(!positive)result = -result;

	return result;

}

REAL AN8::getFloatConst()
{
	char current;
	REAL result = 0;
	bool started = false;
	REAL place = 0;
	bool positive = true;

	for(;;)
	{

		current = nextChar();

		if(current == 0)return result; //end of file

		if(((current >= '0') && (current <= '9')) || ((current == '.') && (place == 0)) || (((current == '+') || (current == '-')) && (started == false)))started = true;
		else if(started)break;

		current = getChar();

		if(started)
		{
			if(current == '.')place = 1;
			else if(current == '+');
			else if(current == '-')positive = false;
			else
			{

				if(place)
				{ //past period
					place *= 10; //10ths, 100th, etc
					result += REAL(current - '0') / place;
				} else
				{ //not past period
					result *= 10; //same as int
					result += current - '0';
				}
			}
		}
	}

	if(!positive)result = -result;

	return result;
}

UV AN8::getUV()
{
	UV result;
	result.u = getFloatConst();
	result.v = getFloatConst();
	return result;

}

SIZE AN8::getSIZE()
{
	SIZE result;
	result.cx = getIntConst();
	result.cy = getIntConst();
	return result;

}

V3 AN8::getPoint()
{
	V3 result;


	result.x = getFloatConst();
	result.y = getFloatConst();
	result.z = getFloatConst();

	return result;

}

CLR AN8::getRGB()
{
	CLR result;

    
        result.r = getIntConst();
	    result.g = getIntConst();
	    result.b = getIntConst();
        result.a = 255;
    

	return result;

}

Quat AN8::getQuat()
{
	Quat result;

	result.x = getFloatConst();
	result.y = getFloatConst();
	result.z = getFloatConst();
	result.w = getFloatConst();

	return result;

}

void    AN8::skipsection()
{
    int s = _scope;
    readIdent();
    while(_scope != s)
        readIdent();
}

void    AN8::skip(int k)
{
    while(--k>-1)
        readIdent();
}


string AN8::readIdent()
{
	//an identifier starts with a letter or an underscore, and may have
	//digits after the first character.
	char current;
	string result;

	//skip over everything until we get a valid begining character (alphabet chars or underscore)
	for(;;)
	{
		current = getChar();

		if(current == 0)return ""; //end of file

		//so we can escape from chunks when they end
        if(current == '}'){ return string("}");}
        if(current == '{'){ return string("{");}

		if((current >= 'a') && (current <= 'z'))break;
		if((current >= 'A') && (current <= 'Z'))break;
		if(current == '_')break;

	}

	result += current;

	//ok we have found our first valid character, now keep adding stuff
	//until we find an invalid character
	for(;;)
	{
		char current = nextChar();

		if(current == 0)return ""; //end of file

		if((current < 'a') || (current > 'z'))
		if((current < 'A') || (current > 'Z'))
		if((current < '0') || (current > '9'))
		if(current != '_')break;

		result += getChar();
	}

	return result;
}

inline char AN8::getChar()
{
	if(_cpos != s_data.end())
	{
		char temp = *_cpos;
		_cpos++;

        if(temp == '{') ++_scope; else if(temp == '}') --_scope;

		return temp;
	}
	return 0;
}

inline char AN8::getNonWhiteSpaceChar()
{
	while(_cpos != s_data.end())
	{
		char temp = *_cpos;
		_cpos++;
		if(temp != ' ')return temp;
	}
	return 0;
}


char AN8::nextChar()
{
	if(_cpos != s_data.end())return *_cpos;
	return 0;
}

char AN8::next2Char()
{
	if((_cpos+1) != s_data.end())return *(_cpos+1);
	return 0;
}


char AN8::nextNonWhiteSpaceChar()
{
	string::const_iterator temp = _cpos;
	while(temp != s_data.end())
	{
		if(*temp != ' ')return *temp;
		temp++;
	}
	return 0;
}

void AN8::reset()
{
	s_data.clear();
	_cpos = s_data.begin();

    map<string, Htex>::iterator b = _texAssoc.begin();
    map<string, Htex>::iterator e = _texAssoc.end();
    for(;b!=e;b++)
    {
        Psys->DeleteTex(&b->second, 1);
    }

    _objects.deleteelements();
    _sequences.deleteelements();
}

void   AN8::eat2(const char c)
{
    char current;
    do{ 
        current = getChar(); 
    }while(current != c);

}
