#include <windows.h>
#include "Monster.h"
#include <algorithm>
#include <wingdi.h>

Monster::Monster() : CMySprite(), m_speed(2), m_currentFrame(0), m_frameWidth(0),
 m_frameHeight(0), m_animationDelay(5), m_animationCounter(0), m_direction(0) {
  // double colons: scope resolution operator, define a function that belongs to the class
  std::fill(m_keyStates, m_keyStates+4, false);
}

Monster::~Monster() {}

bool Monster::Initialize(HINSTANCE hInstance, UINT resourceID) {
  if (!LoadBitmapFromResource(hInstance, resourceID)) {
    return false;
  }
  // calculate frame dimensions
  m_frameWidth = GetWidth() / 4;
  m_frameHeight = GetHeight() / 4;

  return true;
}

void Monster::Update(int windowWidth, int windowHeight) {
  // Update key states
  m_keyStates[0] = (GetAsyncKeyState(VK_UP) & 0x8000) != 0;
  m_keyStates[1] = (GetAsyncKeyState(VK_DOWN) & 0x8000) != 0;
  m_keyStates[2] = (GetAsyncKeyState(VK_LEFT) & 0x8000) != 0;
  m_keyStates[3] = (GetAsyncKeyState(VK_RIGHT) & 0x8000) != 0;

  // Move the monster based on key states
  int dx = 0, dy = 0;
  if (m_keyStates[0]) { dy -= m_speed; m_direction = 3; } // Up
  if (m_keyStates[1]) { dy += m_speed; m_direction = 0; } // Down
  if (m_keyStates[2]) { dx -= m_speed; m_direction = 1; } // Left
  if (m_keyStates[3]) { dx += m_speed; m_direction = 2; } // Right


  // Check boundaries
  int newX = GetX() + dx;
  int newY = GetY() + dy;
  // Clamp newX and newY to stay within the window
  if (newX < 0) newX = 0;
  if (newX > windowWidth - m_frameWidth) newX = windowWidth - m_frameWidth;
  if (newY < 0) newY = 0;
  if (newY > windowHeight - m_frameHeight) newY = windowHeight - m_frameHeight;



  // Update position
  SetPosition(newX, newY);

  // Update animation
  UpdateAnimation();
}

void Monster::UpdateAnimation() {
  if (m_keyStates[0] || m_keyStates[1] || m_keyStates[2] || m_keyStates[3]) {
    m_animationCounter++;
    if (m_animationCounter >= m_animationDelay) {
      m_currentFrame = (m_currentFrame + 1) % 4; // cycle through animation frames
      m_animationCounter = 0;
    }
  } else {
      m_currentFrame = 0; // Reset to standing frame when not moving
      m_animationCounter = 0;
      
  }
}

void Monster::Render(HDC hdcDest) {
  int srcX = m_currentFrame * m_frameWidth;
  int srcY = m_direction * m_frameHeight;

  COLORREF transparentColor = RGB(181, 230, 29);

  TransparentBlt(
    hdcDest,
    GetX(),
    GetY(),
    m_frameWidth,
    m_frameHeight,
    GetMemDC(),
    srcX,
    srcY,
    m_frameWidth,
    m_frameHeight,
    transparentColor
  );
}