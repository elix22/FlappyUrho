#ifndef CROWN_H
#define CROWN_H

#include <Global.h>


class Crown : public LogicComponent
{
    URHO3D_OBJECT(Crown, LogicComponent);
public:
    Crown(Context* context);
    static void RegisterObject(Context* context);
    virtual void Update(float timeStep);
    virtual void OnNodeSet(Node* node);
};

#endif // CROWN_H
