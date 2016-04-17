#include "UfoLogic.h"

UfoLogic::UfoLogic(Context* context) :
    LogicComponent(context),
    verticalSpeed_(0.0f),
    jumpDelay_(0.0f)
{
    SetUpdateEventMask(USE_UPDATE);
}

void UfoLogic::RegisterObject(Context* context)
{
    context->RegisterFactory<UfoLogic>();
}

void UfoLogic::OnNodeSet(Node* node)
{
    SubscribeToEvent(node_, E_NODECOLLISIONSTART, URHO3D_HANDLER(UfoLogic, HandleCollisionStart));
    SubscribeToEvent(node_, E_NODECOLLISIONEND, URHO3D_HANDLER(UfoLogic, HandleCollisionEnd));
}

void UfoLogic::HandleCollisionStart(StringHash eventType, VariantMap& eventData)
{
    using namespace NodeCollisionStart;
    Node* otherNode = static_cast<Node*>(eventData[P_OTHERNODE].GetPtr());

    if (otherNode->GetName() == "Pipe")
        GLOBAL->neededGameState_ = GS_DEAD;
}

void UfoLogic::HandleCollisionEnd(StringHash eventType, VariantMap& eventData)
{
    using namespace NodeCollisionEnd;
    Node* otherNode = static_cast<Node*>(eventData[P_OTHERNODE].GetPtr());

    if (otherNode->GetName() == "Barrier")
    {
        if (GLOBAL->gameState_ == GS_GAMEPLAY)
            GLOBAL->SetScore(GLOBAL->GetScore() + 1);
    }
}

void UfoLogic::Reset()
{
    verticalSpeed_ = 0.0f;
    jumpDelay_ = 0.0f;
}

void UfoLogic::Update(float timeStep)
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

    auto pos = node_->GetPosition();

    verticalSpeed_ -= timeStep * GRAV_ACC;
    
    if (jumpDelay_ > 0.0f)
        jumpDelay_ -= timeStep;

    if (INPUT->GetMouseButtonPress(MOUSEB_LEFT) && jumpDelay_ <= 0.0f)
    {
        verticalSpeed_ = UP_SPEED;
        auto soundSource = node_->GetOrCreateComponent<SoundSource>();
        soundSource->Play(CACHE->GetResource<Sound>("Sounds/Jump.wav"));

        jumpDelay_ = 0.75f;
    }

    pos += Vector3::UP * verticalSpeed_ * timeStep;
    node_->SetPosition(pos);

    auto animatedModel = node_->GetComponent<AnimatedModel>();
    if (!animatedModel->IsInView())
        GLOBAL->neededGameState_ = GS_DEAD;
}
