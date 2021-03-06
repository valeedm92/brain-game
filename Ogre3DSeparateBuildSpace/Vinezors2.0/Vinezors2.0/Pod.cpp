//
//  Pod.cpp
//  Testing
//
//  Created by Calvin Phung on 9/3/13.
//
//

#include "Pod.h"

#include <cstdlib>

extern Util::ConfigGlobal globals;

static int podID = 0;
static int glowID = 0;
static int indicatorID = 0;
static int headEffectID = 0;

Pod::Pod()
: parentNode(NULL), entirePod(NULL), stem(NULL), head(NULL), shell(NULL), bPFXNode(NULL), bPFX(NULL), bPFXwidth(0.0f), bPFXcolor(1.0f)
{
}

Pod::Pod(Ogre::SceneNode* parentNode, Vector3 base, Vector3 tip, PodMeshType mtype, PodSignal podSignal, PodColor podColor, PodShape podShape, PodSound podSound, Direction loc, float stemRadius, float headRadius)
: parentNode(parentNode), mtype(mtype), materialName(""), headContentEntity(NULL), headContentEffect(NULL), glowNode(NULL), glowEffect(NULL), indicatorNode(NULL), indicatorEffect(NULL), base(base), tip(tip), podSignal(podSignal), podColor(podColor), podShape(podShape), podSound(podSound), stemRadius(stemRadius), stemLength(base.distance(tip)), headRadius(headRadius), entirePod(NULL), stem(NULL), head(NULL), shell(NULL), moveSpeed(0.0), rotateSpeed(0.0, 0.0, 0.0), loc(loc), podTested(false), podTaken(false), podGood(false), podZapped(false), dest(), bPFXNode(NULL), bPFX(NULL), bPFXwidth(0.0f), bPFXcolor(1.0f)
{
    loadPod();
}

void Pod::loadPod()
{
    switch (mtype)
    {
        case POD_FUEL:
            //loadFuelCell();
            loadCrystal();
            break;
        case POD_FLOWER:
            loadFlower();
            break;
        case POD_HAZARD:
            loadHazard();
            break;
        case POD_POWERUP:
            loadPowerup();
            break;
        default:
            loadBasicShape();
            break;
    }
    ++podID;

}

void Pod::loadBasicShape()
{
    removeFromScene();
    
    entirePod = parentNode->createChildSceneNode("entirePodNode" + Util::toStringInt(podID));
    
    stem = entirePod->createChildSceneNode("stemNode" + Util::toStringInt(podID));
    
    Entity* stemEntity = stem->getCreator()->createEntity("stemEntity" + Util::toStringInt(podID), "cylinderMesh");
    stemEntity->setMaterialName("General/PodStem");
    stem->attachObject(stemEntity);
    
    Vector3 v = tip - base;
    stem->setOrientation(globals.tunnelReferenceUpward.getRotationTo(v));
    
    stem->translate(v / -2);
    
    head = entirePod->createChildSceneNode("headNode" + Util::toStringInt(podID));
    
    switch (podShape)
    {
        case POD_SHAPE_CONE:
            headContentEntity = head->getCreator()->createEntity("headEntity" + Util::toStringInt(podID), "cylinderMesh");
            head->scale(headRadius * 1.5, headRadius * 3, headRadius * 1.5);
            break;
        case POD_SHAPE_SPHERE:
            headContentEntity = head->getCreator()->createEntity("headEntity" + Util::toStringInt(podID), "sphereMesh");
            head->scale(headRadius, headRadius, headRadius);
            break;
        case POD_SHAPE_DIAMOND:
            headContentEntity = head->getCreator()->createEntity("headEntity" + Util::toStringInt(podID), "diamondMesh");
            head->scale(headRadius * 1.5, headRadius * 2, headRadius * 1.5);
            setRotateSpeed(Vector3(globals.podRotateSpeed, 0, 0));
            break;
        case POD_SHAPE_TRIANGLE:
            headContentEntity = head->getCreator()->createEntity("headEntity" + Util::toStringInt(podID), "boxMesh");
            head->scale(headRadius, headRadius, headRadius);
            setRotateSpeed(Vector3(globals.podRotateSpeed, 0, 0));
            break;
        default:
            headContentEntity = head->getCreator()->createEntity("headEntity" + Util::toStringInt(podID), "cylinderMesh");
            head->scale(headRadius * 1.5, headRadius * 3, headRadius * 1.5);
            break;
    }
    materialName = "General/PodUnknown";
    headContentEntity->setMaterialName(materialName);
    head->attachObject(headContentEntity);
    head->setOrientation(globals.tunnelReferenceUpward.getRotationTo(v));
    
    setToGrowth(0.0);
}

