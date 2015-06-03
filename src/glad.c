#include <stdio.h>
#include <string.h>
#include <glad/glad.h>

static void* get_proc(const char *namez);

#ifdef _WIN32
#include <windows.h>
static HMODULE libGL;

typedef void* (APIENTRYP PFNWGLGETPROCADDRESSPROC_PRIVATE)(const char*);
PFNWGLGETPROCADDRESSPROC_PRIVATE gladGetProcAddressPtr;

static
int open_gl(void) {
    libGL = LoadLibraryA("opengl32.dll");
    if(libGL != NULL) {
        gladGetProcAddressPtr = (PFNWGLGETPROCADDRESSPROC_PRIVATE)GetProcAddress(
                libGL, "wglGetProcAddress");
        return gladGetProcAddressPtr != NULL;
    }

    return 0;
}

static
void close_gl(void) {
    if(libGL != NULL) {
        FreeLibrary(libGL);
        libGL = NULL;
    }
}
#else
#include <dlfcn.h>
static void* libGL;

#ifndef __APPLE__
typedef void* (APIENTRYP PFNGLXGETPROCADDRESSPROC_PRIVATE)(const char*);
PFNGLXGETPROCADDRESSPROC_PRIVATE gladGetProcAddressPtr;
#endif

static
int open_gl(void) {
#ifdef __APPLE__
    static const char *NAMES[] = {
        "../Frameworks/OpenGL.framework/OpenGL",
        "/Library/Frameworks/OpenGL.framework/OpenGL",
        "/System/Library/Frameworks/OpenGL.framework/OpenGL",
        "/System/Library/Frameworks/OpenGL.framework/Versions/Current/OpenGL"
    };
#else
    static const char *NAMES[] = {"libGL.so.1", "libGL.so"};
#endif

    unsigned int index = 0;
    for(index = 0; index < (sizeof(NAMES) / sizeof(NAMES[0])); index++) {
        libGL = dlopen(NAMES[index], RTLD_NOW | RTLD_GLOBAL);

        if(libGL != NULL) {
#ifdef __APPLE__
            return 1;
#else
            gladGetProcAddressPtr = (PFNGLXGETPROCADDRESSPROC_PRIVATE)dlsym(libGL,
                "glXGetProcAddressARB");
            return gladGetProcAddressPtr != NULL;
#endif
        }
    }

    return 0;
}

static
void close_gl() {
    if(libGL != NULL) {
        dlclose(libGL);
        libGL = NULL;
    }
}
#endif

static
void* get_proc(const char *namez) {
    void* result = NULL;
    if(libGL == NULL) return NULL;

#ifndef __APPLE__
    if(gladGetProcAddressPtr != NULL) {
        result = gladGetProcAddressPtr(namez);
    }
#endif
    if(result == NULL) {
#ifdef _WIN32
        result = (void*)GetProcAddress(libGL, namez);
#else
        result = dlsym(libGL, namez);
#endif
    }

    return result;
}

int gladLoadGL(void) {
    int status = 0;

    if(open_gl()) {
        status = gladLoadGLLoader(&get_proc);
        close_gl();
    }

    return status;
}

struct gladGLversionStruct GLVersion;

#if defined(GL_ES_VERSION_3_0) || defined(GL_VERSION_3_0)
#define _GLAD_IS_SOME_NEW_VERSION 1
#endif

