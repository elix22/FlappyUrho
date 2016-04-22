﻿#include "Global.h"
#include "BarrierLogic.h"
#include "FishLogic.h"
#include "EnvironmentLogic.h"
#include "CameraLogic.h"

class Game : public Application
{
    URHO3D_OBJECT(Game, Application);

private:
    Scene* scene_;
    bool drawDebug_;

public:
    Game(Context* context) :
        Application(context),
        scene_{nullptr},
        drawDebug_{false}
    {
        SubscribeToEvent(E_BEGINFRAME, URHO3D_HANDLER(Game, HandleBeginFrame));

        BarrierLogic::RegisterObject(context);
        FishLogic::RegisterObject(context);
        EnvironmentLogic::RegisterObject(context);
        CameraLogic::RegisterObject(context);

        context->RegisterSubsystem(new Global(context));
    }

    void Setup()
    {
        engineParameters_["ResourcePaths"] = "Data;CoreData;Resources";
        engineParameters_["WindowTitle"] = "Flappy Urho";
        engineParameters_["WindowIcon"] = "icon.png";
    }

    void Start()
    {
        SetRandomSeed(Time::GetSystemTime());

        CreateScene();
        CreateUrho();
        CreateNets();
        CreateUI();

        Camera* camera{scene_->GetChild("Camera")->GetComponent<Camera>()};
        Viewport* viewport{new Viewport(context_, scene_, camera)};
        RENDERER->SetViewport(0, viewport);

        viewport->GetRenderPath()->Append(CACHE->GetResource<XMLFile>("PostProcess/FXAA3.xml"));

        SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(Game, HandleUpdate));
        SubscribeToEvent(E_POSTRENDERUPDATE, URHO3D_HANDLER(Game, HandlePostRenderUpdate));

