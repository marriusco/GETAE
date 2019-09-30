#ifndef __GLEXT_H__
#define __GLEXT_H_

//#include "../Platform.h"

#include <string.h>

#include <GL/gl.h>
#define wglxGetProcAddress wglGetProcAddress


typedef int GLintptrARB;
typedef int GLsizeiptrARB;


#ifndef GL_ARB_depth_texture
    #define GL_ARB_depth_texture
    #define GL_DEPTH_COMPONENT16_ARB        0x81A5
    #define GL_DEPTH_COMPONENT24_ARB        0x81A6
    #define GL_DEPTH_COMPONENT32_ARB        0x81A7
    #define GL_TEXTURE_DEPTH_SIZE_ARB       0x884A
    #define GL_DEPTH_TEXTURE_MODE_ARB       0x884B
#endif


#ifndef GL_ARB_fragment_program
    #define GL_ARB_fragment_program
    #define GL_FRAGMENT_PROGRAM_ARB                                 0x8804
    #define GL_PROGRAM_ALU_INSTRUCTIONS_ARB                         0x8805
    #define GL_PROGRAM_TEX_INSTRUCTIONS_ARB                         0x8806
    #define GL_PROGRAM_TEX_INDIRECTIONS_ARB                         0x8807
    #define GL_PROGRAM_NATIVE_ALU_INSTRUCTIONS_ARB                  0x8808
    #define GL_PROGRAM_NATIVE_TEX_INSTRUCTIONS_ARB                  0x8809
    #define GL_PROGRAM_NATIVE_TEX_INDIRECTIONS_ARB                  0x880A
    #define GL_MAX_PROGRAM_ALU_INSTRUCTIONS_ARB                     0x880B
    #define GL_MAX_PROGRAM_TEX_INSTRUCTIONS_ARB                     0x880C
    #define GL_MAX_PROGRAM_TEX_INDIRECTIONS_ARB                     0x880D
    #define GL_MAX_PROGRAM_NATIVE_ALU_INSTRUCTIONS_ARB              0x880E
    #define GL_MAX_PROGRAM_NATIVE_TEX_INSTRUCTIONS_ARB              0x880F
    #define GL_MAX_PROGRAM_NATIVE_TEX_INDIRECTIONS_ARB              0x8810
    #define GL_MAX_TEXTURE_COORDS_ARB                               0x8871
    #define GL_MAX_TEXTURE_IMAGE_UNITS_ARB                          0x8872
#endif


#ifndef GL_ARB_fragment_shader
    #define GL_ARB_fragment_shader
    #define GL_FRAGMENT_SHADER_ARB                      0x8B30
    #define GL_MAX_FRAGMENT_UNIFORM_COMPONENTS_ARB      0x8B49
#endif


#ifndef GL_ARB_multisample
    #define GL_ARB_multisample
    #define GL_MULTISAMPLE_ARB              0x809D
    #define GL_SAMPLE_ALPHA_TO_COVERAGE_ARB 0x809E
    #define GL_SAMPLE_ALPHA_TO_ONE_ARB      0x809F
    #define GL_SAMPLE_COVERAGE_ARB          0x80A0
    #define GL_SAMPLE_BUFFERS_ARB           0x80A8
    #define GL_SAMPLES_ARB                  0x80A9
    #define GL_SAMPLE_COVERAGE_VALUE_ARB    0x80AA
    #define GL_SAMPLE_COVERAGE_INVERT_ARB   0x80AB
    #define GL_MULTISAMPLE_BIT_ARB          0x20000000

    typedef void (APIENTRY * PF_GLSAMPLECOVERAGEARBPROC)(GLclampf value, GLboolean invert);
#endif

#ifndef GL_ARB_multitexture
    #define GL_ARB_multitexture
    #define GL_TEXTURE0_ARB                     0x84C0
    #define GL_TEXTURE1_ARB                     0x84C1
    #define GL_TEXTURE2_ARB                     0x84C2
    #define GL_TEXTURE3_ARB                     0x84C3
    #define GL_TEXTURE4_ARB                     0x84C4
    #define GL_TEXTURE5_ARB                     0x84C5
    #define GL_TEXTURE6_ARB                     0x84C6
    #define GL_TEXTURE7_ARB                     0x84C7
    #define GL_TEXTURE8_ARB                     0x84C8
    #define GL_TEXTURE9_ARB                     0x84C9
    #define GL_TEXTURE10_ARB                    0x84CA
    #define GL_TEXTURE11_ARB                    0x84CB
    #define GL_TEXTURE12_ARB                    0x84CC
    #define GL_TEXTURE13_ARB                    0x84CD
    #define GL_TEXTURE14_ARB                    0x84CE
    #define GL_TEXTURE15_ARB                    0x84CF
    #define GL_TEXTURE16_ARB                    0x84D0
    #define GL_TEXTURE17_ARB                    0x84D1
    #define GL_TEXTURE18_ARB                    0x84D2
    #define GL_TEXTURE19_ARB                    0x84D3
    #define GL_TEXTURE20_ARB                    0x84D4
    #define GL_TEXTURE21_ARB                    0x84D5
    #define GL_TEXTURE22_ARB                    0x84D6
    #define GL_TEXTURE23_ARB                    0x84D7
    #define GL_TEXTURE24_ARB                    0x84D8
    #define GL_TEXTURE25_ARB                    0x84D9
    #define GL_TEXTURE26_ARB                    0x84DA
    #define GL_TEXTURE27_ARB                    0x84DB
    #define GL_TEXTURE28_ARB                    0x84DC
    #define GL_TEXTURE29_ARB                    0x84DD
    #define GL_TEXTURE30_ARB                    0x84DE
    #define GL_TEXTURE31_ARB                    0x84DF
    #define GL_ACTIVE_TEXTURE_ARB               0x84E0
    #define GL_CLIENT_ACTIVE_TEXTURE_ARB        0x84E1
    #define GL_MAX_TEXTURE_UNITS_ARB            0x84E2

    typedef void (APIENTRY * PF_GLACTIVETEXTUREARBPROC) (GLenum texture);
    typedef void (APIENTRY * PF_GLCLIENTACTIVETEXTUREARBPROC) (GLenum texture);
    typedef void (APIENTRY * PF_GLMULTITEXCOORD1FARBPROC) (GLenum texture, GLfloat s);
    typedef void (APIENTRY * PF_GLMULTITEXCOORD1FVARBPROC) (GLenum texture, const GLfloat *v);
    typedef void (APIENTRY * PF_GLMULTITEXCOORD2FARBPROC) (GLenum texture, GLfloat s, GLfloat t);
    typedef void (APIENTRY * PF_GLMULTITEXCOORD2FVARBPROC) (GLenum texture, const GLfloat *v);
    typedef void (APIENTRY * PF_GLMULTITEXCOORD3FARBPROC) (GLenum texture, GLfloat s, GLfloat t, GLfloat r);
    typedef void (APIENTRY * PF_GLMULTITEXCOORD3FVARBPROC) (GLenum texture, const GLfloat *v);
    typedef void (APIENTRY * PF_GLMULTITEXCOORD4FARBPROC) (GLenum texture, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
    typedef void (APIENTRY * PF_GLMULTITEXCOORD4FVARBPROC) (GLenum texture, const GLfloat *v);
#endif

#ifndef GL_ARB_occlusion_query
    #define GL_ARB_occlusion_query
    #define GL_SAMPLES_PASSED_ARB         0x8914
    #define GL_QUERY_COUNTER_BITS_ARB     0x8864
    #define GL_CURRENT_QUERY_ARB          0x8865
    #define GL_QUERY_RESULT_ARB           0x8866
    #define GL_QUERY_RESULT_AVAILABLE_ARB 0x8867

    typedef void (APIENTRY * PF_GLGENQUERIESARBPROC) (GLsizei n, GLuint *ids);
    typedef void (APIENTRY * PF_GLDELETEQUERIESARBPROC) (GLsizei n, const GLuint *ids);
    typedef void (APIENTRY * PF_GLISQUERYARBPROC) (GLuint id);
    typedef void (APIENTRY * PF_GLBEGINQUERYARBPROC) (GLenum target, GLuint id);
    typedef void (APIENTRY * PF_GLENDQUERYARBPROC) (GLenum target);
    typedef void (APIENTRY * PF_GLGETQUERYIVARBPROC) (GLenum target, GLenum pname, GLint *params);
    typedef void (APIENTRY * PF_GLGETQUERYOBJECTIVARBPROC) (GLuint id, GLenum pname, GLint *params);
    typedef void (APIENTRY * PF_GLGETQUERYOBJECTUIVARBPROC) (GLuint id, GLenum pname, GLuint *params);
#endif


#ifndef GL_ARB_shader_objects
    #define GL_ARB_shader_objects
    #define GL_PROGRAM_OBJECT_ARB                       0x8B40

    #define GL_OBJECT_TYPE_ARB                          0x8B4E
    #define GL_OBJECT_SUBTYPE_ARB                       0x8B4F
    #define GL_OBJECT_DELETE_STATUS_ARB                 0x8B80
    #define GL_OBJECT_COMPILE_STATUS_ARB                0x8B81
    #define GL_OBJECT_LINK_STATUS_ARB                   0x8B82
    #define GL_OBJECT_VALIDATE_STATUS_ARB               0x8B83
    #define GL_OBJECT_INFO_LOG_LENGTH_ARB               0x8B84
    #define GL_OBJECT_ATTACHED_OBJECTS_ARB              0x8B85
    #define GL_OBJECT_ACTIVE_UNIFORMS_ARB               0x8B86
    #define GL_OBJECT_ACTIVE_UNIFORM_MAX_LENGTH_ARB     0x8B87
    #define GL_OBJECT_SHADER_SOURCE_LENGTH_ARB          0x8B88

    #define GL_SHADER_OBJECT_ARB                        0x8B48

    #define GL_FLOAT_VEC2_ARB                           0x8B50
    #define GL_FLOAT_VEC3_ARB                           0x8B51
    #define GL_FLOAT_VEC4_ARB                           0x8B52
    #define GL_INT_VEC2_ARB                             0x8B53
    #define GL_INT_VEC3_ARB                             0x8B54
    #define GL_INT_VEC4_ARB                             0x8B55
    #define GL_BOOL_ARB                                 0x8B56
    #define GL_BOOL_VEC2_ARB                            0x8B57
    #define GL_BOOL_VEC3_ARB                            0x8B58
    #define GL_BOOL_VEC4_ARB                            0x8B59
    #define GL_FLOAT_MAT2_ARB                           0x8B5A
    #define GL_FLOAT_MAT3_ARB                           0x8B5B
    #define GL_FLOAT_MAT4_ARB                           0x8B5C

    typedef char         GLcharARB;
    typedef unsigned int GLhandleARB;

    typedef GLvoid      (APIENTRY *PF_GLDELETEOBJECTARBPROC)(GLhandleARB obj);
    typedef GLhandleARB (APIENTRY *PF_GLGETHANDLEARBPROC)(GLenum pname);
    typedef GLvoid      (APIENTRY *PF_GLDETACHOBJECTARBPROC)(GLhandleARB containerObj, GLhandleARB attachedObj);
    typedef GLhandleARB (APIENTRY *PF_GLCREATESHADEROBJECTARBPROC)(GLenum shaderType);
    typedef GLvoid      (APIENTRY *PF_GLSHADERSOURCEARBPROC)(GLhandleARB shaderObj, GLsizei count, const GLcharARB **string, const GLint *length);
    typedef GLvoid      (APIENTRY *PF_GLCOMPILESHADERARBPROC)(GLhandleARB shaderObj);
    typedef GLhandleARB (APIENTRY *PF_GLCREATEPROGRAMOBJECTARBPROC)(GLvoid);
    typedef GLvoid      (APIENTRY *PF_GLATTACHOBJECTARBPROC)(GLhandleARB containerObj, GLhandleARB obj);
    typedef GLvoid      (APIENTRY *PF_GLLINKPROGRAMARBPROC)(GLhandleARB programObj);
    typedef GLvoid      (APIENTRY *PF_GLUSEPROGRAMOBJECTARBPROC)(GLhandleARB programObj);
    typedef GLvoid      (APIENTRY *PF_GLVALIDATEPROGRAMARBPROC)(GLhandleARB programObj);

    typedef GLvoid      (APIENTRY *PF_GLUNIFORM1FARBPROC)(GLint location, GLfloat v0);
    typedef GLvoid      (APIENTRY *PF_GLUNIFORM2FARBPROC)(GLint location, GLfloat v0, GLfloat v1);
    typedef GLvoid      (APIENTRY *PF_GLUNIFORM3FARBPROC)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
    typedef GLvoid      (APIENTRY *PF_GLUNIFORM4FARBPROC)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);

    typedef GLvoid      (APIENTRY *PF_GLUNIFORM1IARBPROC)(GLint location, GLint v0);
    typedef GLvoid      (APIENTRY *PF_GLUNIFORM2IARBPROC)(GLint location, GLint v0, GLint v1);
    typedef GLvoid      (APIENTRY *PF_GLUNIFORM3IARBPROC)(GLint location, GLint v0, GLint v1, GLint v2);
    typedef GLvoid      (APIENTRY *PF_GLUNIFORM4IARBPROC)(GLint location, GLint v0, GLint v1, GLint v2, GLint v3);

    typedef GLvoid      (APIENTRY *PF_GLUNIFORM1FVARBPROC)(GLint location, GLsizei count, GLfloat *value);
    typedef GLvoid      (APIENTRY *PF_GLUNIFORM2FVARBPROC)(GLint location, GLsizei count, GLfloat *value);
    typedef GLvoid      (APIENTRY *PF_GLUNIFORM3FVARBPROC)(GLint location, GLsizei count, GLfloat *value);
    typedef GLvoid      (APIENTRY *PF_GLUNIFORM4FVARBPROC)(GLint location, GLsizei count, GLfloat *value);

    typedef GLvoid      (APIENTRY *PF_GLUNIFORM1IVARBPROC)(GLint location, GLsizei count, GLint *value);
    typedef GLvoid      (APIENTRY *PF_GLUNIFORM2IVARBPROC)(GLint location, GLsizei count, GLint *value);
    typedef GLvoid      (APIENTRY *PF_GLUNIFORM3IVARBPROC)(GLint location, GLsizei count, GLint *value);
    typedef GLvoid      (APIENTRY *PF_GLUNIFORM4IVARBPROC)(GLint location, GLsizei count, GLint *value);

    typedef GLvoid      (APIENTRY *PF_GLUNIFORMMATRIX2FVARBPROC)(GLint location, GLsizei count, GLboolean transpose, GLfloat *value);
    typedef GLvoid      (APIENTRY *PF_GLUNIFORMMATRIX3FVARBPROC)(GLint location, GLsizei count, GLboolean transpose, GLfloat *value);
    typedef GLvoid      (APIENTRY *PF_GLUNIFORMMATRIX4FVARBPROC)(GLint location, GLsizei count, GLboolean transpose, GLfloat *value);

    typedef GLvoid      (APIENTRY *PF_GLGETOBJECTPARAMETERFVARBPROC)(GLhandleARB obj, GLenum pname, GLfloat *params);
    typedef GLvoid      (APIENTRY *PF_GLGETOBJECTPARAMETERIVARBPROC)(GLhandleARB obj, GLenum pname, GLint *params);
    typedef GLvoid      (APIENTRY *PF_GLGETINFOLOGARBPROC)(GLhandleARB obj, GLsizei maxLength, GLsizei *length, GLcharARB *infoLog);
    typedef GLvoid      (APIENTRY *PF_GLGETATTACHEDOBJECTSARBPROC)(GLhandleARB containerObj, GLsizei maxCount, GLsizei *count, GLhandleARB *obj);
    typedef GLint       (APIENTRY *PF_GLGETUNIFORMLOCATIONARBPROC)(GLhandleARB programObj, const GLcharARB *name);
    typedef GLvoid      (APIENTRY *PF_GLGETACTIVEUNIFORMARBPROC)(GLhandleARB programObj, GLuint index, GLsizei maxLength, GLsizei *length, GLint *size, GLenum *type, GLcharARB *name);
    typedef GLvoid      (APIENTRY *PF_GLGETUNIFORMFVARBPROC)(GLhandleARB programObj, GLint location, GLfloat *params);
    typedef GLvoid      (APIENTRY *PF_GLGETUNIFORMIVARBPROC)(GLhandleARB programObj, GLint location, GLint *params);
    typedef GLvoid      (APIENTRY *PF_GLGETSHADERSOURCEARBPROC)(GLhandleARB obj, GLsizei maxLength, GLsizei *length, GLcharARB *source);
