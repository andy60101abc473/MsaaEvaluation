#pragma once
#include <FrmPlatform.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <GLES2/gl2ext.h>
#include <FrmApplication.h>
#include <OpenGLES/FrmShader.h>
#include "Renderer.h"

class ModelRenderer : public Renderer
{
public:
	ModelRenderer();
	~ModelRenderer();
	BOOL Initialize(UINT32 windowWidth, UINT32 windowHeight);
	VOID Render(FLOAT fTime);
	VOID setRenderFBO(FrameBufferObject* fbo) { screen_fbo = fbo; }

private:
	struct ShaderProgram
	{
		GLuint g_hShaderProgram;
		GLuint g_hModelViewMatrixLoc;
		GLuint g_hProjMatrixLoc;
		GLuint g_hVertexLoc;
		GLuint g_hColorLoc;
	};

	BOOL InitShaders();
	VOID RenderScene(FLOAT fTime);

	ShaderProgram* shaderProgram;
	FrameBufferObject* screen_fbo;

	GLuint g_hPositionVbo;
	GLuint g_hColorVbo;
	GLuint g_hVao;
};