void Pod::loadFuelCell()
{
    removeFromScene();
    
	float stemLength = base.distance(tip);
    entirePod = parentNode->createChildSceneNode("entirePodNode" + Util::toStringInt(podID));
    Vector3 v = tip - base;
    
    head = entirePod->createChildSceneNode("headNode" + Util::toStringInt(podID));
    
    materialName = "General/PodUnknown";
    switch (podShape)
    {
        case POD_SHAPE_CONE:
            headContentEntity = head->getCreator()->createEntity("headEntity" + Util::toStringInt(podID), "FuelCell/fuelCylinder.mesh");
            break;
        case POD_SHAPE_SPHERE:
            headContentEntity = head->getCreator()->createEntity("headEntity" + Util::toStringInt(podID), "FuelCell/fuelSphere.mesh");
            break;
        case POD_SHAPE_DIAMOND:
            headContentEntity = head->getCreator()->createEntity("headEntity" + Util::toStringInt(podID), "FuelCell/fuelCube.mesh");;
            break;
        case POD_SHAPE_TRIANGLE:
            headContentEntity = head->getCreator()->createEntity("headEntity" + Util::toStringInt(podID), "FuelCell/fuelTri.mesh");
            break;
        case POD_SHAPE_HOLDOUT:
            generateHoldoutEffect();
            break;
        default:
            if( podSignal == POD_SIGNAL_UNKNOWN ) {
                materialName = "General/PodPurple";
            }
            headContentEntity = head->getCreator()->createEntity("headEntity" + Util::toStringInt(podID), "FuelCell/fuelPenta.mesh");
            headContentEntity->getSubEntity(1)->setMaterialName("General/PodMetal"); // Assign with no specular
            headContentEntity->getSubEntity(0)->setMaterialName(materialName);
            break;
    }
    if (podShape != POD_SHAPE_UNKNOWN && podShape != POD_SHAPE_HOLDOUT)
    {
        headContentEntity->getSubEntity(0)->setMaterialName("General/PodMetal"); // Assign with no specular
        headContentEntity->getSubEntity(1)->setMaterialName(materialName);
    }
    if( podShape != POD_SHAPE_HOLDOUT ) head->attachObject(headContentEntity);
    head->setOrientation(globals.tunnelReferenceUpward.getRotationTo(v));
    head->setPosition(base);
    head->translate(v / 2);
    setRotateSpeed(Vector3(globals.podRotateSpeed, 0, 0));
    
    setToGrowth(0.0);
}