static int has_ext(const char *ext) {
#ifdef _GLAD_IS_SOME_NEW_VERSION
    if(GLVersion.major < 3) {
#endif
        const char *extensions;
        const char *loc;
        const char *terminator;
        extensions = (const char *)glGetString(GL_EXTENSIONS);
        if(extensions == NULL || ext == NULL) {
            return 0;
        }

        while(1) {
            loc = strstr(extensions, ext);
            if(loc == NULL) {
                return 0;
            }

            terminator = loc + strlen(ext);
            if((loc == extensions || *(loc - 1) == ' ') &&
                (*terminator == ' ' || *terminator == '\0')) {
                return 1;
            }
            extensions = terminator;
        }
#ifdef _GLAD_IS_SOME_NEW_VERSION
    } else {
        int num, index;

        glGetIntegerv(GL_NUM_EXTENSIONS, &num);

        for(index = 0; index < num; index++) {
            const char *e = (const char*)glGetStringi(GL_EXTENSIONS, index);

            if(strcmp(e, ext) == 0) {
                return 1;
            }
        }
    }
#endif

    return 0;
}
int GLAD_GL_VERSION_1_0;
int GLAD_GL_VERSION_1_1;
int GLAD_GL_VERSION_1_2;
int GLAD_GL_VERSION_1_3;
int GLAD_GL_VERSION_1_4;
int GLAD_GL_VERSION_1_5;
int GLAD_GL_VERSION_2_0;
int GLAD_GL_VERSION_2_1;
int GLAD_GL_VERSION_3_0;
int GLAD_GL_VERSION_3_1;
int GLAD_GL_VERSION_3_2;
int GLAD_GL_VERSION_3_3;
int GLAD_GL_VERSION_4_0;
int GLAD_GL_VERSION_4_1;
PFNGLCOPYTEXIMAGE1DPROC glad_glCopyTexImage1D;
PFNGLVERTEXATTRIBI3UIPROC glad_glVertexAttribI3ui;
PFNGLSTENCILMASKSEPARATEPROC glad_glStencilMaskSeparate;
PFNGLMINSAMPLESHADINGPROC glad_glMinSampleShading;
PFNGLFRAMEBUFFERRENDERBUFFERPROC glad_glFramebufferRenderbuffer;
PFNGLUNIFORMSUBROUTINESUIVPROC glad_glUniformSubroutinesuiv;
PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC glad_glCompressedTexSubImage3D;
PFNGLTEXCOORDP3UIVPROC glad_glTexCoordP3uiv;
PFNGLGETDOUBLEI_VPROC glad_glGetDoublei_v;
PFNGLVERTEXATTRIB1SVPROC glad_glVertexAttrib1sv;
PFNGLBINDSAMPLERPROC glad_glBindSampler;
PFNGLLINEWIDTHPROC glad_glLineWidth;
PFNGLCOLORP3UIVPROC glad_glColorP3uiv;
PFNGLGETINTEGERI_VPROC glad_glGetIntegeri_v;
PFNGLCOMPILESHADERPROC glad_glCompileShader;
PFNGLGETTRANSFORMFEEDBACKVARYINGPROC glad_glGetTransformFeedbackVarying;
PFNGLDEPTHRANGEFPROC glad_glDepthRangef;
PFNGLVERTEXATTRIBIPOINTERPROC glad_glVertexAttribIPointer;
PFNGLMULTITEXCOORDP3UIPROC glad_glMultiTexCoordP3ui;
PFNGLVERTEXP4UIPROC glad_glVertexP4ui;
PFNGLENABLEIPROC glad_glEnablei;
PFNGLVERTEXATTRIBP4UIPROC glad_glVertexAttribP4ui;
PFNGLCREATESHADERPROC glad_glCreateShader;
PFNGLISBUFFERPROC glad_glIsBuffer;
PFNGLGETMULTISAMPLEFVPROC glad_glGetMultisamplefv;
PFNGLPROGRAMUNIFORMMATRIX2DVPROC glad_glProgramUniformMatrix2dv;
PFNGLGENRENDERBUFFERSPROC glad_glGenRenderbuffers;
PFNGLCOPYTEXSUBIMAGE2DPROC glad_glCopyTexSubImage2D;
PFNGLCOMPRESSEDTEXIMAGE2DPROC glad_glCompressedTexImage2D;
PFNGLVERTEXATTRIB1FPROC glad_glVertexAttrib1f;
PFNGLBLENDFUNCSEPARATEPROC glad_glBlendFuncSeparate;
PFNGLPROGRAMUNIFORMMATRIX4FVPROC glad_glProgramUniformMatrix4fv;
PFNGLHINTPROC glad_glHint;
PFNGLVERTEXATTRIB1SPROC glad_glVertexAttrib1s;
PFNGLSAMPLEMASKIPROC glad_glSampleMaski;
PFNGLVERTEXP2UIPROC glad_glVertexP2ui;
PFNGLUNIFORMMATRIX3X2FVPROC glad_glUniformMatrix3x2fv;
PFNGLPOINTSIZEPROC glad_glPointSize;
PFNGLVERTEXATTRIB2DVPROC glad_glVertexAttrib2dv;
PFNGLDELETEPROGRAMPROC glad_glDeleteProgram;
PFNGLVERTEXATTRIB4NUIVPROC glad_glVertexAttrib4Nuiv;
PFNGLRENDERBUFFERSTORAGEPROC glad_glRenderbufferStorage;
PFNGLWAITSYNCPROC glad_glWaitSync;
PFNGLUNIFORMMATRIX4X3FVPROC glad_glUniformMatrix4x3fv;
PFNGLUNIFORM3IPROC glad_glUniform3i;
PFNGLCLEARBUFFERFVPROC glad_glClearBufferfv;
PFNGLPROGRAMUNIFORM1UIPROC glad_glProgramUniform1ui;
PFNGLBLENDEQUATIONSEPARATEIPROC glad_glBlendEquationSeparatei;
PFNGLUNIFORM3DPROC glad_glUniform3d;
PFNGLUNIFORM3FPROC glad_glUniform3f;
PFNGLVERTEXATTRIB4UBVPROC glad_glVertexAttrib4ubv;
PFNGLGETBUFFERPARAMETERIVPROC glad_glGetBufferParameteriv;
PFNGLTEXCOORDP2UIPROC glad_glTexCoordP2ui;
PFNGLCOLORMASKIPROC glad_glColorMaski;
PFNGLCLEARBUFFERFIPROC glad_glClearBufferfi;
PFNGLDRAWARRAYSINDIRECTPROC glad_glDrawArraysIndirect;
PFNGLGENVERTEXARRAYSPROC glad_glGenVertexArrays;
PFNGLPAUSETRANSFORMFEEDBACKPROC glad_glPauseTransformFeedback;
PFNGLMULTITEXCOORDP2UIPROC glad_glMultiTexCoordP2ui;
PFNGLPROGRAMUNIFORMMATRIX3X2DVPROC glad_glProgramUniformMatrix3x2dv;
PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC glad_glProgramUniformMatrix3x2fv;
PFNGLGETSAMPLERPARAMETERIIVPROC glad_glGetSamplerParameterIiv;
PFNGLGETFRAGDATAINDEXPROC glad_glGetFragDataIndex;
PFNGLVERTEXATTRIBL4DPROC glad_glVertexAttribL4d;
PFNGLGETVERTEXATTRIBDVPROC glad_glGetVertexAttribdv;
PFNGLACTIVESHADERPROGRAMPROC glad_glActiveShaderProgram;
PFNGLUNIFORMMATRIX3X4FVPROC glad_glUniformMatrix3x4fv;
PFNGLUNIFORMMATRIX3DVPROC glad_glUniformMatrix3dv;
PFNGLPROGRAMUNIFORMMATRIX3X4DVPROC glad_glProgramUniformMatrix3x4dv;
PFNGLRESUMETRANSFORMFEEDBACKPROC glad_glResumeTransformFeedback;
PFNGLMULTITEXCOORDP4UIPROC glad_glMultiTexCoordP4ui;
PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC glad_glProgramUniformMatrix4x3fv;
PFNGLVIEWPORTARRAYVPROC glad_glViewportArrayv;
PFNGLDELETEFRAMEBUFFERSPROC glad_glDeleteFramebuffers;
PFNGLDRAWARRAYSPROC glad_glDrawArrays;
PFNGLUNIFORM1UIPROC glad_glUniform1ui;
PFNGLPROGRAMUNIFORM2UIVPROC glad_glProgramUniform2uiv;
PFNGLVERTEXATTRIBI2IPROC glad_glVertexAttribI2i;
PFNGLTEXCOORDP3UIPROC glad_glTexCoordP3ui;
PFNGLVERTEXATTRIB3DPROC glad_glVertexAttrib3d;
PFNGLCLEARPROC glad_glClear;
PFNGLPROGRAMPARAMETERIPROC glad_glProgramParameteri;
PFNGLGETACTIVEUNIFORMNAMEPROC glad_glGetActiveUniformName;
PFNGLISENABLEDPROC glad_glIsEnabled;
PFNGLSTENCILOPPROC glad_glStencilOp;
PFNGLFRAMEBUFFERTEXTURE2DPROC glad_glFramebufferTexture2D;
PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC glad_glGetFramebufferAttachmentParameteriv;
PFNGLVERTEXATTRIB4NUBPROC glad_glVertexAttrib4Nub;
PFNGLGETFRAGDATALOCATIONPROC glad_glGetFragDataLocation;
PFNGLTEXIMAGE1DPROC glad_glTexImage1D;
PFNGLTEXPARAMETERIVPROC glad_glTexParameteriv;
PFNGLGETTEXIMAGEPROC glad_glGetTexImage;
PFNGLGETQUERYOBJECTI64VPROC glad_glGetQueryObjecti64v;
PFNGLGENFRAMEBUFFERSPROC glad_glGenFramebuffers;
PFNGLGETATTACHEDSHADERSPROC glad_glGetAttachedShaders;
PFNGLISRENDERBUFFERPROC glad_glIsRenderbuffer;
PFNGLDELETEVERTEXARRAYSPROC glad_glDeleteVertexArrays;
PFNGLPROGRAMUNIFORM1UIVPROC glad_glProgramUniform1uiv;
PFNGLISVERTEXARRAYPROC glad_glIsVertexArray;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glad_glDisableVertexAttribArray;
PFNGLPROGRAMUNIFORM2IVPROC glad_glProgramUniform2iv;
PFNGLGETQUERYIVPROC glad_glGetQueryiv;
PFNGLGETSAMPLERPARAMETERFVPROC glad_glGetSamplerParameterfv;
PFNGLPROGRAMUNIFORMMATRIX4X2DVPROC glad_glProgramUniformMatrix4x2dv;
PFNGLGETUNIFORMINDICESPROC glad_glGetUniformIndices;
PFNGLISSHADERPROC glad_glIsShader;
PFNGLVERTEXATTRIBI4UBVPROC glad_glVertexAttribI4ubv;
PFNGLBEGINQUERYINDEXEDPROC glad_glBeginQueryIndexed;
PFNGLPOINTPARAMETERIVPROC glad_glPointParameteriv;
PFNGLENABLEPROC glad_glEnable;
PFNGLGETACTIVEUNIFORMSIVPROC glad_glGetActiveUniformsiv;
PFNGLBLENDEQUATIONIPROC glad_glBlendEquationi;
PFNGLGETATTRIBLOCATIONPROC glad_glGetAttribLocation;
PFNGLVERTEXATTRIB4DVPROC glad_glVertexAttrib4dv;
PFNGLUNIFORM2DVPROC glad_glUniform2dv;
PFNGLMULTITEXCOORDP3UIVPROC glad_glMultiTexCoordP3uiv;
PFNGLVERTEXATTRIBP3UIPROC glad_glVertexAttribP3ui;
PFNGLVERTEXATTRIBL1DVPROC glad_glVertexAttribL1dv;
PFNGLPROGRAMUNIFORM3UIPROC glad_glProgramUniform3ui;
PFNGLGETUNIFORMFVPROC glad_glGetUniformfv;
PFNGLGETUNIFORMUIVPROC glad_glGetUniformuiv;
PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC glad_glProgramUniformMatrix2x3fv;
PFNGLGETVERTEXATTRIBIIVPROC glad_glGetVertexAttribIiv;
PFNGLDRAWBUFFERPROC glad_glDrawBuffer;
PFNGLENDQUERYINDEXEDPROC glad_glEndQueryIndexed;
PFNGLCLEARBUFFERUIVPROC glad_glClearBufferuiv;
PFNGLDRAWELEMENTSINSTANCEDPROC glad_glDrawElementsInstanced;
PFNGLPROGRAMUNIFORM1IPROC glad_glProgramUniform1i;
PFNGLPATCHPARAMETERIPROC glad_glPatchParameteri;
PFNGLPROGRAMUNIFORM1DPROC glad_glProgramUniform1d;
PFNGLPROGRAMUNIFORM1FPROC glad_glProgramUniform1f;
PFNGLFLUSHPROC glad_glFlush;
PFNGLGETRENDERBUFFERPARAMETERIVPROC glad_glGetRenderbufferParameteriv;
PFNGLPROGRAMUNIFORM3IVPROC glad_glProgramUniform3iv;
PFNGLGETVERTEXATTRIBPOINTERVPROC glad_glGetVertexAttribPointerv;
PFNGLFENCESYNCPROC glad_glFenceSync;
PFNGLCOLORP3UIPROC glad_glColorP3ui;
PFNGLVERTEXATTRIB3SVPROC glad_glVertexAttrib3sv;
PFNGLBEGINCONDITIONALRENDERPROC glad_glBeginConditionalRender;
PFNGLVALIDATEPROGRAMPIPELINEPROC glad_glValidateProgramPipeline;
PFNGLGETTEXLEVELPARAMETERIVPROC glad_glGetTexLevelParameteriv;
PFNGLMULTITEXCOORDP4UIVPROC glad_glMultiTexCoordP4uiv;
PFNGLSTENCILFUNCSEPARATEPROC glad_glStencilFuncSeparate;
PFNGLGENSAMPLERSPROC glad_glGenSamplers;
PFNGLCLAMPCOLORPROC glad_glClampColor;
PFNGLUNIFORM4IVPROC glad_glUniform4iv;
PFNGLCLEARSTENCILPROC glad_glClearStencil;
PFNGLTEXCOORDP1UIVPROC glad_glTexCoordP1uiv;
PFNGLGENTEXTURESPROC glad_glGenTextures;
PFNGLDRAWTRANSFORMFEEDBACKPROC glad_glDrawTransformFeedback;
PFNGLUNIFORM1DVPROC glad_glUniform1dv;
PFNGLGETTEXPARAMETERIUIVPROC glad_glGetTexParameterIuiv;
PFNGLVERTEXATTRIB4NBVPROC glad_glVertexAttrib4Nbv;
PFNGLISSYNCPROC glad_glIsSync;
PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC glad_glGetActiveUniformBlockName;
PFNGLUNIFORM2IPROC glad_glUniform2i;
PFNGLUNIFORM2FPROC glad_glUniform2f;
PFNGLUNIFORM2DPROC glad_glUniform2d;
PFNGLTEXCOORDP4UIPROC glad_glTexCoordP4ui;
PFNGLGETPROGRAMIVPROC glad_glGetProgramiv;
PFNGLVERTEXATTRIBPOINTERPROC glad_glVertexAttribPointer;
PFNGLFRAMEBUFFERTEXTURELAYERPROC glad_glFramebufferTextureLayer;
PFNGLPROGRAMUNIFORM4FVPROC glad_glProgramUniform4fv;
PFNGLFLUSHMAPPEDBUFFERRANGEPROC glad_glFlushMappedBufferRange;
PFNGLPROGRAMUNIFORM2FVPROC glad_glProgramUniform2fv;
PFNGLUNIFORMMATRIX2X3DVPROC glad_glUniformMatrix2x3dv;
PFNGLPROGRAMUNIFORMMATRIX4DVPROC glad_glProgramUniformMatrix4dv;
PFNGLVERTEXATTRIBL3DPROC glad_glVertexAttribL3d;
PFNGLPROGRAMUNIFORMMATRIX2X4DVPROC glad_glProgramUniformMatrix2x4dv;
PFNGLGENQUERIESPROC glad_glGenQueries;
PFNGLVERTEXATTRIBP1UIPROC glad_glVertexAttribP1ui;
PFNGLTEXSUBIMAGE3DPROC glad_glTexSubImage3D;
PFNGLGETINTEGER64I_VPROC glad_glGetInteger64i_v;
PFNGLDELETESAMPLERSPROC glad_glDeleteSamplers;
PFNGLCOPYTEXIMAGE2DPROC glad_glCopyTexImage2D;
PFNGLBLITFRAMEBUFFERPROC glad_glBlitFramebuffer;
PFNGLISENABLEDIPROC glad_glIsEnabledi;
PFNGLSECONDARYCOLORP3UIPROC glad_glSecondaryColorP3ui;
PFNGLBINDFRAGDATALOCATIONINDEXEDPROC glad_glBindFragDataLocationIndexed;
PFNGLUNIFORM2IVPROC glad_glUniform2iv;
PFNGLVERTEXATTRIB1FVPROC glad_glVertexAttrib1fv;
PFNGLUNIFORM4UIVPROC glad_glUniform4uiv;
PFNGLPROGRAMUNIFORM2DVPROC glad_glProgramUniform2dv;
PFNGLFRAMEBUFFERTEXTURE1DPROC glad_glFramebufferTexture1D;
PFNGLGETSHADERIVPROC glad_glGetShaderiv;
PFNGLPROGRAMUNIFORMMATRIX3FVPROC glad_glProgramUniformMatrix3fv;
PFNGLBINDFRAGDATALOCATIONPROC glad_glBindFragDataLocation;
PFNGLSCISSORARRAYVPROC glad_glScissorArrayv;
PFNGLPOLYGONOFFSETPROC glad_glPolygonOffset;
PFNGLGETDOUBLEVPROC glad_glGetDoublev;
PFNGLVERTEXATTRIB1DPROC glad_glVertexAttrib1d;
PFNGLUNIFORM4DVPROC glad_glUniform4dv;
PFNGLPROGRAMUNIFORM3DVPROC glad_glProgramUniform3dv;
PFNGLGETUNIFORMIVPROC glad_glGetUniformiv;
PFNGLMULTITEXCOORDP1UIVPROC glad_glMultiTexCoordP1uiv;
PFNGLUNIFORM3FVPROC glad_glUniform3fv;
PFNGLDEPTHRANGEPROC glad_glDepthRange;
PFNGLMAPBUFFERPROC glad_glMapBuffer;
PFNGLCOMPRESSEDTEXIMAGE3DPROC glad_glCompressedTexImage3D;
PFNGLDELETESYNCPROC glad_glDeleteSync;
PFNGLCOPYTEXSUBIMAGE3DPROC glad_glCopyTexSubImage3D;
PFNGLUNIFORMMATRIX4DVPROC glad_glUniformMatrix4dv;
PFNGLGETVERTEXATTRIBIVPROC glad_glGetVertexAttribiv;
PFNGLUNIFORMMATRIX4X2DVPROC glad_glUniformMatrix4x2dv;
PFNGLMULTIDRAWELEMENTSPROC glad_glMultiDrawElements;
PFNGLVERTEXATTRIB3FVPROC glad_glVertexAttrib3fv;
PFNGLUNIFORM3IVPROC glad_glUniform3iv;
PFNGLPOLYGONMODEPROC glad_glPolygonMode;
PFNGLDRAWBUFFERSPROC glad_glDrawBuffers;
PFNGLGETACTIVEUNIFORMBLOCKIVPROC glad_glGetActiveUniformBlockiv;
PFNGLPROGRAMUNIFORM4IVPROC glad_glProgramUniform4iv;
PFNGLGETPROGRAMBINARYPROC glad_glGetProgramBinary;
PFNGLUSEPROGRAMPROC glad_glUseProgram;
PFNGLGETPROGRAMINFOLOGPROC glad_glGetProgramInfoLog;
PFNGLBINDTRANSFORMFEEDBACKPROC glad_glBindTransformFeedback;
PFNGLBINDVERTEXARRAYPROC glad_glBindVertexArray;
PFNGLDELETEBUFFERSPROC glad_glDeleteBuffers;
PFNGLSAMPLERPARAMETERIIVPROC glad_glSamplerParameterIiv;
PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC glad_glMultiDrawElementsBaseVertex;
PFNGLUNIFORM2UIVPROC glad_glUniform2uiv;
PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC glad_glCompressedTexSubImage1D;
PFNGLFINISHPROC glad_glFinish;
PFNGLDEPTHRANGEINDEXEDPROC glad_glDepthRangeIndexed;
PFNGLDELETESHADERPROC glad_glDeleteShader;
PFNGLVERTEXATTRIB4NSVPROC glad_glVertexAttrib4Nsv;
PFNGLVIEWPORTPROC glad_glViewport;
PFNGLUNIFORM1UIVPROC glad_glUniform1uiv;
PFNGLTRANSFORMFEEDBACKVARYINGSPROC glad_glTransformFeedbackVaryings;
PFNGLUNIFORM2UIPROC glad_glUniform2ui;
PFNGLVERTEXATTRIBI3IPROC glad_glVertexAttribI3i;
PFNGLCLEARDEPTHPROC glad_glClearDepth;
PFNGLVERTEXATTRIBI4USVPROC glad_glVertexAttribI4usv;
PFNGLTEXPARAMETERFPROC glad_glTexParameterf;
PFNGLTEXPARAMETERIPROC glad_glTexParameteri;
PFNGLGETACTIVESUBROUTINEUNIFORMIVPROC glad_glGetActiveSubroutineUniformiv;
PFNGLGETSHADERSOURCEPROC glad_glGetShaderSource;
PFNGLTEXBUFFERPROC glad_glTexBuffer;
PFNGLPIXELSTOREIPROC glad_glPixelStorei;
PFNGLVALIDATEPROGRAMPROC glad_glValidateProgram;
PFNGLPIXELSTOREFPROC glad_glPixelStoref;
PFNGLGETBOOLEANI_VPROC glad_glGetBooleani_v;
PFNGLMULTITEXCOORDP2UIVPROC glad_glMultiTexCoordP2uiv;
PFNGLGENPROGRAMPIPELINESPROC glad_glGenProgramPipelines;
PFNGLVERTEXATTRIBP1UIVPROC glad_glVertexAttribP1uiv;
PFNGLLINKPROGRAMPROC glad_glLinkProgram;
PFNGLBINDTEXTUREPROC glad_glBindTexture;
PFNGLGETPROGRAMPIPELINEINFOLOGPROC glad_glGetProgramPipelineInfoLog;
PFNGLGETSTRINGPROC glad_glGetString;
PFNGLVERTEXATTRIBP2UIVPROC glad_glVertexAttribP2uiv;
PFNGLDETACHSHADERPROC glad_glDetachShader;
PFNGLPROGRAMUNIFORM3IPROC glad_glProgramUniform3i;
PFNGLUNIFORMMATRIX3X4DVPROC glad_glUniformMatrix3x4dv;
PFNGLENDQUERYPROC glad_glEndQuery;
PFNGLNORMALP3UIPROC glad_glNormalP3ui;
PFNGLUNIFORMMATRIX4X3DVPROC glad_glUniformMatrix4x3dv;
PFNGLDEPTHRANGEARRAYVPROC glad_glDepthRangeArrayv;
PFNGLVERTEXATTRIBI2UIPROC glad_glVertexAttribI2ui;
PFNGLDELETETEXTURESPROC glad_glDeleteTextures;
PFNGLSTENCILOPSEPARATEPROC glad_glStencilOpSeparate;
PFNGLDELETEQUERIESPROC glad_glDeleteQueries;
PFNGLNORMALP3UIVPROC glad_glNormalP3uiv;
PFNGLVERTEXATTRIB4FPROC glad_glVertexAttrib4f;
PFNGLVERTEXATTRIB4DPROC glad_glVertexAttrib4d;
PFNGLVIEWPORTINDEXEDFVPROC glad_glViewportIndexedfv;
PFNGLVERTEXATTRIBL4DVPROC glad_glVertexAttribL4dv;
PFNGLGETTEXPARAMETERIVPROC glad_glGetTexParameteriv;
PFNGLPROGRAMUNIFORM1DVPROC glad_glProgramUniform1dv;
PFNGLVERTEXATTRIB4SPROC glad_glVertexAttrib4s;
PFNGLDRAWELEMENTSBASEVERTEXPROC glad_glDrawElementsBaseVertex;
PFNGLSAMPLECOVERAGEPROC glad_glSampleCoverage;
PFNGLSAMPLERPARAMETERIPROC glad_glSamplerParameteri;
PFNGLSAMPLERPARAMETERFPROC glad_glSamplerParameterf;
PFNGLUNIFORM1FPROC glad_glUniform1f;
PFNGLGETVERTEXATTRIBFVPROC glad_glGetVertexAttribfv;
PFNGLUNIFORM1DPROC glad_glUniform1d;
PFNGLGETCOMPRESSEDTEXIMAGEPROC glad_glGetCompressedTexImage;
PFNGLUNIFORM1IPROC glad_glUniform1i;
PFNGLGETACTIVEATTRIBPROC glad_glGetActiveAttrib;
PFNGLTEXSUBIMAGE2DPROC glad_glTexSubImage2D;
PFNGLDISABLEPROC glad_glDisable;
PFNGLLOGICOPPROC glad_glLogicOp;
PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC glad_glProgramUniformMatrix3x4fv;
PFNGLPROGRAMUNIFORM4UIVPROC glad_glProgramUniform4uiv;
PFNGLUNIFORM4UIPROC glad_glUniform4ui;
PFNGLBINDFRAMEBUFFERPROC glad_glBindFramebuffer;
PFNGLCULLFACEPROC glad_glCullFace;
PFNGLPROGRAMUNIFORM4IPROC glad_glProgramUniform4i;
PFNGLPROGRAMUNIFORM4FPROC glad_glProgramUniform4f;
PFNGLVIEWPORTINDEXEDFPROC glad_glViewportIndexedf;
PFNGLPROGRAMUNIFORM4DPROC glad_glProgramUniform4d;
PFNGLGETSTRINGIPROC glad_glGetStringi;
PFNGLSCISSORINDEXEDPROC glad_glScissorIndexed;
PFNGLDRAWTRANSFORMFEEDBACKSTREAMPROC glad_glDrawTransformFeedbackStream;
PFNGLATTACHSHADERPROC glad_glAttachShader;
PFNGLQUERYCOUNTERPROC glad_glQueryCounter;
PFNGLPROVOKINGVERTEXPROC glad_glProvokingVertex;
PFNGLSHADERBINARYPROC glad_glShaderBinary;
PFNGLDRAWELEMENTSPROC glad_glDrawElements;
PFNGLVERTEXATTRIBI4SVPROC glad_glVertexAttribI4sv;
PFNGLUNIFORM1IVPROC glad_glUniform1iv;
PFNGLCREATESHADERPROGRAMVPROC glad_glCreateShaderProgramv;
PFNGLGETQUERYOBJECTIVPROC glad_glGetQueryObjectiv;
PFNGLREADBUFFERPROC glad_glReadBuffer;
PFNGLTEXPARAMETERIUIVPROC glad_glTexParameterIuiv;
PFNGLDRAWARRAYSINSTANCEDPROC glad_glDrawArraysInstanced;
PFNGLGENERATEMIPMAPPROC glad_glGenerateMipmap;
PFNGLPROGRAMUNIFORMMATRIX2FVPROC glad_glProgramUniformMatrix2fv;
PFNGLSAMPLERPARAMETERIVPROC glad_glSamplerParameteriv;
PFNGLVERTEXATTRIB3FPROC glad_glVertexAttrib3f;
PFNGLVERTEXATTRIB4UIVPROC glad_glVertexAttrib4uiv;
PFNGLPOINTPARAMETERIPROC glad_glPointParameteri;
PFNGLBLENDCOLORPROC glad_glBlendColor;
PFNGLSAMPLERPARAMETERIUIVPROC glad_glSamplerParameterIuiv;
PFNGLUNMAPBUFFERPROC glad_glUnmapBuffer;
PFNGLPOINTPARAMETERFPROC glad_glPointParameterf;
PFNGLPROGRAMUNIFORM1IVPROC glad_glProgramUniform1iv;
PFNGLVERTEXATTRIB3SPROC glad_glVertexAttrib3s;
PFNGLBINDRENDERBUFFERPROC glad_glBindRenderbuffer;
PFNGLVERTEXATTRIBP4UIVPROC glad_glVertexAttribP4uiv;
PFNGLGETPROGRAMSTAGEIVPROC glad_glGetProgramStageiv;
PFNGLISPROGRAMPROC glad_glIsProgram;
PFNGLVERTEXATTRIB4BVPROC glad_glVertexAttrib4bv;
PFNGLUNIFORMMATRIX3X2DVPROC glad_glUniformMatrix3x2dv;
PFNGLVERTEXATTRIB4FVPROC glad_glVertexAttrib4fv;
PFNGLPROGRAMUNIFORMMATRIX2X3DVPROC glad_glProgramUniformMatrix2x3dv;
PFNGLISTRANSFORMFEEDBACKPROC glad_glIsTransformFeedback;
PFNGLUNIFORM4IPROC glad_glUniform4i;
PFNGLACTIVETEXTUREPROC glad_glActiveTexture;
PFNGLENABLEVERTEXATTRIBARRAYPROC glad_glEnableVertexAttribArray;
PFNGLISPROGRAMPIPELINEPROC glad_glIsProgramPipeline;
PFNGLREADPIXELSPROC glad_glReadPixels;
PFNGLVERTEXATTRIBI3IVPROC glad_glVertexAttribI3iv;
PFNGLUNIFORM4FPROC glad_glUniform4f;
PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC glad_glRenderbufferStorageMultisample;
PFNGLUNIFORMMATRIX3FVPROC glad_glUniformMatrix3fv;
PFNGLVERTEXATTRIBLPOINTERPROC glad_glVertexAttribLPointer;
PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC glad_glDrawElementsInstancedBaseVertex;
PFNGLVERTEXATTRIBL2DVPROC glad_glVertexAttribL2dv;
PFNGLGETACTIVESUBROUTINENAMEPROC glad_glGetActiveSubroutineName;
PFNGLVERTEXATTRIBL2DPROC glad_glVertexAttribL2d;
PFNGLSTENCILFUNCPROC glad_glStencilFunc;
PFNGLUNIFORMBLOCKBINDINGPROC glad_glUniformBlockBinding;
PFNGLCOLORP4UIPROC glad_glColorP4ui;
PFNGLUSEPROGRAMSTAGESPROC glad_glUseProgramStages;
PFNGLPROGRAMUNIFORM3FPROC glad_glProgramUniform3f;
PFNGLPROGRAMUNIFORM3DPROC glad_glProgramUniform3d;
PFNGLVERTEXATTRIBI4IVPROC glad_glVertexAttribI4iv;
PFNGLGETPROGRAMPIPELINEIVPROC glad_glGetProgramPipelineiv;
PFNGLGETQUERYINDEXEDIVPROC glad_glGetQueryIndexediv;
PFNGLGETSHADERINFOLOGPROC glad_glGetShaderInfoLog;
PFNGLVERTEXATTRIBI4IPROC glad_glVertexAttribI4i;
PFNGLGETBUFFERSUBDATAPROC glad_glGetBufferSubData;
PFNGLGETVERTEXATTRIBLDVPROC glad_glGetVertexAttribLdv;
PFNGLBLENDEQUATIONSEPARATEPROC glad_glBlendEquationSeparate;
PFNGLVERTEXATTRIBI1UIPROC glad_glVertexAttribI1ui;
PFNGLGENBUFFERSPROC glad_glGenBuffers;
PFNGLGETSUBROUTINEINDEXPROC glad_glGetSubroutineIndex;
PFNGLVERTEXATTRIB2SVPROC glad_glVertexAttrib2sv;
PFNGLBLENDFUNCPROC glad_glBlendFunc;
PFNGLCREATEPROGRAMPROC glad_glCreateProgram;
PFNGLTEXIMAGE3DPROC glad_glTexImage3D;
PFNGLISFRAMEBUFFERPROC glad_glIsFramebuffer;
PFNGLPRIMITIVERESTARTINDEXPROC glad_glPrimitiveRestartIndex;
PFNGLGETINTEGER64VPROC glad_glGetInteger64v;
PFNGLBINDPROGRAMPIPELINEPROC glad_glBindProgramPipeline;
PFNGLSCISSORPROC glad_glScissor;
PFNGLTEXCOORDP4UIVPROC glad_glTexCoordP4uiv;
PFNGLGETBOOLEANVPROC glad_glGetBooleanv;
PFNGLVERTEXP2UIVPROC glad_glVertexP2uiv;
PFNGLUNIFORM3UIVPROC glad_glUniform3uiv;
PFNGLCLEARCOLORPROC glad_glClearColor;
PFNGLVERTEXATTRIB4NIVPROC glad_glVertexAttrib4Niv;
PFNGLCLEARBUFFERIVPROC glad_glClearBufferiv;
PFNGLGETBUFFERPARAMETERI64VPROC glad_glGetBufferParameteri64v;
PFNGLPROGRAMUNIFORM4DVPROC glad_glProgramUniform4dv;
PFNGLCOLORP4UIVPROC glad_glColorP4uiv;
PFNGLVERTEXATTRIBI2UIVPROC glad_glVertexAttribI2uiv;
PFNGLUNIFORM3UIPROC glad_glUniform3ui;
PFNGLPROGRAMUNIFORM3UIVPROC glad_glProgramUniform3uiv;
PFNGLVERTEXATTRIBI4UIVPROC glad_glVertexAttribI4uiv;
PFNGLPOINTPARAMETERFVPROC glad_glPointParameterfv;
PFNGLUNIFORM2FVPROC glad_glUniform2fv;
PFNGLGETACTIVESUBROUTINEUNIFORMNAMEPROC glad_glGetActiveSubroutineUniformName;
PFNGLDRAWELEMENTSINDIRECTPROC glad_glDrawElementsIndirect;
PFNGLGETSAMPLERPARAMETERIUIVPROC glad_glGetSamplerParameterIuiv;
PFNGLBINDBUFFERRANGEPROC glad_glBindBufferRange;
PFNGLVERTEXATTRIBL3DVPROC glad_glVertexAttribL3dv;
PFNGLGETUNIFORMDVPROC glad_glGetUniformdv;
PFNGLCLEARDEPTHFPROC glad_glClearDepthf;
PFNGLUNIFORMMATRIX2X3FVPROC glad_glUniformMatrix2x3fv;
PFNGLGENTRANSFORMFEEDBACKSPROC glad_glGenTransformFeedbacks;
PFNGLGETVERTEXATTRIBIUIVPROC glad_glGetVertexAttribIuiv;
PFNGLVERTEXATTRIB4NUSVPROC glad_glVertexAttrib4Nusv;
PFNGLPROGRAMUNIFORMMATRIX4X3DVPROC glad_glProgramUniformMatrix4x3dv;
PFNGLDEPTHFUNCPROC glad_glDepthFunc;
PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC glad_glCompressedTexSubImage2D;
PFNGLPROGRAMBINARYPROC glad_glProgramBinary;
PFNGLVERTEXATTRIBI4BVPROC glad_glVertexAttribI4bv;
PFNGLGETTEXPARAMETERFVPROC glad_glGetTexParameterfv;
PFNGLMULTITEXCOORDP1UIPROC glad_glMultiTexCoordP1ui;
PFNGLCLIENTWAITSYNCPROC glad_glClientWaitSync;
PFNGLVERTEXATTRIBI4UIPROC glad_glVertexAttribI4ui;
PFNGLGETFLOATI_VPROC glad_glGetFloati_v;
PFNGLCOLORMASKPROC glad_glColorMask;
PFNGLTEXPARAMETERIIVPROC glad_glTexParameterIiv;
PFNGLBLENDEQUATIONPROC glad_glBlendEquation;
PFNGLGETUNIFORMLOCATIONPROC glad_glGetUniformLocation;
PFNGLUNIFORMMATRIX2X4DVPROC glad_glUniformMatrix2x4dv;
PFNGLENDTRANSFORMFEEDBACKPROC glad_glEndTransformFeedback;
PFNGLVERTEXATTRIB4USVPROC glad_glVertexAttrib4usv;
PFNGLGETUNIFORMSUBROUTINEUIVPROC glad_glGetUniformSubroutineuiv;
PFNGLUNIFORM4FVPROC glad_glUniform4fv;
PFNGLBEGINTRANSFORMFEEDBACKPROC glad_glBeginTransformFeedback;
PFNGLVERTEXATTRIBI1IVPROC glad_glVertexAttribI1iv;
PFNGLISSAMPLERPROC glad_glIsSampler;
PFNGLVERTEXP3UIPROC glad_glVertexP3ui;
PFNGLVERTEXATTRIBDIVISORPROC glad_glVertexAttribDivisor;
PFNGLCOMPRESSEDTEXIMAGE1DPROC glad_glCompressedTexImage1D;
PFNGLDELETETRANSFORMFEEDBACKSPROC glad_glDeleteTransformFeedbacks;
PFNGLCOPYTEXSUBIMAGE1DPROC glad_glCopyTexSubImage1D;
PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC glad_glDrawRangeElementsBaseVertex;
PFNGLCHECKFRAMEBUFFERSTATUSPROC glad_glCheckFramebufferStatus;
PFNGLENDCONDITIONALRENDERPROC glad_glEndConditionalRender;
PFNGLVERTEXP3UIVPROC glad_glVertexP3uiv;
PFNGLBINDATTRIBLOCATIONPROC glad_glBindAttribLocation;
PFNGLUNIFORMMATRIX4X2FVPROC glad_glUniformMatrix4x2fv;
PFNGLUNIFORMMATRIX2DVPROC glad_glUniformMatrix2dv;
PFNGLBLENDFUNCIPROC glad_glBlendFunci;
PFNGLVERTEXATTRIB1DVPROC glad_glVertexAttrib1dv;
PFNGLDRAWRANGEELEMENTSPROC glad_glDrawRangeElements;
PFNGLGETQUERYOBJECTUIVPROC glad_glGetQueryObjectuiv;
PFNGLBINDBUFFERBASEPROC glad_glBindBufferBase;
PFNGLBUFFERSUBDATAPROC glad_glBufferSubData;
PFNGLVERTEXATTRIB4IVPROC glad_glVertexAttrib4iv;
PFNGLMAPBUFFERRANGEPROC glad_glMapBufferRange;
PFNGLFRAMEBUFFERTEXTUREPROC glad_glFramebufferTexture;
PFNGLBLENDFUNCSEPARATEIPROC glad_glBlendFuncSeparatei;
PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC glad_glProgramUniformMatrix4x2fv;
PFNGLVERTEXATTRIBL1DPROC glad_glVertexAttribL1d;
PFNGLMULTIDRAWARRAYSPROC glad_glMultiDrawArrays;
PFNGLVERTEXP4UIVPROC glad_glVertexP4uiv;
PFNGLVERTEXATTRIBI2IVPROC glad_glVertexAttribI2iv;
PFNGLGETSHADERPRECISIONFORMATPROC glad_glGetShaderPrecisionFormat;
PFNGLDISABLEIPROC glad_glDisablei;
PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC glad_glProgramUniformMatrix2x4fv;
PFNGLSHADERSOURCEPROC glad_glShaderSource;
PFNGLDELETERENDERBUFFERSPROC glad_glDeleteRenderbuffers;
PFNGLVERTEXATTRIBI3UIVPROC glad_glVertexAttribI3uiv;
PFNGLRELEASESHADERCOMPILERPROC glad_glReleaseShaderCompiler;
PFNGLGETSYNCIVPROC glad_glGetSynciv;
PFNGLTEXCOORDP2UIVPROC glad_glTexCoordP2uiv;
PFNGLPATCHPARAMETERFVPROC glad_glPatchParameterfv;
PFNGLPROGRAMUNIFORM2IPROC glad_glProgramUniform2i;
PFNGLBEGINQUERYPROC glad_glBeginQuery;
PFNGLUNIFORMMATRIX4FVPROC glad_glUniformMatrix4fv;
PFNGLBINDBUFFERPROC glad_glBindBuffer;
PFNGLPROGRAMUNIFORM2DPROC glad_glProgramUniform2d;
PFNGLPROGRAMUNIFORM2FPROC glad_glProgramUniform2f;
PFNGLUNIFORMMATRIX2FVPROC glad_glUniformMatrix2fv;
PFNGLUNIFORMMATRIX2X4FVPROC glad_glUniformMatrix2x4fv;
PFNGLBUFFERDATAPROC glad_glBufferData;
PFNGLGETTEXPARAMETERIIVPROC glad_glGetTexParameterIiv;
PFNGLTEXCOORDP1UIPROC glad_glTexCoordP1ui;
PFNGLGETERRORPROC glad_glGetError;
PFNGLVERTEXATTRIBP2UIPROC glad_glVertexAttribP2ui;
PFNGLGETFLOATVPROC glad_glGetFloatv;
PFNGLTEXSUBIMAGE1DPROC glad_glTexSubImage1D;
PFNGLVERTEXATTRIB2FVPROC glad_glVertexAttrib2fv;
PFNGLGETTEXLEVELPARAMETERFVPROC glad_glGetTexLevelParameterfv;
PFNGLVERTEXATTRIBI1IPROC glad_glVertexAttribI1i;
PFNGLVERTEXATTRIBP3UIVPROC glad_glVertexAttribP3uiv;
PFNGLUNIFORM4DPROC glad_glUniform4d;
PFNGLSECONDARYCOLORP3UIVPROC glad_glSecondaryColorP3uiv;
PFNGLGETINTEGERVPROC glad_glGetIntegerv;
PFNGLGETBUFFERPOINTERVPROC glad_glGetBufferPointerv;
PFNGLPROGRAMUNIFORMMATRIX3DVPROC glad_glProgramUniformMatrix3dv;
PFNGLFRAMEBUFFERTEXTURE3DPROC glad_glFramebufferTexture3D;
PFNGLISQUERYPROC glad_glIsQuery;
PFNGLPROGRAMUNIFORM2UIPROC glad_glProgramUniform2ui;
PFNGLPROGRAMUNIFORM4UIPROC glad_glProgramUniform4ui;
PFNGLVERTEXATTRIB4SVPROC glad_glVertexAttrib4sv;
PFNGLTEXIMAGE2DPROC glad_glTexImage2D;
PFNGLSTENCILMASKPROC glad_glStencilMask;
PFNGLSAMPLERPARAMETERFVPROC glad_glSamplerParameterfv;
PFNGLISTEXTUREPROC glad_glIsTexture;
PFNGLUNIFORM1FVPROC glad_glUniform1fv;
PFNGLVERTEXATTRIB4NUBVPROC glad_glVertexAttrib4Nubv;
PFNGLTEXPARAMETERFVPROC glad_glTexParameterfv;
PFNGLSCISSORINDEXEDVPROC glad_glScissorIndexedv;
PFNGLUNIFORM3DVPROC glad_glUniform3dv;
PFNGLPROGRAMUNIFORM3FVPROC glad_glProgramUniform3fv;
PFNGLGETSUBROUTINEUNIFORMLOCATIONPROC glad_glGetSubroutineUniformLocation;
PFNGLGETSAMPLERPARAMETERIVPROC glad_glGetSamplerParameteriv;
PFNGLCOPYBUFFERSUBDATAPROC glad_glCopyBufferSubData;
PFNGLVERTEXATTRIBI1UIVPROC glad_glVertexAttribI1uiv;
PFNGLVERTEXATTRIB2DPROC glad_glVertexAttrib2d;
PFNGLVERTEXATTRIB2FPROC glad_glVertexAttrib2f;
PFNGLVERTEXATTRIB3DVPROC glad_glVertexAttrib3dv;
PFNGLGETQUERYOBJECTUI64VPROC glad_glGetQueryObjectui64v;
PFNGLDEPTHMASKPROC glad_glDepthMask;
PFNGLVERTEXATTRIB2SPROC glad_glVertexAttrib2s;
PFNGLTEXIMAGE3DMULTISAMPLEPROC glad_glTexImage3DMultisample;
PFNGLPROGRAMUNIFORM1FVPROC glad_glProgramUniform1fv;
PFNGLGETUNIFORMBLOCKINDEXPROC glad_glGetUniformBlockIndex;
PFNGLTEXIMAGE2DMULTISAMPLEPROC glad_glTexImage2DMultisample;
PFNGLGETACTIVEUNIFORMPROC glad_glGetActiveUniform;
PFNGLFRONTFACEPROC glad_glFrontFace;
PFNGLDELETEPROGRAMPIPELINESPROC glad_glDeleteProgramPipelines;
int GLAD_GL_KHR_debug;
PFNGLDEBUGMESSAGECONTROLPROC glad_glDebugMessageControl;
PFNGLDEBUGMESSAGEINSERTPROC glad_glDebugMessageInsert;
PFNGLDEBUGMESSAGECALLBACKPROC glad_glDebugMessageCallback;
PFNGLGETDEBUGMESSAGELOGPROC glad_glGetDebugMessageLog;
PFNGLPUSHDEBUGGROUPPROC glad_glPushDebugGroup;
PFNGLPOPDEBUGGROUPPROC glad_glPopDebugGroup;
PFNGLOBJECTLABELPROC glad_glObjectLabel;
PFNGLGETOBJECTLABELPROC glad_glGetObjectLabel;
PFNGLOBJECTPTRLABELPROC glad_glObjectPtrLabel;
PFNGLGETOBJECTPTRLABELPROC glad_glGetObjectPtrLabel;
PFNGLGETPOINTERVPROC glad_glGetPointerv;
PFNGLDEBUGMESSAGECONTROLKHRPROC glad_glDebugMessageControlKHR;
PFNGLDEBUGMESSAGEINSERTKHRPROC glad_glDebugMessageInsertKHR;
PFNGLDEBUGMESSAGECALLBACKKHRPROC glad_glDebugMessageCallbackKHR;
PFNGLGETDEBUGMESSAGELOGKHRPROC glad_glGetDebugMessageLogKHR;
PFNGLPUSHDEBUGGROUPKHRPROC glad_glPushDebugGroupKHR;
PFNGLPOPDEBUGGROUPKHRPROC glad_glPopDebugGroupKHR;
PFNGLOBJECTLABELKHRPROC glad_glObjectLabelKHR;
PFNGLGETOBJECTLABELKHRPROC glad_glGetObjectLabelKHR;
PFNGLOBJECTPTRLABELKHRPROC glad_glObjectPtrLabelKHR;
PFNGLGETOBJECTPTRLABELKHRPROC glad_glGetObjectPtrLabelKHR;
PFNGLGETPOINTERVKHRPROC glad_glGetPointervKHR;
static void load_GL_VERSION_1_0(GLADloadproc load) {
	if(!GLAD_GL_VERSION_1_0) return;
	glad_glCullFace = (PFNGLCULLFACEPROC)load("glCullFace");
	glad_glFrontFace = (PFNGLFRONTFACEPROC)load("glFrontFace");
	glad_glHint = (PFNGLHINTPROC)load("glHint");
	glad_glLineWidth = (PFNGLLINEWIDTHPROC)load("glLineWidth");
	glad_glPointSize = (PFNGLPOINTSIZEPROC)load("glPointSize");
	glad_glPolygonMode = (PFNGLPOLYGONMODEPROC)load("glPolygonMode");
	glad_glScissor = (PFNGLSCISSORPROC)load("glScissor");
	glad_glTexParameterf = (PFNGLTEXPARAMETERFPROC)load("glTexParameterf");
	glad_glTexParameterfv = (PFNGLTEXPARAMETERFVPROC)load("glTexParameterfv");
	glad_glTexParameteri = (PFNGLTEXPARAMETERIPROC)load("glTexParameteri");
	glad_glTexParameteriv = (PFNGLTEXPARAMETERIVPROC)load("glTexParameteriv");
	glad_glTexImage1D = (PFNGLTEXIMAGE1DPROC)load("glTexImage1D");
	glad_glTexImage2D = (PFNGLTEXIMAGE2DPROC)load("glTexImage2D");
	glad_glDrawBuffer = (PFNGLDRAWBUFFERPROC)load("glDrawBuffer");
	glad_glClear = (PFNGLCLEARPROC)load("glClear");
	glad_glClearColor = (PFNGLCLEARCOLORPROC)load("glClearColor");
	glad_glClearStencil = (PFNGLCLEARSTENCILPROC)load("glClearStencil");
	glad_glClearDepth = (PFNGLCLEARDEPTHPROC)load("glClearDepth");
	glad_glStencilMask = (PFNGLSTENCILMASKPROC)load("glStencilMask");
	glad_glColorMask = (PFNGLCOLORMASKPROC)load("glColorMask");
	glad_glDepthMask = (PFNGLDEPTHMASKPROC)load("glDepthMask");
	glad_glDisable = (PFNGLDISABLEPROC)load("glDisable");
	glad_glEnable = (PFNGLENABLEPROC)load("glEnable");
	glad_glFinish = (PFNGLFINISHPROC)load("glFinish");
	glad_glFlush = (PFNGLFLUSHPROC)load("glFlush");
	glad_glBlendFunc = (PFNGLBLENDFUNCPROC)load("glBlendFunc");
	glad_glLogicOp = (PFNGLLOGICOPPROC)load("glLogicOp");
	glad_glStencilFunc = (PFNGLSTENCILFUNCPROC)load("glStencilFunc");
	glad_glStencilOp = (PFNGLSTENCILOPPROC)load("glStencilOp");
	glad_glDepthFunc = (PFNGLDEPTHFUNCPROC)load("glDepthFunc");
	glad_glPixelStoref = (PFNGLPIXELSTOREFPROC)load("glPixelStoref");
	glad_glPixelStorei = (PFNGLPIXELSTOREIPROC)load("glPixelStorei");
	glad_glReadBuffer = (PFNGLREADBUFFERPROC)load("glReadBuffer");
	glad_glReadPixels = (PFNGLREADPIXELSPROC)load("glReadPixels");
	glad_glGetBooleanv = (PFNGLGETBOOLEANVPROC)load("glGetBooleanv");
	glad_glGetDoublev = (PFNGLGETDOUBLEVPROC)load("glGetDoublev");
	glad_glGetError = (PFNGLGETERRORPROC)load("glGetError");
	glad_glGetFloatv = (PFNGLGETFLOATVPROC)load("glGetFloatv");
	glad_glGetIntegerv = (PFNGLGETINTEGERVPROC)load("glGetIntegerv");
	glad_glGetString = (PFNGLGETSTRINGPROC)load("glGetString");
	glad_glGetTexImage = (PFNGLGETTEXIMAGEPROC)load("glGetTexImage");
	glad_glGetTexParameterfv = (PFNGLGETTEXPARAMETERFVPROC)load("glGetTexParameterfv");
	glad_glGetTexParameteriv = (PFNGLGETTEXPARAMETERIVPROC)load("glGetTexParameteriv");
	glad_glGetTexLevelParameterfv = (PFNGLGETTEXLEVELPARAMETERFVPROC)load("glGetTexLevelParameterfv");
	glad_glGetTexLevelParameteriv = (PFNGLGETTEXLEVELPARAMETERIVPROC)load("glGetTexLevelParameteriv");
	glad_glIsEnabled = (PFNGLISENABLEDPROC)load("glIsEnabled");
	glad_glDepthRange = (PFNGLDEPTHRANGEPROC)load("glDepthRange");
	glad_glViewport = (PFNGLVIEWPORTPROC)load("glViewport");
}
static void load_GL_VERSION_1_1(GLADloadproc load) {
	if(!GLAD_GL_VERSION_1_1) return;
	glad_glDrawArrays = (PFNGLDRAWARRAYSPROC)load("glDrawArrays");
	glad_glDrawElements = (PFNGLDRAWELEMENTSPROC)load("glDrawElements");
	glad_glPolygonOffset = (PFNGLPOLYGONOFFSETPROC)load("glPolygonOffset");
	glad_glCopyTexImage1D = (PFNGLCOPYTEXIMAGE1DPROC)load("glCopyTexImage1D");
	glad_glCopyTexImage2D = (PFNGLCOPYTEXIMAGE2DPROC)load("glCopyTexImage2D");
	glad_glCopyTexSubImage1D = (PFNGLCOPYTEXSUBIMAGE1DPROC)load("glCopyTexSubImage1D");
	glad_glCopyTexSubImage2D = (PFNGLCOPYTEXSUBIMAGE2DPROC)load("glCopyTexSubImage2D");
	glad_glTexSubImage1D = (PFNGLTEXSUBIMAGE1DPROC)load("glTexSubImage1D");
	glad_glTexSubImage2D = (PFNGLTEXSUBIMAGE2DPROC)load("glTexSubImage2D");
	glad_glBindTexture = (PFNGLBINDTEXTUREPROC)load("glBindTexture");
	glad_glDeleteTextures = (PFNGLDELETETEXTURESPROC)load("glDeleteTextures");
	glad_glGenTextures = (PFNGLGENTEXTURESPROC)load("glGenTextures");
	glad_glIsTexture = (PFNGLISTEXTUREPROC)load("glIsTexture");
}
static void load_GL_VERSION_1_2(GLADloadproc load) {
	if(!GLAD_GL_VERSION_1_2) return;
	glad_glDrawRangeElements = (PFNGLDRAWRANGEELEMENTSPROC)load("glDrawRangeElements");
	glad_glTexImage3D = (PFNGLTEXIMAGE3DPROC)load("glTexImage3D");
	glad_glTexSubImage3D = (PFNGLTEXSUBIMAGE3DPROC)load("glTexSubImage3D");
	glad_glCopyTexSubImage3D = (PFNGLCOPYTEXSUBIMAGE3DPROC)load("glCopyTexSubImage3D");
}
static void load_GL_VERSION_1_3(GLADloadproc load) {
	if(!GLAD_GL_VERSION_1_3) return;
	glad_glActiveTexture = (PFNGLACTIVETEXTUREPROC)load("glActiveTexture");
	glad_glSampleCoverage = (PFNGLSAMPLECOVERAGEPROC)load("glSampleCoverage");
	glad_glCompressedTexImage3D = (PFNGLCOMPRESSEDTEXIMAGE3DPROC)load("glCompressedTexImage3D");
	glad_glCompressedTexImage2D = (PFNGLCOMPRESSEDTEXIMAGE2DPROC)load("glCompressedTexImage2D");
	glad_glCompressedTexImage1D = (PFNGLCOMPRESSEDTEXIMAGE1DPROC)load("glCompressedTexImage1D");
	glad_glCompressedTexSubImage3D = (PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC)load("glCompressedTexSubImage3D");
	glad_glCompressedTexSubImage2D = (PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC)load("glCompressedTexSubImage2D");
	glad_glCompressedTexSubImage1D = (PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC)load("glCompressedTexSubImage1D");
	glad_glGetCompressedTexImage = (PFNGLGETCOMPRESSEDTEXIMAGEPROC)load("glGetCompressedTexImage");
}
static void load_GL_VERSION_1_4(GLADloadproc load) {
	if(!GLAD_GL_VERSION_1_4) return;
	glad_glBlendFuncSeparate = (PFNGLBLENDFUNCSEPARATEPROC)load("glBlendFuncSeparate");
	glad_glMultiDrawArrays = (PFNGLMULTIDRAWARRAYSPROC)load("glMultiDrawArrays");
	glad_glMultiDrawElements = (PFNGLMULTIDRAWELEMENTSPROC)load("glMultiDrawElements");
	glad_glPointParameterf = (PFNGLPOINTPARAMETERFPROC)load("glPointParameterf");
	glad_glPointParameterfv = (PFNGLPOINTPARAMETERFVPROC)load("glPointParameterfv");
	glad_glPointParameteri = (PFNGLPOINTPARAMETERIPROC)load("glPointParameteri");
	glad_glPointParameteriv = (PFNGLPOINTPARAMETERIVPROC)load("glPointParameteriv");
	glad_glBlendColor = (PFNGLBLENDCOLORPROC)load("glBlendColor");
	glad_glBlendEquation = (PFNGLBLENDEQUATIONPROC)load("glBlendEquation");
}
static void load_GL_VERSION_1_5(GLADloadproc load) {
	if(!GLAD_GL_VERSION_1_5) return;
	glad_glGenQueries = (PFNGLGENQUERIESPROC)load("glGenQueries");
	glad_glDeleteQueries = (PFNGLDELETEQUERIESPROC)load("glDeleteQueries");
	glad_glIsQuery = (PFNGLISQUERYPROC)load("glIsQuery");
	glad_glBeginQuery = (PFNGLBEGINQUERYPROC)load("glBeginQuery");
	glad_glEndQuery = (PFNGLENDQUERYPROC)load("glEndQuery");
	glad_glGetQueryiv = (PFNGLGETQUERYIVPROC)load("glGetQueryiv");
	glad_glGetQueryObjectiv = (PFNGLGETQUERYOBJECTIVPROC)load("glGetQueryObjectiv");
	glad_glGetQueryObjectuiv = (PFNGLGETQUERYOBJECTUIVPROC)load("glGetQueryObjectuiv");
	glad_glBindBuffer = (PFNGLBINDBUFFERPROC)load("glBindBuffer");
	glad_glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)load("glDeleteBuffers");
	glad_glGenBuffers = (PFNGLGENBUFFERSPROC)load("glGenBuffers");
	glad_glIsBuffer = (PFNGLISBUFFERPROC)load("glIsBuffer");
	glad_glBufferData = (PFNGLBUFFERDATAPROC)load("glBufferData");
	glad_glBufferSubData = (PFNGLBUFFERSUBDATAPROC)load("glBufferSubData");
	glad_glGetBufferSubData = (PFNGLGETBUFFERSUBDATAPROC)load("glGetBufferSubData");
	glad_glMapBuffer = (PFNGLMAPBUFFERPROC)load("glMapBuffer");
	glad_glUnmapBuffer = (PFNGLUNMAPBUFFERPROC)load("glUnmapBuffer");
	glad_glGetBufferParameteriv = (PFNGLGETBUFFERPARAMETERIVPROC)load("glGetBufferParameteriv");
	glad_glGetBufferPointerv = (PFNGLGETBUFFERPOINTERVPROC)load("glGetBufferPointerv");
}
static void load_GL_VERSION_2_0(GLADloadproc load) {
	if(!GLAD_GL_VERSION_2_0) return;
	glad_glBlendEquationSeparate = (PFNGLBLENDEQUATIONSEPARATEPROC)load("glBlendEquationSeparate");
	glad_glDrawBuffers = (PFNGLDRAWBUFFERSPROC)load("glDrawBuffers");
	glad_glStencilOpSeparate = (PFNGLSTENCILOPSEPARATEPROC)load("glStencilOpSeparate");
	glad_glStencilFuncSeparate = (PFNGLSTENCILFUNCSEPARATEPROC)load("glStencilFuncSeparate");
	glad_glStencilMaskSeparate = (PFNGLSTENCILMASKSEPARATEPROC)load("glStencilMaskSeparate");
	glad_glAttachShader = (PFNGLATTACHSHADERPROC)load("glAttachShader");
	glad_glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC)load("glBindAttribLocation");
	glad_glCompileShader = (PFNGLCOMPILESHADERPROC)load("glCompileShader");
	glad_glCreateProgram = (PFNGLCREATEPROGRAMPROC)load("glCreateProgram");
	glad_glCreateShader = (PFNGLCREATESHADERPROC)load("glCreateShader");
	glad_glDeleteProgram = (PFNGLDELETEPROGRAMPROC)load("glDeleteProgram");
	glad_glDeleteShader = (PFNGLDELETESHADERPROC)load("glDeleteShader");
	glad_glDetachShader = (PFNGLDETACHSHADERPROC)load("glDetachShader");
	glad_glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)load("glDisableVertexAttribArray");
	glad_glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)load("glEnableVertexAttribArray");
	glad_glGetActiveAttrib = (PFNGLGETACTIVEATTRIBPROC)load("glGetActiveAttrib");
	glad_glGetActiveUniform = (PFNGLGETACTIVEUNIFORMPROC)load("glGetActiveUniform");
	glad_glGetAttachedShaders = (PFNGLGETATTACHEDSHADERSPROC)load("glGetAttachedShaders");
	glad_glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)load("glGetAttribLocation");
	glad_glGetProgramiv = (PFNGLGETPROGRAMIVPROC)load("glGetProgramiv");
	glad_glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)load("glGetProgramInfoLog");
	glad_glGetShaderiv = (PFNGLGETSHADERIVPROC)load("glGetShaderiv");
	glad_glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)load("glGetShaderInfoLog");
	glad_glGetShaderSource = (PFNGLGETSHADERSOURCEPROC)load("glGetShaderSource");
	glad_glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)load("glGetUniformLocation");
	glad_glGetUniformfv = (PFNGLGETUNIFORMFVPROC)load("glGetUniformfv");
	glad_glGetUniformiv = (PFNGLGETUNIFORMIVPROC)load("glGetUniformiv");
	glad_glGetVertexAttribdv = (PFNGLGETVERTEXATTRIBDVPROC)load("glGetVertexAttribdv");
	glad_glGetVertexAttribfv = (PFNGLGETVERTEXATTRIBFVPROC)load("glGetVertexAttribfv");
	glad_glGetVertexAttribiv = (PFNGLGETVERTEXATTRIBIVPROC)load("glGetVertexAttribiv");
	glad_glGetVertexAttribPointerv = (PFNGLGETVERTEXATTRIBPOINTERVPROC)load("glGetVertexAttribPointerv");
	glad_glIsProgram = (PFNGLISPROGRAMPROC)load("glIsProgram");
	glad_glIsShader = (PFNGLISSHADERPROC)load("glIsShader");
	glad_glLinkProgram = (PFNGLLINKPROGRAMPROC)load("glLinkProgram");
	glad_glShaderSource = (PFNGLSHADERSOURCEPROC)load("glShaderSource");
	glad_glUseProgram = (PFNGLUSEPROGRAMPROC)load("glUseProgram");
	glad_glUniform1f = (PFNGLUNIFORM1FPROC)load("glUniform1f");
	glad_glUniform2f = (PFNGLUNIFORM2FPROC)load("glUniform2f");
	glad_glUniform3f = (PFNGLUNIFORM3FPROC)load("glUniform3f");
	glad_glUniform4f = (PFNGLUNIFORM4FPROC)load("glUniform4f");
	glad_glUniform1i = (PFNGLUNIFORM1IPROC)load("glUniform1i");
	glad_glUniform2i = (PFNGLUNIFORM2IPROC)load("glUniform2i");
	glad_glUniform3i = (PFNGLUNIFORM3IPROC)load("glUniform3i");
	glad_glUniform4i = (PFNGLUNIFORM4IPROC)load("glUniform4i");
	glad_glUniform1fv = (PFNGLUNIFORM1FVPROC)load("glUniform1fv");
	glad_glUniform2fv = (PFNGLUNIFORM2FVPROC)load("glUniform2fv");
	glad_glUniform3fv = (PFNGLUNIFORM3FVPROC)load("glUniform3fv");
	glad_glUniform4fv = (PFNGLUNIFORM4FVPROC)load("glUniform4fv");
	glad_glUniform1iv = (PFNGLUNIFORM1IVPROC)load("glUniform1iv");
	glad_glUniform2iv = (PFNGLUNIFORM2IVPROC)load("glUniform2iv");
	glad_glUniform3iv = (PFNGLUNIFORM3IVPROC)load("glUniform3iv");
	glad_glUniform4iv = (PFNGLUNIFORM4IVPROC)load("glUniform4iv");
	glad_glUniformMatrix2fv = (PFNGLUNIFORMMATRIX2FVPROC)load("glUniformMatrix2fv");
	glad_glUniformMatrix3fv = (PFNGLUNIFORMMATRIX3FVPROC)load("glUniformMatrix3fv");
	glad_glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)load("glUniformMatrix4fv");
	glad_glValidateProgram = (PFNGLVALIDATEPROGRAMPROC)load("glValidateProgram");
	glad_glVertexAttrib1d = (PFNGLVERTEXATTRIB1DPROC)load("glVertexAttrib1d");
	glad_glVertexAttrib1dv = (PFNGLVERTEXATTRIB1DVPROC)load("glVertexAttrib1dv");
	glad_glVertexAttrib1f = (PFNGLVERTEXATTRIB1FPROC)load("glVertexAttrib1f");
	glad_glVertexAttrib1fv = (PFNGLVERTEXATTRIB1FVPROC)load("glVertexAttrib1fv");
	glad_glVertexAttrib1s = (PFNGLVERTEXATTRIB1SPROC)load("glVertexAttrib1s");
	glad_glVertexAttrib1sv = (PFNGLVERTEXATTRIB1SVPROC)load("glVertexAttrib1sv");
	glad_glVertexAttrib2d = (PFNGLVERTEXATTRIB2DPROC)load("glVertexAttrib2d");
	glad_glVertexAttrib2dv = (PFNGLVERTEXATTRIB2DVPROC)load("glVertexAttrib2dv");
	glad_glVertexAttrib2f = (PFNGLVERTEXATTRIB2FPROC)load("glVertexAttrib2f");
	glad_glVertexAttrib2fv = (PFNGLVERTEXATTRIB2FVPROC)load("glVertexAttrib2fv");
	glad_glVertexAttrib2s = (PFNGLVERTEXATTRIB2SPROC)load("glVertexAttrib2s");
	glad_glVertexAttrib2sv = (PFNGLVERTEXATTRIB2SVPROC)load("glVertexAttrib2sv");
	glad_glVertexAttrib3d = (PFNGLVERTEXATTRIB3DPROC)load("glVertexAttrib3d");
	glad_glVertexAttrib3dv = (PFNGLVERTEXATTRIB3DVPROC)load("glVertexAttrib3dv");
	glad_glVertexAttrib3f = (PFNGLVERTEXATTRIB3FPROC)load("glVertexAttrib3f");
	glad_glVertexAttrib3fv = (PFNGLVERTEXATTRIB3FVPROC)load("glVertexAttrib3fv");
	glad_glVertexAttrib3s = (PFNGLVERTEXATTRIB3SPROC)load("glVertexAttrib3s");
	glad_glVertexAttrib3sv = (PFNGLVERTEXATTRIB3SVPROC)load("glVertexAttrib3sv");
	glad_glVertexAttrib4Nbv = (PFNGLVERTEXATTRIB4NBVPROC)load("glVertexAttrib4Nbv");
	glad_glVertexAttrib4Niv = (PFNGLVERTEXATTRIB4NIVPROC)load("glVertexAttrib4Niv");
	glad_glVertexAttrib4Nsv = (PFNGLVERTEXATTRIB4NSVPROC)load("glVertexAttrib4Nsv");
	glad_glVertexAttrib4Nub = (PFNGLVERTEXATTRIB4NUBPROC)load("glVertexAttrib4Nub");
	glad_glVertexAttrib4Nubv = (PFNGLVERTEXATTRIB4NUBVPROC)load("glVertexAttrib4Nubv");
	glad_glVertexAttrib4Nuiv = (PFNGLVERTEXATTRIB4NUIVPROC)load("glVertexAttrib4Nuiv");
	glad_glVertexAttrib4Nusv = (PFNGLVERTEXATTRIB4NUSVPROC)load("glVertexAttrib4Nusv");
	glad_glVertexAttrib4bv = (PFNGLVERTEXATTRIB4BVPROC)load("glVertexAttrib4bv");
	glad_glVertexAttrib4d = (PFNGLVERTEXATTRIB4DPROC)load("glVertexAttrib4d");
	glad_glVertexAttrib4dv = (PFNGLVERTEXATTRIB4DVPROC)load("glVertexAttrib4dv");
	glad_glVertexAttrib4f = (PFNGLVERTEXATTRIB4FPROC)load("glVertexAttrib4f");
	glad_glVertexAttrib4fv = (PFNGLVERTEXATTRIB4FVPROC)load("glVertexAttrib4fv");
	glad_glVertexAttrib4iv = (PFNGLVERTEXATTRIB4IVPROC)load("glVertexAttrib4iv");
	glad_glVertexAttrib4s = (PFNGLVERTEXATTRIB4SPROC)load("glVertexAttrib4s");
	glad_glVertexAttrib4sv = (PFNGLVERTEXATTRIB4SVPROC)load("glVertexAttrib4sv");
	glad_glVertexAttrib4ubv = (PFNGLVERTEXATTRIB4UBVPROC)load("glVertexAttrib4ubv");
	glad_glVertexAttrib4uiv = (PFNGLVERTEXATTRIB4UIVPROC)load("glVertexAttrib4uiv");
	glad_glVertexAttrib4usv = (PFNGLVERTEXATTRIB4USVPROC)load("glVertexAttrib4usv");
	glad_glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)load("glVertexAttribPointer");
}
static void load_GL_VERSION_2_1(GLADloadproc load) {
	if(!GLAD_GL_VERSION_2_1) return;
	glad_glUniformMatrix2x3fv = (PFNGLUNIFORMMATRIX2X3FVPROC)load("glUniformMatrix2x3fv");
	glad_glUniformMatrix3x2fv = (PFNGLUNIFORMMATRIX3X2FVPROC)load("glUniformMatrix3x2fv");
	glad_glUniformMatrix2x4fv = (PFNGLUNIFORMMATRIX2X4FVPROC)load("glUniformMatrix2x4fv");
	glad_glUniformMatrix4x2fv = (PFNGLUNIFORMMATRIX4X2FVPROC)load("glUniformMatrix4x2fv");
	glad_glUniformMatrix3x4fv = (PFNGLUNIFORMMATRIX3X4FVPROC)load("glUniformMatrix3x4fv");
	glad_glUniformMatrix4x3fv = (PFNGLUNIFORMMATRIX4X3FVPROC)load("glUniformMatrix4x3fv");
}
static void load_GL_VERSION_3_0(GLADloadproc load) {
	if(!GLAD_GL_VERSION_3_0) return;
	glad_glColorMaski = (PFNGLCOLORMASKIPROC)load("glColorMaski");
	glad_glGetBooleani_v = (PFNGLGETBOOLEANI_VPROC)load("glGetBooleani_v");
	glad_glGetIntegeri_v = (PFNGLGETINTEGERI_VPROC)load("glGetIntegeri_v");
	glad_glEnablei = (PFNGLENABLEIPROC)load("glEnablei");
	glad_glDisablei = (PFNGLDISABLEIPROC)load("glDisablei");
	glad_glIsEnabledi = (PFNGLISENABLEDIPROC)load("glIsEnabledi");
	glad_glBeginTransformFeedback = (PFNGLBEGINTRANSFORMFEEDBACKPROC)load("glBeginTransformFeedback");
	glad_glEndTransformFeedback = (PFNGLENDTRANSFORMFEEDBACKPROC)load("glEndTransformFeedback");
	glad_glBindBufferRange = (PFNGLBINDBUFFERRANGEPROC)load("glBindBufferRange");
	glad_glBindBufferBase = (PFNGLBINDBUFFERBASEPROC)load("glBindBufferBase");
	glad_glTransformFeedbackVaryings = (PFNGLTRANSFORMFEEDBACKVARYINGSPROC)load("glTransformFeedbackVaryings");
	glad_glGetTransformFeedbackVarying = (PFNGLGETTRANSFORMFEEDBACKVARYINGPROC)load("glGetTransformFeedbackVarying");
	glad_glClampColor = (PFNGLCLAMPCOLORPROC)load("glClampColor");
	glad_glBeginConditionalRender = (PFNGLBEGINCONDITIONALRENDERPROC)load("glBeginConditionalRender");
	glad_glEndConditionalRender = (PFNGLENDCONDITIONALRENDERPROC)load("glEndConditionalRender");
	glad_glVertexAttribIPointer = (PFNGLVERTEXATTRIBIPOINTERPROC)load("glVertexAttribIPointer");
	glad_glGetVertexAttribIiv = (PFNGLGETVERTEXATTRIBIIVPROC)load("glGetVertexAttribIiv");
	glad_glGetVertexAttribIuiv = (PFNGLGETVERTEXATTRIBIUIVPROC)load("glGetVertexAttribIuiv");
	glad_glVertexAttribI1i = (PFNGLVERTEXATTRIBI1IPROC)load("glVertexAttribI1i");
	glad_glVertexAttribI2i = (PFNGLVERTEXATTRIBI2IPROC)load("glVertexAttribI2i");
	glad_glVertexAttribI3i = (PFNGLVERTEXATTRIBI3IPROC)load("glVertexAttribI3i");
	glad_glVertexAttribI4i = (PFNGLVERTEXATTRIBI4IPROC)load("glVertexAttribI4i");
	glad_glVertexAttribI1ui = (PFNGLVERTEXATTRIBI1UIPROC)load("glVertexAttribI1ui");
	glad_glVertexAttribI2ui = (PFNGLVERTEXATTRIBI2UIPROC)load("glVertexAttribI2ui");
	glad_glVertexAttribI3ui = (PFNGLVERTEXATTRIBI3UIPROC)load("glVertexAttribI3ui");
	glad_glVertexAttribI4ui = (PFNGLVERTEXATTRIBI4UIPROC)load("glVertexAttribI4ui");
	glad_glVertexAttribI1iv = (PFNGLVERTEXATTRIBI1IVPROC)load("glVertexAttribI1iv");
	glad_glVertexAttribI2iv = (PFNGLVERTEXATTRIBI2IVPROC)load("glVertexAttribI2iv");
	glad_glVertexAttribI3iv = (PFNGLVERTEXATTRIBI3IVPROC)load("glVertexAttribI3iv");
	glad_glVertexAttribI4iv = (PFNGLVERTEXATTRIBI4IVPROC)load("glVertexAttribI4iv");
	glad_glVertexAttribI1uiv = (PFNGLVERTEXATTRIBI1UIVPROC)load("glVertexAttribI1uiv");
	glad_glVertexAttribI2uiv = (PFNGLVERTEXATTRIBI2UIVPROC)load("glVertexAttribI2uiv");
	glad_glVertexAttribI3uiv = (PFNGLVERTEXATTRIBI3UIVPROC)load("glVertexAttribI3uiv");
	glad_glVertexAttribI4uiv = (PFNGLVERTEXATTRIBI4UIVPROC)load("glVertexAttribI4uiv");
	glad_glVertexAttribI4bv = (PFNGLVERTEXATTRIBI4BVPROC)load("glVertexAttribI4bv");
	glad_glVertexAttribI4sv = (PFNGLVERTEXATTRIBI4SVPROC)load("glVertexAttribI4sv");
	glad_glVertexAttribI4ubv = (PFNGLVERTEXATTRIBI4UBVPROC)load("glVertexAttribI4ubv");
	glad_glVertexAttribI4usv = (PFNGLVERTEXATTRIBI4USVPROC)load("glVertexAttribI4usv");
	glad_glGetUniformuiv = (PFNGLGETUNIFORMUIVPROC)load("glGetUniformuiv");
	glad_glBindFragDataLocation = (PFNGLBINDFRAGDATALOCATIONPROC)load("glBindFragDataLocation");
	glad_glGetFragDataLocation = (PFNGLGETFRAGDATALOCATIONPROC)load("glGetFragDataLocation");
	glad_glUniform1ui = (PFNGLUNIFORM1UIPROC)load("glUniform1ui");
	glad_glUniform2ui = (PFNGLUNIFORM2UIPROC)load("glUniform2ui");
	glad_glUniform3ui = (PFNGLUNIFORM3UIPROC)load("glUniform3ui");
	glad_glUniform4ui = (PFNGLUNIFORM4UIPROC)load("glUniform4ui");
	glad_glUniform1uiv = (PFNGLUNIFORM1UIVPROC)load("glUniform1uiv");
	glad_glUniform2uiv = (PFNGLUNIFORM2UIVPROC)load("glUniform2uiv");
	glad_glUniform3uiv = (PFNGLUNIFORM3UIVPROC)load("glUniform3uiv");
	glad_glUniform4uiv = (PFNGLUNIFORM4UIVPROC)load("glUniform4uiv");
	glad_glTexParameterIiv = (PFNGLTEXPARAMETERIIVPROC)load("glTexParameterIiv");
	glad_glTexParameterIuiv = (PFNGLTEXPARAMETERIUIVPROC)load("glTexParameterIuiv");
	glad_glGetTexParameterIiv = (PFNGLGETTEXPARAMETERIIVPROC)load("glGetTexParameterIiv");
	glad_glGetTexParameterIuiv = (PFNGLGETTEXPARAMETERIUIVPROC)load("glGetTexParameterIuiv");
	glad_glClearBufferiv = (PFNGLCLEARBUFFERIVPROC)load("glClearBufferiv");
	glad_glClearBufferuiv = (PFNGLCLEARBUFFERUIVPROC)load("glClearBufferuiv");
	glad_glClearBufferfv = (PFNGLCLEARBUFFERFVPROC)load("glClearBufferfv");
	glad_glClearBufferfi = (PFNGLCLEARBUFFERFIPROC)load("glClearBufferfi");
	glad_glGetStringi = (PFNGLGETSTRINGIPROC)load("glGetStringi");
	glad_glIsRenderbuffer = (PFNGLISRENDERBUFFERPROC)load("glIsRenderbuffer");
	glad_glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC)load("glBindRenderbuffer");
	glad_glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC)load("glDeleteRenderbuffers");
	glad_glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC)load("glGenRenderbuffers");
	glad_glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC)load("glRenderbufferStorage");
	glad_glGetRenderbufferParameteriv = (PFNGLGETRENDERBUFFERPARAMETERIVPROC)load("glGetRenderbufferParameteriv");
	glad_glIsFramebuffer = (PFNGLISFRAMEBUFFERPROC)load("glIsFramebuffer");
	glad_glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)load("glBindFramebuffer");
	glad_glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)load("glDeleteFramebuffers");
	glad_glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)load("glGenFramebuffers");
	glad_glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)load("glCheckFramebufferStatus");
	glad_glFramebufferTexture1D = (PFNGLFRAMEBUFFERTEXTURE1DPROC)load("glFramebufferTexture1D");
	glad_glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)load("glFramebufferTexture2D");
	glad_glFramebufferTexture3D = (PFNGLFRAMEBUFFERTEXTURE3DPROC)load("glFramebufferTexture3D");
	glad_glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC)load("glFramebufferRenderbuffer");
	glad_glGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC)load("glGetFramebufferAttachmentParameteriv");
	glad_glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)load("glGenerateMipmap");
	glad_glBlitFramebuffer = (PFNGLBLITFRAMEBUFFERPROC)load("glBlitFramebuffer");
	glad_glRenderbufferStorageMultisample = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC)load("glRenderbufferStorageMultisample");
	glad_glFramebufferTextureLayer = (PFNGLFRAMEBUFFERTEXTURELAYERPROC)load("glFramebufferTextureLayer");
	glad_glMapBufferRange = (PFNGLMAPBUFFERRANGEPROC)load("glMapBufferRange");
	glad_glFlushMappedBufferRange = (PFNGLFLUSHMAPPEDBUFFERRANGEPROC)load("glFlushMappedBufferRange");
	glad_glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)load("glBindVertexArray");
	glad_glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)load("glDeleteVertexArrays");
	glad_glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)load("glGenVertexArrays");
	glad_glIsVertexArray = (PFNGLISVERTEXARRAYPROC)load("glIsVertexArray");
}
static void load_GL_VERSION_3_1(GLADloadproc load) {
	if(!GLAD_GL_VERSION_3_1) return;
	glad_glDrawArraysInstanced = (PFNGLDRAWARRAYSINSTANCEDPROC)load("glDrawArraysInstanced");
	glad_glDrawElementsInstanced = (PFNGLDRAWELEMENTSINSTANCEDPROC)load("glDrawElementsInstanced");
	glad_glTexBuffer = (PFNGLTEXBUFFERPROC)load("glTexBuffer");
	glad_glPrimitiveRestartIndex = (PFNGLPRIMITIVERESTARTINDEXPROC)load("glPrimitiveRestartIndex");
	glad_glCopyBufferSubData = (PFNGLCOPYBUFFERSUBDATAPROC)load("glCopyBufferSubData");
	glad_glGetUniformIndices = (PFNGLGETUNIFORMINDICESPROC)load("glGetUniformIndices");
	glad_glGetActiveUniformsiv = (PFNGLGETACTIVEUNIFORMSIVPROC)load("glGetActiveUniformsiv");
	glad_glGetActiveUniformName = (PFNGLGETACTIVEUNIFORMNAMEPROC)load("glGetActiveUniformName");
	glad_glGetUniformBlockIndex = (PFNGLGETUNIFORMBLOCKINDEXPROC)load("glGetUniformBlockIndex");
	glad_glGetActiveUniformBlockiv = (PFNGLGETACTIVEUNIFORMBLOCKIVPROC)load("glGetActiveUniformBlockiv");
	glad_glGetActiveUniformBlockName = (PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC)load("glGetActiveUniformBlockName");
	glad_glUniformBlockBinding = (PFNGLUNIFORMBLOCKBINDINGPROC)load("glUniformBlockBinding");
	glad_glBindBufferRange = (PFNGLBINDBUFFERRANGEPROC)load("glBindBufferRange");
	glad_glBindBufferBase = (PFNGLBINDBUFFERBASEPROC)load("glBindBufferBase");
	glad_glGetIntegeri_v = (PFNGLGETINTEGERI_VPROC)load("glGetIntegeri_v");
}
static void load_GL_VERSION_3_2(GLADloadproc load) {
	if(!GLAD_GL_VERSION_3_2) return;
	glad_glDrawElementsBaseVertex = (PFNGLDRAWELEMENTSBASEVERTEXPROC)load("glDrawElementsBaseVertex");
	glad_glDrawRangeElementsBaseVertex = (PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC)load("glDrawRangeElementsBaseVertex");
	glad_glDrawElementsInstancedBaseVertex = (PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC)load("glDrawElementsInstancedBaseVertex");
	glad_glMultiDrawElementsBaseVertex = (PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC)load("glMultiDrawElementsBaseVertex");
	glad_glProvokingVertex = (PFNGLPROVOKINGVERTEXPROC)load("glProvokingVertex");
	glad_glFenceSync = (PFNGLFENCESYNCPROC)load("glFenceSync");
	glad_glIsSync = (PFNGLISSYNCPROC)load("glIsSync");
	glad_glDeleteSync = (PFNGLDELETESYNCPROC)load("glDeleteSync");
	glad_glClientWaitSync = (PFNGLCLIENTWAITSYNCPROC)load("glClientWaitSync");
	glad_glWaitSync = (PFNGLWAITSYNCPROC)load("glWaitSync");
	glad_glGetInteger64v = (PFNGLGETINTEGER64VPROC)load("glGetInteger64v");
	glad_glGetSynciv = (PFNGLGETSYNCIVPROC)load("glGetSynciv");
	glad_glGetInteger64i_v = (PFNGLGETINTEGER64I_VPROC)load("glGetInteger64i_v");
	glad_glGetBufferParameteri64v = (PFNGLGETBUFFERPARAMETERI64VPROC)load("glGetBufferParameteri64v");
	glad_glFramebufferTexture = (PFNGLFRAMEBUFFERTEXTUREPROC)load("glFramebufferTexture");
	glad_glTexImage2DMultisample = (PFNGLTEXIMAGE2DMULTISAMPLEPROC)load("glTexImage2DMultisample");
	glad_glTexImage3DMultisample = (PFNGLTEXIMAGE3DMULTISAMPLEPROC)load("glTexImage3DMultisample");
	glad_glGetMultisamplefv = (PFNGLGETMULTISAMPLEFVPROC)load("glGetMultisamplefv");
	glad_glSampleMaski = (PFNGLSAMPLEMASKIPROC)load("glSampleMaski");
}
static void load_GL_VERSION_3_3(GLADloadproc load) {
	if(!GLAD_GL_VERSION_3_3) return;
	glad_glBindFragDataLocationIndexed = (PFNGLBINDFRAGDATALOCATIONINDEXEDPROC)load("glBindFragDataLocationIndexed");
	glad_glGetFragDataIndex = (PFNGLGETFRAGDATAINDEXPROC)load("glGetFragDataIndex");
	glad_glGenSamplers = (PFNGLGENSAMPLERSPROC)load("glGenSamplers");
	glad_glDeleteSamplers = (PFNGLDELETESAMPLERSPROC)load("glDeleteSamplers");
	glad_glIsSampler = (PFNGLISSAMPLERPROC)load("glIsSampler");
	glad_glBindSampler = (PFNGLBINDSAMPLERPROC)load("glBindSampler");
	glad_glSamplerParameteri = (PFNGLSAMPLERPARAMETERIPROC)load("glSamplerParameteri");
	glad_glSamplerParameteriv = (PFNGLSAMPLERPARAMETERIVPROC)load("glSamplerParameteriv");
	glad_glSamplerParameterf = (PFNGLSAMPLERPARAMETERFPROC)load("glSamplerParameterf");
	glad_glSamplerParameterfv = (PFNGLSAMPLERPARAMETERFVPROC)load("glSamplerParameterfv");
	glad_glSamplerParameterIiv = (PFNGLSAMPLERPARAMETERIIVPROC)load("glSamplerParameterIiv");
	glad_glSamplerParameterIuiv = (PFNGLSAMPLERPARAMETERIUIVPROC)load("glSamplerParameterIuiv");
	glad_glGetSamplerParameteriv = (PFNGLGETSAMPLERPARAMETERIVPROC)load("glGetSamplerParameteriv");
	glad_glGetSamplerParameterIiv = (PFNGLGETSAMPLERPARAMETERIIVPROC)load("glGetSamplerParameterIiv");
	glad_glGetSamplerParameterfv = (PFNGLGETSAMPLERPARAMETERFVPROC)load("glGetSamplerParameterfv");
	glad_glGetSamplerParameterIuiv = (PFNGLGETSAMPLERPARAMETERIUIVPROC)load("glGetSamplerParameterIuiv");
	glad_glQueryCounter = (PFNGLQUERYCOUNTERPROC)load("glQueryCounter");
	glad_glGetQueryObjecti64v = (PFNGLGETQUERYOBJECTI64VPROC)load("glGetQueryObjecti64v");
	glad_glGetQueryObjectui64v = (PFNGLGETQUERYOBJECTUI64VPROC)load("glGetQueryObjectui64v");
	glad_glVertexAttribDivisor = (PFNGLVERTEXATTRIBDIVISORPROC)load("glVertexAttribDivisor");
	glad_glVertexAttribP1ui = (PFNGLVERTEXATTRIBP1UIPROC)load("glVertexAttribP1ui");
	glad_glVertexAttribP1uiv = (PFNGLVERTEXATTRIBP1UIVPROC)load("glVertexAttribP1uiv");
	glad_glVertexAttribP2ui = (PFNGLVERTEXATTRIBP2UIPROC)load("glVertexAttribP2ui");
	glad_glVertexAttribP2uiv = (PFNGLVERTEXATTRIBP2UIVPROC)load("glVertexAttribP2uiv");
	glad_glVertexAttribP3ui = (PFNGLVERTEXATTRIBP3UIPROC)load("glVertexAttribP3ui");
	glad_glVertexAttribP3uiv = (PFNGLVERTEXATTRIBP3UIVPROC)load("glVertexAttribP3uiv");
	glad_glVertexAttribP4ui = (PFNGLVERTEXATTRIBP4UIPROC)load("glVertexAttribP4ui");
	glad_glVertexAttribP4uiv = (PFNGLVERTEXATTRIBP4UIVPROC)load("glVertexAttribP4uiv");
	glad_glVertexP2ui = (PFNGLVERTEXP2UIPROC)load("glVertexP2ui");
	glad_glVertexP2uiv = (PFNGLVERTEXP2UIVPROC)load("glVertexP2uiv");
	glad_glVertexP3ui = (PFNGLVERTEXP3UIPROC)load("glVertexP3ui");
	glad_glVertexP3uiv = (PFNGLVERTEXP3UIVPROC)load("glVertexP3uiv");
	glad_glVertexP4ui = (PFNGLVERTEXP4UIPROC)load("glVertexP4ui");
	glad_glVertexP4uiv = (PFNGLVERTEXP4UIVPROC)load("glVertexP4uiv");
	glad_glTexCoordP1ui = (PFNGLTEXCOORDP1UIPROC)load("glTexCoordP1ui");
	glad_glTexCoordP1uiv = (PFNGLTEXCOORDP1UIVPROC)load("glTexCoordP1uiv");
	glad_glTexCoordP2ui = (PFNGLTEXCOORDP2UIPROC)load("glTexCoordP2ui");
	glad_glTexCoordP2uiv = (PFNGLTEXCOORDP2UIVPROC)load("glTexCoordP2uiv");
	glad_glTexCoordP3ui = (PFNGLTEXCOORDP3UIPROC)load("glTexCoordP3ui");
	glad_glTexCoordP3uiv = (PFNGLTEXCOORDP3UIVPROC)load("glTexCoordP3uiv");
	glad_glTexCoordP4ui = (PFNGLTEXCOORDP4UIPROC)load("glTexCoordP4ui");
	glad_glTexCoordP4uiv = (PFNGLTEXCOORDP4UIVPROC)load("glTexCoordP4uiv");
	glad_glMultiTexCoordP1ui = (PFNGLMULTITEXCOORDP1UIPROC)load("glMultiTexCoordP1ui");
	glad_glMultiTexCoordP1uiv = (PFNGLMULTITEXCOORDP1UIVPROC)load("glMultiTexCoordP1uiv");
	glad_glMultiTexCoordP2ui = (PFNGLMULTITEXCOORDP2UIPROC)load("glMultiTexCoordP2ui");
	glad_glMultiTexCoordP2uiv = (PFNGLMULTITEXCOORDP2UIVPROC)load("glMultiTexCoordP2uiv");
	glad_glMultiTexCoordP3ui = (PFNGLMULTITEXCOORDP3UIPROC)load("glMultiTexCoordP3ui");
	glad_glMultiTexCoordP3uiv = (PFNGLMULTITEXCOORDP3UIVPROC)load("glMultiTexCoordP3uiv");
	glad_glMultiTexCoordP4ui = (PFNGLMULTITEXCOORDP4UIPROC)load("glMultiTexCoordP4ui");
	glad_glMultiTexCoordP4uiv = (PFNGLMULTITEXCOORDP4UIVPROC)load("glMultiTexCoordP4uiv");
	glad_glNormalP3ui = (PFNGLNORMALP3UIPROC)load("glNormalP3ui");
	glad_glNormalP3uiv = (PFNGLNORMALP3UIVPROC)load("glNormalP3uiv");
	glad_glColorP3ui = (PFNGLCOLORP3UIPROC)load("glColorP3ui");
	glad_glColorP3uiv = (PFNGLCOLORP3UIVPROC)load("glColorP3uiv");
	glad_glColorP4ui = (PFNGLCOLORP4UIPROC)load("glColorP4ui");
	glad_glColorP4uiv = (PFNGLCOLORP4UIVPROC)load("glColorP4uiv");
	glad_glSecondaryColorP3ui = (PFNGLSECONDARYCOLORP3UIPROC)load("glSecondaryColorP3ui");
	glad_glSecondaryColorP3uiv = (PFNGLSECONDARYCOLORP3UIVPROC)load("glSecondaryColorP3uiv");
}
static void load_GL_VERSION_4_0(GLADloadproc load) {
	if(!GLAD_GL_VERSION_4_0) return;
	glad_glMinSampleShading = (PFNGLMINSAMPLESHADINGPROC)load("glMinSampleShading");
	glad_glBlendEquationi = (PFNGLBLENDEQUATIONIPROC)load("glBlendEquationi");
	glad_glBlendEquationSeparatei = (PFNGLBLENDEQUATIONSEPARATEIPROC)load("glBlendEquationSeparatei");
	glad_glBlendFunci = (PFNGLBLENDFUNCIPROC)load("glBlendFunci");
	glad_glBlendFuncSeparatei = (PFNGLBLENDFUNCSEPARATEIPROC)load("glBlendFuncSeparatei");
	glad_glDrawArraysIndirect = (PFNGLDRAWARRAYSINDIRECTPROC)load("glDrawArraysIndirect");
	glad_glDrawElementsIndirect = (PFNGLDRAWELEMENTSINDIRECTPROC)load("glDrawElementsIndirect");
	glad_glUniform1d = (PFNGLUNIFORM1DPROC)load("glUniform1d");
	glad_glUniform2d = (PFNGLUNIFORM2DPROC)load("glUniform2d");
	glad_glUniform3d = (PFNGLUNIFORM3DPROC)load("glUniform3d");
	glad_glUniform4d = (PFNGLUNIFORM4DPROC)load("glUniform4d");
	glad_glUniform1dv = (PFNGLUNIFORM1DVPROC)load("glUniform1dv");
	glad_glUniform2dv = (PFNGLUNIFORM2DVPROC)load("glUniform2dv");
	glad_glUniform3dv = (PFNGLUNIFORM3DVPROC)load("glUniform3dv");
	glad_glUniform4dv = (PFNGLUNIFORM4DVPROC)load("glUniform4dv");
	glad_glUniformMatrix2dv = (PFNGLUNIFORMMATRIX2DVPROC)load("glUniformMatrix2dv");
	glad_glUniformMatrix3dv = (PFNGLUNIFORMMATRIX3DVPROC)load("glUniformMatrix3dv");
	glad_glUniformMatrix4dv = (PFNGLUNIFORMMATRIX4DVPROC)load("glUniformMatrix4dv");
	glad_glUniformMatrix2x3dv = (PFNGLUNIFORMMATRIX2X3DVPROC)load("glUniformMatrix2x3dv");
	glad_glUniformMatrix2x4dv = (PFNGLUNIFORMMATRIX2X4DVPROC)load("glUniformMatrix2x4dv");
	glad_glUniformMatrix3x2dv = (PFNGLUNIFORMMATRIX3X2DVPROC)load("glUniformMatrix3x2dv");
	glad_glUniformMatrix3x4dv = (PFNGLUNIFORMMATRIX3X4DVPROC)load("glUniformMatrix3x4dv");
	glad_glUniformMatrix4x2dv = (PFNGLUNIFORMMATRIX4X2DVPROC)load("glUniformMatrix4x2dv");
	glad_glUniformMatrix4x3dv = (PFNGLUNIFORMMATRIX4X3DVPROC)load("glUniformMatrix4x3dv");
	glad_glGetUniformdv = (PFNGLGETUNIFORMDVPROC)load("glGetUniformdv");
	glad_glGetSubroutineUniformLocation = (PFNGLGETSUBROUTINEUNIFORMLOCATIONPROC)load("glGetSubroutineUniformLocation");
	glad_glGetSubroutineIndex = (PFNGLGETSUBROUTINEINDEXPROC)load("glGetSubroutineIndex");
	glad_glGetActiveSubroutineUniformiv = (PFNGLGETACTIVESUBROUTINEUNIFORMIVPROC)load("glGetActiveSubroutineUniformiv");
	glad_glGetActiveSubroutineUniformName = (PFNGLGETACTIVESUBROUTINEUNIFORMNAMEPROC)load("glGetActiveSubroutineUniformName");
	glad_glGetActiveSubroutineName = (PFNGLGETACTIVESUBROUTINENAMEPROC)load("glGetActiveSubroutineName");
	glad_glUniformSubroutinesuiv = (PFNGLUNIFORMSUBROUTINESUIVPROC)load("glUniformSubroutinesuiv");
	glad_glGetUniformSubroutineuiv = (PFNGLGETUNIFORMSUBROUTINEUIVPROC)load("glGetUniformSubroutineuiv");
	glad_glGetProgramStageiv = (PFNGLGETPROGRAMSTAGEIVPROC)load("glGetProgramStageiv");
	glad_glPatchParameteri = (PFNGLPATCHPARAMETERIPROC)load("glPatchParameteri");
	glad_glPatchParameterfv = (PFNGLPATCHPARAMETERFVPROC)load("glPatchParameterfv");
	glad_glBindTransformFeedback = (PFNGLBINDTRANSFORMFEEDBACKPROC)load("glBindTransformFeedback");
	glad_glDeleteTransformFeedbacks = (PFNGLDELETETRANSFORMFEEDBACKSPROC)load("glDeleteTransformFeedbacks");
	glad_glGenTransformFeedbacks = (PFNGLGENTRANSFORMFEEDBACKSPROC)load("glGenTransformFeedbacks");
	glad_glIsTransformFeedback = (PFNGLISTRANSFORMFEEDBACKPROC)load("glIsTransformFeedback");
	glad_glPauseTransformFeedback = (PFNGLPAUSETRANSFORMFEEDBACKPROC)load("glPauseTransformFeedback");
	glad_glResumeTransformFeedback = (PFNGLRESUMETRANSFORMFEEDBACKPROC)load("glResumeTransformFeedback");
	glad_glDrawTransformFeedback = (PFNGLDRAWTRANSFORMFEEDBACKPROC)load("glDrawTransformFeedback");
	glad_glDrawTransformFeedbackStream = (PFNGLDRAWTRANSFORMFEEDBACKSTREAMPROC)load("glDrawTransformFeedbackStream");
	glad_glBeginQueryIndexed = (PFNGLBEGINQUERYINDEXEDPROC)load("glBeginQueryIndexed");
	glad_glEndQueryIndexed = (PFNGLENDQUERYINDEXEDPROC)load("glEndQueryIndexed");
	glad_glGetQueryIndexediv = (PFNGLGETQUERYINDEXEDIVPROC)load("glGetQueryIndexediv");
}
static void load_GL_VERSION_4_1(GLADloadproc load) {
	if(!GLAD_GL_VERSION_4_1) return;
	glad_glReleaseShaderCompiler = (PFNGLRELEASESHADERCOMPILERPROC)load("glReleaseShaderCompiler");
	glad_glShaderBinary = (PFNGLSHADERBINARYPROC)load("glShaderBinary");
	glad_glGetShaderPrecisionFormat = (PFNGLGETSHADERPRECISIONFORMATPROC)load("glGetShaderPrecisionFormat");
	glad_glDepthRangef = (PFNGLDEPTHRANGEFPROC)load("glDepthRangef");
	glad_glClearDepthf = (PFNGLCLEARDEPTHFPROC)load("glClearDepthf");
	glad_glGetProgramBinary = (PFNGLGETPROGRAMBINARYPROC)load("glGetProgramBinary");
	glad_glProgramBinary = (PFNGLPROGRAMBINARYPROC)load("glProgramBinary");
	glad_glProgramParameteri = (PFNGLPROGRAMPARAMETERIPROC)load("glProgramParameteri");
	glad_glUseProgramStages = (PFNGLUSEPROGRAMSTAGESPROC)load("glUseProgramStages");
	glad_glActiveShaderProgram = (PFNGLACTIVESHADERPROGRAMPROC)load("glActiveShaderProgram");
	glad_glCreateShaderProgramv = (PFNGLCREATESHADERPROGRAMVPROC)load("glCreateShaderProgramv");
	glad_glBindProgramPipeline = (PFNGLBINDPROGRAMPIPELINEPROC)load("glBindProgramPipeline");
	glad_glDeleteProgramPipelines = (PFNGLDELETEPROGRAMPIPELINESPROC)load("glDeleteProgramPipelines");
	glad_glGenProgramPipelines = (PFNGLGENPROGRAMPIPELINESPROC)load("glGenProgramPipelines");
	glad_glIsProgramPipeline = (PFNGLISPROGRAMPIPELINEPROC)load("glIsProgramPipeline");
	glad_glGetProgramPipelineiv = (PFNGLGETPROGRAMPIPELINEIVPROC)load("glGetProgramPipelineiv");
	glad_glProgramUniform1i = (PFNGLPROGRAMUNIFORM1IPROC)load("glProgramUniform1i");
	glad_glProgramUniform1iv = (PFNGLPROGRAMUNIFORM1IVPROC)load("glProgramUniform1iv");
	glad_glProgramUniform1f = (PFNGLPROGRAMUNIFORM1FPROC)load("glProgramUniform1f");
	glad_glProgramUniform1fv = (PFNGLPROGRAMUNIFORM1FVPROC)load("glProgramUniform1fv");
	glad_glProgramUniform1d = (PFNGLPROGRAMUNIFORM1DPROC)load("glProgramUniform1d");
	glad_glProgramUniform1dv = (PFNGLPROGRAMUNIFORM1DVPROC)load("glProgramUniform1dv");
	glad_glProgramUniform1ui = (PFNGLPROGRAMUNIFORM1UIPROC)load("glProgramUniform1ui");
	glad_glProgramUniform1uiv = (PFNGLPROGRAMUNIFORM1UIVPROC)load("glProgramUniform1uiv");
	glad_glProgramUniform2i = (PFNGLPROGRAMUNIFORM2IPROC)load("glProgramUniform2i");
	glad_glProgramUniform2iv = (PFNGLPROGRAMUNIFORM2IVPROC)load("glProgramUniform2iv");
	glad_glProgramUniform2f = (PFNGLPROGRAMUNIFORM2FPROC)load("glProgramUniform2f");
	glad_glProgramUniform2fv = (PFNGLPROGRAMUNIFORM2FVPROC)load("glProgramUniform2fv");
	glad_glProgramUniform2d = (PFNGLPROGRAMUNIFORM2DPROC)load("glProgramUniform2d");
	glad_glProgramUniform2dv = (PFNGLPROGRAMUNIFORM2DVPROC)load("glProgramUniform2dv");
	glad_glProgramUniform2ui = (PFNGLPROGRAMUNIFORM2UIPROC)load("glProgramUniform2ui");
	glad_glProgramUniform2uiv = (PFNGLPROGRAMUNIFORM2UIVPROC)load("glProgramUniform2uiv");
	glad_glProgramUniform3i = (PFNGLPROGRAMUNIFORM3IPROC)load("glProgramUniform3i");
	glad_glProgramUniform3iv = (PFNGLPROGRAMUNIFORM3IVPROC)load("glProgramUniform3iv");
	glad_glProgramUniform3f = (PFNGLPROGRAMUNIFORM3FPROC)load("glProgramUniform3f");
	glad_glProgramUniform3fv = (PFNGLPROGRAMUNIFORM3FVPROC)load("glProgramUniform3fv");
	glad_glProgramUniform3d = (PFNGLPROGRAMUNIFORM3DPROC)load("glProgramUniform3d");
	glad_glProgramUniform3dv = (PFNGLPROGRAMUNIFORM3DVPROC)load("glProgramUniform3dv");
	glad_glProgramUniform3ui = (PFNGLPROGRAMUNIFORM3UIPROC)load("glProgramUniform3ui");
	glad_glProgramUniform3uiv = (PFNGLPROGRAMUNIFORM3UIVPROC)load("glProgramUniform3uiv");
	glad_glProgramUniform4i = (PFNGLPROGRAMUNIFORM4IPROC)load("glProgramUniform4i");
	glad_glProgramUniform4iv = (PFNGLPROGRAMUNIFORM4IVPROC)load("glProgramUniform4iv");
	glad_glProgramUniform4f = (PFNGLPROGRAMUNIFORM4FPROC)load("glProgramUniform4f");
	glad_glProgramUniform4fv = (PFNGLPROGRAMUNIFORM4FVPROC)load("glProgramUniform4fv");
	glad_glProgramUniform4d = (PFNGLPROGRAMUNIFORM4DPROC)load("glProgramUniform4d");
	glad_glProgramUniform4dv = (PFNGLPROGRAMUNIFORM4DVPROC)load("glProgramUniform4dv");
	glad_glProgramUniform4ui = (PFNGLPROGRAMUNIFORM4UIPROC)load("glProgramUniform4ui");
	glad_glProgramUniform4uiv = (PFNGLPROGRAMUNIFORM4UIVPROC)load("glProgramUniform4uiv");
	glad_glProgramUniformMatrix2fv = (PFNGLPROGRAMUNIFORMMATRIX2FVPROC)load("glProgramUniformMatrix2fv");
	glad_glProgramUniformMatrix3fv = (PFNGLPROGRAMUNIFORMMATRIX3FVPROC)load("glProgramUniformMatrix3fv");
	glad_glProgramUniformMatrix4fv = (PFNGLPROGRAMUNIFORMMATRIX4FVPROC)load("glProgramUniformMatrix4fv");
	glad_glProgramUniformMatrix2dv = (PFNGLPROGRAMUNIFORMMATRIX2DVPROC)load("glProgramUniformMatrix2dv");
	glad_glProgramUniformMatrix3dv = (PFNGLPROGRAMUNIFORMMATRIX3DVPROC)load("glProgramUniformMatrix3dv");
	glad_glProgramUniformMatrix4dv = (PFNGLPROGRAMUNIFORMMATRIX4DVPROC)load("glProgramUniformMatrix4dv");
	glad_glProgramUniformMatrix2x3fv = (PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC)load("glProgramUniformMatrix2x3fv");
	glad_glProgramUniformMatrix3x2fv = (PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC)load("glProgramUniformMatrix3x2fv");
	glad_glProgramUniformMatrix2x4fv = (PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC)load("glProgramUniformMatrix2x4fv");
	glad_glProgramUniformMatrix4x2fv = (PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC)load("glProgramUniformMatrix4x2fv");
	glad_glProgramUniformMatrix3x4fv = (PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC)load("glProgramUniformMatrix3x4fv");
	glad_glProgramUniformMatrix4x3fv = (PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC)load("glProgramUniformMatrix4x3fv");
	glad_glProgramUniformMatrix2x3dv = (PFNGLPROGRAMUNIFORMMATRIX2X3DVPROC)load("glProgramUniformMatrix2x3dv");
	glad_glProgramUniformMatrix3x2dv = (PFNGLPROGRAMUNIFORMMATRIX3X2DVPROC)load("glProgramUniformMatrix3x2dv");
	glad_glProgramUniformMatrix2x4dv = (PFNGLPROGRAMUNIFORMMATRIX2X4DVPROC)load("glProgramUniformMatrix2x4dv");
	glad_glProgramUniformMatrix4x2dv = (PFNGLPROGRAMUNIFORMMATRIX4X2DVPROC)load("glProgramUniformMatrix4x2dv");
	glad_glProgramUniformMatrix3x4dv = (PFNGLPROGRAMUNIFORMMATRIX3X4DVPROC)load("glProgramUniformMatrix3x4dv");
	glad_glProgramUniformMatrix4x3dv = (PFNGLPROGRAMUNIFORMMATRIX4X3DVPROC)load("glProgramUniformMatrix4x3dv");
	glad_glValidateProgramPipeline = (PFNGLVALIDATEPROGRAMPIPELINEPROC)load("glValidateProgramPipeline");
	glad_glGetProgramPipelineInfoLog = (PFNGLGETPROGRAMPIPELINEINFOLOGPROC)load("glGetProgramPipelineInfoLog");
	glad_glVertexAttribL1d = (PFNGLVERTEXATTRIBL1DPROC)load("glVertexAttribL1d");
	glad_glVertexAttribL2d = (PFNGLVERTEXATTRIBL2DPROC)load("glVertexAttribL2d");
	glad_glVertexAttribL3d = (PFNGLVERTEXATTRIBL3DPROC)load("glVertexAttribL3d");
	glad_glVertexAttribL4d = (PFNGLVERTEXATTRIBL4DPROC)load("glVertexAttribL4d");
	glad_glVertexAttribL1dv = (PFNGLVERTEXATTRIBL1DVPROC)load("glVertexAttribL1dv");
	glad_glVertexAttribL2dv = (PFNGLVERTEXATTRIBL2DVPROC)load("glVertexAttribL2dv");
	glad_glVertexAttribL3dv = (PFNGLVERTEXATTRIBL3DVPROC)load("glVertexAttribL3dv");
	glad_glVertexAttribL4dv = (PFNGLVERTEXATTRIBL4DVPROC)load("glVertexAttribL4dv");
	glad_glVertexAttribLPointer = (PFNGLVERTEXATTRIBLPOINTERPROC)load("glVertexAttribLPointer");
	glad_glGetVertexAttribLdv = (PFNGLGETVERTEXATTRIBLDVPROC)load("glGetVertexAttribLdv");
	glad_glViewportArrayv = (PFNGLVIEWPORTARRAYVPROC)load("glViewportArrayv");
	glad_glViewportIndexedf = (PFNGLVIEWPORTINDEXEDFPROC)load("glViewportIndexedf");
	glad_glViewportIndexedfv = (PFNGLVIEWPORTINDEXEDFVPROC)load("glViewportIndexedfv");
	glad_glScissorArrayv = (PFNGLSCISSORARRAYVPROC)load("glScissorArrayv");
	glad_glScissorIndexed = (PFNGLSCISSORINDEXEDPROC)load("glScissorIndexed");
	glad_glScissorIndexedv = (PFNGLSCISSORINDEXEDVPROC)load("glScissorIndexedv");
	glad_glDepthRangeArrayv = (PFNGLDEPTHRANGEARRAYVPROC)load("glDepthRangeArrayv");
	glad_glDepthRangeIndexed = (PFNGLDEPTHRANGEINDEXEDPROC)load("glDepthRangeIndexed");
	glad_glGetFloati_v = (PFNGLGETFLOATI_VPROC)load("glGetFloati_v");
	glad_glGetDoublei_v = (PFNGLGETDOUBLEI_VPROC)load("glGetDoublei_v");
}
static void load_GL_KHR_debug(GLADloadproc load) {
	if(!GLAD_GL_KHR_debug) return;
	glad_glDebugMessageControl = (PFNGLDEBUGMESSAGECONTROLPROC)load("glDebugMessageControl");
	glad_glDebugMessageInsert = (PFNGLDEBUGMESSAGEINSERTPROC)load("glDebugMessageInsert");
	glad_glDebugMessageCallback = (PFNGLDEBUGMESSAGECALLBACKPROC)load("glDebugMessageCallback");
	glad_glGetDebugMessageLog = (PFNGLGETDEBUGMESSAGELOGPROC)load("glGetDebugMessageLog");
	glad_glPushDebugGroup = (PFNGLPUSHDEBUGGROUPPROC)load("glPushDebugGroup");
	glad_glPopDebugGroup = (PFNGLPOPDEBUGGROUPPROC)load("glPopDebugGroup");
	glad_glObjectLabel = (PFNGLOBJECTLABELPROC)load("glObjectLabel");
	glad_glGetObjectLabel = (PFNGLGETOBJECTLABELPROC)load("glGetObjectLabel");
	glad_glObjectPtrLabel = (PFNGLOBJECTPTRLABELPROC)load("glObjectPtrLabel");
	glad_glGetObjectPtrLabel = (PFNGLGETOBJECTPTRLABELPROC)load("glGetObjectPtrLabel");
	glad_glGetPointerv = (PFNGLGETPOINTERVPROC)load("glGetPointerv");
	glad_glDebugMessageControlKHR = (PFNGLDEBUGMESSAGECONTROLKHRPROC)load("glDebugMessageControlKHR");
	glad_glDebugMessageInsertKHR = (PFNGLDEBUGMESSAGEINSERTKHRPROC)load("glDebugMessageInsertKHR");
	glad_glDebugMessageCallbackKHR = (PFNGLDEBUGMESSAGECALLBACKKHRPROC)load("glDebugMessageCallbackKHR");
	glad_glGetDebugMessageLogKHR = (PFNGLGETDEBUGMESSAGELOGKHRPROC)load("glGetDebugMessageLogKHR");
	glad_glPushDebugGroupKHR = (PFNGLPUSHDEBUGGROUPKHRPROC)load("glPushDebugGroupKHR");
	glad_glPopDebugGroupKHR = (PFNGLPOPDEBUGGROUPKHRPROC)load("glPopDebugGroupKHR");
	glad_glObjectLabelKHR = (PFNGLOBJECTLABELKHRPROC)load("glObjectLabelKHR");
	glad_glGetObjectLabelKHR = (PFNGLGETOBJECTLABELKHRPROC)load("glGetObjectLabelKHR");
	glad_glObjectPtrLabelKHR = (PFNGLOBJECTPTRLABELKHRPROC)load("glObjectPtrLabelKHR");
	glad_glGetObjectPtrLabelKHR = (PFNGLGETOBJECTPTRLABELKHRPROC)load("glGetObjectPtrLabelKHR");
	glad_glGetPointervKHR = (PFNGLGETPOINTERVKHRPROC)load("glGetPointervKHR");
}
static void find_extensionsGL(void) {
	GLAD_GL_KHR_debug = has_ext("GL_KHR_debug");
}

