#include "Crown.h"


Crown::Crown(Context* context) : LogicComponent(context)
{
}

void Crown::RegisterObject(Context* context)
{
    context->RegisterFactory<Crown>();
}

void Crown::OnNodeSet(Node* node)
{ (void)node;

    node_->Rotate(Quaternion(23.5f, Vector3::FORWARD), TS_WORLD);

    StaticModel* crownModel{ node_->CreateComponent<StaticModel>() };
    crownModel->SetModel(CACHE->GetResource<Model>("Models/Crown.mdl"));
    crownModel->SetCastShadows(true);
    crownModel->ApplyMaterialList();
}

void Crown::Update(float timeStep)
{
    node_->Rotate(Quaternion(timeStep * 23.0f, Vector3::UP), TS_WORLD);

    node_->Translate(Vector3::UP * 0.01f * (node_->GetScene()->GetChild("Urho")->GetPosition().y_ - node_->GetPosition().y_), TS_WORLD);

    float targetXPos{ (2.3f + 25.0f * (GLOBAL->GetHighscore() - GLOBAL->GetScore()) / GLOBAL->GetHighscore()) };
    node_->SetPosition(Vector3(0.01f * (targetXPos + 99.0f * node_->GetPosition().x_),
                               node_->GetPosition().y_,
                               node_->GetPosition().z_));

    if (GLOBAL->GetScore() > GLOBAL->GetHighscore())
        node_->SetEnabled(false);
}



