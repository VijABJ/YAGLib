/*
Yet Another Game Library
Copyright (c) 2001-2007, Virgilio A. Blones, Jr. (vij_blones_jr@yahoo.com)
See https://sourceforge.net/projects/yaglib/ for the latest updates.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, 
      this list of conditions and the following disclaimer.
      
    * Redistributions in binary form must reproduce the above copyright notice, 
      this list of conditions and the following disclaimer in the documentation 
      and/or other materials provided with the distribution.
      
    * Neither the name of this library (YAGLib) nor the names of its contributors 
      may be used to endorse or promote products derived from this software 
      without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "GjDefs.h"
#include <cstdlib>
#include "GjGameLib.h"
#include "GjGameLibConstants.h"
#include <fstream>
#include <iostream>

using namespace yaglib;

#define SPRITE_BACKGROUND L"Background"

class TestGame : public BaseEventFramework
{
public:
  TestGame();
  virtual ~TestGame() {};

  virtual bool startup();
  virtual void shutdown();

protected:
  // extras
  BaseView* mButtonStatus;
  BaseView* mButtonPosition;
  BaseView* mTestButton;

  void showMouseMoves();
  void showMouseStates();
  virtual bool consumeEvent(Event& event);
  virtual bool frame(const double timeSinceLast);
};

TestGame::TestGame() : BaseEventFramework(new D3DDisplayDevice())
{
}

bool TestGame::startup()
{
  if(!BaseEventFramework::startup())
    return false;

  GJRECT r = GJRECT(GJPOINT(0, 0), 
    GJPOINT(
      getApplication()->getWindow()->getWidth(),
      getApplication()->getWindow()->getHeight()
    ));
  mRoot->changeSprite(SPRITE_BACKGROUND);

  BaseView* testPanel = new BorderedPanel(mRoot, GJRECT(50, 100, 250, 300), mScreen, L"double-borders");
  testPanel->setOption(VO_DRAGGABLE|VO_AUTODRAG);

  mButtonStatus = new SimplePanel(mRoot, GJRECT(0, r.bottom - 40, r.right/2, r.bottom), mScreen);
  mButtonStatus->setCaption(L"Button Status");
  mButtonStatus->setOption(VO_DRAGGABLE);

  mButtonPosition = new SimplePanel(mRoot, GJRECT(r.right/2, r.bottom - 40, r.right, r.bottom), mScreen);
  mButtonPosition->setCaption(L"Button Position");
  mButtonPosition->setOption(VO_DRAGGABLE);

  GJRECT bounds = GJRECT(0, 0, 150, 60); bounds.translate(30, 30);
  mTestButton = new SimpleButton(testPanel, bounds, mScreen, L"white-button", STDMESSAGE_EXIT);
  mTestButton->setCaption(L"Click Here To Quit");
  mTestButton->refreshSprite();
  //GJFLOAT deltaY = mTestButton->getBounds().height + 10;
  //mTestButton->setOption(VO_DRAGGABLE | VO_OWNERBOUNDED);
  
  return true;
}

void TestGame::shutdown()
{
  BaseEventFramework::shutdown();
}

bool TestGame::frame(const double timeSinceLast)
{
  return BaseEventFramework::frame(timeSinceLast);
}

void TestGame::showMouseMoves()
{
  Point3 position;
  mMouse->getAxesMonitor()->getPosition(position);
  Point3 delta;
  mMouse->getAxesMonitor()->getMovement(delta);

  char buf[80];
  _snprintf(buf, 64, "Button Position (X,Y)=(%d,%d), delta (dX,dY)=(%d,%d)", position.x, position.y, delta.x, delta.y);
  WideString ws = from_char_p(buf);
  if(mButtonPosition)
    mButtonPosition->setCaption(ws);
}

void TestGame::showMouseStates()
{
  ButtonMonitor* bm = mMouse->getButtonMonitor();
  int count = bm->getNumberOfButtons();
  WideString ws(L"Buttons ");
  for(int i = 0; i < count; ++i)
  {
    static char* buttonNames[] = {"left", "right", "middle", "extra 1", "extra 2"};
    bool isDown = bm->isPressed(i);
    char buf[64];
    _snprintf(buf, 64, "| %s is %s ", buttonNames[i], isDown ? "down" : "up");
    ws += from_char_p(buf);
  }
  if(mButtonStatus)
    mButtonStatus->setCaption(ws);
}

bool TestGame::consumeEvent(Event& event)
{
  BaseEventFramework::consumeEvent(event);
  switch(event.event)
  {
  case EVENT_MOUSE:
    if(event.mouse.type == MOUSE_MOVE)
      showMouseMoves();
    else
      showMouseStates();
    break;
  }
  return false;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
  WideString args(from_char_p(lpCmdLine));
  bool fullScreen = (args.length() > 0);

  TestGame game;
  game.execute(hInstance, L"TestGameClass", L"Event Framework Test", fullScreen, 800, 600);

  return (0);
}
