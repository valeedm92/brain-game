//
//  TunnelSlice.cpp
//  Testing
//
//  Created by Calvin Phung on 9/9/13.
//
//

#include "TunnelSlice.h"

#include <cstdlib>

using namespace std;

extern Util::ConfigGlobal globals;

static int wallID = 0;
static int intermediateMeshID = 0;

TunnelSlice::TunnelSlice()
: parentNode(NULL), tunnelSliceID(0), center(), rot(), dir(NO_DIRECTION), dirAngle(0), width(0), depth(0), type(NORMAL), materialNames(), sliceNode(NULL), entireWall(NULL),
topLeftWall(NULL), topWall(NULL), topRightWall(NULL), rightWall(NULL), bottomRightWall(NULL), bottomWall(NULL), bottomLeftWall(NULL), leftWall(NULL), entireIntermediate(NULL), topLeftIntermediate(NULL), topIntermediate(NULL), topRightIntermediate(NULL), rightIntermediate(NULL), bottomRightIntermediate(NULL), bottomIntermediate(NULL), bottomLeftIntermediate(NULL), leftIntermediate(NULL),
pods(), growthT(0), sidesUsed(), podHistory(false), infoStored(false)
{
    for (int i = 0; i < NUM_DIRECTIONS; ++i)
        sidesUsed[i] = false;
}

TunnelSlice::TunnelSlice(Ogre::SceneNode* parentNode, int nid, SectionInfo info, Vector3 start, float width, float depth, const std::string & material)
: parentNode(parentNode), tunnelSliceID(nid), center(start), rot(rot), dir(info.tunnelDir), dirAngle(info.tunnelDirAngle), width(width), depth(depth), type(info.tunnelType), materialNames(), sliceNode(NULL), entireWall(NULL),
topLeftWall(NULL), topWall(NULL), topRightWall(NULL), rightWall(NULL), bottomRightWall(NULL), bottomWall(NULL), bottomLeftWall(NULL), leftWall(NULL), entireIntermediate(NULL),topLeftIntermediate(NULL), topIntermediate(NULL), topRightIntermediate(NULL), rightIntermediate(NULL), bottomRightIntermediate(NULL), bottomIntermediate(NULL), bottomLeftIntermediate(NULL), leftIntermediate(NULL), pods(), growthT(0), sidesUsed(), podHistory(false), infoStored(false), makeDecreasingTransition(false)
{
    
    Vector3 forward = info.tunnelRot * globals.tunnelReferenceForward;
    Vector3 end = start + forward * (depth + globals.tunnelSegmentBuffer);
    center = (start + end) / 2;
    rot = info.tunnelRot;
    
    materialNames.push_back(material);
    for (int i = 0; i < NUM_DIRECTIONS; ++i)
        sidesUsed[i] = info.sidesUsed[i];
    
    initWalls();
}

TunnelSlice::TunnelSlice(Ogre::SceneNode* parentNode, int nid, SectionInfo info, Vector3 start, float width, float depth, const std::vector<std::string> & materials)
: parentNode(parentNode), tunnelSliceID(nid), center(start), rot(rot), dir(info.tunnelDir), dirAngle(info.tunnelDirAngle), width(width), depth(depth), type(info.tunnelType), materialNames(materials), sliceNode(NULL), entireWall(NULL),
topLeftWall(NULL), topWall(NULL), topRightWall(NULL), rightWall(NULL), bottomRightWall(NULL), bottomWall(NULL), bottomLeftWall(NULL), leftWall(NULL), entireIntermediate(NULL),topLeftIntermediate(NULL), topIntermediate(NULL), topRightIntermediate(NULL), rightIntermediate(NULL), bottomRightIntermediate(NULL), bottomIntermediate(NULL), bottomLeftIntermediate(NULL), leftIntermediate(NULL), pods(), growthT(0), sidesUsed(), podHistory(false), infoStored(false), makeDecreasingTransition(false)
{
    
    Vector3 forward = info.tunnelRot * globals.tunnelReferenceForward;
    Vector3 end = start + forward * (depth + globals.tunnelSegmentBuffer);
    center = (start + end) / 2;
    rot = info.tunnelRot;
    
    for (int i = 0; i < NUM_DIRECTIONS; ++i)
        sidesUsed[i] = info.sidesUsed[i];
    
    initWalls();
}