void Pod::loadCrystal()
{
    mtype = POD_CRYSTAL;
    removeFromScene();
    
    float stemLength = base.distance(tip);
    entirePod = parentNode->createChildSceneNode("entirePodNode" + Util::toStringInt(podID));
    Vector3 v = tip - base;
    
    head = entirePod->createChildSceneNode("headNode" + Util::toStringInt(podID));
    
    materialName = "General/PodUnknown";
    switch (podShape)
    {
        case POD_SHAPE_CONE:
            headContentEntity = head->getCreator()->createEntity("headEntity" + Util::toStringInt(podID), "Crystals/roundish.mesh");
            break;
        case POD_SHAPE_SPHERE:
            headContentEntity = head->getCreator()->createEntity("headEntity" + Util::toStringInt(podID), "Crystals/spikyBall.mesh");
            break;
        case POD_SHAPE_DIAMOND:
            headContentEntity = head->getCreator()->createEntity("headEntity" + Util::toStringInt(podID), "Crystals/crystal_long.mesh");
            break;
        case POD_SHAPE_TRIANGLE:
            headContentEntity = head->getCreator()->createEntity("headEntity" + Util::toStringInt(podID), "Crystals/star_coin.mesh");
            break;
        case POD_SHAPE_HOLDOUT:
            generateIndicator();
            break;
        default:
            if( podSignal == POD_SIGNAL_UNKNOWN ) {
                materialName = "General/PodPurple";
            }
            headContentEntity = head->getCreator()->createEntity("headEntity" + Util::toStringInt(podID), "Crystals/gumdrop.mesh");
            //headContentEntity = head->getCreator()->createEntity("headEntity" + Util::toStringInt(podID), "FuelCell/spikyBall.mesh");
            headContentEntity->getSubEntity(0)->setMaterialName(materialName);
            break;
    }
    if (podShape != POD_SHAPE_UNKNOWN && podShape != POD_SHAPE_HOLDOUT)
    {
        headContentEntity->getSubEntity(0)->setMaterialName(materialName);
    }
    if( podShape != POD_SHAPE_HOLDOUT ) head->attachObject(headContentEntity);
    head->setOrientation(globals.tunnelReferenceUpward.getRotationTo(v));
    head->setPosition(base);
    head->translate(v / 2);
    
    direction = (globals.tunnelReferenceForward).randomDeviant(Radian(rand() % 180));
    direction.normalise();
    std::cout << direction.x << " " << direction.y << " " << direction.z << std::endl;
    setRotateSpeed(direction * 2);
    //setRotateSpeed(Vector3(globals.podRotateSpeed, 0, 0));
    
    setToGrowth(0.0);
}

void Pod::loadFlower()
{
    removeFromScene();
    
	float stemLength = base.distance(tip);
    entirePod = parentNode->createChildSceneNode("entirePodNode" + Util::toStringInt(podID));
    Vector3 v = tip - base;
    
    head = entirePod->createChildSceneNode("headNode" + Util::toStringInt(podID));
    headContentEntity = head->getCreator()->createEntity("headEntity" + Util::toStringInt(podID), "Flowers/rose.mesh");
    head->attachObject(headContentEntity);
    head->setOrientation(globals.tunnelReferenceUpward.getRotationTo(v));
    head->setPosition(base);
    head->translate(v / 2);
    setRotateSpeed(Vector3(globals.podRotateSpeed, 0, 0));
    
    setToGrowth(0.0);
}

void Pod::loadHazard()
{
    removeFromScene();
    
	float stemLength = base.distance(tip);
    entirePod = parentNode->createChildSceneNode("entirePodNode" + Util::toStringInt(podID));
    Vector3 v = tip - base;
    
    // Though the variable names are not symmetric,
    // Use the stem to be the other obelisk on the other side
    head = entirePod->createChildSceneNode("headNode" + Util::toStringInt(podID));
    stem = entirePod->createChildSceneNode("stemNode" + Util::toStringInt(podID));
    
    headContentEntity = head->getCreator()->createEntity("headEntity" + Util::toStringInt(podID), "Barriers/obelisk.mesh");
    Entity* stemContentEntity = stem->getCreator()->createEntity("stemEntity" + Util::toStringInt(podID), "Barriers/obelisk.mesh");
    headContentEntity->setMaterialName("General/BarrierMaterial");
    stemContentEntity->setMaterialName("General/BarrierMaterial");

    head->attachObject(headContentEntity);
    head->setOrientation(globals.tunnelReferenceUpward.getRotationTo(v));
    head->setPosition(base);
    head->translate(Vector3(-3.5, 0.0, 0.0), Ogre::Node::TS_LOCAL);
    head->yaw(Degree(180.0));
    head->roll(Degree(5.0));
    
    stem->attachObject(stemContentEntity);
    stem->setOrientation(globals.tunnelReferenceUpward.getRotationTo(v));
    stem->setPosition(base);
    stem->translate(Vector3(3.5, 0.0, 0.0), Ogre::Node::TS_LOCAL);
    stem->roll(Degree(5.0));

    hazardID = podID;
    
    setToGrowth(0.0);
}

