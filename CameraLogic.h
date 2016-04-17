#ifndef CAMERALOGIC_H
#define CAMERALOGIC_H

#include "Global.h"

class CameraLogic : public LogicComponent
{
    URHO3D_OBJECT(CameraLogic, LogicComponent);

public:
    CameraLogic(Context* context);
    static void RegisterObject(Context* context);
    void Update(float timeStep);
};

#endif // CAMERALOGIC_H