void TunnelSlice::initWalls()
{
    float angle;
    Quaternion q;
    Vector3 move;
    
    sliceNode = parentNode->createChildSceneNode("tunnelSliceNode" + Util::toStringInt(wallID));
    entireWall = sliceNode->createChildSceneNode("entireWallNode" + Util::toStringInt(wallID));
    
    Entity* entireWallEntity = NULL;
    int countSides = 0;
    for (int i = 0; i < NUM_DIRECTIONS; ++i)
        if (sidesUsed[i]) ++countSides;
    switch (countSides)
    {
        case 3:
            entireWallEntity = parentNode->getCreator()->createEntity("entireWallEntity" + Util::toStringInt(wallID), "segmentMesh3");
            break;
        case 5:
            entireWallEntity = parentNode->getCreator()->createEntity("entireWallEntity" + Util::toStringInt(wallID), "segmentMesh5");
            break;
        case 7:
            entireWallEntity = parentNode->getCreator()->createEntity("entireWallEntity" + Util::toStringInt(wallID), "segmentMesh7");
            break;
        default:
            entireWallEntity = parentNode->getCreator()->createEntity("entireWallEntity" + Util::toStringInt(wallID), "segmentMesh8");
            break;
    }
    //entireWallEntity->setMaterialName(getMaterialName());
    for (int i = 0; i < entireWallEntity->getNumSubEntities(); ++i)
        entireWallEntity->getSubEntity(i)->setMaterialName(getMaterialName());
    entireWall->attachObject(entireWallEntity);
    entireWall->scale(width, width, depth);
    
    
    if( type == CHECKPOINT_PASS )
    {
        gateNode = sliceNode->createChildSceneNode("gateNode" + Util::toStringInt(wallID));
        gateEntity = gateNode->getCreator()->createEntity("gateEntity" + Util::toStringInt(wallID), "ExitGate/ExitGate.mesh");
    
        gateEntity->getSubEntity(0)->setMaterialName("Gate/TransparentNeonAqua");
        gateEntity->getSubEntity(1)->setMaterialName("Gate/NeonAqua");
        gateEntity->getSubEntity(2)->setMaterialName("Gate/LightGray");
        gateEntity->getSubEntity(3)->setMaterialName("Gate/DarkGray");
    
        for( int i = 0; i < 8; ++i )
        {
            gateDoorNodes[i] = gateNode->createChildSceneNode("gateDoorNode" + Util::toStringInt(wallID) + Util::toStringInt(i));
            gateDoorEntities[i] = gateDoorNodes[i]->getCreator()->createEntity("gateDoorEntity" + Util::toStringInt(wallID) + Util::toStringInt(i), "ExitGate/ExitGateDoor.mesh");
        
            gateDoorEntities[i]->getSubEntity(0)->setMaterialName("Gate/DarkestGray");
            gateDoorEntities[i]->getSubEntity(1)->setMaterialName("Gate/DarkGray");
        
            gateDoorNodes[i]->attachObject(gateDoorEntities[i]);
        
            gateDoorNodes[i]->roll(Degree(45*i));
        }	
    
        gateNode->attachObject(gateEntity);
        gateNode->translate(Vector3(0,0,-10));
        gateNode->scale(2.45f,2.45f,2.45f);
    }
    else if( type == CHECKPOINT_FAIL ) {
        gateNode = sliceNode->createChildSceneNode("gateNode" + Util::toStringInt(wallID));
        gateEntity = gateNode->getCreator()->createEntity("gateEntity" + Util::toStringInt(wallID), "ExitGate/ExitGate.mesh");
        
        gateEntity->getSubEntity(0)->setMaterialName("Gate/TransparentNeonAqua");
        gateEntity->getSubEntity(1)->setMaterialName("Gate/NeonAqua");
        gateEntity->getSubEntity(2)->setMaterialName("Gate/LightGray");
        gateEntity->getSubEntity(3)->setMaterialName("Gate/DarkGray");
        
        for( int i = 0; i < 8; ++i )
        {
            gateDoorNodes[i] = gateNode->createChildSceneNode("gateDoorNode" + Util::toStringInt(wallID) + Util::toStringInt(i));
            gateDoorEntities[i] = gateDoorNodes[i]->getCreator()->createEntity("gateDoorEntity" + Util::toStringInt(wallID) + Util::toStringInt(i), "ExitGate/ExitGateDoor.mesh");
            
            gateDoorEntities[i]->getSubEntity(0)->setMaterialName("Gate/DarkestGray");
            gateDoorEntities[i]->getSubEntity(1)->setMaterialName("Gate/DarkGray");
            
            gateDoorNodes[i]->attachObject(gateDoorEntities[i]);
            
            gateDoorNodes[i]->roll(Degree(45*i));
            
            Vector3 t = Vector3(0,4.0f,0);
            Vector3 r = Vector3(Math::Cos(Degree(45*i))*t.x - Math::Sin(Degree(45*i))*t.y,Math::Sin(Degree(45*i))*t.x + Math::Cos(Degree(45*i))*t.y,0);
            gateDoorNodes[i]->setPosition(r);
        }
        
        gateNode->attachObject(gateEntity);
        gateNode->translate(Vector3(0,0,-10));
        gateNode->scale(2.45f,2.45f,2.45f);
    }
    
    
    
    
    float wallLength = getWallLength();
    float scaleValue = 0.017f;
    Vector3 leftbarrierOffset = Vector3(-scaleValue,scaleValue,0);
    Vector3 rightbarrierOffset = leftbarrierOffset * Vector3(-1,1,1);
    if( countSides == 3 ) {
        move = Vector3(-wallLength * (0.5 + Math::Cos(Ogre::Radian(Math::PI) / 4)), -wallLength / 2, 0);
        
        leftWall = sliceNode->createChildSceneNode("leftWallNode" + Util::toStringInt(wallID));
        leftWall->translate(move);
        leftWall->translate(leftbarrierOffset);
        leftWall->scale(scaleValue,scaleValue,width/2*scaleValue);
        
        Entity* leftWallEntity = sliceNode->getCreator()->createEntity("leftWallEntity" + Util::toStringInt(wallID), "Railing/cube.mesh");
        leftWallEntity->setMaterialName("Railing/Cyan");
        leftWall->attachObject(leftWallEntity);

        
        move = Vector3(wallLength * (0.5 + Math::Cos(Ogre::Radian(Math::PI) / 4)), -wallLength / 2, 0);
        
        rightWall = sliceNode->createChildSceneNode("rightWallNode" + Util::toStringInt(wallID));
        rightWall->translate(move);
        rightWall->translate(rightbarrierOffset);
        rightWall->scale(scaleValue,scaleValue,width/2*scaleValue);
        
        Entity* rightWallEntity = sliceNode->getCreator()->createEntity("rightWallEntity" + Util::toStringInt(wallID), "Railing/cube.mesh");
        rightWallEntity->setMaterialName("Railing/Cyan");
        rightWall->attachObject(rightWallEntity);
    }
    else if( countSides == 5 ) {
        move = Vector3(-wallLength * (0.5 + Math::Cos(Ogre::Radian(Math::PI) / 4)), wallLength / 2, 0);
        
        leftWall = sliceNode->createChildSceneNode("leftWallNode" + Util::toStringInt(wallID));
        leftWall->translate(move);
        leftWall->translate(Util::EulerRotate(leftbarrierOffset,Degree(-45),'z'));
        leftWall->scale(scaleValue,scaleValue,width/2*scaleValue);
        leftWall->roll(Degree(-45));
        
        Entity* leftWallEntity = sliceNode->getCreator()->createEntity("leftWallEntity" + Util::toStringInt(wallID), "Railing/cube.mesh");
        leftWallEntity->setMaterialName("Railing/Cyan");
        leftWall->attachObject(leftWallEntity);
        
        
        move = Vector3(wallLength * (0.5 + Math::Cos(Ogre::Radian(Math::PI) / 4)), wallLength / 2, 0);
        
        rightWall = sliceNode->createChildSceneNode("rightWallNode" + Util::toStringInt(wallID));
        rightWall->translate(move);
        rightWall->translate(Util::EulerRotate(rightbarrierOffset,Degree(45),'z'));
        rightWall->scale(scaleValue,scaleValue,width/2*scaleValue);
        rightWall->roll(Degree(45));
        
        Entity* rightWallEntity = sliceNode->getCreator()->createEntity("rightWallEntity" + Util::toStringInt(wallID), "Railing/cube.mesh");
        rightWallEntity->setMaterialName("Railing/Cyan");
        rightWall->attachObject(rightWallEntity);
    }
    else if( countSides == 7 ) {
        move = Vector3(-wallLength / 2, wallLength  * (0.5 + Math::Sin(Ogre::Radian(Math::PI) / 4)), 0);
        
        leftWall = sliceNode->createChildSceneNode("leftWallNode" + Util::toStringInt(wallID));
        leftWall->translate(move);
        leftWall->translate(Util::EulerRotate(leftbarrierOffset,Degree(-90),'z'));
        leftWall->scale(scaleValue,scaleValue,width/2*scaleValue);
        leftWall->roll(Degree(-90));
        
        Entity* leftWallEntity = sliceNode->getCreator()->createEntity("leftWallEntity" + Util::toStringInt(wallID), "Railing/cube.mesh");
        leftWallEntity->setMaterialName("Railing/Cyan");
        leftWall->attachObject(leftWallEntity);
        
        
        move = Vector3(wallLength / 2, wallLength  * (0.5 + Math::Sin(Ogre::Radian(Math::PI) / 4)), 0);
        
        rightWall = sliceNode->createChildSceneNode("rightWallNode" + Util::toStringInt(wallID));
        rightWall->translate(move);
        rightWall->translate(Util::EulerRotate(rightbarrierOffset,Degree(90),'z'));
        rightWall->scale(scaleValue,scaleValue,width/2*scaleValue);
        rightWall->roll(Degree(90));
        
        Entity* rightWallEntity = sliceNode->getCreator()->createEntity("rightWallEntity" + Util::toStringInt(wallID), "Railing/cube.mesh");
        rightWallEntity->setMaterialName("Railing/Cyan");
        rightWall->attachObject(rightWallEntity);
    }
    
    // Transitions
    float tWidthOffset = 0.3;
    float tHeightOffset = wallLength/16;
    float tDepthOffset = -width/2;
    Vector3 scaleAmount = Vector3(1, 3.775, 14);
    if( makeDecreasingTransition && countSides == 3 ) {
        move = Vector3(-wallLength * (0.5 + Math::Cos(Ogre::Radian(Math::PI) / 4)) - tWidthOffset, tHeightOffset, -width - tDepthOffset);
        
        topLeftIntermediate = sliceNode->createChildSceneNode("leftTransition" + Util::toStringInt(intermediateMeshID));
        topLeftIntermediate->translate(move);
        topLeftIntermediate->scale(scaleAmount);
        Entity* leftTransitionEntity = sliceNode->getCreator()->createEntity("leftTransitionEntity" + Util::toStringInt(intermediateMeshID), "Railing/railing_transition.mesh");
        leftTransitionEntity->getSubEntity(1)->setMaterialName("Railing/Cyan");
        leftTransitionEntity->getSubEntity(0)->setMaterialName("Railing/Cyan");
        topLeftIntermediate->attachObject(leftTransitionEntity);
        
        
        move = Vector3(wallLength * (0.5 + Math::Cos(Ogre::Radian(Math::PI) / 4)) + tWidthOffset, tHeightOffset, -width - tDepthOffset);
        
        topRightIntermediate = sliceNode->createChildSceneNode("rightTransition" + Util::toStringInt(intermediateMeshID));
        topRightIntermediate->translate(move);
        topRightIntermediate->scale(scaleAmount);
        Entity* rightTransitionEntity = sliceNode->getCreator()->createEntity("rightTransitionEntity" + Util::toStringInt(intermediateMeshID), "Railing/railing_transition.mesh");
        rightTransitionEntity->getSubEntity(1)->setMaterialName("Railing/Cyan");
        rightTransitionEntity->getSubEntity(0)->setMaterialName("Railing/Cyan");
        topRightIntermediate->attachObject(rightTransitionEntity);
        
        topLeftIntermediate->yaw(Degree(180));
        topRightIntermediate->yaw(Degree(180));
    }
    if( makeDecreasingTransition && countSides == 5 ) {
    
        move = Vector3(-wallLength * (0.5 + Math::Cos(Ogre::Radian(Math::PI) / 4)) - tWidthOffset, tHeightOffset, -width - tDepthOffset);
        
        move = Vector3(Util::EulerRotate(move, Degree(-45), 'z'));
        topLeftIntermediate = sliceNode->createChildSceneNode("leftTransition" + Util::toStringInt(intermediateMeshID));
        topLeftIntermediate->translate(move);
        topLeftIntermediate->scale(scaleAmount);
        topLeftIntermediate->roll(Degree(-45));
        Entity* leftTransitionEntity = sliceNode->getCreator()->createEntity("leftTransitionEntity" + Util::toStringInt(intermediateMeshID), "Railing/railing_transition.mesh");
        leftTransitionEntity->getSubEntity(1)->setMaterialName("Railing/Cyan");
        leftTransitionEntity->getSubEntity(0)->setMaterialName("Railing/Cyan");
        topLeftIntermediate->attachObject(leftTransitionEntity);
        
        
        move = Vector3(wallLength * (0.5 + Math::Cos(Ogre::Radian(Math::PI) / 4)) + tWidthOffset, tHeightOffset, -width - tDepthOffset);
        
        move = Vector3(Util::EulerRotate(move, Degree(45), 'z'));
        topRightIntermediate = sliceNode->createChildSceneNode("rightTransition" + Util::toStringInt(intermediateMeshID));
        topRightIntermediate->translate(move);
        topRightIntermediate->scale(scaleAmount);
        topRightIntermediate->roll(Degree(45));
        Entity* rightTransitionEntity = sliceNode->getCreator()->createEntity("rightTransitionEntity" + Util::toStringInt(intermediateMeshID), "Railing/railing_transition.mesh");
        rightTransitionEntity->getSubEntity(1)->setMaterialName("Railing/Cyan");
        rightTransitionEntity->getSubEntity(0)->setMaterialName("Railing/Cyan");
        topRightIntermediate->attachObject(rightTransitionEntity);

        topLeftIntermediate->yaw(Degree(180));
        topRightIntermediate->yaw(Degree(180));
    }
    if( makeDecreasingTransition && countSides == 7 ) {
        move = Vector3(0,wallLength * (0.5 + Math::Sin(Ogre::Radian(Math::PI) / 4)) + tHeightOffset, width/4);
        
        topIntermediate = sliceNode->createChildSceneNode("topTransition" + Util::toStringInt(intermediateMeshID));
        topIntermediate->translate(move);
        topIntermediate->scale(0.05,0.0125,0.03);
        Entity* topIntermediateEntity = sliceNode->getCreator()->createEntity("topTransitionEntity" + Util::toStringInt(intermediateMeshID), "Railing/cube.mesh");
        topIntermediateEntity->setMaterialName("Railing/Cyan");
        topIntermediate->attachObject(topIntermediateEntity);
    }
    
    sliceNode->setPosition(center);
    sliceNode->setOrientation(rot);
    ++wallID;
}