static void find_coreGL(void) {

    /* Thank you @elmindreda
     * https://github.com/elmindreda/greg/blob/master/templates/greg.c.in#L176
     * https://github.com/glfw/glfw/blob/master/src/context.c#L36
     */
    int i, major, minor;

    const char* version;
    const char* prefixes[] = {
        "OpenGL ES-CM ",
        "OpenGL ES-CL ",
        "OpenGL ES ",
        NULL
    };

    version = (const char*) glGetString(GL_VERSION);
    if (!version) return;

    for (i = 0;  prefixes[i];  i++) {
        const size_t length = strlen(prefixes[i]);
        if (strncmp(version, prefixes[i], length) == 0) {
            version += length;
            break;
        }
    }

/* PR #18 */
#ifdef _MSC_VER
    sscanf_s(version, "%d.%d", &major, &minor);
#else
    sscanf(version, "%d.%d", &major, &minor);
#endif

    GLVersion.major = major; GLVersion.minor = minor;
	GLAD_GL_VERSION_1_0 = (major == 1 && minor >= 0) || major > 1;
	GLAD_GL_VERSION_1_1 = (major == 1 && minor >= 1) || major > 1;
	GLAD_GL_VERSION_1_2 = (major == 1 && minor >= 2) || major > 1;
	GLAD_GL_VERSION_1_3 = (major == 1 && minor >= 3) || major > 1;
	GLAD_GL_VERSION_1_4 = (major == 1 && minor >= 4) || major > 1;
	GLAD_GL_VERSION_1_5 = (major == 1 && minor >= 5) || major > 1;
	GLAD_GL_VERSION_2_0 = (major == 2 && minor >= 0) || major > 2;
	GLAD_GL_VERSION_2_1 = (major == 2 && minor >= 1) || major > 2;
	GLAD_GL_VERSION_3_0 = (major == 3 && minor >= 0) || major > 3;
	GLAD_GL_VERSION_3_1 = (major == 3 && minor >= 1) || major > 3;
	GLAD_GL_VERSION_3_2 = (major == 3 && minor >= 2) || major > 3;
	GLAD_GL_VERSION_3_3 = (major == 3 && minor >= 3) || major > 3;
	GLAD_GL_VERSION_4_0 = (major == 4 && minor >= 0) || major > 4;
	GLAD_GL_VERSION_4_1 = (major == 4 && minor >= 1) || major > 4;
}

int gladLoadGLLoader(GLADloadproc load) {
	GLVersion.major = 0; GLVersion.minor = 0;
	glGetString = (PFNGLGETSTRINGPROC)load("glGetString");
	if(glGetString == NULL) return 0;
	if(glGetString(GL_VERSION) == NULL) return 0;
	find_coreGL();
	load_GL_VERSION_1_0(load);
	load_GL_VERSION_1_1(load);
	load_GL_VERSION_1_2(load);
	load_GL_VERSION_1_3(load);
	load_GL_VERSION_1_4(load);
	load_GL_VERSION_1_5(load);
	load_GL_VERSION_2_0(load);
	load_GL_VERSION_2_1(load);
	load_GL_VERSION_3_0(load);
	load_GL_VERSION_3_1(load);
	load_GL_VERSION_3_2(load);
	load_GL_VERSION_3_3(load);
	load_GL_VERSION_4_0(load);
	load_GL_VERSION_4_1(load);

	find_extensionsGL();
	load_GL_KHR_debug(load);
	return GLVersion.major != 0 || GLVersion.minor != 0;
}