void Pod::loadPowerup()
{
    
    removeFromScene();
    
	float stemLength = base.distance(tip);
    entirePod = parentNode->createChildSceneNode("entirePodNode" + Util::toStringInt(podID));
    Vector3 v = tip - base;
    
    head = entirePod->createChildSceneNode("headNode" + Util::toStringInt(podID));
    switch (podColor)
    {
        case POD_COLOR_PINK:
//            headContentEntity = head->getCreator()->createEntity("headEntity" + Util::toStringInt(podID), "Powerups/TractorBeam.mesh");
            headContentEntity = head->getCreator()->createEntity("headEntity" + Util::toStringInt(podID), "sphereMesh");
            headContentEntity->setMaterialName("General/PodRed");
            break;
        case POD_COLOR_GREEN:
            headContentEntity = head->getCreator()->createEntity("headEntity" + Util::toStringInt(podID), "Powerups/slowdown.mesh");
            headContentEntity->getSubEntity(0)->setMaterialName("slowdown/Bubble");
            break;
        case POD_COLOR_BLUE:
            headContentEntity = head->getCreator()->createEntity("headEntity" + Util::toStringInt(podID), "Powerups/shields.mesh");
            headContentEntity->getSubEntity(0)->setMaterialName("General/VineShellActive");
            break;
        default:
            std::cout << "WARNING: Unknown Powerup Model Type!\n";
            break;
    }
    
    head->attachObject(headContentEntity);
    head->setOrientation(globals.tunnelReferenceUpward.getRotationTo(v));
    head->setPosition(base);
    head->translate(v);
    
    setRotateSpeed(Vector3(globals.podRotateSpeed, 0, 0));
    
    setToGrowth(0.0);
}

PodMeshType Pod::getMeshType() const
{
    return mtype;
}

Vector3 Pod::getBase() const
{
    return base;
}

Vector3 Pod::getTip() const
{
    return tip;
}

void Pod::setToGrowth(float t)
{
    if (mtype == POD_BASIC)
    {
        entirePod->setPosition(tip);
        entirePod->translate((base - tip) * (1 - t));
        stem->setScale(Vector3(t * stemRadius, t * stemLength, t * stemRadius));
    }
    else if (mtype == POD_FUEL)
    {
        head->setScale(Vector3(headRadius / 1.5, t * headRadius / 1.5, headRadius / 1.5));
    }
    else if (mtype == POD_FLOWER)
    {
        head->setScale(Vector3(headRadius / 20, t * headRadius / 20, headRadius / 20));
    }
    else if (mtype == POD_HAZARD)
    {
        head->setScale(Vector3(0.9, 0.9 * t, 0.9));
        stem->setScale(Vector3(0.9, 0.9 * t, 0.9));
        if( t >= 1.0f && !bPFXNode) {
            bPFXNode = entirePod->createChildSceneNode("BarrierPFXNode" + Util::toStringInt(hazardID));
            bPFXNode->setPosition(base);
            bPFXNode->setOrientation(globals.tunnelReferenceUpward.getRotationTo(tip - base));
            bPFXNode->translate(Vector3(0,2.5,0), Ogre::Node::TS_LOCAL);
            bPFX = bPFXNode->getCreator()->createParticleSystem("BarrierPFX" + Util::toStringInt(hazardID), "Barrier/PFX");
            bPFXNode->attachObject(bPFX);
        }
    }
    else if (mtype == POD_POWERUP)
    {
        head->setScale(Vector3(t * headRadius / 1.5, t * headRadius / 1.5, t * headRadius / 1.5));
//        head->setScale(Vector3(t * headRadius * 1.5, t * headRadius * 1.5, t * headRadius * 1.5));
    }
    else if (mtype == POD_CRYSTAL)
    {
        head->setScale(Vector3(headRadius, t * headRadius, headRadius));
        
        setRotateSpeed(direction * 2);
        rotateSpeed = 2 + 98 * (1 - t);
        setRotateSpeed(direction * rotateSpeed);
    }
}

