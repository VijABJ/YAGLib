/** 
 * @file  GjD3DScreen.h
 * @brief DirectGraphics screen
 *
 */

#ifndef GJ_DIRECT3D_SCREEN_HEADER
#define GJ_DIRECT3D_SCREEN_HEADER

#include "GjDefs.h"
#include "GjScreen.h"
#include "GjRectangles.h"
#include "GjD3DTextures.h"
#include "GjDirectX.h"

//#define PERSPECTIVE_PROJECTION

namespace yaglib
{

const int MAX_TEXTURE_STAGES = 2;

class D3DStateCache 
{
public:
  D3DStateCache(D3DDEVICE device);
  virtual ~D3DStateCache();

  HRESULT SetTexture(DWORD Sampler, D3DTEXTURE pTexture);
  HRESULT SetRenderState(D3DRENDERSTATETYPE State, DWORD Value);
  HRESULT SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value);
#ifdef USE_DIRECTX_9
  HRESULT SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value);
#endif

private:
  static const int MAX_TEXTURE_STAGES = 2;
#ifdef USE_DIRECTX_9
  static const DWORD MAX_RENDER_STATE = D3DRS_BLENDOPALPHA;
  static const DWORD MAX_TEXTURE_STAGE_STATE = D3DTSS_CONSTANT;
  static const DWORD MAX_SAMPLER_STATE = D3DSAMP_DMAPOFFSET;
#else
  static const DWORD MAX_RENDER_STATE = D3DRS_NORMALORDER;
  static const DWORD MAX_TEXTURE_STAGE_STATE = D3DTSS_RESULTARG;
#endif
  //
  D3DDEVICE mDevice;
  D3DTEXTURE mLastTexture;
  DWORD mRenderStates[MAX_RENDER_STATE];
  DWORD mTextureStageStates[MAX_TEXTURE_STAGES][MAX_TEXTURE_STAGE_STATE];
#ifdef USE_DIRECTX_9
  DWORD mSamplerStates[MAX_SAMPLER_STATE];
#endif
};

class D3DScreen : public Screen
{
public:
  D3DScreen(HWND windowHandle, const D3DFORMAT resFormat = D3DFMT_A8R8G8B8);

  virtual bool _beginDrawing(const ColorQuad& background = 0);
  virtual void _endDrawing();
  virtual void clear(const ColorQuad& color = 0);

  virtual Sprite* createSprite(const WideString spriteName);
  virtual MultiBlitSprite* createMultiBlitSprite(const WideString spriteName);
  virtual FontSprite* createFontSprite(const WideString fontName);

  D3D getD3d() { return mD3d; };        
  D3DDEVICE getDevice() { return mDevice; };

  HRESULT SetTexture(DWORD Sampler, D3DTEXTURE pTexture)
  { return mStateCache->SetTexture(Sampler, pTexture); };
  HRESULT SetRenderState(D3DRENDERSTATETYPE State, DWORD Value)
  { return mStateCache->SetRenderState(State, Value); };
  HRESULT SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value)
  { return mStateCache->SetTextureStageState(Stage, Type, Value); };
#ifdef USE_DIRECTX_9
  HRESULT SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value)
  { return mStateCache->SetSamplerState(Sampler, Type, Value); };
#endif

protected:
  virtual bool _initialize();
  virtual void _shutdown();

private:
  D3DFORMAT mDeviceFormat;
  D3D mD3d;                         // the d3d interface
  D3DDEVICE mDevice;                // the device
  D3DVERTEXBUFFER mVb;              // vertex buffer, owned by the renderer
  D3DStateCache* mStateCache;

  bool d3dInit();
  void d3dShutdown();
};

}; /* namespace yaglib */


#endif /* GJ_DIRECT3D_SCREEN_HEADER */
