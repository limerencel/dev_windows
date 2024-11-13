#include <windows.h>
#include "Monster.h"
#include <algorithm>

Monster::Monster() : CMySprite(), m_speed(1) {  // double colons: scope resolution operator
  for (int i = 0; i < 4; ++i) { // double colons: define a function that belongs to the class
    m_keyStates[i] = false;
  }
}

Monster::~Monster() {}

bool Monster::Initialize(HINSTANCE hInstance, const std::string& path) {
  if (!LoadBitmap(hInstance, path)) {
    return false;
  }
  SetPosition(100, 100);
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
  if (m_keyStates[0]) dy -= m_speed;
  if (m_keyStates[1]) dy += m_speed;
  if (m_keyStates[2]) dx -= m_speed;
  if (m_keyStates[3]) dx += m_speed;

  // Check boundaries
  int newX = GetX() + dx;
  int newY = GetY() + dy;
  newX = std::max(0, std::min(newX, windowWidth - GetWidth()));
  newY = std::max(0, std::min(newY, windowHeight - GetHeight()));

  // Update position
  SetPosition(newX, newY);
}