int TunnelSlice::getTunnelSliceID() const
{
    return tunnelSliceID;
}

float TunnelSlice::getWallLength() const
{
	return width / (2 * Math::Cos(Ogre::Radian(Math::PI) / 4) + 1);
}

TunnelType TunnelSlice::getType() const
{
    return type;
}

Quaternion TunnelSlice::getQuaternion() const
{
    return rot;
}

Vector3 TunnelSlice::getStart() const
{
    return center - getForward() * (depth / 2);
}

Vector3 TunnelSlice::getEnd() const
{
    return center + getForward() * (depth / 2);
}

Vector3 TunnelSlice::getCenter() const
{
	return center;
}

Vector3 TunnelSlice::getCenter(float t) const
{
    Vector3 start = getStart();
    Vector3 end = getEnd();
    
    return start + (end - start) * t;
}

// Since slices have an axis for its center, we project pos onto the axis
float TunnelSlice::getT(Vector3 pos) const
{
    Vector3 start = getStart();
    Vector3 end = getEnd();
    return getForward().dotProduct(pos - start) / (end - start).length();
}

Vector3 TunnelSlice::getForward() const
{
    Quaternion forward = Quaternion(0, 0, 0, -1);
    forward = rot * forward * rot.Inverse();
    return Vector3(forward.x, forward.y, forward.z);
}
Vector3 TunnelSlice::getUpward() const
{
    Quaternion upward = Quaternion(0, 0, 1, 0);
    upward = rot * upward * rot.Inverse();
    return Vector3(upward.x, upward.y, upward.z);
}
Vector3 TunnelSlice::getRight() const
{
    Quaternion right = Quaternion(0, 1, 0, 0);
    right = rot * right * rot.Inverse();
    return Vector3(right.x, right.y, right.z);
}

std::vector<Pod*> TunnelSlice::getPods() const
{
    return pods;
}

SectionInfo TunnelSlice::getSectionInfo() const
{
    return SectionInfo(type, dir, dirAngle, rot, sidesUsed);
}

bool TunnelSlice::isPodHistory() const
{
    return podHistory;
}

bool TunnelSlice::isInfoStored() const
{
    return infoStored;
}

bool TunnelSlice::hasAvailableSide(Direction side) const
{
    if (side == NO_DIRECTION)
        return false;
    return sidesUsed[side];
}

std::string TunnelSlice::getMaterialName() const
{
    std::string materialName;
    /*if (type == CHECKPOINT_PASS)
        materialName = "General/WallCheckpointPass";
     else if (type == CHECKPOINT_FAIL)
         materialName = "General/WallCheckpointFail";
    else*/ if (type == CHECKPOINT_EVEN)
        materialName = "General/WallCheckpointEven";
    else if (materialNames.size() > 0)
        materialName = materialNames[rand() % materialNames.size()];
    return materialName;
}

// Finds collisions through bounding boxes
std::vector<Pod*> TunnelSlice::findCollisions(SceneNode* ent) const
{
    std::vector<Pod*> ret;
    
	for (int i = 0; i < pods.size(); ++i)
	{
		if (ent->_getWorldAABB().intersects(pods[i]->getHead()->_getWorldAABB()))
			ret.push_back(pods[i]);
	}
    
	return ret;
}

// Finds collisions through t-prediction: can be improved
std::vector<Pod*> TunnelSlice::findCollisions(Vine* vine)
{
    std::vector<Pod*> ret;
    
	for (int i = 0; i < pods.size(); ++i)
	{
        // Is Vine in same panel as pod?
        if (pods[i]->getPodTrigger())
        {
            // Distractors
            if (!pods[i]->isPodTaken() &&
                ((vine->loc == vine->dest && vine->loc == pods[i]->getLoc()) ||
                 (vine->loc != vine->dest && ((vine->loc == pods[i]->getLoc() && vine->transition < 0.50) || (vine->dest == pods[i]->getLoc() && vine->transition >= 0.50)))))
            {
                // Is vine where pod is in time?
                //std::cout << tunnelSliceID << ": " << vine->previousID << " " << vine->afterID << " " << vine->previoust << " " << vine->aftert << std::endl;
                if ((vine->previousID < tunnelSliceID && vine->afterID > tunnelSliceID) ||
                    (vine->previousID == tunnelSliceID && vine->previoust < globals.distractorCollisionMin && (vine->aftert > globals.distractorCollisionMax || vine->afterID > vine->previousID)) ||
                    (vine->afterID == tunnelSliceID && ((vine->aftert >= globals.distractorCollisionMin && vine->aftert <= globals.distractorCollisionMax) || (vine->aftert > globals.distractorCollisionMax && vine->previousID < vine->afterID))))
                {
                    pods[i]->takePod();
                    ret.push_back(pods[i]);
                }
            }
        }
        else
        {
            // Pod Signals
            if (!pods[i]->isPodTaken() &&
            ((vine->loc == vine->dest && vine->loc == pods[i]->getLoc()) ||
             (vine->loc != vine->dest && ((vine->loc == pods[i]->getLoc() && vine->transition < 0.50) || (vine->dest == pods[i]->getLoc() && vine->transition >= 0.50)))))
            {
                // Is vine where pod is in time?
                //std::cout << tunnelSliceID << ": " << vine->previousID << " " << vine->afterID << " " << vine->previoust << " " << vine->aftert << std::endl;
                if ((vine->previousID < tunnelSliceID && vine->afterID > tunnelSliceID) ||
                    (vine->previousID == tunnelSliceID && vine->previoust < globals.podCollisionMin && (vine->aftert > globals.podCollisionMax || vine->afterID > vine->previousID)) ||
                    (vine->afterID == tunnelSliceID && ((vine->aftert >= globals.podCollisionMin && vine->aftert <= globals.podCollisionMax) || (vine->aftert > globals.podCollisionMax && vine->previousID < vine->afterID))))
                {
                    pods[i]->takePod();
                    ret.push_back(pods[i]);
                }
            }
        }
	}
    
	return ret;
}

