#include "Global.h"
#include "Barrier.h"
#include "Crown.h"
#include "Weed.h"
#include "Fish.h"
#include "Environment.h"
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

        Barrier::RegisterObject(context);
        Crown::RegisterObject(context);
        Weed::RegisterObject(context);
        Fish::RegisterObject(context);
        Environment::RegisterObject(context);
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
        CreateUI();

        Camera* camera{scene_->GetChild("Camera")->GetComponent<Camera>()};
        Viewport* viewport{new Viewport(context_, scene_, camera)};
        RENDERER->SetViewport(0, viewport);

        RenderPath* effectRenderPath{viewport->GetRenderPath()};
        effectRenderPath->Append(CACHE->GetResource<XMLFile>("PostProcess/FXAA3.xml"));
        effectRenderPath->Append(CACHE->GetResource<XMLFile>("PostProcess/BloomHDR.xml"));
        effectRenderPath->SetShaderParameter("BloomHDRThreshold", 0.8f);
        effectRenderPath->SetShaderParameter("BloomHDRMix", Vector2(0.7f, 0.8f));
        effectRenderPath->SetEnabled("BloomHDR", true);

        SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(Game, HandleUpdate));
        SubscribeToEvent(E_POSTRENDERUPDATE, URHO3D_HANDLER(Game, HandlePostRenderUpdate));

        SoundSource* musicSource{scene_->GetOrCreateComponent<SoundSource>()};
        musicSource->SetSoundType(SOUND_MUSIC);
        Sound* music{CACHE->GetResource<Sound>("Music/Urho - Disciples of Urho_LOOP.ogg")};
        music->SetLooped(true);
        musicSource->Play(music);
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
        zone->SetFogColor(Color(0.05f, 0.23f, 0.23f));

        Node* lightNode{scene_->CreateChild()};
        Light* light{lightNode->CreateComponent<Light>()};
        light->SetLightType(LIGHT_DIRECTIONAL);
        light->SetCastShadows(true);
        light->SetShadowIntensity(0.23f);
        light->SetBrightness(1.23f);
        light->SetColor(Color(0.8f, 1.0f, 1.0f));
        lightNode->SetDirection(Vector3(-0.5f, -1.0f, 1.0f));

        Node* envNode{scene_->CreateChild()};
        Skybox* skybox{envNode->CreateComponent<Skybox>()};
        skybox->SetModel(CACHE->GetResource<Model>("Models/Box.mdl"));
        skybox->SetMaterial(CACHE->GetResource<Material>("Materials/Env.xml"));
        skybox->SetZone(zone);
        envNode->CreateComponent<Environment>();

        CreateUrho();
        CreateNets();
        CreateWeeds();
        CreateCrown();
    }

    void CreateUrho()
    {
        Node* urhoNode{scene_->CreateChild("Urho")};
        urhoNode->CreateComponent<Fish>();
    }

    void CreateNets()
    {
        for (int i{0}; i < NUM_BARRIERS; ++i)
        {
            Node* barrierNode{scene_->CreateChild("Barrier")};
            barrierNode->CreateComponent<Barrier>();
            barrierNode->SetPosition(Vector3(BAR_OUTSIDE_X + i * BAR_INTERVAL, BAR_RANDOM_Y, 0.0f));
        }
    }
    void CreateWeeds()
    {
        for (int r{0}; r < 3; ++r){
            for (int i{0}; i < NUM_WEEDS; ++i)
            {
                Node* weedNode{scene_->CreateChild("Weed")};
                weedNode->CreateComponent<Weed>();
                weedNode->SetPosition(Vector3(i * BAR_INTERVAL * Random(0.1f, 0.23f) - 23.0f, WEED_RANDOM_Y, Random(-27.0f + r * 34.0f, -13.0f + r * 42.0f)));
                weedNode->SetRotation(Quaternion(0.0f, Random(360.0f), 0.0f));
                weedNode->SetScale(Vector3(Random(0.5f, 1.23f), Random(0.8f, 2.3f), Random(0.5f, 1.23f)));
            }
        }
    }

    void CreateCrown()
    {
        Node* crownNode{ scene_->CreateChild("Crown") };
        crownNode->Translate(Vector3::RIGHT * 19.0f, TS_WORLD);
        crownNode->CreateComponent<Crown>();
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

    void HandleBeginFrame(StringHash eventType, VariantMap& eventData)
    { (void)eventType; (void)eventData;

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
            Fish* fishLogic{urhoNode->GetComponent<Fish>()};
            fishLogic->Reset();

            GLOBAL->SetScore(0);
            GLOBAL->sinceLastReset_ = 0.0f;

            PODVector<Node*> barriers{};
            scene_->GetChildrenWithComponent<Barrier>(barriers);
            for (Node* b : barriers)
            {
                Vector3 pos{b->GetPosition()};
                pos.y_ = BAR_RANDOM_Y;

                if (pos.x_ < BAR_OUTSIDE_X)
                    pos.x_ += NUM_BARRIERS * BAR_INTERVAL;

                b->SetPosition(pos);
            }

            PODVector<Node*> weeds{};
            scene_->GetChildrenWithComponent<Weed>(weeds);
            for (Node* w : weeds)
            {
                w->Remove();
            }
            CreateWeeds();
        }

        GLOBAL->gameState_ = GLOBAL->neededGameState_;

        UpdateUIVisibility();
    }

    void UpdateUIVisibility()
    {
        String tag{};
        if (GLOBAL->gameState_ == GS_PLAY)      tag = "Gameplay";
        else if (GLOBAL->gameState_ == GS_DEAD) tag = "Dead";
        else                                    tag = "Intro";

        Vector<SharedPtr<UIElement> > uiElements{UI_ROOT->GetChildren()};
        for (UIElement* e : uiElements)
        {
            e->SetVisible(e->HasTag(tag));
        }
    }

    void HandleUpdate(StringHash eventType, VariantMap& eventData)
    { (void)eventType;

        if (GLOBAL->gameState_ == GS_PLAY){
            GLOBAL->sinceLastReset_ += eventData[Update::P_TIMESTEP].GetFloat();
        }
        scene_->SetTimeScale(1.0f + Clamp(GLOBAL->sinceLastReset_ * 0.0023f, 0.0f, 1.0f));
        if (INPUT->GetMouseButtonPress(MOUSEB_LEFT))
        {
            if (GLOBAL->gameState_ == GS_INTRO)
                GLOBAL->neededGameState_ = GS_PLAY;
            else if (GLOBAL->gameState_ == GS_DEAD)
                GLOBAL->neededGameState_ = GS_INTRO;
        }

        if (INPUT->GetKeyPress(KEY_9)){
            Image screenshot{GetContext()};
            GRAPHICS->TakeScreenShot(screenshot);
            //Here we save in the Data folder with date and time appended
            String fileName{GetSubsystem<FileSystem>()->GetProgramDir() + "Screenshots/Screenshot_" +
                    Time::GetTimeStamp().Replaced(':', '_').Replaced('.', '_').Replaced(' ', '_')+".png"};
            Log::Write(1, fileName);
            screenshot.SavePNG(fileName);
        }
        if (INPUT->GetKeyPress(KEY_M)){
            SoundSource* musicSource{ scene_->GetComponent<SoundSource>() };
            musicSource->SetEnabled(!musicSource->IsEnabled());
        }

        if (INPUT->GetKeyPress(KEY_ESCAPE))
            engine_->Exit();

//        if (INPUT->GetKeyPress(KEY_SPACE))
//            drawDebug_ = !drawDebug_;
    }

    void HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData)
    { (void)eventType; (void)eventData;

        if (drawDebug_)
        {
            scene_->GetComponent<PhysicsWorld>()->DrawDebugGeometry(true);
        }
    }
};

URHO3D_DEFINE_APPLICATION_MAIN(Game);
