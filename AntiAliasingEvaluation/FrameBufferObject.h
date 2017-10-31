#pragma once
#include <FrmPlatform.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <GLES3/gl31.h>
#include <GLES2/gl2ext.h>
#include <FrmApplication.h>
#include <OpenGLES/FrmShader.h>

class FrameBufferObject
{
public:
	BOOL CreateFBO(UINT32 nWidth, UINT32 nHeight, UINT32 nFormat, UINT32 nType, UINT32 nInternalFormat, BOOL multiSample = FALSE);
	VOID DestroyFBO();
	VOID BeginFBO();
	VOID EndFBO();
	VOID setSampleNumber(int sampleNum) { SAMPLE_NUM = sampleNum; }

	UINT32 m_nWidth;
	UINT32 m_nHeight;
	UINT32 SAMPLE_NUM;

	UINT32 m_hTexture;
	UINT32 m_hFrameBuffer;
	UINT32 m_hRenderBuffer;
};