Vector3 TunnelSlice::requestWallDirection(Direction dir) const
{
    Vector3 move;
	switch (dir)
	{
        case NORTHWEST:
            move = Vector3(-sqrt(2.0) / 2, sqrt(2.0) / 2, 0.0);
            break;
        case NORTH:
            move = Vector3(0.0, 1.0, 0.0);
            break;
        case NORTHEAST:
            move = Vector3(sqrt(2.0) / 2, sqrt(2.0) / 2, 0.0);
            break;
        case EAST:
            move = Vector3(1.0, 0.0, 0.0);
            break;
        case SOUTHEAST:
            move = Vector3(sqrt(2.0) / 2, -sqrt(2.0) / 2, 0.0);
            break;
        case SOUTH:
            move = Vector3(0.0, -1.0, 0.0);
            break;
        case SOUTHWEST:
            move = Vector3(-sqrt(2.0) / 2, -sqrt(2.0) / 2, 0.0);
            break;
        case WEST:
            move = Vector3(-1.0, 0.0, 0.0);
            break;
        default:
            // No Direction
            move = Vector3(0, 0, 0);
            break;
	}
    return rot * move;
}

Vector3 TunnelSlice::requestWallDistance(Direction dir) const
{
	float wallLength = getWallLength();
    Vector3 move;
	switch (dir)
	{
        case NORTHWEST:
            move = Vector3(-(width + wallLength) / 4, (width + wallLength) / 4, 0);
            break;
        case NORTH:
            move = Vector3(0, width / 2, 0);
            break;
        case NORTHEAST:
            move = Vector3((width + wallLength) / 4, (width + wallLength) / 4, 0);
            break;
        case EAST:
            move = Vector3(width / 2, 0, 0);
            break;
        case SOUTHEAST:
            move = Vector3((width + wallLength) / 4, -(width + wallLength) / 4, 0);
            break;
        case SOUTH:
            move = Vector3(0, -width / 2, 0);
            break;
        case SOUTHWEST:
            move = Vector3(-(width + wallLength) / 4, -(width + wallLength) / 4, 0);
            break;
        case WEST:
            move = Vector3(-width / 2, 0, 0);
            break;
        default:
            // No Direction
            move = Vector3(0, 0, 0);
            break;
	}
    return move;
}

Vector3 TunnelSlice::requestWallMove(Direction dir, float offset) const
{
    Vector3 move = rot * requestWallDistance(dir);
    move = move * ((move.length() - offset) / move.length());
    
    return move;
}

// Returns a position in the side of the segment from provided position and an offset in direction of the pipe
Vector3 TunnelSlice::requestWallPosition(Vector3 cur, Direction dir, float offset) const
{
    return cur + requestWallMove(dir, offset);
}

void TunnelSlice::setPodHistory(bool value)
{
    podHistory = value;
}

void TunnelSlice::setInfoStored(bool value)
{
    infoStored = value;
}

void TunnelSlice::move(Vector3 delta)
{
	center.x += delta.x;
	center.y += delta.y;
	center.z += delta.z;
	
    entireWall->translate(delta);
	//topLeftWall->translate(delta);
	//topWall->translate(delta);
	//topRightWall->translate(delta);
	//rightWall->translate(delta);
	//bottomRightWall->translate(delta);
	//bottomWall->translate(delta);
	//bottomLeftWall->translate(delta);
	//leftWall->translate(delta);
    if (entireIntermediate)
        entireIntermediate->translate(delta);
    
	for (int i = 0; i < pods.size(); ++i)
		pods[i]->move(delta);
}

void TunnelSlice::addPod(const PodInfo & value)
{
	float wallLength = getWallLength();
	const float STEM_RADIUS = globals.podStemRadius;
	const float HEAD_RADIUS = globals.podHeadRadius;
	const float STEM_LENGTH = globals.podStemLength;
    
    Vector3 move = requestWallDistance(value.podLoc);
    Vector3 base = move;
    move = move * ((move.length() - STEM_LENGTH) / move.length());
    Vector3 head = move;
 
    Pod* pod = new Pod(sliceNode, base, head, value.meshType, value.podSignal, value.podColor, value.podShape, value.podSound, value.podLoc, STEM_RADIUS, HEAD_RADIUS);
    pod->setPodGood(value.goodPod);
    pod->setPodTrigger(value.podTrigger);
	pods.push_back(pod);
}

void TunnelSlice::setIntermediateWall(SceneNode* entire, Direction dir, ManualObject * manual, Vector3 p1, Vector3 p2, Vector3 p3, Vector3 p4, Vector3 & bl, Vector3 & tr)
{
    manual->begin("BaseWhiteNoLighting", RenderOperation::OT_TRIANGLE_LIST);
    
    Vector3 d1 = (p2 - p1).normalisedCopy();
    Vector3 d2 = (p4 - p1).normalisedCopy();
    Vector3 n = d1.crossProduct(d2);
    
    d1 *= 64 * Util::EPSILON;
    d2 *= 32 * Util::EPSILON;
    
    manual->position(p1 - d1 - d2);
    manual->normal(n);
    manual->textureCoord(1, 1);
    manual->position(p2 + d1 - d2);
    manual->normal(n);
    manual->textureCoord(1, 0);
    manual->position(p3 + d1 + d2);
    manual->normal(n);
    manual->textureCoord(0, 0);
    manual->position(p4 - d1 + d2);
    manual->normal(n);
    manual->textureCoord(0, 1);
    manual->quad(0, 1, 2, 3);
    
    manual->end();
    
    bl = Vector3(fmin(bl.x, p1.x), fmin(bl.y, p2.y), fmin(bl.z, p1.z));
    bl = Vector3(fmin(bl.x, p2.x), fmin(bl.y, p2.y), fmin(bl.z, p2.z));
    bl = Vector3(fmin(bl.x, p3.x), fmin(bl.y, p3.y), fmin(bl.z, p3.z));
    bl = Vector3(fmin(bl.x, p4.x), fmin(bl.y, p4.y), fmin(bl.z, p4.z));
    tr = Vector3(fmax(tr.x, p1.x), fmax(tr.y, p2.y), fmax(tr.z, p1.z));
    tr = Vector3(fmax(tr.x, p2.x), fmax(tr.y, p2.y), fmax(tr.z, p2.z));
    tr = Vector3(fmax(tr.x, p3.x), fmax(tr.y, p3.y), fmax(tr.z, p3.z));
    tr = Vector3(fmax(tr.x, p4.x), fmax(tr.y, p4.y), fmax(tr.z, p4.z));
}

