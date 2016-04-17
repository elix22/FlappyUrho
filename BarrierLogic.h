#ifndef BARRIERLOGIC_H
#define BARRIERLOGIC_H

#include "Global.h"

class BarrierLogic : public LogicComponent
{
    URHO3D_OBJECT(BarrierLogic, LogicComponent);

public:
    BarrierLogic(Context* context);
    static void RegisterObject(Context* context);
    void Update(float timeStep);
};

#endif // BARRIERLOGIC_H