        SoundSource* musicSource{scene_->GetOrCreateComponent<SoundSource>()};
        musicSource->SetSoundType(SOUND_MUSIC);
        musicSource->SetGain(0.23f);
        Sound* music{CACHE->GetResource<Sound>("Music/Urho - Disciples of Urho_LOOP.ogg")};
        music->SetLooped(true);
        musicSource->Play(music);

//        SubscribeToEvent();
    }

    void HandleBeginFrame(StringHash eventType, VariantMap& eventData)
    {
        if (GLOBAL->gameState_ == GLOBAL->neededGameState_)
            return;

        if (GLOBAL->neededGameState_ == GS_DEAD)
        {
            Node* urhoNode{scene_->GetChild("Urho")};
            SoundSource* soundSource{urhoNode->GetOrCreateComponent<SoundSource>()};
            soundSource->Play(CACHE->GetResource<Sound>("Samples/Hit.ogg"));
        }
        else if (GLOBAL->neededGameState_ == GS_INTRO)
        {
            Node* urhoNode{scene_->GetChild("Urho")};
            urhoNode->SetPosition(Vector3::ZERO);
            urhoNode->SetRotation(URHO_DEFAULT_ROTATION);
            FishLogic* fishLogic{urhoNode->GetComponent<FishLogic>()};
            fishLogic->Reset();

            GLOBAL->SetScore(0);

            PODVector<Node*> barriers{};
            scene_->GetChildrenWithComponent<BarrierLogic>(barriers);
            for (Node* b : barriers)
            {
                Vector3 pos{b->GetPosition()};
                pos.y_ = BAR_RANDOM_Y;

                if (pos.x_ < BAR_OUTSIDE_X)
                    pos.x_ += NUM_BARRIERS * BAR_INTERVAL;

                b->SetPosition(pos);
            }
        }

        GLOBAL->gameState_ = GLOBAL->neededGameState_;

        UpdateUIVisibility();
    }

    void UpdateUIVisibility()
    {
        String tag{};
        if (GLOBAL->gameState_ == GS_GAMEPLAY)     tag = "Gameplay";
        else if (GLOBAL->gameState_ == GS_DEAD)    tag = "Dead";
        else                                       tag = "Intro";

        Vector<SharedPtr<UIElement> > uiElements{UI_ROOT->GetChildren()};
        for (UIElement* e : uiElements)
        {
            e->SetVisible(e->HasTag(tag));
        }
    }

    void CreateUI()
    {
        Font* font{CACHE->GetResource<Font>("Fonts/Ubuntu-BI.ttf")};

        Text* scoreText{UI_ROOT->CreateChild<Text>("Score")};
        scoreText->SetFont(font, 40);
        scoreText->SetTextEffect(TE_STROKE);
        scoreText->SetColor(Color::BLACK);
        scoreText->SetEffectColor(Color::WHITE);
        
        scoreText->SetVisible(false);
        
        scoreText->AddTags("Gameplay;Dead");

        Text* helpText{UI_ROOT->CreateChild<Text>()};
        helpText->SetFont(font, 20);
        helpText->SetTextEffect(TE_SHADOW);
        helpText->SetAlignment(HA_CENTER, VA_CENTER);
        helpText->SetPosition(0, UI_ROOT->GetHeight() / 4);
        helpText->AddTags("Intro;Dead");

        helpText->SetText("Left click to swim");
    }

    void CreateScene()
    {
        scene_ = new Scene(context_);
        scene_->CreateComponent<Octree>();
        scene_->CreateComponent<DebugRenderer>();

        scene_->CreateComponent<PhysicsWorld>();

        Node* cameraNode{scene_->CreateChild("Camera")};
        cameraNode->CreateComponent<Camera>();
        cameraNode->SetPosition(CAMERA_DEFAULT_POS);
        cameraNode->CreateComponent<CameraLogic>();

        Zone* zone{cameraNode->CreateComponent<Zone>()};
        zone->SetBoundingBox(BoundingBox(-100.0f * Vector3::ONE, 100.0f * Vector3::ONE));
        zone->SetFogStart(34.0f);
        zone->SetFogEnd(62.0f);
        zone->SetFogHeight(-19.0f);
        zone->SetHeightFog(true);
        zone->SetFogHeightScale(0.1f);
        zone->SetFogColor(Color(0.1f, 0.3f, 0.3f));

        Node* lightNode{scene_->CreateChild()};
        Light* light{lightNode->CreateComponent<Light>()};
        light->SetLightType(LIGHT_DIRECTIONAL);
        light->SetCastShadows(true);
        light->SetShadowIntensity(0.23f);
        light->SetColor(Color(0.8f, 1.0f, 1.0f));
        lightNode->SetDirection(Vector3(-0.5f, -1.0f, 1.0f));

        Node* envNode{scene_->CreateChild()};
        Skybox* skybox{envNode->CreateComponent<Skybox>()};
        skybox->SetModel(CACHE->GetResource<Model>("Models/Box.mdl"));
        skybox->SetMaterial(CACHE->GetResource<Material>("Materials/Env.xml"));
        skybox->SetZone(zone);
        envNode->CreateComponent<EnvironmentLogic>();
    }

    void CreateUrho()
    {
        Node* urhoNode{scene_->CreateChild("Urho")};
        AnimatedModel* urhoObject{urhoNode->CreateComponent<AnimatedModel>()};
        urhoObject->SetModel(CACHE->GetResource<Model>("Models/Urho.mdl"));
        urhoObject->SetCastShadows(true);
        urhoNode->SetRotation(URHO_DEFAULT_ROTATION);
        urhoNode->CreateComponent<FishLogic>();

        urhoObject->ApplyMaterialList();

        AnimationController* animCtrl{urhoNode->CreateComponent<AnimationController>()};
        animCtrl->PlayExclusive("Models/Swim.ani", 0, true);

        RigidBody* body{urhoNode->CreateComponent<RigidBody>()};
        body->SetMass(1.0f);
        body->SetKinematic(true);

        CollisionShape* shape1{urhoNode->CreateComponent<CollisionShape>()};
        shape1->SetShapeType(SHAPE_CAPSULE);
        shape1->SetSize(Vector3(2.0f, 3.8f, 0.0f));
        shape1->SetPosition(Vector3(0.0f, 0.1f, -0.2f));
        shape1->SetRotation(Quaternion(90.f, 0.0f, 0.0f));
    }

    void CreateNets()
    {
        for (int i{0}; i < NUM_BARRIERS; ++i)
        {
            Node* barrierNode{scene_->CreateChild("Barrier")};
            barrierNode->CreateComponent<BarrierLogic>();

            barrierNode->SetPosition(Vector3(BAR_OUTSIDE_X + i * BAR_INTERVAL, BAR_RANDOM_Y, 0.0f));
            barrierNode->SetRotation(Quaternion(Random(2) ? 180.0f : 0.0f,
                                                Random(2) ? 180.0f + Random(-5.0f, 5.0f) : 0.0f + Random(-5.0f, 5.0f) ,
                                                Random(2) ? 180.0f + Random(-5.0f, 5.0f) : 0.0f + Random(-5.0f, 5.0f) ));

            barrierNode->CreateComponent<RigidBody>();
            CollisionShape* shape{barrierNode->CreateComponent<CollisionShape>()};
            shape->SetShapeType(SHAPE_BOX);
            shape->SetSize(Vector3(1.0f, BAR_GAP, 7.8f));

            Node* netNode{barrierNode->CreateChild("Net")};

            StaticModel* staticModel{netNode->CreateComponent<StaticModel>()};
            staticModel->SetModel(CACHE->GetResource<Model>("Models/Net.mdl"));
            staticModel->SetCastShadows(true);
            staticModel->ApplyMaterialList();

            for (float y : {15.0f, -15.0f}){
                netNode->CreateComponent<RigidBody>();
                CollisionShape* shape{netNode->CreateComponent<CollisionShape>()};
                shape->SetShapeType(SHAPE_BOX);
                shape->SetSize(Vector3(0.23f, 30.0f, 64.0f));
                shape->SetPosition(Vector3(0.0f, y + Sign(y)*(BAR_GAP / 2), 0.0f));
            }
        }
    }

    void HandleUpdate(StringHash eventType, VariantMap& eventData)
    {
        if (INPUT->GetMouseButtonPress(MOUSEB_LEFT))
        {
            if (GLOBAL->gameState_ == GS_INTRO)
                GLOBAL->neededGameState_ = GS_GAMEPLAY;
            else if (GLOBAL->gameState_ == GS_DEAD)
                GLOBAL->neededGameState_ = GS_INTRO;
        }

        if (INPUT->GetKeyPress(KEY_SPACE))
            drawDebug_ = !drawDebug_;

        if (INPUT->GetKeyPress(KEY_ESC))
            engine_->Exit();
    }

    void HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData)
    {
        if (drawDebug_)
        {
            scene_->GetComponent<PhysicsWorld>()->DrawDebugGeometry(true);
        }
    }
};

URHO3D_DEFINE_APPLICATION_MAIN(Game);
