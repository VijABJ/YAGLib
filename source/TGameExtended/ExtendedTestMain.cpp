
#include "GjDefs.h"
#include <cstdlib>
#include "GjGameLib.h"
using namespace yaglib;

class TestGame : public ExtendedFramework
{
public:
  TestGame() : ExtendedFramework(new D3DDisplayDevice()) {};

  virtual bool startup();
  virtual void shutdown();

protected:
  virtual bool frame(const double timeSinceLast);
};

Sprite* sampleSprite = NULL;
Sprite* anotherSprite = NULL;
FontSprite* fs = NULL;

bool TestGame::startup()
{
  if(!ExtendedFramework::startup())
    return false;

  sampleSprite = mScreen->createSprite(L"Background");
  int w = getApplication()->getWindow()->getWidth(),
      h = getApplication()->getWindow()->getHeight();
  sampleSprite->setSize(GJSIZE(w, h));
  //sampleSprite->move(0, 0, 1.0f);

  //anotherSprite = new D3DSprite(mScreen, L"terrain");
  anotherSprite = mScreen->createSprite(L"terrain");
  //anotherSprite->setColor(0xff00ff80);
  anotherSprite->move(100, 50, -1.0f);
  //anotherSprite->setColor(ColorQuad(0, 0xff, 0));

  fs = mScreen->createFontSprite(L"default");
  //fs->setText(L"The Quick Brown Fox Jumps Over The Lazy Dog");
  fs->move(10, 10, -2.0f);

  SoundManager* sm = getSoundManager();
  sm->quickPlay(L"rollingSound");

  return true;
}

void TestGame::shutdown()
{
  SAFE_DELETE(anotherSprite);
  SAFE_DELETE(sampleSprite);

  ExtendedFramework::shutdown();
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