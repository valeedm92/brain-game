//
//  Tunnel.h
//  Testing
//
//  Created by Calvin Phung on 9/10/13.
//
//

#ifndef __Vinezors2_0__Tunnel__
#define __Vinezors2_0__Tunnel__

#include <list>
#include <vector>
#include "Util.h"
#include "TunnelSlice.h"
#include "History.h"

enum GameMode { GAME_PROFICIENCY, GAME_TIMED, GAME_NAVIGATION };

// Stores the list of tunnel segments
class Tunnel
{
public:
    enum SetPodTarget { UNKNOWN, BAD_TARGET, GOOD_TARGET };
    
    SceneNode* parentNode;
    
    SceneNode* mainTunnelNode;
	Vector3 start;
	Vector3 end;
    std::list<TunnelSlice*> segments;
    float tLeftPrevious;
    float tLeftCurrent;
    std::list<TunnelSlice*>::iterator previous;
    std::list<TunnelSlice*>::iterator current;
    float tLeftOffsetPrevious;
    float tLeftOffsetCurrent;
    std::list<TunnelSlice*>::iterator previousOffset;
    std::list<TunnelSlice*>::iterator currentOffset;
    int segmentCounter;
	float segmentWidth;
	float segmentDepth;
	int segmentMinAngleTurn;
	int segmentMaxAngleTurn;
    Quaternion endRot;
    
    std::vector<SectionInfo> sections;
    std::vector<PodInfo> types;
    int sectionSize;
    int podSegmentSize;
    int distractorSegmentSize;
    int spawnIndex; // The current index to spawn which pod.
    int spawnLimit;
    int numTargets;
    
    // These indices used to track current player's spot
    int sectionIndex;
    int podIndex;
    int renewalSectionCounter;
    int renewalPodCounter;
    int renewalDistractorCounter;
    std::list<Pod*> activePods; // Animating pods
    
    // Stage attributes
    GameMode mode;
    float totalElapsed;
    float timePenalty;
    int nback;
    int control;
    History* history;
    Direction basis;
    bool sidesUsed[NUM_DIRECTIONS];
    Evaluation eval;
    
    bool setColors;
    bool setSounds;
    bool setShapes;
    
    bool done;      // Says stage is over, but not the ending animation
    bool cleanup;   // Totally done, ending animation is over
public:
	Tunnel();
    
	Tunnel(Ogre::SceneNode* parentNode, Vector3 start, float segmentWidth, float segmentDepth, int segmentMinAngleTurn, int segmentMaxAngleTurn, GameMode mode, int nback, int control, Direction sloc, int sectionSize, int podSegmentSize, int distractorSegmentSize, bool setColors, bool setSounds, bool setShapes);
	
    SceneNode* getMainTunnelNode() const;
	Vector3 getStart() const;
	Vector3 getEnd() const;
	Vector3 getCenter() const;
    float getTLeftPrevious() const;
    float getTLeftCurrent() const;
    float getTLeftOffsetPrevious() const;
    float getTLeftOffsetCurrent() const;
    std::list<TunnelSlice*>::const_iterator getPreviousIterator() const;
    std::list<TunnelSlice*>::const_iterator getCurrentIterator() const;
    std::list<TunnelSlice*>::const_iterator getBeginIterator() const;
    std::list<TunnelSlice*>::const_iterator getEndIterator() const;
    std::vector<TunnelSlice*> findSlicesSincePrevious() const;
    std::vector<TunnelSlice*> findSlicesSincePreviousOffset() const;
    void setOffsetIterators(Vector3 pos, float tOffset);
    TunnelSlice* findSliceFrom(std::list<TunnelSlice*>::const_iterator stit, Vector3 pos, float tOffset, float & tLeft) const;
	TunnelSlice* getPrevious() const;
	TunnelSlice* getCurrent() const;
	TunnelSlice* getPreviousOffset() const;
	TunnelSlice* getCurrentOffset() const;
	TunnelSlice* getFront() const;
	TunnelSlice* getBack() const;
	TunnelSlice* getNext(int i) const;
	int getSpawnIndex() const;
	int getSpawnLimit() const;
	int getNumTargets() const;
	int getSignalsLeft() const;
	int getSectionIndex() const;
	int getPodIndex() const;
    Quaternion getQuaternion() const;
    Quaternion getCombinedQuaternion(TunnelSlice* slice) const;
    History* getHistory() const;
    Evaluation getEval() const;
    
	float getSegmentWidth() const;
	float getSegmentDepth() const;
    std::vector<SectionInfo> getSectionInfo() const;
    std::vector<PodInfo> getPodInfo() const;
    Quaternion getNewSegmentQuaternion(Direction dir, int degrees);
    PodSignal getNBackTest(int section) const;
    GameMode getMode() const;
    float getTotalElapsed() const;
    float getTimePenalty() const;
    int getNBack() const;
    int getControl() const;
    Direction getBasis() const;
    bool hasAvailableSide(Direction side) const;
    std::string determineMaterial() const;
    
    void checkIfDone(Player* player);
    bool isDone() const;
    void setDone(int stars);
    void upgradeControl();
    void addToTimePenalty(float value);
    bool needsCleaning() const;
    
    void setNewControl(int control);
	void removeSegment();
    
    SectionInfo getNextSectionInfo() const;
    SectionInfo getNextSegmentInfo(SectionInfo sectionInfo) const;
    PodInfo getNextPodInfoAt(SetPodTarget setting, int index);
    PodInfo getNextPodInfo(SetPodTarget setting);
    std::vector<PodInfo> getNextDistractorInfo(PodInfo signal = PodInfo());
    void addSegment(SectionInfo sectionInfo, const std::vector<PodInfo> & podInfos = std::vector<PodInfo>());
	void renewSegment(SectionInfo sectionInfo, const std::vector<PodInfo> & podInfos = std::vector<PodInfo>());
    void addSection(SectionInfo newSection);
    void renewSection(SectionInfo newSection);
    
    bool updateIterators(Vector3 checkPos);
	bool renewIfNecessary();
    
    std::vector<Pod *> findPodCollisions(SceneNode* ent);
    
    void constructTunnel(int size, Quaternion q = Quaternion(1, 0, 0, 0), bool pregenPods = false);
    
    void update(Player* player, Hud* hud, float elapsed);
    
	~Tunnel();
};

#endif /* defined(__Testing__Tunnel__) */
