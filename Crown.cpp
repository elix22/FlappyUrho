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
}





