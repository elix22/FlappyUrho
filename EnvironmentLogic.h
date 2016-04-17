#ifndef ENVIRONMENTLOGIC_H
#define ENVIRONMENTLOGIC_H

#include "Global.h"

class EnvironmentLogic : public LogicComponent
{
    URHO3D_OBJECT(EnvironmentLogic, LogicComponent);

public:
    EnvironmentLogic(Context* context);
    static void RegisterObject(Context* context);
    void Update(float timeStep);
};

#endif // ENVIRONMENTLOGIC_H
