#include "Global.h"
#include "BarrierLogic.h"
#include "UfoLogic.h"
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
        scene_(nullptr),
        drawDebug_(false)
    {
        SubscribeToEvent(E_BEGINFRAME, URHO3D_HANDLER(Game, HandleBeginFrame));

        BarrierLogic::RegisterObject(context);
        UfoLogic::RegisterObject(context);
        EnvironmentLogic::RegisterObject(context);
        CameraLogic::RegisterObject(context);

        context->RegisterSubsystem(new Global(context));
    }

    void Setup()
    {
        engineParameters_["ResourcePaths"] = "Data;CoreData;Resources";
    }

    void Start()
    {
        SetRandomSeed(Time::GetSystemTime());

        CreateScene();
        CreateUfo();
        CreateBarriers();
        CreateUI();

        auto camera = scene_->GetChild("Camera")->GetComponent<Camera>();
        auto viewport = new Viewport(context_, scene_, camera);
        RENDERER->SetViewport(0, viewport);

        viewport->GetRenderPath()->Append(CACHE->GetResource<XMLFile>("PostProcess/FXAA3.xml"));

        SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(Game, HandleUpdate));
        SubscribeToEvent(E_POSTRENDERUPDATE, URHO3D_HANDLER(Game, HandlePostRenderUpdate));
    }

    void HandleBeginFrame(StringHash eventType, VariantMap& eventData)
    {
        if (GLOBAL->gameState_ == GLOBAL->neededGameState_)
            return;

        if (GLOBAL->neededGameState_ == GS_DEAD)
        {
            auto ufoNode = scene_->GetChild("Ufo");
            auto soundSource = ufoNode->GetOrCreateComponent<SoundSource>();
            soundSource->Play(CACHE->GetResource<Sound>("Sounds/Explosion.ogg"));
        }
        else if (GLOBAL->neededGameState_ == GS_INTRO)
        {
            auto ufoNode = scene_->GetChild("Ufo");
            ufoNode->SetPosition(Vector3::ZERO);
            ufoNode->SetRotation(UFO_DEFAULT_ROTATION);
            auto ufoLogic = ufoNode->GetComponent<UfoLogic>();
            ufoLogic->Reset();

            GLOBAL->SetScore(0);

            PODVector<Node*> barriers;
            scene_->GetChildrenWithComponent<BarrierLogic>(barriers);
            for (unsigned i = 0; i < barriers.Size(); i++)
            {
                auto pos = barriers[i]->GetPosition();
                pos.y_ = BAR_RANDOM_Y;

                if (pos.x_ < BAR_OUTSIDE_X)
                    pos.x_ += NUM_BARRIERS * BAR_INTERVAL;

                barriers[i]->SetPosition(pos);
            }
        }

        GLOBAL->gameState_ = GLOBAL->neededGameState_;

        UpdateUIVisibility();
    }

    void UpdateUIVisibility()
    {
        String tag;
        if (GLOBAL->gameState_ == GS_GAMEPLAY)     tag = "Gameplay";
        else if (GLOBAL->gameState_ == GS_DEAD)    tag = "Dead";
        else                                       tag = "Intro";

        auto uiElements = UI_ROOT->GetChildren();
        for (auto i = uiElements.Begin(); i != uiElements.End(); i++)
        {
            auto element = *i;
            element->SetVisible(element->HasTag(tag));
        }
    }

    void CreateUI()
    {
        auto font = CACHE->GetResource<Font>("Fonts/Ubuntu-BI.ttf");

        auto scoreText = UI_ROOT->CreateChild<Text>("Score");
        scoreText->SetFont(font, 40);
        scoreText->SetTextEffect(TE_STROKE);
        scoreText->SetColor(Color::BLACK);
        scoreText->SetEffectColor(Color::WHITE);
        
        scoreText->SetVisible(false);
        
        scoreText->AddTags("Gameplay;Dead");

        auto helpText = UI_ROOT->CreateChild<Text>();
        helpText->SetFont(font, 20);
        helpText->SetTextEffect(TE_SHADOW);
        helpText->SetAlignment(HA_CENTER, VA_CENTER);
        helpText->SetPosition(0, UI_ROOT->GetHeight() / 4);
        helpText->AddTags("Intro;Dead");

        helpText->SetText(L"Жмите левую кнопку мыши");
    }

    void CreateScene()
    {
        scene_ = new Scene(context_);
        scene_->CreateComponent<Octree>();
        scene_->CreateComponent<DebugRenderer>();

        scene_->CreateComponent<PhysicsWorld>();

        auto cameraNode = scene_->CreateChild("Camera");
        auto camera = cameraNode->CreateComponent<Camera>();
        cameraNode->SetPosition(CAMERA_DEFAULT_POS);
        cameraNode->CreateComponent<CameraLogic>();

        auto lightNode = scene_->CreateChild();
        auto light = lightNode->CreateComponent<Light>();
        light->SetLightType(LIGHT_DIRECTIONAL);
        light->SetCastShadows(true);
        lightNode->SetDirection(Vector3(-0.5f, -1.0f, 1.0f));

        auto envNode = scene_->CreateChild();
        auto skybox = envNode->CreateComponent<Skybox>();
        skybox->SetModel(CACHE->GetResource<Model>("Models/Box.mdl"));
        skybox->SetMaterial(CACHE->GetResource<Material>("Materials/Env.xml"));
        envNode->CreateComponent<EnvironmentLogic>();
    }

    void CreateUfo()
    {
        auto ufoNode = scene_->CreateChild("Ufo");
        auto ufoObject = ufoNode->CreateComponent<AnimatedModel>();
        ufoObject->SetModel(CACHE->GetResource<Model>("Models/Ufo.mdl"));
        ufoObject->SetCastShadows(true);
        ufoNode->SetRotation(UFO_DEFAULT_ROTATION);
        ufoNode->CreateComponent<UfoLogic>();

        ufoObject->ApplyMaterialList();

        auto animCtrl = ufoNode->CreateComponent<AnimationController>();
        animCtrl->PlayExclusive("Models/Fly.ani", 0, true);

        auto body = ufoNode->CreateComponent<RigidBody>();
        body->SetMass(1.0f);
        body->SetKinematic(true);

        auto shape1 = ufoNode->CreateComponent<CollisionShape>();
        shape1->SetShapeType(SHAPE_CAPSULE);
        shape1->SetSize(Vector3(1.9f, 6.4f, 0.0f));
        shape1->SetPosition(Vector3(0.0f, -0.3f, 0.0f));
        shape1->SetRotation(Quaternion(90.f, 0.0f, 0.0f));

        auto shape2 = ufoNode->CreateComponent<CollisionShape>();
        shape2->SetShapeType(SHAPE_CAPSULE);
        shape2->SetSize(Vector3(1.6f, 3.3f, 0.0f));
        shape2->SetPosition(Vector3(0.0f, 0.8f, 0.0f));
        shape2->SetRotation(Quaternion(90.f, 0.0f, 0.0f));

        auto lightNode = ufoNode->CreateChild();
        lightNode->SetPosition(Vector3(0.0f, -3.0f, 0.0f));
        auto light = lightNode->CreateComponent<Light>();
        light->SetColor(Color(0.0f, 1.0f, 1.0f));
        light->SetSpecularIntensity(0.0f);
        light->SetRange(5.0f);
    }

    void CreateBarriers()
    {
        for (int i = 0; i < NUM_BARRIERS; i++)
        {
            auto barrierNode = scene_->CreateChild("Barrier");
            barrierNode->CreateComponent<BarrierLogic>();

            barrierNode->SetPosition(Vector3(BAR_OUTSIDE_X + i * BAR_INTERVAL, BAR_RANDOM_Y, 0.0f));

            barrierNode->CreateComponent<RigidBody>();
            auto shape = barrierNode->CreateComponent<CollisionShape>();
            shape->SetShapeType(SHAPE_BOX);
            shape->SetSize(Vector3(7.8f, BAR_GAP, 7.8f));

            CreatePipe(barrierNode, true);
            CreatePipe(barrierNode, false);
        }
    }

    Node* CreatePipe(Node* barrierNode, bool top)
    {
        auto pipeNode = barrierNode->CreateChild("Pipe");
        
        auto staticModel = pipeNode->CreateComponent<StaticModel>();
        staticModel->SetModel(CACHE->GetResource<Model>("Models/Pipe.mdl"));
        staticModel->SetCastShadows(true);
        staticModel->ApplyMaterialList();

        auto body = pipeNode->CreateComponent<RigidBody>();
        auto shape = pipeNode->CreateComponent<CollisionShape>();
        shape->SetShapeType(SHAPE_BOX);
        shape->SetSize(Vector3(7.8f, 30.0f, 7.8f));
        shape->SetPosition(Vector3(0.0f, -15.0f, 0.0f));

        if (top)
        {
            pipeNode->SetPosition(Vector3(0.0f, BAR_GAP / 2, 0.0f));
            pipeNode->SetRotation(Quaternion(180.0f, 0.0f, 0.0f));
        }
        else
        {
            pipeNode->SetPosition(Vector3(0.0f, -BAR_GAP / 2, 0.0f));
        }

        return pipeNode;
    }

    void HandleUpdate(StringHash eventType, VariantMap& eventData)
    {
        using namespace Update;
        float timeStep = eventData[P_TIMESTEP].GetFloat();

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
