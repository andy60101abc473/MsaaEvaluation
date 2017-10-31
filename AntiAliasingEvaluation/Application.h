#ifndef ALLPLICATION_H
#define ALLPLICATION_H

//--------------------------------------------------------------------------------------
// Name: class Application
// Desc: The main application class for this sample
//--------------------------------------------------------------------------------------

class ModelRenderer;
class ScreenRenderer;
class FrameBufferObject;

class Application : public CFrmApplication
{
    
public:
    virtual BOOL Initialize();
	virtual BOOL Resize() { return TRUE; }
    virtual VOID Destroy();
	virtual VOID Update() { return; }
    virtual VOID Render();

	Application(const CHAR* strName);

private:    
    const CHAR* g_strWindowTitle;

    UINT32 g_nWindowWidth;
    UINT32 g_nWindowHeight;
    FLOAT  g_fAspectRatio;

	FrameBufferObject* screen_fbo;
      
	ModelRenderer* m_ModelRenderer;
	ScreenRenderer* m_ScreenRenderer;
};

#endif // ALLPLICATION_H
