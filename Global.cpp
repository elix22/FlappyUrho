#include "Global.h"

Global::Global(Context* context) :
    Object(context),
    gameState_{GS_INTRO},
    neededGameState_{GS_INTRO},
    sinceLastReset_{0.0f},
    score_{0},
    scoreTextDirty_{true}
{
    SubscribeToEvent(E_POSTUPDATE, URHO3D_HANDLER(Global, HandlePostUpdate));
}

void Global::SetScore(int score)
{
    if (score_ == score)
        return;

    score_ = score;
    scoreTextDirty_ = true;
}

void Global::HandlePostUpdate(StringHash eventType, VariantMap& eventData)
{ (void)eventType; (void)eventData;

    if (scoreTextDirty_)
    {
        Text* scoreText{static_cast<Text*>(UI_ROOT->GetChild("Score", false))};

        String str{"Score: "};

        scoreText->SetText(str + String(score_));
        scoreTextDirty_ = false;
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
}
