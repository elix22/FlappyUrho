#include "Global.h"

Global::Global(Context* context) :
    Object(context),
    score_{0},
    scoreTextDirty_{true},
    gameState_{GS_INTRO},
    neededGameState_{GS_INTRO}
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
{
    if (scoreTextDirty_)
    {
        auto scoreText{static_cast<Text*>(UI_ROOT->GetChild("Score", false))};

        String str{"Score: "};

        scoreText->SetText(str + String(score_));
        scoreTextDirty_ = false;
    }
}
