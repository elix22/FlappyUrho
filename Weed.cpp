/* heXon
// Copyright (C) 2016 LucKey Productions (luckeyproductions.nl)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "Weed.h"


Weed::Weed(Context* context) : LogicComponent(context)
{
}

void Weed::RegisterObject(Context* context)
{
    context->RegisterFactory<Weed>();
}

void Weed::OnNodeSet(Node* node)
{ (void)node;

    AnimatedModel* animatedModel{node_->CreateComponent<AnimatedModel>()};
    animatedModel->SetModel(CACHE->GetResource<Model>("Models/Weed.mdl"));
    animatedModel->SetMaterial(CACHE->GetResource<Material>("Materials/VCol.xml")->Clone());
    animatedModel->GetMaterial()->SetShaderParameter("MatDiffColor", Color(Random(0.8f, 1.0f), Random(0.8f, 1.0f), Random(0.8f, 1.0f)));
    animatedModel->SetCastShadows(false);

    AnimationController* animCtrl{node_->CreateComponent<AnimationController>()};
    animCtrl->PlayExclusive("Models/Wave.ani", 1, true);
    animCtrl->SetAnimationTime(Random(5.0f));
    animCtrl->SetSpeed("Models/Wave.ani", Random(0.42f, 0.666f));
//    staticModel->ApplyMaterialList();

}

void Weed::Update(float timeStep)
{
    if (GLOBAL->gameState_ == GS_DEAD)
        return;

    Vector3 pos{node_->GetPosition()};
    pos += Vector3::LEFT * timeStep * BAR_SPEED;

    if (pos.x_ < -BAR_OUTSIDE_X && !node_->GetComponent<AnimatedModel>()->IsInView())
    {
        pos.x_ += NUM_BARRIERS * BAR_INTERVAL + pos.z_;
        pos.y_ = WEED_RANDOM_Y;

        node_->SetRotation(Quaternion(0.0f, Random(360.0f), 0.0f));
        node_->SetScale(Vector3(Random(0.5f, 1.23f), Random(0.8f, 2.3f), Random(0.5f, 1.23f)));
    }

    node_->SetPosition(pos);
}