void TunnelSlice::connect(TunnelSlice* next)
{
	float wallLength1 = getWallLength();
	float wallLength2 = next->getWallLength();
    
    int tempID = intermediateMeshID;
    entireIntermediate = parentNode->createChildSceneNode("intermediateSegmentNode" + Util::toStringInt(intermediateMeshID));
    
    std::string meshName = "intermediateMesh" + Util::toStringInt(intermediateMeshID);
    ManualObject* manual = parentNode->getCreator()->createManualObject(meshName);
    
    Quaternion q1 = getQuaternion();
    Quaternion q2 = next->getQuaternion();
    
    Vector3 start = center + getForward() * (depth / 2);
    Vector3 end = next->center - next->getForward() * (next->depth / 2);
    
    Vector3 move;
    Vector3 p1;
    Vector3 p2;
    Vector3 p3;
    Vector3 p4;
    Vector3 bl = Vector3(Ogre::Math::POS_INFINITY, Ogre::Math::POS_INFINITY, Ogre::Math::POS_INFINITY);
    Vector3 tr = Vector3(Ogre::Math::NEG_INFINITY, Ogre::Math::NEG_INFINITY, Ogre::Math::NEG_INFINITY);
    
    
    float scaleValue = 0.017f;
    Vector3 leftbarrierOffset = Vector3(-scaleValue,scaleValue,0);
    Vector3 rightbarrierOffset = leftbarrierOffset * Vector3(-1,1,1);
    Vector3 leftConnector1Offset = Vector3(-scaleValue,scaleValue,-width/2);
    Vector3 rightConnector1Offset = leftConnector1Offset * Vector3(-1,1,1);
    Vector3 leftConnector2Offset = Vector3(-scaleValue,scaleValue,width/2);
    Vector3 rightConnector2Offset = leftConnector2Offset * Vector3(-1,1,1);
    if( Util::getNumSides(sidesUsed) == 3 ) {
        move = Vector3(-wallLength1 * (0.5 + Math::Cos(Ogre::Radian(Math::PI) / 4)), -wallLength1 / 2, -width);
        
        leftIntermediate = sliceNode->createChildSceneNode("leftIntermediateNode" + Util::toStringInt(intermediateMeshID));
        leftIntermediate->translate(move);
        leftIntermediate->translate(leftbarrierOffset);
        leftIntermediate->scale(scaleValue,scaleValue,width/2*scaleValue);
        
        bottomLeftIntermediate = sliceNode->createChildSceneNode("leftConnector1Node" + Util::toStringInt(intermediateMeshID));
        bottomLeftIntermediate->translate(move);
        bottomLeftIntermediate->translate(leftConnector1Offset);
        bottomLeftIntermediate->scale(scaleValue*2,scaleValue*2,scaleValue*4);
        
        bottomLeftWall = sliceNode->createChildSceneNode("leftConnector2Node" + Util::toStringInt(intermediateMeshID));
        bottomLeftWall->translate(move);
        bottomLeftWall->translate(leftConnector2Offset);
        bottomLeftWall->scale(scaleValue*2,scaleValue*2,scaleValue*4);
        
        Entity* leftIntermediateEntity = sliceNode->getCreator()->createEntity("leftIntermediateEntity" + Util::toStringInt(intermediateMeshID), "Railing/cube.mesh");
        Entity* leftConnector1Entity = sliceNode->getCreator()->createEntity("leftConnector1Entity" + Util::toStringInt(intermediateMeshID), "Railing/cube.mesh");
        Entity* leftConnector2Entity = sliceNode->getCreator()->createEntity("leftConnector2Entity" + Util::toStringInt(intermediateMeshID), "Railing/cube.mesh");
        leftIntermediateEntity->setMaterialName("Railing/Cyan");
        leftConnector1Entity->setMaterialName("Railing/Cyan");
        leftConnector2Entity->setMaterialName("Railing/Cyan");
        
        leftIntermediate->attachObject(leftIntermediateEntity);
        bottomLeftIntermediate->attachObject(leftConnector1Entity);
        bottomLeftWall->attachObject(leftConnector2Entity);
        
        move = Vector3(wallLength1 * (0.5 + Math::Cos(Ogre::Radian(Math::PI) / 4)), -wallLength1 / 2, -width);
        
        rightIntermediate = sliceNode->createChildSceneNode("rightIntermediateNode" + Util::toStringInt(intermediateMeshID));
        rightIntermediate->translate(move);
        rightIntermediate->translate(rightbarrierOffset);
        rightIntermediate->scale(scaleValue,scaleValue,width/2*scaleValue);
        
        bottomRightIntermediate = sliceNode->createChildSceneNode("rightConnector1Node" + Util::toStringInt(intermediateMeshID));
        bottomRightIntermediate->translate(move);
        bottomRightIntermediate->translate(rightConnector1Offset);
        bottomRightIntermediate->scale(scaleValue*2,scaleValue*2,scaleValue*4);
        
        bottomRightWall = sliceNode->createChildSceneNode("rightConnector2Node" + Util::toStringInt(intermediateMeshID));
        bottomRightWall->translate(move);
        bottomRightWall->translate(rightConnector2Offset);
        bottomRightWall->scale(scaleValue*2,scaleValue*2,scaleValue*4);
        
        Entity* rightIntermediateEntity = sliceNode->getCreator()->createEntity("rightIntermediateEntity" + Util::toStringInt(intermediateMeshID), "Railing/cube.mesh");
        Entity* rightConnector1Entity = sliceNode->getCreator()->createEntity("rightConnector1Entity" + Util::toStringInt(intermediateMeshID), "Railing/cube.mesh");
        Entity* rightConnector2Entity = sliceNode->getCreator()->createEntity("rightConnector2Entity" + Util::toStringInt(intermediateMeshID), "Railing/cube.mesh");
        rightIntermediateEntity->setMaterialName("Railing/Cyan");
        rightConnector1Entity->setMaterialName("Railing/Cyan");
        rightConnector2Entity->setMaterialName("Railing/Cyan");
        
        rightIntermediate->attachObject(rightIntermediateEntity);
        bottomRightIntermediate->attachObject(rightConnector1Entity);
        bottomRightWall->attachObject(rightConnector2Entity);
    }
    else if( Util::getNumSides(sidesUsed) == 5 ) {
        move = Vector3(-wallLength1 * (0.5 + Math::Cos(Ogre::Radian(Math::PI) / 4)), wallLength1 / 2, -width);
        
        leftIntermediate = sliceNode->createChildSceneNode("leftIntermediateNode" + Util::toStringInt(intermediateMeshID));
        leftIntermediate->translate(move);
        leftIntermediate->translate(Util::EulerRotate(leftbarrierOffset,Degree(-45),'z'));
        leftIntermediate->scale(scaleValue,scaleValue,width/2*scaleValue);
        leftIntermediate->roll(Degree(-45));
        
        bottomLeftIntermediate = sliceNode->createChildSceneNode("leftConnector1Node" + Util::toStringInt(intermediateMeshID));
        bottomLeftIntermediate->translate(move);
        bottomLeftIntermediate->translate(Util::EulerRotate(leftConnector1Offset,Degree(-45),'z'));
        bottomLeftIntermediate->scale(scaleValue*2,scaleValue*2,scaleValue*4);
        bottomLeftIntermediate->roll(Degree(-45));
        
        bottomLeftWall = sliceNode->createChildSceneNode("leftConnector2Node" + Util::toStringInt(intermediateMeshID));
        bottomLeftWall->translate(move);
        bottomLeftWall->translate(Util::EulerRotate(leftConnector2Offset,Degree(-45),'z'));
        bottomLeftWall->scale(scaleValue*2,scaleValue*2,scaleValue*4);
        bottomLeftWall->roll(Degree(-45));
        
        Entity* leftIntermediateEntity = sliceNode->getCreator()->createEntity("leftIntermediateEntity" + Util::toStringInt(intermediateMeshID), "Railing/cube.mesh");
        Entity* leftConnector1Entity = sliceNode->getCreator()->createEntity("leftConnector1Entity" + Util::toStringInt(intermediateMeshID), "Railing/cube.mesh");
        Entity* leftConnector2Entity = sliceNode->getCreator()->createEntity("leftConnector2Entity" + Util::toStringInt(intermediateMeshID), "Railing/cube.mesh");
        leftIntermediateEntity->setMaterialName("Railing/Cyan");
        leftConnector1Entity->setMaterialName("Railing/Cyan");
        leftConnector2Entity->setMaterialName("Railing/Cyan");
        
        leftIntermediate->attachObject(leftIntermediateEntity);
        bottomLeftIntermediate->attachObject(leftConnector1Entity);
        bottomLeftWall->attachObject(leftConnector2Entity);
        
        move = Vector3(wallLength1 * (0.5 + Math::Cos(Ogre::Radian(Math::PI) / 4)), wallLength1 / 2, -width);
        
        rightIntermediate = sliceNode->createChildSceneNode("rightIntermediateNode" + Util::toStringInt(intermediateMeshID));
        rightIntermediate->translate(move);
        rightIntermediate->translate(Util::EulerRotate(rightbarrierOffset,Degree(45),'z'));
        rightIntermediate->scale(scaleValue,scaleValue,width/2*scaleValue);
        rightIntermediate->roll(Degree(45));
        
        bottomRightIntermediate = sliceNode->createChildSceneNode("rightConnector1Node" + Util::toStringInt(intermediateMeshID));
        bottomRightIntermediate->translate(move);
        bottomRightIntermediate->translate(Util::EulerRotate(rightConnector1Offset,Degree(45),'z'));
        bottomRightIntermediate->scale(scaleValue*2,scaleValue*2,scaleValue*4);
        bottomRightIntermediate->roll(Degree(45));
        
        bottomRightWall = sliceNode->createChildSceneNode("rightConnector2Node" + Util::toStringInt(intermediateMeshID));
        bottomRightWall->translate(move);
        bottomRightWall->translate(Util::EulerRotate(rightConnector2Offset,Degree(45),'z'));
        bottomRightWall->scale(scaleValue*2,scaleValue*2,scaleValue*4);
        bottomRightWall->roll(Degree(45));
        
        Entity* rightIntermediateEntity = sliceNode->getCreator()->createEntity("rightIntermediateEntity" + Util::toStringInt(intermediateMeshID), "Railing/cube.mesh");
        Entity* rightConnector1Entity = sliceNode->getCreator()->createEntity("rightConnector1Entity" + Util::toStringInt(intermediateMeshID), "Railing/cube.mesh");
        Entity* rightConnector2Entity = sliceNode->getCreator()->createEntity("rightConnector2Entity" + Util::toStringInt(intermediateMeshID), "Railing/cube.mesh");
        rightIntermediateEntity->setMaterialName("Railing/Cyan");
        rightConnector1Entity->setMaterialName("Railing/Cyan");
        rightConnector2Entity->setMaterialName("Railing/Cyan");
        
        rightIntermediate->attachObject(rightIntermediateEntity);
        bottomRightIntermediate->attachObject(rightConnector1Entity);
        bottomRightWall->attachObject(rightConnector2Entity);
    }
    else if( Util::getNumSides(sidesUsed) == 7 ) {
        move = Vector3(-wallLength1 / 2, wallLength1  * (0.5 + Math::Sin(Ogre::Radian(Math::PI) / 4)), -width);
        
        leftIntermediate = sliceNode->createChildSceneNode("leftIntermediateNode" + Util::toStringInt(intermediateMeshID));
        leftIntermediate->translate(move);
        leftIntermediate->translate(Util::EulerRotate(leftbarrierOffset,Degree(-90),'z'));
        leftIntermediate->scale(scaleValue,scaleValue,width/2*scaleValue);
        leftIntermediate->roll(Degree(-90));
        
        bottomLeftIntermediate = sliceNode->createChildSceneNode("leftConnector1Node" + Util::toStringInt(intermediateMeshID));
        bottomLeftIntermediate->translate(move);
        bottomLeftIntermediate->translate(Util::EulerRotate(leftConnector1Offset,Degree(-90),'z'));
        bottomLeftIntermediate->scale(scaleValue*2,scaleValue*2,scaleValue*4);
        bottomLeftIntermediate->roll(Degree(-90));
        
        bottomLeftWall = sliceNode->createChildSceneNode("leftConnector2Node" + Util::toStringInt(intermediateMeshID));
        bottomLeftWall->translate(move);
        bottomLeftWall->translate(Util::EulerRotate(leftConnector2Offset,Degree(-90),'z'));
        bottomLeftWall->scale(scaleValue*2,scaleValue*2,scaleValue*4);
        bottomLeftWall->roll(Degree(-90));
        
        Entity* leftIntermediateEntity = sliceNode->getCreator()->createEntity("leftIntermediateEntity" + Util::toStringInt(intermediateMeshID), "Railing/cube.mesh");
        Entity* leftConnector1Entity = sliceNode->getCreator()->createEntity("leftConnector1Entity" + Util::toStringInt(intermediateMeshID), "Railing/cube.mesh");
        Entity* leftConnector2Entity = sliceNode->getCreator()->createEntity("leftConnector2Entity" + Util::toStringInt(intermediateMeshID), "Railing/cube.mesh");
        leftIntermediateEntity->setMaterialName("Railing/Cyan");
        leftConnector1Entity->setMaterialName("Railing/Cyan");
        leftConnector2Entity->setMaterialName("Railing/Cyan");
        
        leftIntermediate->attachObject(leftIntermediateEntity);
        bottomLeftIntermediate->attachObject(leftConnector1Entity);
        bottomLeftWall->attachObject(leftConnector2Entity);
        
        move = Vector3(wallLength1 / 2, wallLength1  * (0.5 + Math::Sin(Ogre::Radian(Math::PI) / 4)), -width);
        
        rightIntermediate = sliceNode->createChildSceneNode("rightIntermediateNode" + Util::toStringInt(intermediateMeshID));
        rightIntermediate->translate(move);
        rightIntermediate->translate(Util::EulerRotate(rightbarrierOffset,Degree(90),'z'));
        rightIntermediate->scale(scaleValue,scaleValue,width/2*scaleValue);
        rightIntermediate->roll(Degree(90));
        
        bottomRightIntermediate = sliceNode->createChildSceneNode("rightConnector1Node" + Util::toStringInt(intermediateMeshID));
        bottomRightIntermediate->translate(move);
        bottomRightIntermediate->translate(Util::EulerRotate(rightConnector1Offset,Degree(90),'z'));
        bottomRightIntermediate->scale(scaleValue*2,scaleValue*2,scaleValue*4);
        bottomRightIntermediate->roll(Degree(90));
        
        bottomRightWall = sliceNode->createChildSceneNode("rightConnector2Node" + Util::toStringInt(intermediateMeshID));
        bottomRightWall->translate(move);
        bottomRightWall->translate(Util::EulerRotate(rightConnector2Offset,Degree(90),'z'));
        bottomRightWall->scale(scaleValue*2,scaleValue*2,scaleValue*4);
        bottomRightWall->roll(Degree(90));
        
        Entity* rightIntermediateEntity = sliceNode->getCreator()->createEntity("rightIntermediateEntity" + Util::toStringInt(intermediateMeshID), "Railing/cube.mesh");
        Entity* rightConnector1Entity = sliceNode->getCreator()->createEntity("rightConnector1Entity" + Util::toStringInt(intermediateMeshID), "Railing/cube.mesh");
        Entity* rightConnector2Entity = sliceNode->getCreator()->createEntity("rightConnector2Entity" + Util::toStringInt(intermediateMeshID), "Railing/cube.mesh");
        rightIntermediateEntity->setMaterialName("Railing/Cyan");
        rightConnector1Entity->setMaterialName("Railing/Cyan");
        rightConnector2Entity->setMaterialName("Railing/Cyan");
        
        rightIntermediate->attachObject(rightIntermediateEntity);
        bottomRightIntermediate->attachObject(rightConnector1Entity);
        bottomRightWall->attachObject(rightConnector2Entity);
    }
    
    
    
    //Transitions
    float tWidthOffset = 0.3;
    float tHeightOffset = wallLength1/16;
    float tDepthOffset = -width/4;
    Vector3 scaleAmount = Vector3(1,3.775,14);
    if( Util::getNumSides(sidesUsed) == 3 && Util::getNumSides(next->sidesUsed) == 5 ) {
        
        move = Vector3(-wallLength1 * (0.5 + Math::Cos(Ogre::Radian(Math::PI) / 4)) - tWidthOffset, tHeightOffset, -width - tDepthOffset);
        
        topLeftIntermediate = sliceNode->createChildSceneNode("leftTransition" + Util::toStringInt(intermediateMeshID));
        topLeftIntermediate->translate(move);
        topLeftIntermediate->scale(scaleAmount);
        Entity* leftTransitionEntity = sliceNode->getCreator()->createEntity("leftTransitionEntity" + Util::toStringInt(intermediateMeshID), "Railing/railing_transition.mesh");
        leftTransitionEntity->getSubEntity(1)->setMaterialName("Railing/Cyan");
        leftTransitionEntity->getSubEntity(0)->setMaterialName("Railing/Cyan");
        topLeftIntermediate->attachObject(leftTransitionEntity);
        
        
        move = Vector3(wallLength1 * (0.5 + Math::Cos(Ogre::Radian(Math::PI) / 4)) + tWidthOffset, tHeightOffset, -width - tDepthOffset);
        
        topRightIntermediate = sliceNode->createChildSceneNode("rightTransition" + Util::toStringInt(intermediateMeshID));
        topRightIntermediate->translate(move);
        topRightIntermediate->scale(scaleAmount);
        Entity* rightTransitionEntity = sliceNode->getCreator()->createEntity("rightTransitionEntity" + Util::toStringInt(intermediateMeshID), "Railing/railing_transition.mesh");
        rightTransitionEntity->getSubEntity(1)->setMaterialName("Railing/Cyan");
        rightTransitionEntity->getSubEntity(0)->setMaterialName("Railing/Cyan");
        topRightIntermediate->attachObject(rightTransitionEntity);
    }
    else if( Util::getNumSides(sidesUsed) == 5 && Util::getNumSides(next->sidesUsed) == 7 ) {
        
        move = Vector3(-wallLength1 * (0.5 + Math::Cos(Ogre::Radian(Math::PI) / 4)) - tWidthOffset, tHeightOffset, -width - tDepthOffset);
        
        move = Vector3(Util::EulerRotate(move, Degree(-45), 'z'));
        topLeftIntermediate = sliceNode->createChildSceneNode("leftTransition" + Util::toStringInt(intermediateMeshID));
        topLeftIntermediate->translate(move);
        topLeftIntermediate->scale(scaleAmount);
        topLeftIntermediate->roll(Degree(-45));
        Entity* leftTransitionEntity = sliceNode->getCreator()->createEntity("leftTransitionEntity" + Util::toStringInt(intermediateMeshID), "Railing/railing_transition.mesh");
        leftTransitionEntity->getSubEntity(1)->setMaterialName("Railing/Cyan");
        leftTransitionEntity->getSubEntity(0)->setMaterialName("Railing/Cyan");
        topLeftIntermediate->attachObject(leftTransitionEntity);
        
        
        move = Vector3(wallLength1 * (0.5 + Math::Cos(Ogre::Radian(Math::PI) / 4)) + tWidthOffset, tHeightOffset, -width - tDepthOffset);
        
        move = Vector3(Util::EulerRotate(move, Degree(45), 'z'));
        topRightIntermediate = sliceNode->createChildSceneNode("rightTransition" + Util::toStringInt(intermediateMeshID));
        topRightIntermediate->translate(move);
        topRightIntermediate->scale(scaleAmount);
        topRightIntermediate->roll(Degree(45));
        Entity* rightTransitionEntity = sliceNode->getCreator()->createEntity("rightTransitionEntity" + Util::toStringInt(intermediateMeshID), "Railing/railing_transition.mesh");
        rightTransitionEntity->getSubEntity(1)->setMaterialName("Railing/Cyan");
        rightTransitionEntity->getSubEntity(0)->setMaterialName("Railing/Cyan");
        topRightIntermediate->attachObject(rightTransitionEntity);
    }
    else if( Util::getNumSides(sidesUsed) == 7 && Util::getNumSides(next->sidesUsed) == 8 ) {
        
        move = Vector3(0,wallLength1 * (0.5 + Math::Sin(Ogre::Radian(Math::PI) / 4)) + tHeightOffset, -width - width/2);
        
        topIntermediate = sliceNode->createChildSceneNode("topTransition" + Util::toStringInt(intermediateMeshID));
        topIntermediate->translate(move);
        topIntermediate->scale(0.05,0.0125,0.03);
        Entity* topIntermediateEntity = sliceNode->getCreator()->createEntity("topTransitionEntity" + Util::toStringInt(intermediateMeshID), "Railing/cube.mesh");
        topIntermediateEntity->setMaterialName("Railing/Cyan");
        topIntermediate->attachObject(topIntermediateEntity);
    }
    
    move = Vector3(-wallLength1 * (0.5 + Math::Cos(Ogre::Radian(Math::PI) / 4)), wallLength1 / 2, 0);
    move = q1 * move;
    p1 = start + move;
    move = Vector3(-wallLength2 * (0.5 + Math::Cos(Ogre::Radian(Math::PI) / 4)), wallLength2 / 2, 0);
    move = q2 * move;
    p2 = end + move;
    move = Vector3(-wallLength2 / 2, wallLength2 * (0.5 + Math::Sin(Ogre::Radian(Math::PI) / 4)), 0);
    move = q2 * move;
    p3 = end + move;
    move = Vector3(-wallLength1 / 2, wallLength1 * (0.5 + Math::Sin(Ogre::Radian(Math::PI) / 4)), 0);
    move = q1 * move;
    p4 = start + move;
    if (sidesUsed[NORTHWEST])
        setIntermediateWall(entireIntermediate, NORTHWEST, manual, p1, p2, p3, p4, bl, tr);
    
    p1 = p4;
    p2 = p3;
    move = Vector3(wallLength2 / 2, wallLength2 * (0.5 + Math::Sin(Ogre::Radian(Math::PI) / 4)), 0);
    move = q2 * move;
    p3 = end + move;
    move = Vector3(wallLength1 / 2, wallLength1 * (0.5 + Math::Sin(Ogre::Radian(Math::PI) / 4)), 0);
    move = q1 * move;
    p4 = start + move;
    if (sidesUsed[NORTH])
        setIntermediateWall(entireIntermediate, NORTH, manual, p1, p2, p3, p4, bl, tr);
    
    p1 = p4;
    p2 = p3;
    move = Vector3(wallLength2 * (0.5 + Math::Cos(Ogre::Radian(Math::PI) / 4)), wallLength2 / 2, 0);
    move = q2 * move;
    p3 = end + move;
    move = Vector3(wallLength1 * (0.5 + Math::Cos(Ogre::Radian(Math::PI) / 4)), wallLength1 / 2, 0);
    move = q1 * move;
    p4 = start + move;
    if (sidesUsed[NORTHEAST])
        setIntermediateWall(entireIntermediate, NORTHEAST, manual, p1, p2, p3, p4, bl, tr);
    
    p1 = p4;
    p2 = p3;
    move = Vector3(wallLength2 * (0.5 + Math::Cos(Ogre::Radian(Math::PI) / 4)), -wallLength2 / 2, 0);
    move = q2 * move;
    p3 = end + move;
    move = Vector3(wallLength1 * (0.5 + Math::Cos(Ogre::Radian(Math::PI) / 4)), -wallLength1 / 2, 0);
    move = q1 * move;
    p4 = start + move;
    if (sidesUsed[EAST])
        setIntermediateWall(entireIntermediate, EAST, manual, p1, p2, p3, p4, bl, tr);
    
    p1 = p4;
    p2 = p3;
    move = Vector3(wallLength2 / 2, -wallLength2 * (0.5 + Math::Sin(Ogre::Radian(Math::PI) / 4)), 0);
    move = q2 * move;
    p3 = end + move;
    move = Vector3(wallLength1 / 2, -wallLength1 * (0.5 + Math::Sin(Ogre::Radian(Math::PI) / 4)), 0);
    move = q1 * move;
    p4 = start + move;
    if (sidesUsed[SOUTHEAST])
        setIntermediateWall(entireIntermediate, SOUTHEAST, manual, p1, p2, p3, p4, bl, tr);
    
    p1 = p4;
    p2 = p3;
    move = Vector3(-wallLength2 / 2, -wallLength2 * (0.5 + Math::Sin(Ogre::Radian(Math::PI) / 4)), 0);
    move = q2 * move;
    p3 = end + move;
    move = Vector3(-wallLength1 / 2, -wallLength1 * (0.5 + Math::Sin(Ogre::Radian(Math::PI) / 4)), 0);
    move = q1 * move;
    p4 = start + move;
    if (sidesUsed[SOUTH])
        setIntermediateWall(entireIntermediate, SOUTH, manual, p1, p2, p3, p4, bl, tr);
    
    p1 = p4;
    p2 = p3;
    move = Vector3(-wallLength2 * (0.5 + Math::Cos(Ogre::Radian(Math::PI) / 4)), -wallLength2 / 2, 0);
    move = q2 * move;
    p3 = end + move;
    move = Vector3(-wallLength1 * (0.5 + Math::Cos(Ogre::Radian(Math::PI) / 4)), -wallLength1 / 2, 0);
    move = q1 * move;
    p4 = start + move;
    if (sidesUsed[SOUTHWEST])
        setIntermediateWall(entireIntermediate, SOUTHWEST, manual, p1, p2, p3, p4, bl, tr);
    
    p1 = p4;
    p2 = p3;
    move = Vector3(-wallLength1 * (0.5 + Math::Cos(Ogre::Radian(Math::PI) / 4)), wallLength1 / 2, 0);
    move = q2 * move;
    p3 = end + move;
    move = Vector3(-wallLength2 * (0.5 + Math::Cos(Ogre::Radian(Math::PI) / 4)), wallLength2 / 2, 0);
    move = q1 * move;
    p4 = start + move;
    if (sidesUsed[WEST])
        setIntermediateWall(entireIntermediate, WEST, manual, p1, p2, p3, p4, bl, tr);
    
    MeshPtr mesh = manual->convertToMesh(meshName);
    mesh->_setBounds( AxisAlignedBox( bl, tr ), true );
    
    float l = (tr - bl).length() / 2;
    mesh->_setBoundingSphereRadius(l);
    unsigned short src, dest;
    if (!mesh->suggestTangentVectorBuildParams(VES_TANGENT, src, dest))
    {
        mesh->buildTangentVectors(VES_TANGENT, src, dest);
    }
    
    Entity* intermediateSegmentEntity = entireIntermediate->getCreator()->createEntity("intermediateSegmentEntity" + Util::toStringInt(intermediateMeshID), meshName);
    //intermediateSegmentEntity->setMaterialName(getMaterialName());
    for (int i = 0; i < intermediateSegmentEntity->getNumSubEntities(); ++i)
        intermediateSegmentEntity->getSubEntity(i)->setMaterialName(getMaterialName());
    entireIntermediate->attachObject(intermediateSegmentEntity);
    
    meshes.push_back(mesh);
    intermediateMeshID++;
    parentNode->getCreator()->destroyManualObject(manual);
}