PodSignal Pod::getPodSignal() const
{
	return podSignal;
}

PodColor Pod::getPodColor() const
{
	return podColor;
}

PodShape Pod::getPodShape() const
{
	return podShape;
}

PodSound Pod::getPodSound() const
{
	return podSound;
}

SceneNode* Pod::getEntirePod() const
{
    return entirePod;
}

SceneNode* Pod::getStem() const
{
	return stem;
}

SceneNode* Pod::getHead() const
{
	return head;
}

Vector3 Pod::getDest() const
{
	return dest;
}

Vector3 Pod::getPosition() const
{
	return entirePod->getPosition();
}

Direction Pod::getLoc() const
{
    return loc;
}

PodInfo Pod::getPodInfo() const
{
    return PodInfo(podSignal, mtype, podColor, podShape, podSound, loc, podGood, podTrigger, podTaken, podZapped);
}

float Pod::getStemRadius() const
{
    return stemRadius;
}

float Pod::getStemLength() const
{
    return stemLength;
}

float Pod::getHeadRadius() const
{
    return headRadius;
}

SceneNode* Pod::getGlowNode() const
{
    return glowNode;
}

bool Pod::isPodTested() const
{
    return podTested;
}

bool Pod::isPodTaken() const
{
    return podTaken;
}

bool Pod::getPodTrigger() const
{
    return podTrigger;
}

bool Pod::isPodGood() const
{
    return podGood;
}

bool Pod::isPodZapped() const
{
    return podZapped;
}

void Pod::move(Vector3 delta)
{
    entirePod->translate(delta);
}

void Pod::setSkin()
{
    if (mtype == POD_BASIC)
        headContentEntity->setMaterialName(materialName);
    else if (mtype == POD_FUEL && podShape != POD_SHAPE_HOLDOUT)
    {
        if (podShape != POD_SHAPE_UNKNOWN)
            // Based on Maya model, SubEntity1 is the content in the fuel cell
            headContentEntity->getSubEntity(1)->setMaterialName(materialName);
        else
            headContentEntity->getSubEntity(0)->setMaterialName(materialName);
    }
    else if (mtype == POD_POWERUP)
        headContentEntity->setMaterialName(materialName);
    else if (mtype == POD_CRYSTAL && podShape != POD_SHAPE_HOLDOUT)
    {
        headContentEntity->getSubEntity(0)->setMaterialName(materialName);
    }
}

void Pod::takePod()
{
    if (!podTaken)
    {
        podTaken = true;
    
        materialName += "Transparent";
        if (mtype == POD_POWERUP)
            hidePod();
        setSkin();
    }
}

void Pod::zapPod()
{
    podZapped = true;
}

void Pod::hidePod()
{
    entirePod->setVisible(false);
}

void Pod::revealPod()
{
    entirePod->setVisible(true);
}

void Pod::uncloakPod()
{
    if (mtype == POD_HAZARD || mtype == POD_POWERUP)
        return;
    switch (podColor)
    {
        case POD_COLOR_BLUE:
            materialName = "General/PodBlue";
            break;
        case POD_COLOR_GREEN:
            materialName = "General/PodGreen";
            break;
        case POD_COLOR_PINK:
            materialName = "General/PodRed";
            break;
        case POD_COLOR_YELLOW:
            materialName = "General/PodYellow";
            break;
        case POD_COLOR_PURPLE:
            materialName = "General/PodPurple";
            break;
        case POD_COLOR_HOLDOUT:
            materialName = "General/PodWhite";
            break;
        default:
            materialName = "General/PodPurple";
            break;
    }
    if (podTaken || isIndicatorVisible())
        materialName += "Transparent";
    setSkin();
}

