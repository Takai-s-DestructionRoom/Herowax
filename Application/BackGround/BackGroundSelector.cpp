#include "BackGroundSelector.h"
#include <BGSimple.h>

void BackGroundSelector::Select(std::string name)
{
    if (name == "Simple") {
        mBG = std::make_unique<BGSimple>();
    }
    else {
        mBG = std::make_unique<IBackGround>();
    }
}

void BackGroundSelector::Update()
{
    mBG->Update();
}

void BackGroundSelector::Draw()
{
    mBG->Draw();
}
