#include "FishLogic.h"

FishLogic::FishLogic(Context* context) :
    LogicComponent(context),
    verticalSpeed_{0.0f},
    jumpDelay_{0.0f}
{
    SetUpdateEventMask(USE_UPDATE);
}

void FishLogic::RegisterObject(Context* context)
{
    context->RegisterFactory<FishLogic>();
}

void FishLogic::OnNodeSet(Node* node)
{
    SubscribeToEvent(node_, E_NODECOLLISIONSTART, URHO3D_HANDLER(FishLogic, HandleCollisionStart));
    SubscribeToEvent(node_, E_NODECOLLISIONEND, URHO3D_HANDLER(FishLogic, HandleCollisionEnd));
}

void FishLogic::HandleCollisionStart(StringHash eventType, VariantMap& eventData)
{
    Node* otherNode{static_cast<Node*>(eventData[NodeCollisionStart::P_OTHERNODE].GetPtr())};

    if (otherNode->GetName() == "Pipe")
        GLOBAL->neededGameState_ = GS_DEAD;
}

void FishLogic::HandleCollisionEnd(StringHash eventType, VariantMap& eventData)
{
    Node* otherNode{static_cast<Node*>(eventData[NodeCollisionEnd::P_OTHERNODE].GetPtr())};

    if (otherNode->GetName() == "Barrier")
    {
        if (GLOBAL->gameState_ == GS_GAMEPLAY){
            GLOBAL->SetScore(GLOBAL->GetScore() + 1);
            auto soundSource{node_->GetOrCreateComponent<SoundSource>()};
            soundSource->Play(CACHE->GetResource<Sound>("Samples/Blup" + String{Random(4)} + ".ogg"));
        }
    }
}

void FishLogic::Reset()
{
    verticalSpeed_ = 0.0f;
    jumpDelay_ = 0.0f;
}

void FishLogic::Update(float timeStep)
{
    if (GLOBAL->gameState_ == GS_DEAD)
    {
        node_->Rotate(Quaternion(0.0f, timeStep * 200.0f, -timeStep * 400.0f));

        if (node_->GetPosition().z_ > -50.0f)
        {
            node_->Translate(Vector3(0.0f, -10.0f * timeStep, -30.0f * timeStep), TS_WORLD);
        }

        return;
    }

    if (GLOBAL->gameState_ != GS_GAMEPLAY)
        return;

    auto pos{node_->GetPosition()};

    verticalSpeed_ -= timeStep * GRAV_ACC;
    
    if (jumpDelay_ > 0.0f)
        jumpDelay_ -= timeStep;

    if (INPUT->GetMouseButtonPress(MOUSEB_LEFT) && jumpDelay_ <= 0.0f)
    {
        verticalSpeed_ = UP_SPEED;
        auto soundSource{node_->GetOrCreateComponent<SoundSource>()};
        soundSource->Play(CACHE->GetResource<Sound>("Samples/Jump.ogg"));

        jumpDelay_ = 0.75f;
    }

    pos += Vector3::UP * verticalSpeed_ * timeStep;
    node_->SetPosition(pos);

    auto animatedModel{node_->GetComponent<AnimatedModel>()};
    if (!animatedModel->IsInView())
        GLOBAL->neededGameState_ = GS_DEAD;
}