void Pod::generateGlow(PodColor color, PodShape shape)
{
    if (!glowNode)
    {
        std::string particleName = "General/GlowPod";
        Ogre::ColourValue particleValue;
        switch (color)
        {
            case POD_COLOR_BLUE:
                particleValue.r = 0.1;
                particleValue.g = 0.3;
                particleValue.b = 1.0;
                break;
            case POD_COLOR_GREEN:
                particleValue.r = 0.0;
                particleValue.g = 1.0;
                particleValue.b = 0.0;
                break;
            case POD_COLOR_PINK:
                particleValue.r = 1.0;
                particleValue.g = 0.0;
                particleValue.b = 0.0;
                break;
            case POD_COLOR_YELLOW:
                particleValue.r = 1.0;
                particleValue.g = 1.0;
                particleValue.b = 0.0;
                break;
            default:
                particleValue.r = 0.5;
                particleValue.g = 0.5;
                particleValue.b = 1.0;
                break;
        }
        
        switch (shape)
        {
            case POD_SHAPE_CONE:
                particleName += "Cylinder";
                break;
            case POD_SHAPE_DIAMOND:
                particleName += "Box";
                break;
            case POD_SHAPE_SPHERE:
                particleName += "Ellipsoid";
                break;
            case POD_SHAPE_TRIANGLE:
                particleName += "Ellipsoid"; // No ogre particle shape for tris
                break;
            case POD_SHAPE_HOLDOUT:
                particleName += "Ellipsoid";
                break;
            default:
                particleName += "Cylinder";
                break;
        }
        
        // Make independent to avoid rotation orientions when translating.
        glowNode = OgreFramework::getSingletonPtr()->m_pSceneMgrMain->getRootSceneNode()->createChildSceneNode("GlowNode" + Util::toStringInt(glowID));
        glowEffect = glowNode->getCreator()->createParticleSystem("GlowEffect" + Util::toStringInt(glowID), particleName);
        ParticleEmitter* glowEmitter = glowEffect->getEmitter(0); // Assuming only one emitter
        
        glowEmitter->setColour(particleValue);
        
        glowNode->attachObject(glowEffect);
        glowNode->setPosition(head->_getDerivedPosition());
        ++glowID;
    }
}

void Pod::generateHoldoutEffect()
{
        if (!headContentEffect)
        {
            std::string headEffectName = "General/HoldOutPod";
            headContentEffect = head->getCreator()->createParticleSystem("HeadEffect" + Util::toStringInt(headEffectID), headEffectName);
            Ogre::ColourValue emitterColor;
            switch (podColor)
            {
                case POD_COLOR_BLUE:
                    emitterColor = Ogre::ColourValue(0.0,0.5,1.0);
                    break;
                case POD_COLOR_GREEN:
                    emitterColor = Ogre::ColourValue(0.0,1.0,0.0);
                    break;
                case POD_COLOR_PINK:
                    emitterColor = Ogre::ColourValue(1.0,0.0,0.0);
                    break;
                case POD_COLOR_YELLOW:
                    emitterColor = Ogre::ColourValue(1.0,1.0,0.0);
                    break;
                case POD_COLOR_PURPLE:
                    emitterColor = Ogre::ColourValue(1.0,0.0,1.0);
                    break;
                case POD_COLOR_HOLDOUT:
                    emitterColor = Ogre::ColourValue(1.0,1.0,1.0);
                    break;
                default:
                    emitterColor = Ogre::ColourValue(1.0,1.0,1.0);
                    break;
            }
            headContentEffect->getEmitter(0)->setColour(emitterColor);
            head->attachObject(headContentEffect);
            ++headEffectID;
        }
}

void Pod::generateIndicator()
{
    if (!indicatorNode)
    {
        indicatorNode = head->createChildSceneNode("IndicatorNode" + Util::toStringInt(indicatorID));
        
        std::string indicatorName = "General/GoodPodIndicator";
        indicatorEffect = indicatorNode->getCreator()->createParticleSystem("IndicatorEffect" + Util::toStringInt(indicatorID), indicatorName);
        
        indicatorNode->attachObject(indicatorEffect);
        ++indicatorID;
    }
}

void Pod::setDest(Vector3 value)
{
    dest = value;
}

void Pod::setMoveSpeed(float value)
{
    moveSpeed = value;
}

void Pod::setRotateSpeed(Vector3 value)
{
    rotateSpeed = value;
}