void TunnelSlice::disconnect()
{
    if (!entireIntermediate)
        return;
    
    if (topLeftIntermediate)
        topLeftIntermediate->getCreator()->destroyMovableObject(topLeftIntermediate->getAttachedObject(0)); // Assuming only one entity
    if (topIntermediate)
        topIntermediate->getCreator()->destroyMovableObject(topIntermediate->getAttachedObject(0));
    if (topRightIntermediate)
        topRightIntermediate->getCreator()->destroyMovableObject(topRightIntermediate->getAttachedObject(0));
    if (rightIntermediate)
        rightIntermediate->getCreator()->destroyMovableObject(rightIntermediate->getAttachedObject(0));
    if (bottomRightIntermediate)
        bottomRightIntermediate->getCreator()->destroyMovableObject(bottomRightIntermediate->getAttachedObject(0));
    if (bottomIntermediate)
        bottomIntermediate->getCreator()->destroyMovableObject(bottomIntermediate->getAttachedObject(0));
    if (bottomLeftIntermediate)
        bottomLeftIntermediate->getCreator()->destroyMovableObject(bottomLeftIntermediate->getAttachedObject(0));
    if (leftIntermediate)
        leftIntermediate->getCreator()->destroyMovableObject(leftIntermediate->getAttachedObject(0));
    
    for (int i = 0; i < meshes.size(); ++i)
    {
        // ***** Should understand this further, could not need one or the other statement.
        meshes[i]->unload();
        Ogre::MeshManager::getSingleton().remove(meshes[i]->getName());
    }
    meshes.clear();
    
    entireIntermediate->removeAndDestroyAllChildren();
    entireIntermediate->getCreator()->destroyMovableObject(entireIntermediate->getAttachedObject(0));
    entireIntermediate->getCreator()->destroySceneNode(entireIntermediate);
    entireIntermediate = NULL;
    topLeftIntermediate = NULL;
    topIntermediate = NULL;
    topRightIntermediate = NULL;
    rightIntermediate = NULL;
    bottomRightIntermediate = NULL;
    bottomIntermediate = NULL;
    bottomLeftIntermediate = NULL;
    leftIntermediate = NULL;
}

