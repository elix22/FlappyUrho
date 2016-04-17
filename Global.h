#ifndef GLOBAL_H
#define GLOBAL_H

#include "Urho3DAll.h"

#define NUM_BARRIERS 3

#define BAR_GAP 10.0f

#define BAR_INTERVAL 20.0f

#define BAR_SPEED 5.0f

#define BAR_RANDOM_Y Random(-6.0f, 6.0f)

#define BAR_OUTSIDE_X 30.0f

#define GRAV_ACC 9.8f

#define UP_SPEED 10.0f

#define CAMERA_DEFAULT_POS Vector3(0.0f, 0.0f, -30.0f)

#define UFO_DEFAULT_ROTATION Quaternion(0.0f, 90.0f, 0.0f)

#define CACHE GetSubsystem<ResourceCache>()
#define GLOBAL GetSubsystem<Global>()
#define INPUT GetSubsystem<Input>()
#define UI_ROOT GetSubsystem<UI>()->GetRoot()
#define RENDERER GetSubsystem<Renderer>()
#define GRAPHICS GetSubsystem<Graphics>()

enum GameState
{
    GS_INTRO = 0,

    GS_GAMEPLAY,

    GS_DEAD
};

class Global : public Object
{
    URHO3D_OBJECT(Global, Object);

public:
    Global(Context* context);
    void HandlePostUpdate(StringHash eventType, VariantMap& eventData);
    
    int GetScore() const { return score_; }
    void SetScore(int score);

    GameState gameState_;
    GameState neededGameState_;

private:
    int score_;
    bool scoreTextDirty_;
};

#endif // GLOBAL_H
