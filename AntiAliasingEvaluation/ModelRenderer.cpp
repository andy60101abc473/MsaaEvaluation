#include "ModelRenderer.h"

// Define vertice for a 4-sided pyramid
FLOAT vertexPositions[] =
{
	0.0f,+1.0f, 0.0f,      -1.0f,-1.0f, 1.0f,      +1.0f,-1.0f, 1.0f,
	0.0f,+1.0f, 0.0f,      +1.0f,-1.0f, 1.0f,      +1.0f,-1.0f,-1.0f,
	0.0f,+1.0f, 0.0f,      +1.0f,-1.0f,-1.0f,      -1.0f,-1.0f,-1.0f,
	0.0f,+1.0f, 0.0f,      -1.0f,-1.0f,-1.0f,      -1.0f,-1.0f, 1.0f,
};

FLOAT vertexColors[] =
{
	1.0f, 0.0f, 0.0f,       1.0f, 0.0f, 0.0f,       1.0f, 0.0f, 0.0f, // Red
	0.0f, 1.0f, 0.0f,       0.0f, 1.0f, 0.0f,       0.0f, 1.0f, 0.0f, // Green
	0.0f, 0.0f, 1.0f,       0.0f, 0.0f, 1.0f,       0.0f, 0.0f, 1.0f, // Blue
	1.0f, 1.0f, 0.0f,       1.0f, 1.0f, 0.0f,       1.0f, 1.0f, 0.0f, // Yellow
};

ModelRenderer::ModelRenderer()
{
	shaderProgram = new ShaderProgram();
	shaderProgram->g_hShaderProgram = 0;
	shaderProgram->g_hModelViewMatrixLoc = 0;
	shaderProgram->g_hProjMatrixLoc = 0;
	shaderProgram->g_hVertexLoc = 0;
	shaderProgram->g_hColorLoc = 1;
}

ModelRenderer::~ModelRenderer()
{
	glDeleteProgram(shaderProgram->g_hShaderProgram);
}

BOOL ModelRenderer::InitShaders()
{
	const CHAR* g_strVertexShader =
		"#version 300 es\n"
		"uniform mat4 g_matModelView;\n"
		"uniform mat4 g_matProj;\n"
		"in vec4 g_vPosition;\n"
		"in vec3 g_vColor;\n"
		"out vec3 g_vVSColor;\n"
		"void main()\n"
		"{\n"
		"	vec4 vPositionES = g_matModelView * g_vPosition;\n"
		"	gl_Position = g_matProj * vPositionES;\n"
		"	g_vVSColor = g_vColor;\n"
		"}\n";

	const CHAR* g_strFragmentShader =
		"#version 300 es\n"
		"precision highp float;\n"
		"in vec3 g_vVSColor;\n"
		"out vec4 g_FragColor;\n"
		"void main()\n"
		"{\n"
		"	g_FragColor = vec4( g_vVSColor, 1.0 );\n"
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
		glBindAttribLocation(shaderProgram->g_hShaderProgram, shaderProgram->g_hColorLoc, "g_vColor");

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

		// Get uniform locations
		shaderProgram->g_hModelViewMatrixLoc = glGetUniformLocation(shaderProgram->g_hShaderProgram, "g_matModelView");
		shaderProgram->g_hProjMatrixLoc = glGetUniformLocation(shaderProgram->g_hShaderProgram, "g_matProj");

		glDeleteShader(hVertexShader);
		glDeleteShader(hFragmentShader);
	}

	return TRUE;
}

BOOL ModelRenderer::Initialize(UINT32 windowWidth, UINT32 windowHeight)
{
	g_nWindowWidth = windowWidth;
	g_nWindowHeight = windowHeight;
	g_fAspectRatio = (FLOAT)g_nWindowWidth / (FLOAT)g_nWindowHeight;

	InitShaders();

	// Bind the vertex attributes
	glGenVertexArrays(1, &g_hVao);
	glBindVertexArray(g_hVao);

	glGenBuffers(1, &g_hPositionVbo);
	glBindBuffer(GL_ARRAY_BUFFER, g_hPositionVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(FLOAT32) * 36, vertexPositions, GL_STATIC_DRAW);
	glVertexAttribPointer(shaderProgram->g_hVertexLoc, 3, GL_FLOAT, false, 12, 0);
	glEnableVertexAttribArray(shaderProgram->g_hVertexLoc);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &g_hColorVbo);
	glBindBuffer(GL_ARRAY_BUFFER, g_hColorVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(FLOAT32) * 36, vertexColors, GL_STATIC_DRAW);
	glVertexAttribPointer(shaderProgram->g_hColorLoc, 3, GL_FLOAT, false, 12, 0);
	glEnableVertexAttribArray(shaderProgram->g_hColorLoc);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return TRUE;
}

VOID ModelRenderer::RenderScene(FLOAT fTime)
{
	// Rotate and translate the model view matrix
	float matModelView[16] = { 0 };
	matModelView[0] = +cosf(fTime);
	matModelView[2] = +sinf(fTime);
	matModelView[5] = 1.0f;
	matModelView[8] = -sinf(fTime);
	matModelView[10] = +cosf(fTime);
	matModelView[14] = -5.0f;
	matModelView[15] = 1.0f;

	// Build a perspective projection matrix
	float matProj[16] = { 0 };
	matProj[0] = cosf(0.5f) / sinf(0.5f);
	matProj[5] = matProj[0] * g_fAspectRatio;
	matProj[10] = -(10.0f) / (9.0f);
	matProj[11] = -1.0f;
	matProj[14] = -(10.0f) / (9.0f);

	// Clear the colorbuffer and depth-buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set some state
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Set the shader program
	glUseProgram(shaderProgram->g_hShaderProgram);
	glUniformMatrix4fv(shaderProgram->g_hModelViewMatrixLoc, 1, FALSE, matModelView);
	glUniformMatrix4fv(shaderProgram->g_hProjMatrixLoc, 1, FALSE, matProj);

	glBindVertexArray(g_hVao);

	// Draw the cubemap-reflected triangle
	glDrawArrays(GL_TRIANGLES, 0, 12);

	// Cleanup
	glBindVertexArray(0);
}

VOID ModelRenderer::Render(FLOAT fTime)
{
	screen_fbo->BeginFBO();
	RenderScene(fTime);
	screen_fbo->EndFBO();
}