void TunnelSlice::clearPods()
{
    for (int i = 0; i < pods.size(); ++i) {
        pods[i]->removeFromScene();
        delete pods[i];
    }
    pods.clear();
}

void TunnelSlice::updateGrowth(float nt)
{
    growthT += nt;
    if (growthT > 1) growthT = 1;
    if (growthT < 0) growthT = 0;
    for (int i = 0; i < pods.size(); ++i)
        pods[i]->setToGrowth(growthT);
}

void TunnelSlice::rejuvenate(int nid, SectionInfo info, Vector3 start, float width, float depth, const std::string & material)
{
    this->tunnelSliceID = nid;
    this->type = type;
    Vector3 forward = info.tunnelRot * globals.tunnelReferenceForward;
    Vector3 end = start + forward * (depth + globals.tunnelSegmentBuffer);
    this->center = (start + end) / 2;
    this->rot = info.tunnelRot;
    this->width = width;
    this->depth = depth;
    this->materialNames.clear();
    this->materialNames.push_back(material);
    growthT = 0;
    infoStored = false;
    
    if (!Util::doSidesMatch(sidesUsed, info.sidesUsed))
    {
        removeFromScene();
        for (int i = 0; i < NUM_DIRECTIONS; ++i)
            sidesUsed[i] = info.sidesUsed[i];
        initWalls();
    }
    else
    {
        // Reset orientation and position, remove pods from scene
        sliceNode->setPosition(center);
        sliceNode->setOrientation(rot);
        for (int i = 0; i < pods.size(); ++i)
            pods[i]->removeFromScene();
        for (int i = 0; i < pods.size(); ++i)
            delete pods[i];
        pods.clear();
        disconnect(); //also clears pods and disconnects
    }
}

