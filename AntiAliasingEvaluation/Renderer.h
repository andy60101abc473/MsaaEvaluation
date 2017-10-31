#pragma once
#include <FrmPlatform.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <GLES2/gl2ext.h>
#include <FrmApplication.h>
#include <OpenGLES/FrmShader.h>
#include "FrameBufferObject.h"

class Renderer
{
public:
	virtual ~Renderer() {}
	virtual BOOL Initialize(UINT32 windowWidth, UINT32 windowHeight) = 0;
	virtual VOID Render(FLOAT fTime) = 0;
	virtual VOID setRenderFBO(FrameBufferObject* fbo) = 0;

protected:
	UINT32 g_nWindowWidth;
	UINT32 g_nWindowHeight;
	FLOAT  g_fAspectRatio;
};