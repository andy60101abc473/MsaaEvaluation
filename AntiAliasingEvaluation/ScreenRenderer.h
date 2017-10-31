#pragma once
#include <FrmPlatform.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <GLES2/gl2ext.h>
#include <FrmApplication.h>
#include <OpenGLES/FrmShader.h>
#include <vector>
#include "Renderer.h"

class ScreenRenderer : public Renderer
{
public:
	ScreenRenderer();
	~ScreenRenderer();
	BOOL Initialize(UINT32 windowWidth, UINT32 windowHeight);
	VOID Render(FLOAT fTime);
	VOID setRenderFBO(FrameBufferObject* fbo) { modelRender_fbo = fbo; }
	BOOL setupScreenFBO();

private:
	struct vertex
	{
		float position[2];
		float uv[2];
	};

	struct ShaderProgram
	{
		GLuint g_hShaderProgram;
		GLuint g_textureSampleLoc;
		GLuint g_hVertexLoc;
		GLuint g_TexcoordLoc;
	};

	BOOL InitShaders();
	VOID setupScreenMesh();

	ShaderProgram* shaderProgram;
	FrameBufferObject* modelRender_fbo;

	GLuint screenRender_FBO;
	GLuint screenTexture;

	GLuint g_hPositionVbo;
	GLuint g_hIndexVbo;
	GLuint g_hVao;

	GLuint g_IndexSize;
};