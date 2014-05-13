//
//  Hud.cpp
//  Vinezors2.0
//
//  Created by Calvin Phung on 11/24/13.
//
//

#include "Hud.h"
#include "Tunnel.h"
#include "Player.h"

extern Util::ConfigGlobal globals;

void HudButton::setActive(bool value)
{
    active = value;
}

void HudButton::hide()
{
    setActive(false);
    if (backgroundRef)
        backgroundRef->hide();
    if (textRef)
        textRef->hide();
}

void HudButton::show()
{
    setActive(true);
    if (backgroundRef)
        backgroundRef->show();
    if (textRef)
        textRef->show();
}

// Assigns pointer references to the button and initializes dimensions
void HudButton::setButton(std::string name, Overlay* olay, GuiMetricsMode metricMode, Vector2 pos, Vector2 dimension, PanelOverlayElement* bgPtr, TextAreaOverlayElement* txtPtr)
{
    this->name = name;
    overlay = olay;
    p = pos;
    dim = dimension;
    metric = metricMode;
    backgroundRef = bgPtr;
    if (backgroundRef)
    {
        backgroundRef->setMetricsMode(metric);
        backgroundRef->setPosition(pos.x, pos.y);
        backgroundRef->setDimensions(dim.x, dim.y);
    }
    textRef = txtPtr;
    if (textRef)
    {
        textRef->setMetricsMode(metric);
        textRef->setAlignment(TextAreaOverlayElement::Center);
        textRef->setCharHeight(1.0f / 40.0f);
        textRef->setPosition(pos.x + dim.x / 2, pos.y + dim.y / 2);
        textRef->setFontName("Arial");
        textRef->setColour(ColourValue::ColourValue(1.0, 1.0, 0.0));
    }
}

// Determines whether the parameter is inside the button
bool HudButton::isInside(Vector2 target) const
{
    Vector2 check = p;
    if (metric == GMM_PIXELS)
    {
        check.x /= globals.screenWidth;
        check.y /= globals.screenHeight;
    }
    return (target.x >= check.x && target.x <= check.x + dim.x &&
            target.y >= check.y && target.y <= check.y + dim.y);
}

void HudSlider::setSlider(std::string name, Overlay* olay, Vector2 pos1, Vector2 dimension1, Vector2 dimension2, int min, int max, int slots, PanelOverlayElement* range, PanelOverlayElement* ball)
{
    this->name = name;
    overlay = olay;
    p1 = pos1;
    p2 = Vector2::ZERO;
    p2cache = p2;
    dim1 = dimension1;
    dim2 = dimension2;
    this->min = min;
    this->max = max;
    this->slots = slots;
    metric = GMM_RELATIVE;
    rangeRef = range;
    if (rangeRef)
    {
        rangeRef->setMetricsMode(metric);
        rangeRef->setPosition(pos1.x, pos1.y);
        rangeRef->setDimensions(dim1.x, dim1.y);
    }
    ballRef = ball;
    if (ballRef)
    {
        ballRef->setMetricsMode(metric);
        ballRef->setPosition(0, 0);
        ballRef->setDimensions(dim2.x, dim2.y);
    }
}

// Determines whether the parameter is inside the button
bool HudSlider::isInsideRange(Vector2 target) const
{
    Vector2 check = p1;
    if (metric == GMM_PIXELS)
    {
        check.x /= globals.screenWidth;
        check.y /= globals.screenHeight;
    }
    return (target.x >= check.x && target.x <= check.x + dim1.x + dim2.x / 2 &&
            target.y >= check.y && target.y <= check.y + dim1.y);
}

// Determines whether the parameter is inside the button
bool HudSlider::isInsideBall(Vector2 target) const
{
    Vector2 check = p1 + p2;
    if (metric == GMM_PIXELS)
    {
        check.x /= globals.screenWidth;
        check.y /= globals.screenHeight;
    }
    return (target.x >= check.x && target.x <= check.x + dim2.x &&
            target.y >= check.y && target.y <= check.y + dim2.y);
}

void HudSlider::adjust()
{
    float rheight;
    float rwidth;
    float sheight;
    float swidth;
    if (globals.screenWidth > globals.screenHeight)
    {
        rheight = 0.10;
        rwidth = rheight * globals.screenHeight / globals.screenWidth;
        sheight = 0.10;
        swidth = 0.50;
    }
    else
    {
        rwidth = 0.10;
        rheight = rwidth * globals.screenWidth / globals.screenHeight;
        swidth = 0.50;
        sheight = 0.10 * globals.screenWidth / globals.screenHeight;
    }
    dim1 = Vector2(swidth, sheight);
    dim2 = Vector2(rwidth, rheight);
    rangeRef->setDimensions(dim1.x, dim2.y);
    ballRef->setDimensions(dim2.x, dim2.y);
    
}

float HudSlider::getRangeWidth() const
{
    return dim1.x - dim2.x;
}

void HudSlider::setBallPosition(Vector2 value)
{
    p2 = Vector2(Util::clamp(value.x, 0.0, getRangeWidth()), value.y);
    ballRef->setPosition(p2.x, p2.y);
}

// Set the ball position at the specified slot
void HudSlider::setBallPosition(int slot)
{
    Vector2 pos = Vector2((slot - min) * getRangeWidth() / slots, p2.y);
    setBallPosition(pos);
}

Vector2 HudSlider::getBallPosition() const
{
    return p2;
}

int HudSlider::getIndex() const
{
    //float segmentLength = getRangeWidth() / slots;
    //return p2.x / segmentLength + min;
    //std::cout << p2.x / getRangeWidth() * slots + min << " " << slots << " " << p2.x << " " << getRangeWidth() << " " << min << std::endl;
    
    return p2.x / getRangeWidth() * slots + min;
}

Hud::Hud()
: overlays(), buttons()
{
}

Hud::~Hud()
{
    // Deallocation happens in derived classes
}

void Hud::setOverlay(int index, bool visible)
{
    if (index < 0 || index >= overlays.size()) return;
    if (visible)
        overlays[index]->show();
    else
        overlays[index]->hide();
}

std::string Hud::queryButtons(Vector2 target) const
{
    Vector2 comp = globals.convertToPercentScreen(target);
    for (int i = 0; i < buttons.size(); ++i)
    {
        if ((buttons[i].overlay && buttons[i].overlay->isVisible()) && buttons[i].isInside(comp) && buttons[i].active)
        {
            return buttons[i].name;
        }
    }
    return "";
}

void Hud::hideOverlays()
{
    for (int i = 0; i < overlays.size(); ++i)
        overlays[i]->hide();
}

void Hud::showOverlays()
{
    for (int i = 0; i < overlays.size(); ++i)
        overlays[i]->show();
}

