#include "CMySprite.h"

class Monster : public CMySprite { // inherits from CMySprite
  private:
    int m_speed;
    bool m_keyStates[4]; // Up, Down, Left, Right
    int m_currentFrame;
    int m_frameWidth, m_frameHeight;
    int m_animationDelay;
    int m_animationCounter;
    int m_direction; // 0 = down, 1 = left, 2 = right, 3 = up
  public:
    Monster();
    virtual ~Monster();

    bool Initialize(HINSTANCE hInstance, UINT resourceID);
    void Update(int windowWidth, int windowHeight);
    void UpdateAnimation();
    void Render(HDC hdcDest) override;
};