#ifndef UFOLOGIC_H
#define UFOLOGIC_H

#include "Global.h"

class UfoLogic : public LogicComponent
{
    URHO3D_OBJECT(UfoLogic, LogicComponent);

public:
    UfoLogic(Context* context);
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

#endif // UFOLOGIC_H