void TunnelSlice::rejuvenate(int nid, SectionInfo info, Vector3 start, float width, float depth, const std::vector<std::string> & materials)
{
    this->tunnelSliceID = nid;
    this->type = type;
    Vector3 forward = info.tunnelRot * globals.tunnelReferenceForward;
    Vector3 end = start + forward * (depth + globals.tunnelSegmentBuffer);
    this->center = (start + end) / 2;
    this->rot = info.tunnelRot;
    this->width = width;
    this->depth = depth;
    this->materialNames = materials;
    growthT = 0;
    infoStored = false;
    
    if (!Util::doSidesMatch(sidesUsed, info.sidesUsed))
    {
        removeFromScene();
        for (int i = 0; i < NUM_DIRECTIONS; ++i)
            sidesUsed[i] = info.sidesUsed[i];
        initWalls();
    }
    else
    {
        // Reset orientation and position, remove pods from scene
        sliceNode->setPosition(center);
        sliceNode->setOrientation(rot);
        for (int i = 0; i < pods.size(); ++i)
            pods[i]->removeFromScene();
        for (int i = 0; i < pods.size(); ++i)
            delete pods[i];
        pods.clear();
        disconnect(); //also clears pods and disconnects
    }
}

void TunnelSlice::removeFromScene()
{
	for (int i = 0; i < pods.size(); ++i)
		pods[i]->removeFromScene();
    
    if (topLeftWall)
        topLeftWall->getCreator()->destroyMovableObject(topLeftWall->getAttachedObject(0)); // Assuming only one entity
    if (topWall)
        topWall->getCreator()->destroyMovableObject(topWall->getAttachedObject(0));
    if (topRightWall)
        topRightWall->getCreator()->destroyMovableObject(topRightWall->getAttachedObject(0));
    if (rightWall)
        rightWall->getCreator()->destroyMovableObject(rightWall->getAttachedObject(0));
    if (bottomRightWall)
        bottomRightWall->getCreator()->destroyMovableObject(bottomRightWall->getAttachedObject(0));
    if (bottomWall)
        bottomWall->getCreator()->destroyMovableObject(bottomWall->getAttachedObject(0));
    if (bottomLeftWall)
        bottomLeftWall->getCreator()->destroyMovableObject(bottomLeftWall->getAttachedObject(0));
    if (leftWall)
        leftWall->getCreator()->destroyMovableObject(leftWall->getAttachedObject(0));
    if (entireWall)
    {
        entireWall->removeAndDestroyAllChildren();
        entireWall->getCreator()->destroyMovableObject(entireWall->getAttachedObject(0));
        entireWall->getCreator()->destroySceneNode(entireWall);
    }
    
    disconnect();
    
    sliceNode->removeAndDestroyAllChildren();
    sliceNode->getCreator()->destroySceneNode(sliceNode);
	
	for (int i = 0; i < pods.size(); ++i)
        delete pods[i];
    sliceNode = NULL;
	topLeftWall = NULL;
	topWall = NULL;
	topRightWall = NULL;
	rightWall = NULL;
	bottomRightWall = NULL;
	bottomWall = NULL;
	bottomLeftWall = NULL;
	leftWall = NULL;
    entireWall = NULL;
	pods.clear();
}