#endif


#ifndef GL_ARB_texture_compression
    #define GL_ARB_texture_compression
    #define GL_COMPRESSED_ALPHA_ARB					0x84E9
    #define GL_COMPRESSED_LUMINANCE_ARB				0x84EA
    #define GL_COMPRESSED_LUMINANCE_ALPHA_ARB		0x84EB
    #define GL_COMPRESSED_INTENSITY_ARB				0x84EC
    #define GL_COMPRESSED_RGB_ARB					0x84ED
    #define GL_COMPRESSED_RGBA_ARB					0x84EE
    #define GL_TEXTURE_COMPRESSION_HINT_ARB			0x84EF
    #define GL_TEXTURE_COMPRESSED_IMAGE_SIZE_ARB	0x86A0
    #define GL_TEXTURE_COMPRESSED_ARB				0x86A1
    #define GL_NUM_COMPRESSED_TEXTURE_FORMATS_ARB	0x86A2
    #define GL_COMPRESSED_TEXTURE_FORMATS_ARB		0x86A3

    typedef void (APIENTRY * PF_GLCOMPRESSEDTEXIMAGE1DARBPROC)(GLenum target, GLint level, GLenum internalFormat, GLsizei width,
				    GLint border, GLsizei imageSize, const GLvoid *data);
    typedef void (APIENTRY * PF_GLCOMPRESSEDTEXIMAGE2DARBPROC)(GLenum target, GLint level, GLenum internalFormat, GLsizei width,
				    GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data);
    typedef void (APIENTRY * PF_GLCOMPRESSEDTEXIMAGE3DARBPROC)(GLenum target, GLint level, GLenum internalFormat, GLsizei width,
				    GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const GLvoid *data);
    typedef void (APIENTRY * PF_GLCOMPRESSEDTEXSUBIMAGE1DARBPROC)(GLenum target, GLint level, GLint xoffset, GLsizei width,
				    GLenum format, GLsizei imageSize, const GLvoid *data);
    typedef void (APIENTRY * PF_GLCOMPRESSEDTEXSUBIMAGE2DARBPROC)(GLenum target, GLint level, GLint xoffset, GLint yoffset,
				    GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data);
    typedef void (APIENTRY * PF_GLCOMPRESSEDTEXSUBIMAGE3DARBPROC)(GLenum target, GLint level, GLint xoffset, GLint yoffset,
				    GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid *data);
    typedef void (APIENTRY * PF_GLGETCOMPRESSEDTEXIMAGEARBPROC)(GLenum target, GLint lod, GLvoid *img);
#endif


#ifndef GL_ARB_shadow
    #define GL_ARB_shadow
    #define GL_TEXTURE_COMPARE_MODE_ARB     0x884C
    #define GL_TEXTURE_COMPARE_FUNC_ARB     0X884D
    #define GL_COMPARE_R_TO_TEXTURE_ARB     0x884E
#endif

#ifndef GL_ARB_shadow_ambient
    #define GL_ARB_shadow_ambient
    #define GL_TEXTURE_COMPARE_FAIL_VALUE_ARB 0x80BF
#endif


#ifndef GL_ARB_texture_cube_map
    #define GL_ARB_texture_cube_map
    #define GL_NORMAL_MAP_ARB					0x8511
    #define GL_REFLECTION_MAP_ARB				0x8512
    #define GL_TEXTURE_CUBE_MAP_ARB				0x8513
    #define GL_TEXTURE_BINDING_CUBE_MAP_ARB		0x8514
    #define GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB	0x8515
    #define GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB	0x8516
    #define GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB	0x8517
    #define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB  0x8518
    #define GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB  0x8519
    #define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB  0x851A
    #define GL_PROXY_TEXTURE_CUBE_MAP_ARB		0x851B
    #define GL_MAX_CUBE_MAP_TEXTURE_SIZE_ARB	0x851C
#endif

#ifndef GL_ARB_texture_env_combine
    #define GL_ARB_texture_env_combine
    #define GL_COMBINE_ARB						0x8570
    #define GL_COMBINE_RGB_ARB					0x8571
    #define GL_COMBINE_ALPHA_ARB				0x8572
    #define GL_SOURCE0_RGB_ARB					0x8580
    #define GL_SOURCE1_RGB_ARB					0x8581
    #define GL_SOURCE2_RGB_ARB					0x8582
    #define GL_SOURCE0_ALPHA_ARB				0x8588
    #define GL_SOURCE1_ALPHA_ARB				0x8589
    #define GL_SOURCE2_ALPHA_ARB				0x858A
    #define GL_OPERAND0_RGB_ARB					0x8590
    #define GL_OPERAND1_RGB_ARB					0x8591
    #define GL_OPERAND2_RGB_ARB					0x8592
    #define GL_OPERAND0_ALPHA_ARB				0x8598
    #define GL_OPERAND1_ALPHA_ARB				0x8599
    #define GL_OPERAND2_ALPHA_ARB				0x859A
    #define GL_RGB_SCALE_ARB					0x8573
    #define GL_ADD_SIGNED_ARB					0x8574
    #define GL_INTERPOLATE_ARB					0x8575
    #define GL_CONSTANT_ARB						0x8576
    #define GL_PRIMARY_COLOR_ARB				0x8577
    #define GL_PREVIOUS_ARB						0x8578
    #define GL_SUBTRACT_ARB						0x84E7
#endif

#ifndef GL_ARB_texture_env_dot3
    #define GL_ARB_texture_env_dot3
    #define GL_DOT3_RGB_ARB						0x86AE
    #define GL_DOT3_RGBA_ARB					0x86AF
#endif

#ifndef GL_ARB_transpose_matrix
    #define GL_ARB_transpose_matrix
    #define GL_TRANSPOSE_MODELVIEW_MATRIX_ARB	0x84E3
    #define GL_TRANSPOSE_PROJECTION_MATRIX_ARB  0x84E4
    #define GL_TRANSPOSE_TEXTURE_MATRIX_ARB		0x84E5

    typedef void (APIENTRY * PF_GLLOADTRANSPOSEMATRIXFARBPROC)(const GLfloat m[16]);
    typedef void (APIENTRY * PF_GLLOADTRANSPOSEMATRIXDARBPROC)(const GLdouble m[16]);
    typedef void (APIENTRY * PF_GLMULTTRANSPOSEMATRIXFARBPROC)(const GLfloat m[16]);
    typedef void (APIENTRY * PF_GLMULTTRANSPOSEMATRIXDARBPROC)(const GLdouble m[16]);
