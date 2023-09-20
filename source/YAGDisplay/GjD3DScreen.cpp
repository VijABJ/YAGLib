#include "GjD3DScreen.h"
#include "GjD3DSprite.h"
using namespace yaglib;

D3DStateCache::D3DStateCache(D3DDEVICE device) : mDevice(device), mLastTexture(NULL)
{
  mDevice->AddRef();
  //
  memset(mRenderStates, 0, MAX_RENDER_STATE * sizeof(DWORD));
  for(DWORD i = 0; i < MAX_RENDER_STATE; i++)
    mDevice->GetRenderState(static_cast<D3DRENDERSTATETYPE>(i), &(mRenderStates[i]));
  //
  memset(mTextureStageStates, 0, MAX_TEXTURE_STAGES * MAX_TEXTURE_STAGE_STATE * sizeof(DWORD));
  for(DWORD i = 0; i < MAX_TEXTURE_STAGES; i++)
    for(DWORD j = 0; j < MAX_TEXTURE_STAGE_STATE; j++)
      mDevice->GetTextureStageState(i, static_cast<D3DTEXTURESTAGESTATETYPE>(j), &(mTextureStageStates[i][j]));
  //
#ifdef USE_DIRECTX_9
  memset(mSamplerStates, 0, MAX_SAMPLER_STATE * sizeof(DWORD));
  for(DWORD i = 0; i < MAX_SAMPLER_STATE; i++)
    mDevice->GetSamplerState(0, static_cast<D3DSAMPLERSTATETYPE>(i), &(mSamplerStates[i]));
#endif
}

D3DStateCache::~D3DStateCache()
{
  SAFE_RELEASE(mDevice);
}

HRESULT D3DStateCache::SetTexture(DWORD Sampler, D3DTEXTURE pTexture)
{
  if((Sampler == 0) && (mLastTexture == pTexture))
    return D3D_OK;

  if(Sampler == 0)
    mLastTexture = pTexture;

  return mDevice->SetTexture(Sampler, pTexture);
}

HRESULT D3DStateCache::SetRenderState(D3DRENDERSTATETYPE State, DWORD Value)
{
  if(mRenderStates[State] == Value)
    return D3D_OK;

  mRenderStates[State] = Value;
  return mDevice->SetRenderState(State, Value);
}

HRESULT D3DStateCache::SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value)
{
  if((Stage < MAX_TEXTURE_STAGES) && (mTextureStageStates[Stage][Type] == Value))
    return D3D_OK;

  if(Stage < MAX_TEXTURE_STAGES)
    mTextureStageStates[Stage][Type] = Value;

  return mDevice->SetTextureStageState(Stage, Type, Value);
}

#ifdef USE_DIRECTX_9
HRESULT D3DStateCache::SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value)
{
  if((Sampler == 0) && (mSamplerStates[Type] == Value))
    return D3D_OK;

  if(Sampler == 0)
    mSamplerStates[Type] = Value;

  return mDevice->SetSamplerState(Sampler, Type, Value);
}
#endif

D3DScreen::D3DScreen(HWND windowHandle, const D3DFORMAT resFormat) : Screen(windowHandle), 
  mD3d(NULL), mDevice(NULL), mDeviceFormat(resFormat), mStateCache(NULL)
{
}

bool D3DScreen::_initialize()
{
  if(d3dInit())
    return true;

  d3dShutdown();
  return false;
}

void D3DScreen::_shutdown()
{
  d3dShutdown();
}

bool D3DScreen::d3dInit()
{
  // create the primary d3d interface. if this fails, nothing more we can do...
#ifdef USE_DIRECTX_9  
  mD3d = Direct3DCreate9(D3D_SDK_VERSION);
#else
  mD3d = Direct3DCreate8(D3D_SDK_VERSION);
#endif  
  if(!mD3d) 
    return false;

  // configure the present/flip params
  D3DPRESENT_PARAMETERS d3dp;
  ZeroMemory(&d3dp, sizeof(d3dp));
  d3dp.SwapEffect = D3DSWAPEFFECT_DISCARD;
  d3dp.hDeviceWindow = mWindowHandle;
  d3dp.BackBufferCount = 1;
  d3dp.BackBufferWidth = static_cast<UINT>(mExtents.width);
  d3dp.BackBufferHeight = static_cast<UINT>(mExtents.height);
  d3dp.EnableAutoDepthStencil = TRUE;
  d3dp.AutoDepthStencilFormat = D3DFMT_D16;
#ifdef NO_VSYNC
#ifdef USE_DIRECTX_9
  d3dp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
#else
  if(!mFullScreen)
    d3dp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
#endif
#endif

  // adjust mode if not full screen
  d3dp.Windowed = !mFullScreen;
  if(!mFullScreen)
  {
    D3DDISPLAYMODE d3ddm;
    mD3d->GetAdapterDisplayMode (D3DADAPTER_DEFAULT, &d3ddm);
    d3dp.BackBufferFormat = d3ddm.Format;
  }
  else
    d3dp.BackBufferFormat = mDeviceFormat;

  // get device caps, we'll need it do the rest of init
  D3DCAPS d3dCaps;
  ZeroMemory(&d3dCaps, sizeof(d3dCaps));
  if(FAILED(mD3d->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &d3dCaps)))
    return false;

  bool hvSupported = (d3dCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) != 0;
  if(FAILED(mD3d->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL, mWindowHandle,
    hvSupported ? D3DCREATE_HARDWARE_VERTEXPROCESSING : D3DCREATE_SOFTWARE_VERTEXPROCESSING,
    &d3dp, &mDevice)))
    return false;

  mLoader = new D3DTextureLoader(mDevice);
  mStateCache = new D3DStateCache(mDevice);

  // not tested yet
  //cache_render_states();
  //cache_texture_states();

  // orthographics projection
  D3DXMATRIX mxView;
  D3DXMatrixIdentity(&mxView);

  D3DXMATRIX mxOrtho;
  D3DXMatrixOrthoOffCenterLH(&mxOrtho, 0.0f, mExtents.width, 0.0f, mExtents.height, -1000.0f, 1000.0f);

  mDevice->SetTransform(D3DTS_PROJECTION, &mxOrtho);
  mDevice->SetTransform(D3DTS_VIEW, &mxView);

  return true;
}

void D3DScreen::d3dShutdown()
{
  SAFE_DELETE(mStateCache);
  SAFE_DELETE(mLoader);
  SAFE_RELEASE(mDevice);
  SAFE_RELEASE(mD3d);
}

void D3DScreen::clear(const ColorQuad& color)
{
  mDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER /*| D3DCLEAR_STENCIL*/, (DWORD)(int)color, 1.0f, 0);
}

bool D3DScreen::_beginDrawing(const ColorQuad& background)
{
  mDevice->BeginScene();
  clear(background);
  return true;
}

void D3DScreen::_endDrawing()
{
  mDevice->EndScene();
  mDevice->Present(NULL, NULL, NULL, NULL);
}

Sprite* D3DScreen::createSprite(const WideString spriteName)
{
  return new D3DSprite(this, spriteName);
}

MultiBlitSprite* D3DScreen::createMultiBlitSprite(const WideString spriteName)
{
  return new D3DMultiBlitSprite(this, spriteName);
}

FontSprite* D3DScreen::createFontSprite(const WideString fontName)
{
  FontItem* fItem = g_FontManager.get(fontName);
  if(fItem == NULL)
    return NULL;
  else
    return new D3DFontSprite(this, g_FontManager.get(fontName));
}

