//--------------------------------------------------------------------------------------
// File: Scene.cpp
// Desc: 
//
// Author:      QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#include <FrmPlatform.h>
#define GL_GLEXT_PROTOTYPES
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <GLES2/gl2ext.h>
#include <FrmApplication.h>
#include <OpenGLES/FrmShader.h>
#include "Application.h"

#include "FrameBufferObject.h"
#include "ModelRenderer.h"
#include "ScreenRenderer.h"

#if defined(__linux__)
#include <stdio.h>
#include <unistd.h>
#endif

//--------------------------------------------------------------------------------------
// Name: FrmCreateApplicationInstance()
// Desc: Global function to create the application instance
//--------------------------------------------------------------------------------------
CFrmApplication* FrmCreateApplicationInstance()
{
    return new Application("MASS");
}

//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
Application::Application( const CHAR* strName ) : CFrmApplication( strName )
{
    g_strWindowTitle = strName;
    g_nWindowWidth = 765;
    g_nWindowHeight = 480;
    g_fAspectRatio = (FLOAT)g_nWindowWidth / (FLOAT)g_nWindowHeight;

	screen_fbo = new FrameBufferObject();

	m_ModelRenderer = new ModelRenderer();
	m_ModelRenderer->setRenderFBO(screen_fbo);

	m_ScreenRenderer = new ScreenRenderer();
	m_ScreenRenderer->setRenderFBO(screen_fbo);
}

//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: Initialize process before rendering
//--------------------------------------------------------------------------------------
BOOL Application::Initialize()
{
	// Create an offscreen Multisample screen_fbo to render to
	screen_fbo->setSampleNumber(8);
	if (FALSE == screen_fbo->CreateFBO(g_nWindowWidth, g_nWindowHeight, GL_RGBA, GL_UNSIGNED_BYTE, GL_RGBA8, TRUE))
		return FALSE;

	if (FALSE == m_ModelRenderer->Initialize(g_nWindowWidth, g_nWindowHeight)) {
		return FALSE;
	}

	if (FALSE == m_ScreenRenderer->Initialize(g_nWindowWidth, g_nWindowHeight)) {
		return FALSE;
	}
	
    return TRUE;
}

//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc: 
//--------------------------------------------------------------------------------------
VOID Application::Destroy()
{
	delete m_ModelRenderer;
	screen_fbo->DestroyFBO();
}

//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID Application::Render()
{
    static float fTime = 0.0f;
    fTime += 0.01f;

	glClearColor(0.4f, 0.4f, 0.4f, 1.0f);

	m_ModelRenderer->Render(fTime);
	m_ScreenRenderer->Render(fTime);
}