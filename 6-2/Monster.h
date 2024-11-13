#include "CMySprite.h"

class Monster : public CMySprite {
  private:
    float m_speed;
    bool m_keyStates[4]; // Up, Down, Left, Right
  public:
    Monster();
    virtual ~Monster();

    bool Initialize(HINSTANCE hInstance, const std::string& path);
    void Update(int windowWidth, int windowHeight);
};