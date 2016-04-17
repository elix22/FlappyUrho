#ifndef FISHLOGIC_H
#define FISHLOGIC_H

#include "Global.h"

class FishLogic : public LogicComponent
{
    URHO3D_OBJECT(FishLogic, LogicComponent);

public:
    FishLogic(Context* context);
    static void RegisterObject(Context* context);
    void OnNodeSet(Node* node);
    void Update(float timeStep);
    void Reset();
    void HandleCollisionStart(StringHash eventType, VariantMap& eventData);
    void HandleCollisionEnd(StringHash eventType, VariantMap& eventData);

private:
    float verticalSpeed_;

    float jumpDelay_;
};

#endif // FISHLOGIC_H