#endif

#ifndef GL_ARB_vertex_blend
    #define GL_ARB_vertex_blend
    #define GL_MAX_VERTEX_UNITS_ARB				0x86A4
    #define GL_ACTIVE_VERTEX_UNITS_ARB			0x86A5
    #define GL_WEIGHT_SUM_UNITY_ARB				0x86A6
    #define GL_VERTEX_BLEND_ARB					0x86A7
    #define GL_CURRENT_WEIGHT_ARB				0x86A8
    #define GL_WEIGHT_ARRAY_TYPE_ARB			0x86A9
    #define GL_WEIGHT_ARRAY_STRIDE_ARB			0x86AA
    #define GL_WEIGHT_ARRAY_SIZE_ARB			0x86AB
    #define GL_WEIGHT_ARRAY_POINTER_ARB			0x86AC
    #define GL_WEIGHT_ARRAY_ARB					0x86AD
    #define GL_MODELVIEW0_ARB					0x1700
    #define GL_MODELVIEW1_ARB					0x850a
    #define GL_MODELVIEW2_ARB					0x8722
    #define GL_MODELVIEW3_ARB					0x8723
    #define GL_MODELVIEW4_ARB					0x8724
    #define GL_MODELVIEW5_ARB					0x8725
    #define GL_MODELVIEW6_ARB					0x8726
    #define GL_MODELVIEW7_ARB					0x8727
    #define GL_MODELVIEW8_ARB					0x8728
    #define GL_MODELVIEW9_ARB					0x8729
    #define GL_MODELVIEW10_ARB					0x872A
    #define GL_MODELVIEW11_ARB					0x872B
    #define GL_MODELVIEW12_ARB					0x872C
    #define GL_MODELVIEW13_ARB					0x872D
    #define GL_MODELVIEW14_ARB					0x872E
    #define GL_MODELVIEW15_ARB					0x872F
    #define GL_MODELVIEW16_ARB					0x8730
    #define GL_MODELVIEW17_ARB					0x8731
    #define GL_MODELVIEW18_ARB					0x8732
    #define GL_MODELVIEW19_ARB					0x8733
    #define GL_MODELVIEW20_ARB					0x8734
    #define GL_MODELVIEW21_ARB					0x8735
    #define GL_MODELVIEW22_ARB					0x8736
    #define GL_MODELVIEW23_ARB					0x8737
    #define GL_MODELVIEW24_ARB					0x8738
    #define GL_MODELVIEW25_ARB					0x8739
    #define GL_MODELVIEW26_ARB					0x873A
    #define GL_MODELVIEW27_ARB					0x873B
    #define GL_MODELVIEW28_ARB					0x873C
    #define GL_MODELVIEW29_ARB					0x873D
    #define GL_MODELVIEW30_ARB					0x873E
    #define GL_MODELVIEW31_ARB					0x873F

    typedef void (APIENTRY *PF_GLWEIGHTBVARBPROC)(GLint size, GLbyte *weights);
    typedef void (APIENTRY *PF_GLWEIGHTSVARBPROC)(GLint size, GLshort *weights);
    typedef void (APIENTRY *PF_GLWEIGHTIVARBPROC)(GLint size, GLint *weights);
    typedef void (APIENTRY *PF_GLWEIGHTFVARBPROC)(GLint size, GLfloat *weights);
    typedef void (APIENTRY *PF_GLWEIGHTDVARBPROC)(GLint size, GLdouble *weights);
    typedef void (APIENTRY *PF_GLWEIGHTUBVARBPROC)(GLint size, GLubyte *weights);
    typedef void (APIENTRY *PF_GLWEIGHTUSVARBPROC)(GLint size, GLushort *weights);
    typedef void (APIENTRY *PF_GLWEIGHTUIVARBPROC)(GLint size, GLuint *weights);

    typedef void (APIENTRY *PF_GLWEIGHTPOINTERARBPROC)(GLint size, GLenum type, GLsizei stride, GLvoid *pointer);
    typedef void (APIENTRY *PF_GLVERTEXBLENDARBPROC)(GLint count);
#endif


#ifndef GL_ARB_vertex_buffer_object
    #define GL_ARB_vertex_buffer_object
    #define BUFFER_OFFSET(i) ((char *) NULL + (i))

    #define GL_ARRAY_BUFFER_ARB                             0x8892
    #define GL_ELEMENT_ARRAY_BUFFER_ARB                     0x8893
    #define GL_ELEMENT_ARRAY_BUFFER_ARB                     0x8893
    #define GL_ARRAY_BUFFER_BINDING_ARB                     0x8894
    #define GL_ELEMENT_ARRAY_BUFFER_BINDING_ARB             0x8895
    #define GL_VERTEX_ARRAY_BUFFER_BINDING_ARB              0x8896
    #define GL_NORMAL_ARRAY_BUFFER_BINDING_ARB              0x8897
    #define GL_COLOR_ARRAY_BUFFER_BINDING_ARB               0x8898
    #define GL_INDEX_ARRAY_BUFFER_BINDING_ARB               0x8899
    #define GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING_ARB       0x889A
    #define GL_EDGE_FLAG_ARRAY_BUFFER_BINDING_ARB           0x889B
    #define GL_SECONDARY_COLOR_ARRAY_BUFFER_BINDING_ARB     0x889C
    #define GL_FOG_COORDINATE_ARRAY_BUFFER_BINDING_ARB      0x889D
    #define GL_WEIGHT_ARRAY_BUFFER_BINDING_ARB              0x889E
    #define GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING_ARB       0x889F
    #define GL_STREAM_DRAW_ARB                              0x88E0
    #define GL_STREAM_READ_ARB                              0x88E1
    #define GL_STREAM_COPY_ARB                              0x88E2
    #define GL_STATIC_DRAW_ARB                              0x88E4
    #define GL_STATIC_READ_ARB                              0x88E5
    #define GL_STATIC_COPY_ARB                              0x88E6
    #define GL_DYNAMIC_DRAW_ARB                             0x88E8
    #define GL_DYNAMIC_READ_ARB                             0x88E9
    #define GL_DYNAMIC_COPY_ARB                             0x88EA
    #define GL_READ_ONLY_ARB                                0x88B8
    #define GL_WRITE_ONLY_ARB                               0x88B9
    #define GL_READ_WRITE_ARB                               0x88BA
    #define GL_BUFFER_SIZE_ARB                              0x8764
    #define GL_BUFFER_USAGE_ARB                             0x8765
    #define GL_BUFFER_ACCESS_ARB                            0x88BB
    #define GL_BUFFER_MAPPED_ARB                            0x88BC
    #define GL_BUFFER_MAP_POINTER_ARB                       0x88BD

    typedef void (APIENTRY * PF_GLBINDBUFFERARBPROC) (GLenum target, GLuint buffer);
    typedef void (APIENTRY * PF_GLDELETEBUFFERSARBPROC) (GLsizei n, const GLuint *buffers);
    typedef void (APIENTRY * PF_GLGENBUFFERSARBPROC) (GLsizei n, GLuint *buffers);
    typedef GLboolean (APIENTRY * PF_GLISBUFFERARBPROC) (GLuint buffer);

    typedef void (APIENTRY * PF_GLBUFFERDATAARBPROC) (GLenum target, GLsizeiptrARB size, const void *data, GLenum usage);
    typedef void (APIENTRY * PF_GLBUFFERSUBDATAARBPROC) (GLenum target, GLintptrARB offset, GLsizeiptrARB size, const void *data);
    typedef void (APIENTRY * PF_GLGETBUFFERSUBDATAARBPROC) (GLenum target, GLintptrARB offset, GLsizeiptrARB size, void *data);

    typedef void *(APIENTRY * PF_GLMAPBUFFERARBPROC) (GLenum target, GLenum access);
    typedef GLboolean (APIENTRY * PF_GLUNMAPBUFFERARBPROC) (GLenum target);

    typedef void (APIENTRY * PF_GLGETBUFFERPARAMETERIVARBPROC) (GLenum target, GLenum pname, GLint *params);
    typedef void (APIENTRY * PF_GLGETBUFFERPOINTERVARBPROC) (GLenum target, GLenum pname, void **params);
#endif

