#include "ScreenRenderer.h"

ScreenRenderer::ScreenRenderer()
{
	shaderProgram = new ShaderProgram();
	shaderProgram->g_hShaderProgram = 0;
	shaderProgram->g_hVertexLoc = 0;
	shaderProgram->g_TexcoordLoc = 1;
}

ScreenRenderer::~ScreenRenderer()
{
	glDeleteProgram(shaderProgram->g_hShaderProgram);
}

BOOL ScreenRenderer::InitShaders()
{
	const CHAR* g_strVertexShader =
		"#version 300 es\n"
		"in vec4 g_vPosition;\n"
		"in vec3 g_iTexture;\n"
		"out vec3 g_TexCoord;\n"
		"void main()\n"
		"{\n"
		"	gl_Position = g_vPosition;\n"
		"	g_TexCoord = g_iTexture;\n"
		"}\n";

	const CHAR* g_strFragmentShader =
		"#version 300 es\n"
		"precision highp float;\n"
		"uniform sampler2D g_sImageTexture;\n"
		"in vec3 g_TexCoord;\n"
		"out vec4 g_FragColor;\n"
		"void main()\n"
		"{\n"
		"	g_FragColor = texture2D(g_sImageTexture, g_TexCoord.xy);\n"
		"}\n";

	// Create the shader program needed to render the scene
	{
		// Compile the shaders
		GLuint hVertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(hVertexShader, 1, &g_strVertexShader, NULL);
		glCompileShader(hVertexShader);

		// Check for compile success
		GLint nCompileResult = 0;
		glGetShaderiv(hVertexShader, GL_COMPILE_STATUS, &nCompileResult);
		if (0 == nCompileResult)
		{
			CHAR  strLog[1024];
			GLint nLength;
			glGetShaderInfoLog(hVertexShader, 1024, &nLength, strLog);
			return FALSE;
		}

		GLuint hFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(hFragmentShader, 1, &g_strFragmentShader, NULL);
		glCompileShader(hFragmentShader);

		// Check for compile success
		glGetShaderiv(hFragmentShader, GL_COMPILE_STATUS, &nCompileResult);
		if (0 == nCompileResult)
		{
			CHAR  strLog[1024];
			GLint nLength;
			glGetShaderInfoLog(hFragmentShader, 1024, &nLength, strLog);
			return FALSE;
		}

		// Attach the individual shaders to the common shader program
		shaderProgram->g_hShaderProgram = glCreateProgram();
		glAttachShader(shaderProgram->g_hShaderProgram, hVertexShader);
		glAttachShader(shaderProgram->g_hShaderProgram, hFragmentShader);

		// Init attributes BEFORE linking
		glBindAttribLocation(shaderProgram->g_hShaderProgram, shaderProgram->g_hVertexLoc, "g_vPosition");
		glBindAttribLocation(shaderProgram->g_hShaderProgram, shaderProgram->g_TexcoordLoc, "g_iTexture");

		// Link the vertex shader and fragment shader together
		glLinkProgram(shaderProgram->g_hShaderProgram);

		// Check for link success
		GLint nLinkResult = 0;
		glGetProgramiv(shaderProgram->g_hShaderProgram, GL_LINK_STATUS, &nLinkResult);
		if (0 == nLinkResult)
		{
			CHAR strLog[1024];
			GLint nLength;
			glGetProgramInfoLog(shaderProgram->g_hShaderProgram, 1024, &nLength, strLog);
			return FALSE;
		}

		shaderProgram->g_textureSampleLoc = glGetUniformLocation(shaderProgram->g_hShaderProgram, "g_sImageTexture");

		glDeleteShader(hVertexShader);
		glDeleteShader(hFragmentShader);
	}

	return TRUE;
}

VOID ScreenRenderer::setupScreenMesh()
{
	std::vector<vertex> verts = {
		{ { -1, +1 }, { 0, 1 } },
		{ { +1, +1 }, { 1, 1 } },
		{ { +1, -1 }, { 1, 0 } },
		{ { -1, -1 }, { 0, 0 } }
	};

	std::vector<GLushort> vIndices = {
		1, 0, 3, 
		3, 2, 1
	};
	g_IndexSize = vIndices.size();

	// Bind the vertex attributes
	glGenVertexArrays(1, &g_hVao);
	glBindVertexArray(g_hVao);

	glGenBuffers(1, &g_hPositionVbo);
	glBindBuffer(GL_ARRAY_BUFFER, g_hPositionVbo);
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(vertex), &verts[0], GL_STATIC_DRAW);

	glGenBuffers(1, &g_hIndexVbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_hIndexVbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, vIndices.size() * sizeof(GLushort), &vIndices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(shaderProgram->g_hVertexLoc, 2, GL_FLOAT, false, sizeof(vertex), 0);
	glEnableVertexAttribArray(shaderProgram->g_hVertexLoc);
	glVertexAttribPointer(shaderProgram->g_TexcoordLoc, 2, GL_FLOAT, false, sizeof(vertex), (const void*)(sizeof(FLOAT) * 2));
	glEnableVertexAttribArray(shaderProgram->g_TexcoordLoc);

	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(shaderProgram->g_hVertexLoc);
	glDisableVertexAttribArray(shaderProgram->g_TexcoordLoc);
}

BOOL ScreenRenderer::setupScreenFBO()
{
	glGenFramebuffers(1, &screenRender_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, screenRender_FBO);

	// create a color attachment texture
	glGenTextures(1, &screenTexture);
	glBindTexture(GL_TEXTURE_2D, screenTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, g_nWindowWidth, g_nWindowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return FALSE;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return TRUE;
}

BOOL ScreenRenderer::Initialize(UINT32 windowWidth, UINT32 windowHeight)
{
	g_nWindowWidth = windowWidth;
	g_nWindowHeight = windowHeight;
	g_fAspectRatio = (FLOAT)g_nWindowWidth / (FLOAT)g_nWindowHeight;

	if (InitShaders() == FALSE) {
		return FALSE;
	}

	setupScreenMesh();

	setupScreenFBO();

	return TRUE;
}

VOID ScreenRenderer::Render(FLOAT fTime)
{
	// Blit multisampled buffer to normal colorbuffer
	glBindFramebuffer(GL_READ_FRAMEBUFFER, modelRender_fbo->m_hFrameBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, screenRender_FBO);
	glBlitFramebuffer(0, 0, g_nWindowWidth, g_nWindowHeight, 0, 0, g_nWindowWidth, g_nWindowHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);

	glUseProgram(shaderProgram->g_hShaderProgram);
	glBindVertexArray(g_hVao);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, screenTexture);
	glUniform1i(shaderProgram->g_textureSampleLoc, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glDrawElements(GL_TRIANGLES, g_IndexSize, GL_UNSIGNED_SHORT, 0);
}