void Pod::setPodTested(bool value)
{
    podTested = value;
}

void Pod::setPodGood(bool value)
{
    podGood = value;
}

void Pod::setPodTrigger(bool value)
{
    podTrigger = value;
}

void Pod::setVisibleIndicator(bool value)
{
    if (indicatorNode)
    {
        indicatorNode->setVisible(value);
        uncloakPod();
    }
}

bool Pod::isIndicatorVisible() const
{
    return indicatorNode && indicatorNode->getAttachedObject(0)->isVisible();
}

void Pod::removeGlow()
{
    if (glowNode)
    {
        glowNode->getCreator()->destroyParticleSystem(glowEffect);
        glowNode->removeAndDestroyAllChildren();
        glowNode->getCreator()->destroySceneNode(glowNode);
        glowNode = NULL;
        glowEffect = NULL;
    }
}

void Pod::removeIndicator()
{
    if (indicatorNode)
    {
        indicatorNode->getCreator()->destroyParticleSystem(indicatorEffect);
        indicatorNode->removeAndDestroyAllChildren();
        indicatorNode->getCreator()->destroySceneNode(indicatorNode);
        indicatorNode = NULL;
        indicatorEffect = NULL;
    }
}

void Pod::removeFromScene()
{
    if(bPFXNode) {
        bPFXNode->getCreator()->destroyParticleSystem(bPFX);
        head->getCreator()->destroySceneNode(bPFXNode);
        bPFXNode = NULL;
        bPFX = NULL;
    }
    
    removeGlow();
    if (stem)
    {
        stem->getCreator()->destroyMovableObject(stem->getAttachedObject(0)); // Assuming only one entity
        stem = NULL;
    }
    removeIndicator();
    if (head && podShape != POD_SHAPE_HOLDOUT)
    {
        head->getCreator()->destroyEntity(headContentEntity);
        if (headContentEffect)
        {
            head->getCreator()->destroyParticleSystem(headContentEffect);
            headContentEffect = NULL;
        }
        head = NULL;
        headContentEntity = NULL;
    }
    if (shell)
    {
        shell->getCreator()->destroyMovableObject(shell->getAttachedObject(0));
        shell = NULL;
    }
    if (entirePod)
    {
        entirePod->removeAndDestroyAllChildren();
        entirePod->getCreator()->destroySceneNode(entirePod);
        entirePod = NULL;
    }
}

void Pod::update(float elapsed)
{
    if (glowNode)
    {
        glowNode->setOrientation(head->_getDerivedOrientation());
    }
    if (indicatorNode)
    {
        indicatorNode->roll(Degree(elapsed));;
    }
    if (moveSpeed != 0.0)
    {
        Vector3 dist = dest - entirePod->getPosition();
    
        Vector3 norm = dist;
        norm.normalise();
        Vector3 delta = dist * moveSpeed * elapsed;
        
        //	if (delta.length() > dist.length())
        if (delta.x * delta.x + delta.y * delta.y + delta.z * delta.z >
            dist.x * dist.x + dist.y * dist.y + dist.z * dist.z)
            delta = dist;
    
        move(delta);
    }
    if (rotateSpeed != Vector3::ZERO)
    {
        head->yaw(Degree(rotateSpeed.x));
        head->pitch(Degree(rotateSpeed.y));
        head->roll(Degree(rotateSpeed.z));
    }
    
    if( bPFXNode ) {
        // Animate growth of barrier in the x-axis
        bPFX->getEmitter(0)->setParameter("width",Util::toStringFloat(bPFXwidth));
        
         // Animate to orange particle barrier
        bPFX->getEmitter(0)->setColour(ColourValue(0.5f,bPFXcolor / 2,bPFXcolor / 8));
        if( bPFXwidth+0.75f < 4.0f ) bPFXwidth += 0.75f;
        else bPFXwidth = 4.5f;
        if( bPFXcolor-0.05f > 0.5f ) bPFXcolor -= 0.05f;
        else bPFXcolor = 0.5f;
    }
}

Pod::~Pod()
{

}
