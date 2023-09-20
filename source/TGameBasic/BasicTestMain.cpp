
#include "GjDefs.h"
#include <cstdlib>
#include "GjGameLib.h"
using namespace yaglib;

class TestGame : public BasicFramework
{
public:
  TestGame() : BasicFramework() {};
  virtual ~TestGame() {};

  virtual bool startup();
  virtual void shutdown();

protected:
  virtual bool beforeFrame(const double timeSinceLast);
  virtual bool frame(const double timeSinceLast);
  virtual bool afterFrame(const double timeSinceLast);
  virtual void fpsUpdated();
};

D3DScreen* screen = NULL;
Sprite* sampleSprite = NULL;
Sprite* anotherSprite = NULL;
FontSprite* fs = NULL;

bool TestGame::startup()
{
  if(!BasicFramework::startup())
    return false;

  screen = new D3DScreen(getApplication()->getWindowHandle());
  int w = getApplication()->getWindow()->getWidth(),
      h = getApplication()->getWindow()->getHeight();
  screen->initialize(w, h, getApplication()->isFullScreen());

  sampleSprite = screen->createSprite(L"Background");
  sampleSprite->setSize(w, h);
  //sampleSprite->move(0, 0, 1.0f);

  //anotherSprite = new D3DSprite(screen, L"terrain");
  anotherSprite = screen->createSprite(L"terrain");
  //anotherSprite->setColor(0xff00ff80);
  anotherSprite->move(100, 50, -1.0f);
  //anotherSprite->setColor(ColorQuad(0, 0xff, 0));

  fs = screen->createFontSprite(L"default");
  //fs->setText(L"The Quick Brown Fox Jumps Over The Lazy Dog");
  fs->move(10, 10, -2.0f);

  return true;
}

void TestGame::shutdown()
{
  SAFE_DELETE(anotherSprite);
  SAFE_DELETE(sampleSprite);
  screen->shutdown();
  SAFE_DELETE(screen);
  BasicFramework::shutdown();
}

void TestGame::fpsUpdated()
{
  char buf[60];
  _snprintf(buf, 60, "%d FPS", mFPS);
  WideString fpsText(from_char_p(buf));
  fs->setText(fpsText);
}

bool TestGame::beforeFrame(const double timeSinceLast)
{
  if(screen)
    screen->_beginDrawing();
  
  return true;
}

bool TestGame::afterFrame(const double timeSinceLast)
{
  if(screen)
    screen->_endDrawing();

  return true;
}

static double span = 0.0f;

bool TestGame::frame(const double timeSinceLast)
{
  if(sampleSprite)
  {
    span += timeSinceLast;
    if(span > 1.0f)
    {
      span = 0.0f;
      anotherSprite->changeFrame(CHANGE_TO_NEXT_FRAME);
    }

    anotherSprite->draw();
    sampleSprite->draw();
    fs->draw();
  }

  return true;
}


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
  WideString args(from_char_p(lpCmdLine));
  bool fullScreen = (args.length() > 0);

  TestGame game;
  game.execute(hInstance, L"TestGameClass", L"BasicFramework Test", fullScreen, 800, 600);

  return 0;
}