#ifndef GL_ARB_vertex_program
    #define GL_ARB_vertex_program

    #define GL_VERTEX_PROGRAM_ARB                                   0x8620
    #define GL_VERTEX_PROGRAM_POINT_SIZE_ARB                        0x8642
    #define GL_VERTEX_PROGRAM_TWO_SIDE_ARB                          0x8643
    #define GL_COLOR_SUM_ARB                                        0x8458
    #define GL_PROGRAM_FORMAT_ASCII_ARB                             0x8875
    #define GL_VERTEX_ATTRIB_ARRAY_ENABLED_ARB                      0x8622
    #define GL_VERTEX_ATTRIB_ARRAY_SIZE_ARB                         0x8623
    #define GL_VERTEX_ATTRIB_ARRAY_STRIDE_ARB                       0x8624
    #define GL_VERTEX_ATTRIB_ARRAY_TYPE_ARB                         0x8625
    #define GL_VERTEX_ATTRIB_ARRAY_NORMALIZED_ARB                   0x886A
    #define GL_CURRENT_VERTEX_ATTRIB_ARB                            0x8626
    #define GL_VERTEX_ATTRIB_ARRAY_POINTER_ARB                      0x8645
    #define GL_PROGRAM_LENGTH_ARB                                   0x8627
    #define GL_PROGRAM_FORMAT_ARB                                   0x8876
    #define GL_PROGRAM_BINDING_ARB                                  0x8677
    #define GL_PROGRAM_INSTRUCTIONS_ARB                             0x88A0
    #define GL_MAX_PROGRAM_INSTRUCTIONS_ARB                         0x88A1
    #define GL_PROGRAM_NATIVE_INSTRUCTIONS_ARB                      0x88A2
    #define GL_MAX_PROGRAM_NATIVE_INSTRUCTIONS_ARB                  0x88A3
    #define GL_PROGRAM_TEMPORARIES_ARB                              0x88A4
    #define GL_MAX_PROGRAM_TEMPORARIES_ARB                          0x88A5
    #define GL_PROGRAM_NATIVE_TEMPORARIES_ARB                       0x88A6
    #define GL_MAX_PROGRAM_NATIVE_TEMPORARIES_ARB                   0x88A7
    #define GL_PROGRAM_PARAMETERS_ARB                               0x88A8
    #define GL_MAX_PROGRAM_PARAMETERS_ARB                           0x88A9
    #define GL_PROGRAM_NATIVE_PARAMETERS_ARB                        0x88AA
    #define GL_MAX_PROGRAM_NATIVE_PARAMETERS_ARB                    0x88AB
    #define GL_PROGRAM_ATTRIBS_ARB                                  0x88AC
    #define GL_MAX_PROGRAM_ATTRIBS_ARB                              0x88AD
    #define GL_PROGRAM_NATIVE_ATTRIBS_ARB                           0x88AE
    #define GL_MAX_PROGRAM_NATIVE_ATTRIBS_ARB                       0x88AF
    #define GL_PROGRAM_ADDRESS_REGISTERS_ARB                        0x88B0
    #define GL_MAX_PROGRAM_ADDRESS_REGISTERS_ARB                    0x88B1
    #define GL_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB                 0x88B2
    #define GL_MAX_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB             0x88B3
    #define GL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB                     0x88B4
    #define GL_MAX_PROGRAM_ENV_PARAMETERS_ARB                       0x88B5
    #define GL_PROGRAM_UNDER_NATIVE_LIMITS_ARB                      0x88B6
    #define GL_PROGRAM_STRING_ARB                                   0x8628
    #define GL_PROGRAM_ERROR_POSITION_ARB                           0x864B
    #define GL_CURRENT_MATRIX_ARB                                   0x8641
    #define GL_TRANSPOSE_CURRENT_MATRIX_ARB                         0x88B7
    #define GL_CURRENT_MATRIX_STACK_DEPTH_ARB                       0x8640
    #define GL_MAX_VERTEX_ATTRIBS_ARB                               0x8869
    #define GL_MAX_PROGRAM_MATRICES_ARB                             0x862F
    #define GL_MAX_PROGRAM_MATRIX_STACK_DEPTH_ARB                   0x862E
    #define GL_PROGRAM_ERROR_STRING_ARB                             0x8874
    #define GL_MATRIX0_ARB                                          0x88C0
    #define GL_MATRIX1_ARB                                          0x88C1
    #define GL_MATRIX2_ARB                                          0x88C2
    #define GL_MATRIX3_ARB                                          0x88C3
    #define GL_MATRIX4_ARB                                          0x88C4
    #define GL_MATRIX5_ARB                                          0x88C5
    #define GL_MATRIX6_ARB                                          0x88C6
    #define GL_MATRIX7_ARB                                          0x88C7
    #define GL_MATRIX8_ARB                                          0x88C8
    #define GL_MATRIX9_ARB                                          0x88C9
    #define GL_MATRIX10_ARB                                         0x88CA
    #define GL_MATRIX11_ARB                                         0x88CB
    #define GL_MATRIX12_ARB                                         0x88CC
    #define GL_MATRIX13_ARB                                         0x88CD
    #define GL_MATRIX14_ARB                                         0x88CE
    #define GL_MATRIX15_ARB                                         0x88CF
    #define GL_MATRIX16_ARB                                         0x88D0
    #define GL_MATRIX17_ARB                                         0x88D1
    #define GL_MATRIX18_ARB                                         0x88D2
    #define GL_MATRIX19_ARB                                         0x88D3
    #define GL_MATRIX20_ARB                                         0x88D4
    #define GL_MATRIX21_ARB                                         0x88D5
    #define GL_MATRIX22_ARB                                         0x88D6
    #define GL_MATRIX23_ARB                                         0x88D7
    #define GL_MATRIX24_ARB                                         0x88D8
    #define GL_MATRIX25_ARB                                         0x88D9
    #define GL_MATRIX26_ARB                                         0x88DA
    #define GL_MATRIX27_ARB                                         0x88DB
    #define GL_MATRIX28_ARB                                         0x88DC
    #define GL_MATRIX29_ARB                                         0x88DD
    #define GL_MATRIX30_ARB                                         0x88DE
    #define GL_MATRIX31_ARB                                         0x88DF

    typedef void (APIENTRY * PF_GLVERTEXATTRIB1SARBPROC) (GLuint index, GLshort x);
    typedef void (APIENTRY * PF_GLVERTEXATTRIB1FARBPROC) (GLuint index, GLfloat x);
    typedef void (APIENTRY * PF_GLVERTEXATTRIB1DARBPROC) (GLuint index, GLdouble x);
    typedef void (APIENTRY * PF_GLVERTEXATTRIB2SARBPROC) (GLuint index, GLshort x, GLshort y);
    typedef void (APIENTRY * PF_GLVERTEXATTRIB2FARBPROC) (GLuint index, GLfloat x, GLfloat y);
    typedef void (APIENTRY * PF_GLVERTEXATTRIB2DARBPROC) (GLuint index, GLdouble x, GLdouble y);
    typedef void (APIENTRY * PF_GLVERTEXATTRIB3SARBPROC) (GLuint index, GLshort x, GLshort y, GLshort z);
    typedef void (APIENTRY * PF_GLVERTEXATTRIB3FARBPROC) (GLuint index, GLfloat x, GLfloat y, GLfloat z);
    typedef void (APIENTRY * PF_GLVERTEXATTRIB3DARBPROC) (GLuint index, GLdouble x, GLdouble y, GLdouble z);
    typedef void (APIENTRY * PF_GLVERTEXATTRIB4SARBPROC) (GLuint index, GLshort x, GLshort y, GLshort z, GLshort w);
    typedef void (APIENTRY * PF_GLVERTEXATTRIB4FARBPROC) (GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
    typedef void (APIENTRY * PF_GLVERTEXATTRIB4DARBPROC) (GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
    typedef void (APIENTRY * PF_GLVERTEXATTRIB4NUBARBPROC) (GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w);
    typedef void (APIENTRY * PF_GLVERTEXATTRIB1SVARBPROC) (GLuint index, const GLshort *v);
    typedef void (APIENTRY * PF_GLVERTEXATTRIB1FVARBPROC) (GLuint index, const GLfloat *v);
    typedef void (APIENTRY * PF_GLVERTEXATTRIB1DVARBPROC) (GLuint index, const GLdouble *v);
    typedef void (APIENTRY * PF_GLVERTEXATTRIB2SVARBPROC) (GLuint index, const GLshort *v);
    typedef void (APIENTRY * PF_GLVERTEXATTRIB2FVARBPROC) (GLuint index, const GLfloat *v);
    typedef void (APIENTRY * PF_GLVERTEXATTRIB2DVARBPROC) (GLuint index, const GLdouble *v);
    typedef void (APIENTRY * PF_GLVERTEXATTRIB3SVARBPROC) (GLuint index, const GLshort *v);
    typedef void (APIENTRY * PF_GLVERTEXATTRIB3FVARBPROC) (GLuint index, const GLfloat *v);
    typedef void (APIENTRY * PF_GLVERTEXATTRIB3DVARBPROC) (GLuint index, const GLdouble *v);
    typedef void (APIENTRY * PF_GLVERTEXATTRIB4BVARBPROC) (GLuint index, const GLbyte *v);
    typedef void (APIENTRY * PF_GLVERTEXATTRIB4SVARBPROC) (GLuint index, const GLshort *v);
    typedef void (APIENTRY * PF_GLVERTEXATTRIB4IVARBPROC) (GLuint index, const GLint *v);
    typedef void (APIENTRY * PF_GLVERTEXATTRIB4UBVARBPROC) (GLuint index, const GLubyte *v);
    typedef void (APIENTRY * PF_GLVERTEXATTRIB4USVARBPROC) (GLuint index, const GLushort *v);
    typedef void (APIENTRY * PF_GLVERTEXATTRIB4UIVARBPROC) (GLuint index, const GLuint *v);
    typedef void (APIENTRY * PF_GLVERTEXATTRIB4FVARBPROC) (GLuint index, const GLfloat *v);
    typedef void (APIENTRY * PF_GLVERTEXATTRIB4DVARBPROC) (GLuint index, const GLdouble *v);
    typedef void (APIENTRY * PF_GLVERTEXATTRIB4NBVARBPROC) (GLuint index, const GLbyte *v);
    typedef void (APIENTRY * PF_GLVERTEXATTRIB4NSVARBPROC) (GLuint index, const GLshort *v);
    typedef void (APIENTRY * PF_GLVERTEXATTRIB4NIVARBPROC) (GLuint index, const GLint *v);
    typedef void (APIENTRY * PF_GLVERTEXATTRIB4NUBVARBPROC) (GLuint index, const GLubyte *v);
    typedef void (APIENTRY * PF_GLVERTEXATTRIB4NUSVARBPROC) (GLuint index, const GLushort *v);
    typedef void (APIENTRY * PF_GLVERTEXATTRIB4NUIVARBPROC) (GLuint index, const GLuint *v);
    typedef void (APIENTRY * PF_GLVERTEXATTRIBPOINTERARBPROC) (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer);
    typedef void (APIENTRY * PF_GLENABLEVERTEXATTRIBARRAYARBPROC) (GLuint index);
    typedef void (APIENTRY * PF_GLDISABLEVERTEXATTRIBARRAYARBPROC) (GLuint index);
    typedef void (APIENTRY * PF_GLPROGRAMSTRINGARBPROC) (GLenum target, GLenum format, GLsizei len, const GLvoid *string);
    typedef void (APIENTRY * PF_GLBINDPROGRAMARBPROC) (GLenum target, GLuint program);
    typedef void (APIENTRY * PF_GLDELETEPROGRAMSARBPROC) (GLsizei n, const GLuint *programs);
    typedef void (APIENTRY * PF_GLGENPROGRAMSARBPROC) (GLsizei n, GLuint *programs);
    typedef void (APIENTRY * PF_GLPROGRAMENVPARAMETER4DARBPROC) (GLenum target, GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
    typedef void (APIENTRY * PF_GLPROGRAMENVPARAMETER4DVARBPROC) (GLenum target, GLuint index, const GLdouble *params);
    typedef void (APIENTRY * PF_GLPROGRAMENVPARAMETER4FARBPROC) (GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
    typedef void (APIENTRY * PF_GLPROGRAMENVPARAMETER4FVARBPROC) (GLenum target, GLuint index, const GLfloat *params);
    typedef void (APIENTRY * PF_GLPROGRAMLOCALPARAMETER4DARBPROC) (GLenum target, GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
    typedef void (APIENTRY * PF_GLPROGRAMLOCALPARAMETER4DVARBPROC) (GLenum target, GLuint index, const GLdouble *params);
    typedef void (APIENTRY * PF_GLPROGRAMLOCALPARAMETER4FARBPROC) (GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
    typedef void (APIENTRY * PF_GLPROGRAMLOCALPARAMETER4FVARBPROC) (GLenum target, GLuint index, const GLfloat *params);
    typedef void (APIENTRY * PF_GLGETPROGRAMENVPARAMETERDVARBPROC) (GLenum target, GLuint index, GLdouble *params);
    typedef void (APIENTRY * PF_GLGETPROGRAMENVPARAMETERFVARBPROC) (GLenum target, GLuint index, GLfloat *params);
    typedef void (APIENTRY * PF_GLGETPROGRAMLOCALPARAMETERDVARBPROC) (GLenum target, GLuint index, GLdouble *params);
    typedef void (APIENTRY * PF_GLGETPROGRAMLOCALPARAMETERFVARBPROC) (GLenum target, GLuint index, GLfloat *params);
    typedef void (APIENTRY * PF_GLGETPROGRAMIVARBPROC) (GLenum target, GLenum pname, GLint *params);
    typedef void (APIENTRY * PF_GLGETPROGRAMSTRINGARBPROC) (GLenum target, GLenum pname, GLvoid *string);
    typedef void (APIENTRY * PF_GLGETVERTEXATTRIBDVARBPROC) (GLuint index, GLenum pname, GLdouble *params);
    typedef void (APIENTRY * PF_GLGETVERTEXATTRIBFVARBPROC) (GLuint index, GLenum pname, GLfloat *params);
    typedef void (APIENTRY * PF_GLGETVERTEXATTRIBIVARBPROC) (GLuint index, GLenum pname, GLint *params);
    typedef void (APIENTRY * PF_GLGETVERTEXATTRIBPOINTERVARBPROC) (GLuint index, GLenum pname, GLvoid **pointer);
    typedef GLboolean (APIENTRY * PF_GLISPROGRAMARBPROC) (GLuint program);
#endif

#ifndef GL_ARB_vertex_shader
    #define GL_ARB_vertex_shader
    #define GL_VERTEX_SHADER_ARB                        0x8B31

    #define GL_MAX_VERTEX_UNIFORM_COMPONENTS_ARB        0x8B4A
    #define GL_MAX_VARYING_FLOATS_ARB                   0x8B4B
    #define GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS_ARB       0x8B4C
    #define GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS_ARB     0x8B4D

    #define GL_OBJECT_ACTIVE_ATTRIBUTES_ARB             0x8B89
    #define GL_OBJECT_ACTIVE_ATTRIBUTE_MAX_LENGTH_ARB   0x8B8A

    typedef GLvoid (APIENTRY *PF_GLBINDATTRIBLOCATIONARBPROC)(GLhandleARB programObj, GLuint index, const GLcharARB *name);
    typedef GLvoid (APIENTRY *PF_GLGETACTIVEATTRIBARBPROC)(GLhandleARB programObj, GLuint index, GLsizei maxLength, GLsizei *length, GLint *size, GLenum *type, GLcharARB *name);
    typedef GLint  (APIENTRY *PF_GLGETATTRIBLOCATIONARBPROC)(GLhandleARB programObj, const GLcharARB *name);
#endif


#ifndef GL_ATI_envmap_bumpmap
    #define GL_ATI_envmap_bumpmap
    #define GL_BUMP_ROT_MATRIX_ATI					   0x8775
    #define GL_BUMP_ROT_MATRIX_SIZE_ATI				   0x8776
    #define GL_BUMP_NUM_TEX_UNITS_ATI				   0x8777
    #define GL_BUMP_TEX_UNITS_ATI					   0x8778
    #define GL_DUDV_ATI								   0x8779
    #define GL_DU8DV8_ATI							   0x877A
    #define GL_BUMP_ENVMAP_ATI						   0x877B
    #define GL_BUMP_TARGET_ATI						   0x877C

    typedef void (APIENTRY * PF_GLTEXBUMPPARAMETERIVATIPROC) (GLenum pname, GLint *param);
    typedef void (APIENTRY * PF_GLTEXBUMPPARAMETERFVATIPROC) (GLenum pname, GLfloat *param);
    typedef void (APIENTRY * PF_GLGETTEXBUMPPARAMETERIVATIPROC) (GLenum pname, GLint *param);
    typedef void (APIENTRY * PF_GLGETTEXBUMPPARAMETERFVATIPROC) (GLenum pname, GLfloat *param);
#endif

#ifndef GL_ATI_fragment_shader
    #define GL_ATI_fragment_shader
    #define GL_FRAGMENT_SHADER_ATI						0x8920
    #define GL_REG_0_ATI								0x8921
    #define GL_REG_1_ATI								0x8922
    #define GL_REG_2_ATI								0x8923
    #define GL_REG_3_ATI								0x8924
    #define GL_REG_4_ATI								0x8925
    #define GL_REG_5_ATI								0x8926
    #define GL_REG_6_ATI								0x8927
    #define GL_REG_7_ATI								0x8928
    #define GL_REG_8_ATI								0x8929
    #define GL_REG_9_ATI								0x892A
    #define GL_REG_10_ATI								0x892B
    #define GL_REG_11_ATI								0x892C
    #define GL_REG_12_ATI								0x892D
    #define GL_REG_13_ATI								0x892E
    #define GL_REG_14_ATI								0x892F
    #define GL_REG_15_ATI								0x8930
    #define GL_REG_16_ATI								0x8931
    #define GL_REG_17_ATI								0x8932
    #define GL_REG_18_ATI								0x8933
    #define GL_REG_19_ATI								0x8934
    #define GL_REG_20_ATI								0x8935
    #define GL_REG_21_ATI								0x8936
    #define GL_REG_22_ATI								0x8937
    #define GL_REG_23_ATI								0x8938
    #define GL_REG_24_ATI								0x8939
    #define GL_REG_25_ATI								0x893A
    #define GL_REG_26_ATI								0x893B
    #define GL_REG_27_ATI								0x893C
    #define GL_REG_28_ATI								0x893D
    #define GL_REG_29_ATI								0x893E
    #define GL_REG_30_ATI								0x893F
    #define GL_REG_31_ATI								0x8940
    #define GL_CON_0_ATI								0x8941
    #define GL_CON_1_ATI								0x8942
    #define GL_CON_2_ATI								0x8943
    #define GL_CON_3_ATI								0x8944
    #define GL_CON_4_ATI								0x8945
    #define GL_CON_5_ATI								0x8946
    #define GL_CON_6_ATI								0x8947
    #define GL_CON_7_ATI								0x8948
    #define GL_CON_8_ATI								0x8949
    #define GL_CON_9_ATI								0x894A
    #define GL_CON_10_ATI								0x894B
    #define GL_CON_11_ATI								0x894C
    #define GL_CON_12_ATI								0x894D
    #define GL_CON_13_ATI								0x894E
    #define GL_CON_14_ATI								0x894F
    #define GL_CON_15_ATI								0x8950
    #define GL_CON_16_ATI								0x8951
    #define GL_CON_17_ATI								0x8952
    #define GL_CON_18_ATI								0x8953
    #define GL_CON_19_ATI								0x8954
    #define GL_CON_20_ATI								0x8955
    #define GL_CON_21_ATI								0x8956
    #define GL_CON_22_ATI								0x8957
    #define GL_CON_23_ATI								0x8958
    #define GL_CON_24_ATI								0x8959
    #define GL_CON_25_ATI								0x895A
    #define GL_CON_26_ATI								0x895B
    #define GL_CON_27_ATI								0x895C
    #define GL_CON_28_ATI								0x895D
    #define GL_CON_29_ATI								0x895E
    #define GL_CON_30_ATI								0x895F
    #define GL_CON_31_ATI								0x8960
    #define GL_MOV_ATI									0x8961
    #define GL_ADD_ATI									0x8963
    #define GL_MUL_ATI									0x8964
    #define GL_SUB_ATI									0x8965
    #define GL_DOT3_ATI									0x8966
    #define GL_DOT4_ATI									0x8967
    #define GL_MAD_ATI									0x8968
    #define GL_LERP_ATI									0x8969
    #define GL_CND_ATI									0x896A
    #define GL_CND0_ATI									0x896B
    #define GL_DOT2_ADD_ATI								0x896C
    #define GL_SECONDARY_INTERPOLATOR_ATI				0x896D
    #define GL_NUM_FRAGMENT_REGISTERS_ATI				0x896E
    #define GL_NUM_FRAGMENT_CONSTANTS_ATI				0x896F
    #define GL_NUM_PASSES_ATI							0x8970
    #define GL_NUM_INSTRUCTIONS_PER_PASS_ATI			0x8971
    #define GL_NUM_INSTRUCTIONS_TOTAL_ATI				0x8972
    #define GL_NUM_INPUT_INTERPOLATOR_COMPONENTS_ATI	0x8973
    #define GL_NUM_LOOPBACK_COMPONENTS_ATI				0x8974
    #define GL_COLOR_ALPHA_PAIRING_ATI					0x8975
    #define GL_SWIZZLE_STR_ATI							0x8976
    #define GL_SWIZZLE_STQ_ATI							0x8977
    #define GL_SWIZZLE_STR_DR_ATI						0x8978
    #define GL_SWIZZLE_STQ_DQ_ATI						0x8979
    #define GL_SWIZZLE_STRQ_ATI							0x897A
    #define GL_SWIZZLE_STRQ_DQ_ATI						0x897B
    #define GL_RED_BIT_ATI								0x00000001
    #define GL_GREEN_BIT_ATI							0x00000002
    #define GL_BLUE_BIT_ATI								0x00000004
    #define GL_2X_BIT_ATI								0x00000001
    #define GL_4X_BIT_ATI								0x00000002
    #define GL_8X_BIT_ATI								0x00000004
    #define GL_HALF_BIT_ATI								0x00000008
    #define GL_QUARTER_BIT_ATI							0x00000010
    #define GL_EIGHTH_BIT_ATI							0x00000020
    #define GL_SATURATE_BIT_ATI							0x00000040
    #define GL_COMP_BIT_ATI								0x00000002
    #define GL_NEGATE_BIT_ATI							0x00000004
    #define GL_BIAS_BIT_ATI								0x00000008


    typedef GLuint (APIENTRY *PF_GLGENFRAGMENTSHADERSATIPROC)(GLuint range);
    typedef GLvoid (APIENTRY *PF_GLBINDFRAGMENTSHADERATIPROC)(GLuint id);
    typedef GLvoid (APIENTRY *PF_GLDELETEFRAGMENTSHADERATIPROC)(GLuint id);
    typedef GLvoid (APIENTRY *PF_GLBEGINFRAGMENTSHADERATIPROC)(GLvoid);
    typedef GLvoid (APIENTRY *PF_GLENDFRAGMENTSHADERATIPROC)(GLvoid);
    typedef GLvoid (APIENTRY *PF_GLPASSTEXCOORDATIPROC)(GLuint dst, GLuint coord, GLenum swizzle);
    typedef GLvoid (APIENTRY *PF_GLSAMPLEMAPATIPROC)(GLuint dst, GLuint interp, GLenum swizzle);

    typedef GLvoid (APIENTRY *PF_GLCOLORFRAGMENTOP1ATIPROC)(GLenum op, GLuint dst, GLuint dstMask, GLuint dstMod, GLuint arg1, GLuint arg1Rep, GLuint arg1Mod);
    typedef GLvoid (APIENTRY *PF_GLCOLORFRAGMENTOP2ATIPROC)(GLenum op, GLuint dst, GLuint dstMask, GLuint dstMod, GLuint arg1, GLuint arg1Rep, GLuint arg1Mod,
														    GLuint arg2, GLuint arg2Rep, GLuint arg2Mod);
    typedef GLvoid (APIENTRY *PF_GLCOLORFRAGMENTOP3ATIPROC)(GLenum op, GLuint dst, GLuint dstMask, GLuint dstMod, GLuint arg1, GLuint arg1Rep,
									       GLuint arg1Mod, GLuint arg2, GLuint arg2Rep, GLuint arg2Mod, GLuint arg3, GLuint arg3Rep, GLuint arg3Mod);

    typedef GLvoid (APIENTRY *PF_GLALPHAFRAGMENTOP1ATIPROC)(GLenum op, GLuint dst, GLuint dstMod, GLuint arg1, GLuint arg1Rep, GLuint arg1Mod);
    typedef GLvoid (APIENTRY *PF_GLALPHAFRAGMENTOP2ATIPROC)(GLenum op, GLuint dst, GLuint dstMod, GLuint arg1, GLuint arg1Rep, GLuint arg1Mod, GLuint arg2,
														    GLuint arg2Rep, GLuint arg2Mod);
    typedef GLvoid (APIENTRY *PF_GLALPHAFRAGMENTOP3ATIPROC)(GLenum op, GLuint dst, GLuint dstMod, GLuint arg1, GLuint arg1Rep, GLuint arg1Mod,
														    GLuint arg2, GLuint arg2Rep, GLuint arg2Mod, GLuint arg3, GLuint arg3Rep, GLuint arg3Mod);
    typedef GLvoid (APIENTRY *PF_GLSETFRAGMENTSHADERCONSTANTATIPROC)(GLuint dst, const GLfloat *value);
#endif


#ifndef GL_ATI_pn_triangles
    #define GL_ATI_pn_triangles
    #define GL_PN_TRIANGLES_ATI							0x87F0
    #define GL_MAX_PN_TRIANGLES_TESSELATION_LEVEL_ATI	0x87F1
    #define GL_PN_TRIANGLES_POINT_MODE_ATI				0x87F2
    #define GL_PN_TRIANGLES_NORMAL_MODE_ATI				0x87F3
    #define GL_PN_TRIANGLES_TESSELATION_LEVEL_ATI		0x87F4
    #define GL_PN_TRIANGLES_POINT_MODE_LINEAR_ATI		0x87F5
    #define GL_PN_TRIANGLES_POINT_MODE_CUBIC_ATI		0x87F6
    #define GL_PN_TRIANGLES_NORMAL_MODE_LINEAR_ATI		0x87F7
    #define GL_PN_TRIANGLES_NORMAL_MODE_QUADRATIC_ATI   0x87F8

    typedef void (APIENTRY *PF_GLPNTRIANGLESIATIPROC)(GLenum pname, GLint param);
    typedef void (APIENTRY *PF_GLPNTRIANGLESFATIPROC)(GLenum pname, GLfloat param);
#endif


#ifndef GL_ATI_texture_compression_3dc
    #define GL_ATI_texture_compression_3dc
    #define GL_COMPRESSED_LUMINANCE_ALPHA_3DC_ATI     0x8837
#endif

#ifndef GL_ATI_texture_float
    #define GL_ATI_texture_float
    #define GL_RGBA_FLOAT32_ATI                 0x8814
    #define GL_RGB_FLOAT32_ATI                  0x8815
    #define GL_ALPHA_FLOAT32_ATI                0x8816
    #define GL_INTENSITY_FLOAT32_ATI            0x8817
    #define GL_LUMINANCE_FLOAT32_ATI            0x8818
    #define GL_LUMINANCE_ALPHA_FLOAT32_ATI      0x8819
    #define GL_RGBA_FLOAT16_ATI                 0x881A
    #define GL_RGB_FLOAT16_ATI                  0x881B
    #define GL_ALPHA_FLOAT16_ATI                0x881C
    #define GL_INTENSITY_FLOAT16_ATI            0x881D
    #define GL_LUMINANCE_FLOAT16_ATI            0x881E
    #define GL_LUMINANCE_ALPHA_FLOAT16_ATI      0x881F
#endif

#ifndef GL_ATI_texture_mirror_once
    #define GL_ATI_texture_mirror_once
    #define GL_MIRROR_CLAMP_ATI					0x8742
    #define GL_MIRROR_CLAMP_TO_EDGE_ATI			0x8743
#endif

#ifndef GL_ATI_vertex_streams
    #define GL_ATI_vertex_streams
    #define GL_MAX_VERTEX_STREAMS_ATI				0x876B
    #define GL_VERTEX_SOURCE_ATI					0x876C
    #define GL_VERTEX_STREAM0_ATI					0x876D
    #define GL_VERTEX_STREAM1_ATI					0x876E
    #define GL_VERTEX_STREAM2_ATI					0x876F
    #define GL_VERTEX_STREAM3_ATI					0x8770
    #define GL_VERTEX_STREAM4_ATI					0x8771
    #define GL_VERTEX_STREAM5_ATI					0x8772
    #define GL_VERTEX_STREAM6_ATI					0x8773
    #define GL_VERTEX_STREAM7_ATI					0x8774

    typedef void (APIENTRY * PF_GLCLIENTACTIVEVERTEXSTREAMATIPROC) (GLenum stream);
    typedef void (APIENTRY * PF_GLVERTEXBLENDENVIATIPROC) (GLenum pname, GLint param);
    typedef void (APIENTRY * PF_GLVERTEXBLENDENVFATIPROC) (GLenum pname, GLfloat param);

    typedef void (APIENTRY * PF_GLVERTEXSTREAM2SATIPROC) (GLenum stream, GLshort x, GLshort y);
    typedef void (APIENTRY * PF_GLVERTEXSTREAM2SVATIPROC) (GLenum stream, const GLshort *v);
    typedef void (APIENTRY * PF_GLVERTEXSTREAM2IATIPROC) (GLenum stream, GLint x, GLint y);
    typedef void (APIENTRY * PF_GLVERTEXSTREAM2IVATIPROC) (GLenum stream, const GLint *v);
    typedef void (APIENTRY * PF_GLVERTEXSTREAM2FATIPROC) (GLenum stream, GLfloat x, GLfloat y);
    typedef void (APIENTRY * PF_GLVERTEXSTREAM2FVATIPROC) (GLenum stream, const GLfloat *v);
    typedef void (APIENTRY * PF_GLVERTEXSTREAM2DATIPROC) (GLenum stream, GLdouble x, GLdouble y);
    typedef void (APIENTRY * PF_GLVERTEXSTREAM2DVATIPROC) (GLenum stream, const GLdouble *v);

    typedef void (APIENTRY * PF_GLVERTEXSTREAM3SATIPROC) (GLenum stream, GLshort x, GLshort y, GLshort z);
    typedef void (APIENTRY * PF_GLVERTEXSTREAM3SVATIPROC) (GLenum stream, const GLshort *v);
    typedef void (APIENTRY * PF_GLVERTEXSTREAM3IATIPROC) (GLenum stream, GLint x, GLint y, GLint z);
    typedef void (APIENTRY * PF_GLVERTEXSTREAM3IVATIPROC) (GLenum stream, const GLint *v);
    typedef void (APIENTRY * PF_GLVERTEXSTREAM3FATIPROC) (GLenum stream, GLfloat x, GLfloat y, GLfloat z);
    typedef void (APIENTRY * PF_GLVERTEXSTREAM3FVATIPROC) (GLenum stream, const GLfloat *v);
    typedef void (APIENTRY * PF_GLVERTEXSTREAM3DATIPROC) (GLenum stream, GLdouble x, GLdouble y, GLdouble z);
    typedef void (APIENTRY * PF_GLVERTEXSTREAM3DVATIPROC) (GLenum stream, const GLdouble *v);

    typedef void (APIENTRY * PF_GLVERTEXSTREAM4SATIPROC) (GLenum stream, GLshort x, GLshort y, GLshort z, GLshort w);
    typedef void (APIENTRY * PF_GLVERTEXSTREAM4SVATIPROC) (GLenum stream, const GLshort *v);
    typedef void (APIENTRY * PF_GLVERTEXSTREAM4IATIPROC) (GLenum stream, GLint x, GLint y, GLint z, GLint w);
    typedef void (APIENTRY * PF_GLVERTEXSTREAM4IVATIPROC) (GLenum stream, const GLint *v);
    typedef void (APIENTRY * PF_GLVERTEXSTREAM4FATIPROC) (GLenum stream, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
    typedef void (APIENTRY * PF_GLVERTEXSTREAM4FVATIPROC) (GLenum stream, const GLfloat *v);
    typedef void (APIENTRY * PF_GLVERTEXSTREAM4DATIPROC) (GLenum stream, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
    typedef void (APIENTRY * PF_GLVERTEXSTREAM4DVATIPROC) (GLenum stream, const GLdouble *v);

    typedef void (APIENTRY * PF_GLNORMALSTREAM3BATIPROC) (GLenum stream, GLbyte x, GLbyte y, GLbyte z);
    typedef void (APIENTRY * PF_GLNORMALSTREAM3BVATIPROC) (GLenum stream, const GLbyte *v);
    typedef void (APIENTRY * PF_GLNORMALSTREAM3SATIPROC) (GLenum stream, GLshort x, GLshort y, GLshort z);
    typedef void (APIENTRY * PF_GLNORMALSTREAM3SVATIPROC) (GLenum stream, const GLshort *v);
    typedef void (APIENTRY * PF_GLNORMALSTREAM3IATIPROC) (GLenum stream, GLint x, GLint y, GLint z);
    typedef void (APIENTRY * PF_GLNORMALSTREAM3IVATIPROC) (GLenum stream, const GLint *v);
    typedef void (APIENTRY * PF_GLNORMALSTREAM3FATIPROC) (GLenum stream, GLfloat x, GLfloat y, GLfloat z);
    typedef void (APIENTRY * PF_GLNORMALSTREAM3FVATIPROC) (GLenum stream, const GLfloat *v);
    typedef void (APIENTRY * PF_GLNORMALSTREAM3DATIPROC) (GLenum stream, GLdouble x, GLdouble y, GLdouble z);
    typedef void (APIENTRY * PF_GLNORMALSTREAM3DVATIPROC) (GLenum stream, const GLdouble *v);
#endif

#ifndef GL_EXT_blend_color
    #define GL_EXT_blend_color
    #define GL_CONSTANT_COLOR_EXT               0x8001
    #define GL_ONE_MINUS_CONSTANT_COLOR_EXT     0x8002
    #define GL_CONSTANT_ALPHA_EXT               0x8003
    #define GL_ONE_MINUS_CONSTANT_ALPHA_EXT     0x8004
    #define GL_BLEND_COLOR_EXT                  0x8005

    typedef void (APIENTRY * PF_GLBLENDCOLOREXTPROC) (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
#endif


#ifndef GL_EXT_blend_func_separate
    #define GL_EXT_blend_func_separate
    #define GL_BLEND_DST_RGB_EXT				0x80C8
    #define GL_BLEND_SRC_RGB_EXT				0x80C9
    #define GL_BLEND_DST_ALPHA_EXT				0x80CA
    #define GL_BLEND_SRC_ALPHA_EXT				0x80CB

    typedef void (APIENTRY * PF_GLBLENDFUNCSEPARATEEXTPROC) (GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha);
#endif

#ifndef GL_EXT_blend_minmax
    #define GL_EXT_blend_minmax
    #define GL_FUNC_ADD_EXT						0x8006
    #define GL_MIN_EXT							0x8007
    #define GL_MAX_EXT							0x8008
    #define GL_BLEND_EQUATION_EXT				0x8009

    typedef void (APIENTRY * PF_GLBLENDEQUATIONEXTPROC) (GLenum mode);
#endif


#ifndef GL_EXT_blend_subtract
    #define GL_EXT_blend_subtract
    #define GL_FUNC_SUBTRACT_EXT					0x800A
    #define GL_FUNC_REVERSE_SUBTRACT_EXT			0x800B
#endif

#ifndef GL_EXT_draw_range_elements
    #define GL_EXT_draw_range_elements
    #define GL_MAX_ELEMENTS_VERTICES_EXT         0x80E8
    #define GL_MAX_ELEMENTS_INDICES_EXT          0x80E9

    typedef void (APIENTRY * PF_GLDRAWRANGEELEMENTSEXTPROC) (GLenum  mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid *indices);
#endif


#ifndef GL_EXT_fog_coord
    #define GL_EXT_fog_coord
    #define GL_FOG_COORDINATE_SOURCE_EXT         0x8450
    #define GL_FOG_COORDINATE_EXT                0x8451
    #define GL_FRAGMENT_DEPTH_EXT                0x8452
    #define GL_CURRENT_FOG_COORDINATE_EXT        0x8453
    #define GL_FOG_COORDINATE_ARRAY_TYPE_EXT     0x8454
    #define GL_FOG_COORDINATE_ARRAY_STRIDE_EXT   0x8455
    #define GL_FOG_COORDINATE_ARRAY_POINTER_EXT  0x8456
    #define GL_FOG_COORDINATE_ARRAY_EXT          0x8457

    typedef void (APIENTRY * PF_GLFOGCOORDFEXTPROC) (GLfloat f);
    typedef void (APIENTRY * PF_GLFOGCOORDDEXTPROC) (GLdouble f);
    typedef void (APIENTRY * PF_GLFOGCOORDFVEXTPROC) (const GLfloat *v);
    typedef void (APIENTRY * PF_GLFOGCOORDDVEXTPROC) (const GLdouble *v);
    typedef void (APIENTRY * PF_GLFOGCOORDPOINTEREXTPROC) (GLenum type, GLsizei stride, const GLvoid *pointer);
#endif


#ifndef GL_EXT_packed_pixels
    #define GL_EXT_packed_pixels
    #define GL_UNSIGNED_BYTE_3_3_2_EXT          0x8032
    #define GL_UNSIGNED_SHORT_4_4_4_4_EXT       0x8033
    #define GL_UNSIGNED_SHORT_5_5_5_1_EXT       0x8034
    #define GL_UNSIGNED_INT_8_8_8_8_EXT         0x8035
    #define GL_UNSIGNED_INT_10_10_10_2_EXT      0x8036
#endif

#ifndef GL_EXT_polygon_offset
    #define GL_EXT_polygon_offset
    #define GL_POLYGON_OFFSET_EXT               0x8037
    #define GL_POLYGON_OFFSET_FACTOR_EXT        0x8038
    #define GL_POLYGON_OFFSET_BIAS_EXT          0x8039

    typedef void (APIENTRY * PF_GLPOLYGONOFFSETEXTPROC) (GLfloat factor, GLfloat bias);
#endif


#ifndef GL_EXT_separate_specular_color
    #define GL_EXT_separate_specular_color
    #define GL_LIGHT_MODEL_COLOR_CONTROL_EXT	0x81F8
    #define GL_SINGLE_COLOR_EXT					0x81F9
    #define GL_SEPARATE_SPECULAR_COLOR_EXT		0x81FA
#endif


#ifndef GL_EXT_stencil_wrap
    #define GL_EXT_stencil_wrap
    #define GL_INCR_WRAP_EXT					0x8507
    #define GL_DECR_WRAP_EXT					0x8508
#endif


#ifndef GL_EXT_texture3D
    #define GL_EXT_texture3D
    #define GL_TEXTURE_BINDING_3D_EXT		    0x806A
    #define GL_PACK_SKIP_IMAGES_EXT             0x806B
    #define GL_PACK_IMAGE_HEIGHT_EXT            0x806C
    #define GL_UNPACK_SKIP_IMAGES_EXT           0x806D
    #define GL_UNPACK_IMAGE_HEIGHT_EXT          0x806E
    #define GL_TEXTURE_3D_EXT                   0x806F
    #define GL_PROXY_TEXTURE_3D_EXT             0x8070
    #define GL_TEXTURE_DEPTH_EXT                0x8071
    #define GL_TEXTURE_WRAP_R_EXT               0x8072
    #define GL_MAX_3D_TEXTURE_SIZE_EXT          0x8073

    #ifndef GL_TEXTURE_WRAP_R
    #define GL_TEXTURE_WRAP_R GL_TEXTURE_WRAP_R_EXT
#endif

typedef void (APIENTRY * PF_GLTEXIMAGE3DEXTPROC)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth,
												 GLint border, GLenum format, GLenum type,  const GLvoid* pixels);

#endif


#ifndef GL_EXT_texture_compression_s3tc
    #define GL_EXT_texture_compression_s3tc
    #define GL_COMPRESSED_RGB_S3TC_DXT1_EXT      0x83F0
    #define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT     0x83F1
    #define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT     0x83F2
    #define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT     0x83F3
#endif

#ifndef GL_EXT_texture_edge_clamp
    #define GL_EXT_texture_edge_clamp
    #define GL_CLAMP_TO_EDGE_EXT                0x812F
	#define GL_CLAMP_TO_EDGE					0x812F
#endif

#ifndef GL_EXT_texture_filter_anisotropic
    #define GL_EXT_texture_filter_anisotropic
    #define GL_TEXTURE_MAX_ANISOTROPY_EXT		0x84FE
    #define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT	0x84FF
#endif


#ifndef GL_EXT_texture_lod_bias
    #define GL_EXT_texture_lod_bias
    #define GL_TEXTURE_FILTER_CONTROL_EXT		0x8500
    #define GL_TEXTURE_LOD_BIAS_EXT				0x8501
    #define GL_MAX_TEXTURE_LOD_BIAS_EXT			0x84FD
#endif


#ifndef GL_EXT_texture_rectangle
    #define GL_EXT_texture_rectangle
    #define GL_TEXTURE_RECTANGLE_EXT            0x85B3
    #define GL_TEXTURE_BINDING_RECTANGLE_EXT    0x85B4
    #define GL_PROXY_TEXTURE_RECTANGLE_EXT      0x85B5
    #define GL_MAX_RECTANGLE_TEXTURE_SIZE_EXT   0x85B6
#endif

#ifndef GL_HP_occlusion_test
    #define GL_HP_occlusion_test
    #define GL_OCCLUSION_TEST_HP              0x8165
    #define GL_OCCLUSION_TEST_RESULT_HP       0x8166
#endif


#ifndef GL_NV_occlusion_query
    #define GL_NV_occlusion_query
    #define GL_PIXEL_COUNTER_BITS_NV           0x8864
    #define GL_CURRENT_OCCLUSION_QUERY_ID_NV   0x8865
    #define GL_PIXEL_COUNT_NV                  0x8866
    #define GL_PIXEL_COUNT_AVAILABLE_NV        0x8867

    typedef GLvoid    (APIENTRY * PF_GLGENOCCLUSIONQUERIESNVPROC) (GLsizei n, GLuint *ids);
    typedef GLvoid    (APIENTRY * PF_GLDELETEOCCLUSIONQUERIESNVPROC) (GLsizei n, const GLuint *ids);
    typedef GLboolean (APIENTRY * PF_GLISOCCLUSIONQUERYNVPROC) (GLuint id);
    typedef GLvoid    (APIENTRY * PF_GLBEGINOCCLUSIONQUERYNVPROC) (GLuint id);
    typedef GLvoid    (APIENTRY * PF_GLENDOCCLUSIONQUERYNVPROC) (GLvoid);
    typedef GLvoid    (APIENTRY * PF_GLGETOCCLUSIONQUERYIVNVPROC) (GLuint id, GLenum pname, GLint *params);
    typedef GLvoid    (APIENTRY * PF_GLGETOCCLUSIONQUERYUIVNVPROC) (GLuint id, GLenum pname, GLuint *params);
#endif

#ifndef GL_NV_register_combiners
    #define GL_NV_register_combiners
    #define GL_REGISTER_COMBINERS_NV          0x8522
    #define GL_VARIABLE_A_NV                  0x8523
    #define GL_VARIABLE_B_NV                  0x8524
    #define GL_VARIABLE_C_NV                  0x8525
    #define GL_VARIABLE_D_NV                  0x8526
    #define GL_VARIABLE_E_NV                  0x8527
    #define GL_VARIABLE_F_NV                  0x8528
    #define GL_VARIABLE_G_NV                  0x8529
    #define GL_CONSTANT_COLOR0_NV             0x852A
    #define GL_CONSTANT_COLOR1_NV             0x852B
    #define GL_PRIMARY_COLOR_NV               0x852C
    #define GL_SECONDARY_COLOR_NV             0x852D
    #define GL_SPARE0_NV                      0x852E
    #define GL_SPARE1_NV                      0x852F
    #define GL_DISCARD_NV                     0x8530
    #define GL_E_TIMES_F_NV                   0x8531
    #define GL_SPARE0_PLUS_SECONDARY_COLOR_NV 0x8532
    #define GL_UNSIGNED_IDENTITY_NV           0x8536
    #define GL_UNSIGNED_INVERT_NV             0x8537
    #define GL_EXPAND_NORMAL_NV               0x8538
    #define GL_EXPAND_NEGATE_NV               0x8539
    #define GL_HALF_BIAS_NORMAL_NV            0x853A
    #define GL_HALF_BIAS_NEGATE_NV            0x853B
    #define GL_SIGNED_IDENTITY_NV             0x853C
    #define GL_SIGNED_NEGATE_NV               0x853D
    #define GL_SCALE_BY_TWO_NV                0x853E
    #define GL_SCALE_BY_FOUR_NV               0x853F
    #define GL_SCALE_BY_ONE_HALF_NV           0x8540
    #define GL_BIAS_BY_NEGATIVE_ONE_HALF_NV   0x8541
    #define GL_COMBINER_INPUT_NV              0x8542
    #define GL_COMBINER_MAPPING_NV            0x8543
    #define GL_COMBINER_COMPONENT_USAGE_NV    0x8544
    #define GL_COMBINER_AB_DOT_PRODUCT_NV     0x8545
    #define GL_COMBINER_CD_DOT_PRODUCT_NV     0x8546
    #define GL_COMBINER_MUX_SUM_NV            0x8547
    #define GL_COMBINER_SCALE_NV              0x8548
    #define GL_COMBINER_BIAS_NV               0x8549
    #define GL_COMBINER_AB_OUTPUT_NV          0x854A
    #define GL_COMBINER_CD_OUTPUT_NV          0x854B
    #define GL_COMBINER_SUM_OUTPUT_NV         0x854C
    #define GL_MAX_GENERAL_COMBINERS_NV       0x854D
    #define GL_NUM_GENERAL_COMBINERS_NV       0x854E
    #define GL_COLOR_SUM_CLAMP_NV             0x854F
    #define GL_COMBINER0_NV                   0x8550
    #define GL_COMBINER1_NV                   0x8551
    #define GL_COMBINER2_NV                   0x8552
    #define GL_COMBINER3_NV                   0x8553
    #define GL_COMBINER4_NV                   0x8554
    #define GL_COMBINER5_NV                   0x8555
    #define GL_COMBINER6_NV                   0x8556
    #define GL_COMBINER7_NV                   0x8557

    typedef void (APIENTRY * PF_GLCOMBINERPARAMETERFVNVPROC) (GLenum pname, const GLfloat *params);
    typedef void (APIENTRY * PF_GLCOMBINERPARAMETERFNVPROC) (GLenum pname, GLfloat param);
    typedef void (APIENTRY * PF_GLCOMBINERPARAMETERIVNVPROC) (GLenum pname, const GLint *params);
    typedef void (APIENTRY * PF_GLCOMBINERPARAMETERINVPROC) (GLenum pname, GLint param);

    typedef void (APIENTRY * PF_GLCOMBINERINPUTNVPROC) (GLenum stage, GLenum portion, GLenum variable, GLenum input, GLenum mapping, GLenum componentUsage);
    typedef void (APIENTRY * PF_GLCOMBINEROUTPUTNVPROC) (GLenum stage, GLenum portion, GLenum abOutput, GLenum cdOutput, GLenum sumOutput, GLenum scale, GLenum bias, GLboolean abDotProduct, GLboolean cdDotProduct, GLboolean muxSum);
    typedef void (APIENTRY * PF_GLFINALCOMBINERINPUTNVPROC) (GLenum variable, GLenum input, GLenum mapping, GLenum componentUsage);

    typedef void (APIENTRY * PF_GLGETCOMBINERINPUTPARAMETERFVNVPROC) (GLenum stage, GLenum portion, GLenum variable, GLenum pname, GLfloat *params);
    typedef void (APIENTRY * PF_GLGETCOMBINERINPUTPARAMETERIVNVPROC) (GLenum stage, GLenum portion, GLenum variable, GLenum pname, GLint *params);
    typedef void (APIENTRY * PF_GLGETCOMBINEROUTPUTPARAMETERFVNVPROC) (GLenum stage, GLenum portion, GLenum pname, GLfloat *params);
    typedef void (APIENTRY * PF_GLGETCOMBINEROUTPUTPARAMETERIVNVPROC) (GLenum stage, GLenum portion, GLenum pname, GLint *params);

    typedef void (APIENTRY * PF_GLGETFINALCOMBINERINPUTPARAMETERFVNVPROC) (GLenum variable, GLenum pname, GLfloat *params);
    typedef void (APIENTRY * PF_GLGETFINALCOMBINERINPUTPARAMETERIVNVPROC) (GLenum variable, GLenum pname, GLint *params);
#endif


#ifndef GL_NV_texture_env_combine4
    #define GL_NV_texture_env_combine4
    #define GL_COMBINE4_NV                      0x8503

    #define GL_SOURCE3_RGB_NV                   0x8583
    #define GL_SOURCE3_ALPHA_NV                 0x858B
    #define GL_OPERAND3_RGB_NV                  0x8593
    #define GL_OPERAND3_ALPHA_NV                0x859B
#endif


#ifndef GL_SGIS_generate_mipmap
    #define GL_SGIS_generate_mipmap
    #define GL_GENERATE_MIPMAP_SGIS				0x8191
    #define GL_GENERATE_MIPMAP_HINT_SGIS		0x8192
#endif


#ifdef _WIN32

#ifndef WGL_ARB_extensions_string
    #define WGL_ARB_extensions_string
    typedef char * (APIENTRY * WGL_ARB_EXTENSIONS_STRING) (HDC hdc);
#endif


#ifndef WGL_ARB_make_current_read
    #define WGL_ARB_make_current_read
    typedef BOOL (WINAPI * PF_WGLMAKECONTEXTCURRENTARBPROC) (HDC hDrawDC, HDC hReadDC, HGLRC hGLRC);
    typedef HDC (WINAPI * PF_WGLGETCURRENTREADDCARBPROC) (VOID);
#endif


#ifndef WGL_ARB_multisample
    #define WGL_ARB_multisample
    #define WGL_SAMPLE_BUFFERS_ARB          0x2041
    #define WGL_SAMPLES_ARB                 0x2042
#endif


#ifndef WGL_ARB_pbuffer
    #define WGL_ARB_pbuffer
    #define WGL_DRAW_TO_PBUFFER_ARB              0x202D
    #define WGL_MAX_PBUFFER_PIXELS_ARB           0x202E
    #define WGL_MAX_PBUFFER_WIDTH_ARB            0x202F
    #define WGL_MAX_PBUFFER_HEIGHT_ARB           0x2030
    #define WGL_PBUFFER_LARGEST_ARB              0x2033
    #define WGL_PBUFFER_WIDTH_ARB                0x2034
    #define WGL_PBUFFER_HEIGHT_ARB               0x2035
    #define WGL_PBUFFER_LOST_ARB				 0x2036

    DECLARE_HANDLE(HPBUFFERARB);

    typedef HPBUFFERARB (WINAPI * PF_WGLCREATEPBUFFERARBPROC) (HDC hDC, int iPixelFormat, int iWidth, int iHeight, const int *piAttribList);
    typedef HDC (WINAPI * PF_WGLGETPBUFFERDCARBPROC) (HPBUFFERARB hPbuffer);
    typedef int (WINAPI * PF_WGLRELEASEPBUFFERDCARBPROC) (HPBUFFERARB hPbuffer, HDC hDC);
    typedef BOOL (WINAPI * PF_WGLDESTROYPBUFFERARBPROC) (HPBUFFERARB hPbuffer);
    typedef BOOL (WINAPI * PF_WGLQUERYPBUFFERARBPROC) (HPBUFFERARB hPbuffer, int iAttribute, int *piValue);
#endif


#ifndef WGL_ARB_pixel_format
    #define WGL_ARB_pixel_format
    #define WGL_NUMBER_PIXEL_FORMATS_ARB		0x2000
    #define WGL_DRAW_TO_WINDOW_ARB				0x2001
    #define WGL_DRAW_TO_BITMAP_ARB				0x2002
    #define WGL_ACCELERATION_ARB				0x2003
    #define WGL_NEED_PALETTE_ARB				0x2004
    #define	WGL_NEED_SYSTEM_PALETTE_ARB			0x2005
    #define	WGL_SWAP_LAYER_BUFFERS_ARB			0x2006
    #define	WGL_SWAP_METHOD_ARB					0x2007
    #define	WGL_NUMBER_OVERLAYS_ARB				0x2008
    #define	WGL_NUMBER_UNDERLAYS_ARB			0x2009
    #define WGL_TRANSPARENT_ARB					0x200A
    #define WGL_TRANSPARENT_RED_VALUE_ARB		0x2037
    #define WGL_TRANSPARENT_GREEN_VALUE_ARB		0x2038
    #define WGL_TRANSPARENT_BLUE_VALUE_ARB		0x2039
    #define WGL_TRANSPARENT_ALPHA_VALUE_ARB		0x203A
    #define WGL_TRANSPARENT_INDEX_VALUE_ARB		0x203B
    #define WGL_SHARE_DEPTH_ARB					0x200C
    #define WGL_SHARE_STENCIL_ARB				0x200D
    #define WGL_SHARE_ACCUM_ARB					0x200E
    #define WGL_SUPPORT_GDI_ARB					0x200F
    #define WGL_SUPPORT_OPENGL_ARB				0x2010
    #define WGL_DOUBLE_BUFFER_ARB				0x2011
    #define WGL_STEREO_ARB						0x2012
    #define WGL_PIXEL_TYPE_ARB					0x2013
    #define WGL_COLOR_BITS_ARB					0x2014
    #define WGL_RED_BITS_ARB					0x2015
    #define WGL_RED_SHIFT_ARB					0x2016
    #define WGL_GREEN_BITS_ARB					0x2017
    #define WGL_GREEN_SHIFT_ARB					0x2018
    #define WGL_BLUE_BITS_ARB					0x2019
    #define WGL_BLUE_SHIFT_ARB					0x201A
    #define WGL_ALPHA_BITS_ARB					0x201B
    #define WGL_ALPHA_SHIFT_ARB					0x201C
    #define WGL_ACCUM_BITS_ARB					0x201D
    #define WGL_ACCUM_RED_BITS_ARB				0x201E
    #define WGL_ACCUM_GREEN_BITS_ARB			0x201F
    #define WGL_ACCUM_BLUE_BITS_ARB				0x2020
    #define WGL_ACCUM_ALPHA_BITS_ARB			0x2021
    #define WGL_DEPTH_BITS_ARB					0x2022
    #define WGL_STENCIL_BITS_ARB				0x2023
    #define WGL_AUX_BUFFERS_ARB					0x2024
    #define WGL_NO_ACCELERATION_ARB				0x2025
    #define WGL_GENERIC_ACCELERATION_ARB		0x2026
    #define WGL_FULL_ACCELERATION_ARB			0x2027
    #define WGL_SWAP_EXCHANGE_ARB				0x2028
    #define WGL_SWAP_COPY_ARB					0x2029
    #define WGL_SWAP_UNDEFINED_ARB				0x202A
    #define WGL_TYPE_RGBA_ARB					0x202B
    #define WGL_TYPE_COLORINDEX_ARB				0x202C


    typedef BOOL (WINAPI * PF_WGLGETPIXELFORMATATTRIBIVARBPROC) (HDC hDC, int iPixelFormat, int iLayerPlane, UINT nAttributes, const int *piAttributes, int *piValues);
    typedef BOOL (WINAPI * PF_WGLGETPIXELFORMATATTRIBFVARBPROC) (HDC hDC, int iPixelFormat, int iLayerPlane, UINT nAttributes, const int *piAttributes, FLOAT *pfValues);
    typedef BOOL (WINAPI * PF_WGLCHOOSEPIXELFORMATARBPROC) (HDC hDC, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);
#endif


#ifndef WGL_ARB_render_texture
    #define WGL_ARB_render_texture
    #define WGL_BIND_TO_TEXTURE_RGB_ARB			0x2070
    #define WGL_BIND_TO_TEXTURE_RGBA_ARB		0x2071
    #define WGL_TEXTURE_FORMAT_ARB				0x2072
    #define WGL_TEXTURE_TARGET_ARB				0x2073
    #define WGL_MIPMAP_TEXTURE_ARB				0x2074
    #define WGL_TEXTURE_RGB_ARB					0x2075
    #define WGL_TEXTURE_RGBA_ARB				0x2076
    #define WGL_NO_TEXTURE_ARB					0x2077
    #define WGL_TEXTURE_CUBE_MAP_ARB			0x2078
    #define WGL_TEXTURE_1D_ARB					0x2079
    #define WGL_TEXTURE_2D_ARB					0x207A
    #define WGL_MIPMAP_LEVEL_ARB				0x207B
    #define WGL_CUBE_MAP_FACE_ARB				0x207C
    #define WGL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB	0x207D
    #define WGL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB 0x207E
    #define WGL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB	0x207F
    #define WGL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB 0x2080
    #define WGL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB	0x2081
    #define WGL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB 0x2082
    #define WGL_FRONT_LEFT_ARB					0x2083
    #define WGL_FRONT_RIGHT_ARB					0x2084
    #define WGL_BACK_LEFT_ARB					0x2085
    #define WGL_BACK_RIGHT_ARB					0x2086
    #define WGL_AUX0_ARB						0x2087
    #define WGL_AUX1_ARB						0x2088
    #define WGL_AUX2_ARB						0x2089
    #define WGL_AUX3_ARB						0x208A
    #define WGL_AUX4_ARB						0x208B
    #define WGL_AUX5_ARB						0x208C
    #define WGL_AUX6_ARB						0x208D
    #define WGL_AUX7_ARB						0x208E
    #define WGL_AUX8_ARB						0x208F
    #define WGL_AUX9_ARB						0x2090

    typedef BOOL (WINAPI * PF_WGLBINDTEXIMAGEARBPROC)(HPBUFFERARB hPbuffer, int iBuffer);
    typedef BOOL (WINAPI * PF_WGLRELEASETEXIMAGEARBPROC)(HPBUFFERARB hPbuffer, int iBuffer);
    typedef BOOL (WINAPI * PF_WGLSETPBUFFERATTRIBARBPROC)(HPBUFFERARB hPbuffer, const int *piAttribList);
#endif


#ifndef WGL_ATI_pixel_format_float
    #define WGL_ATI_pixel_format_float
    #define WGL_TYPE_RGBA_FLOAT_ATI             0x21A0
    #define GL_TYPE_RGBA_FLOAT_ATI              0x8820
    #define GL_COLOR_CLEAR_UNCLAMPED_VALUE_ATI  0x8835
#endif


#ifndef WGL_EXT_swap_control
    #define WGL_EXT_swap_control
    typedef void (APIENTRY * PF_WGLEXTSWAPINTERVALPROC) (int i);
#endif

extern PF_WGLEXTSWAPINTERVALPROC wglSwapIntervalEXT;

#endif //// _WIN32


#endif // _GLEXTENSIONS_H_
