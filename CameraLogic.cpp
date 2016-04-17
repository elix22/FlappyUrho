#include "CameraLogic.h"

CameraLogic::CameraLogic(Context* context) :
    LogicComponent(context)
{
    SetUpdateEventMask(USE_UPDATE);
}

void CameraLogic::RegisterObject(Context* context)
{
    context->RegisterFactory<CameraLogic>();
}

void CameraLogic::Update(float timeStep)
{
    int wheel{INPUT->GetMouseMoveWheel()};
    if (wheel != 0)
    {
        auto pos{node_->GetPosition()};
        pos.z_ += wheel * 4.0f;

        if (pos.z_ > -5.0f)
            pos.z_ = -5.0f;
        
        node_->SetPosition(pos);
    }

    if (INPUT->GetMouseButtonPress(MOUSEB_RIGHT))
        node_->SetPosition(CAMERA_DEFAULT_POS);
}
