﻿#ifndef BARRIERLOGIC_H
#define BARRIERLOGIC_H

#include "Global.h"

class Barrier : public LogicComponent
{
    URHO3D_OBJECT(Barrier, LogicComponent);

public:
    Barrier(Context* context);
    static void RegisterObject(Context* context);
    void Update(float timeStep);
    void OnNodeSet(Node* node);
};

#endif // BARRIERLOGIC_H