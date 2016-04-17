#include "BarrierLogic.h"

BarrierLogic::BarrierLogic(Context* context) :
    LogicComponent(context)
{
    SetUpdateEventMask(USE_UPDATE);
}

void BarrierLogic::RegisterObject(Context* context)
{
    context->RegisterFactory<BarrierLogic>();
}

void BarrierLogic::Update(float timeStep)
{
    if (GLOBAL->gameState_ != GS_GAMEPLAY)
        return;

    auto pos{node_->GetPosition()};
    pos += Vector3::LEFT * timeStep * BAR_SPEED;

    if (pos.x_ < -BAR_OUTSIDE_X)
    {
        pos.x_ += NUM_BARRIERS * BAR_INTERVAL;
        pos.y_ = BAR_RANDOM_Y;
    }

    node_->SetPosition(pos);
}
