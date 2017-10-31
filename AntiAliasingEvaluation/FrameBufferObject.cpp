#include "FrameBufferObject.h"

//--------------------------------------------------------------------------------------
// Name: CreateFBO()
// Desc: Create objects needed for rendering to an FBO
//--------------------------------------------------------------------------------------
BOOL FrameBufferObject::CreateFBO(UINT32 nWidth, UINT32 nHeight, UINT32 nFormat, UINT32 nType, UINT32 nInternalFormat, BOOL multiSample)
{
	m_nWidth = nWidth;
	m_nHeight = nHeight;

	if (multiSample == TRUE) {
		// Create the color buffer attachment texture
		glGenTextures(1, &m_hTexture);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_hTexture);
		glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, SAMPLE_NUM, nInternalFormat, nWidth, nHeight, GL_TRUE);

		// Create a renderbuffer object for depth
		glGenRenderbuffers(1, &m_hRenderBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, m_hRenderBuffer);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, SAMPLE_NUM, GL_DEPTH_COMPONENT24, nWidth, nHeight);

		// Create the frame buffer
		glGenFramebuffers(1, &m_hFrameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, m_hFrameBuffer);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_hTexture, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_hRenderBuffer);
	}
	else {
		glGenTextures(1, &m_hTexture);
		glBindTexture(GL_TEXTURE_2D, m_hTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, nInternalFormat, nWidth, nHeight, 0, nFormat, nType, NULL);

		// Create a renderbuffer object for depth
		glGenRenderbuffers(1, &m_hRenderBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, m_hRenderBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, nWidth, nHeight);

		// Create the frame buffer
		glGenFramebuffers(1, &m_hFrameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, m_hFrameBuffer);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_hTexture, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_hRenderBuffer);
	}

	if (GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER))
		return FALSE;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: DestroyFBO()
// Desc: Destroy the FBO
//--------------------------------------------------------------------------------------
VOID FrameBufferObject::DestroyFBO()
{
	glDeleteFramebuffers(1, &m_hFrameBuffer);
	glDeleteRenderbuffers(1, &m_hRenderBuffer);
	glDeleteTextures(1, &m_hTexture);
}


//--------------------------------------------------------------------------------------
// Name: BeginFBO()
// Desc: Bind objects for rendering to the frame buffer object
//--------------------------------------------------------------------------------------
VOID FrameBufferObject::BeginFBO()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_hFrameBuffer);
	glViewport(0, 0, m_nWidth, m_nHeight);
}


//--------------------------------------------------------------------------------------
// Name: EndFBO()
// Desc: Restore rendering back to the primary frame buffer
//--------------------------------------------------------------------------------------
VOID FrameBufferObject::EndFBO()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, m_nWidth, m_nHeight);
}