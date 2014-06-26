//
//  Player.h
//  Testing
//
//  Created by Calvin Phung on 9/13/13.
//
//

#ifndef __Vinezors2_0__Player__
#define __Vinezors2_0__Player__

#include <vector>
#include <fstream>
#include <map>
#include "Util.h"
#include "Vine.h"
#include "Tunnel.h"
#include "Powerup.h"
#include "PlayerLevel.h"
#include "PlayerProgress.h"
#include "LevelSet.h"
#include "TutorialManager.h"

class Player
{
private:
    enum MovementMode { MOVEMENT_STATIC, MOVEMENT_ROTATING };
    
    LevelSet* levels;
    Tunnel* tunnel;
    
    unsigned seed;
    
    std::string name;
    int hp;
    int numCorrectTotal;
    int numSafeTotal;
    int numMissedTotal;
    int numWrongTotal;
    int numCorrectBonus;
    int numCorrectCombo;
    int numWrongCombo;
    int numAvoidancesTotal;
    int numCollisionsTotal;
    float score;
	bool mouseLeft;
	bool keyUp;
	bool keyDown;
	bool keyLeft;
	bool keyRight;
	bool keySpace;
	
    std::vector<Vine*> vines;
    
    MovementMode movementMode;
    bool showCombo;
    
	Direction camDir; // direction offset on tunnel for player camera
	Vector2 mousePos;
    Vector3 oldPos;
	Vector3 camPos;
    Quaternion oldRot;
    float oldRoll;
    Quaternion camRot;
    float camRoll;
    Quaternion desireRot;
    float desireRoll;
    float baseSpeed;
    float bonusSpeed;
    float finalSpeed;
    float initSpeed;
    float minSpeed;
    float maxSpeed;
    
	float vineOffset; // offset to camPos in direction of forward
    TunnelSlice* lookback;
    Pod* selectedTarget;
    float glowSpeed;
    
    int toggleBack; // 0, 1, or 2
    
    static const char ENC_NO_PANEL = '0';
    static const char ENC_YES_PANEL = '1';
    static const char ENC_TARGET = '2';
    static const char ENC_FAKE = '3';
    static const char ENC_OBSTACLE_NO_HIT = '4';
    static const char ENC_OBSTACLE_YES_HIT = '5';
    static const char ENC_PLAYER_STATIC = '6';
    static const char ENC_PLAYER_CORRECT = '7';
    static const char ENC_PLAYER_WRONG = '8';
    struct Result {
        char segmentEncoding[NUM_DIRECTIONS];
        int eventID;
        int taskType;
        int nback;
        int navigation;
        Direction playerLoc;
        PodInfo podInfo;
        SectionInfo sectionInfo;
        int timestamp;
        float minSpeed;
        float maxSpeed;
        float baseSpeed;
        float finalSpeed;
        int navScore;
    };
    struct Action {
        int eventID;
        ActionCode action;
        int timestamp;
        int baseSpeed;
        int finalSpeed;
    };
    struct Session {
        int sessionNo;
        int eventID;
        int taskType;
        int stageTime;
        int timestampIn;
        int timestampOut;
        int nback;
        int rep;
        int runSpeedIn;
        int runSpeedOut;
        int maxSpeed;
        int navScore;
        int TP;
        int FP;
        int TN;
        int FN;
        int obsHit;
        int obsAvoided;
    };
    std::list<Result> results;
    std::list<Action> actions;
    std::list<Session> sessions;
    PlayerLevel skillLevel;
    
    std::map<std::string,Powerup*> powerups;
    
    float totalElapsed;
    float totalDistanceTraveled;
    float animationTimer;
    float speedTimer;
    float badFuelPickUpTimer;
    float boostTimer;
    float selectTimer;
    bool selectTimerFlag;
    float startMusicTimer;
    bool godMode;
    
    OgreOggSound::OgreOggISound* soundMusic;
    OgreOggSound::OgreOggISound* soundFeedbackGreat;
    OgreOggSound::OgreOggISound* soundFeedbackGood;
    OgreOggSound::OgreOggISound* soundFeedbackBad;
    OgreOggSound::OgreOggISound* soundCollision;
    OgreOggSound::OgreOggISound* soundStartup;
    OgreOggSound::OgreOggISound* soundBoost;
    OgreOggSound::OgreOggISound* soundButtonPress;
    std::vector<OgreOggSound::OgreOggISound*> soundPods;
    bool triggerStartup;
    
    // Study report at the end of the session
    int numStagesWon;
    
    // Stage
    int levelRequestRow;
    int levelRequestCol;
    // Level Selection
    int menuRowIndex;
    
    // Level Results
    std::vector< std::vector<PlayerProgress> > levelProgress;
    
    // Tutorials
    TutorialManager* tutorialMgr;
    
public:
    // Settings Parameters
    float musicVolume;
    float soundVolume;
    float maxVel;
    float minVelStopper;
    float dampingDecayFree;
    float dampingDecayStop;
    float dampingDropFree;
    float dampingDropStop;
    
    float offsetRoll;
    float offsetRollDest;
    
	Player();
	Player(const std::string & name, Vector3 camPos, Quaternion camRot, float camSpeed, float offset, unsigned seed, const std::string & filename);
	
    LevelSet* getLevels() const;
    unsigned getSeed() const;
    std::string getName() const;
    int getHP() const;
    int getNumCorrectTotal() const;
    int getNumSafeTotal() const;
    int getNumMissedTotal() const;
    int getNumWrongTotal() const;
    int getNumCorrectBonus() const;
    int getNumCorrectCombo() const;
    int getNumWrongCombo() const;
    int getNumCollisionsTotal() const;
    float getScore() const;
	bool getMouseLeft() const;
	bool getKeyUp() const;
	bool getKeyDown() const;
	bool getKeyLeft() const;
	bool getKeyRight() const;
	bool getKeySpace() const;
	Direction getCamDir() const;
	Direction getVineDir() const;
	Direction getVineDest() const;
	Vector2 getMousePos() const;
	Vector3 getOldPos() const;
	Vector3 getCamPos() const;
    Vector3 getPos() const;
	Quaternion getOldRot() const;
    float getOldRoll() const;
	Quaternion getCamRot() const;
	float getCamRoll() const;
	Quaternion getDesireRot() const;
	float getDesireRoll() const;
	float getBaseSpeed() const;
	float getFinalSpeed() const;
	float getTotalSpeed() const;
	float getVineOffset() const;
	float getTotalElapsed() const;
	float getTotalDistanceTraveled() const;
	float getAnimationTimer() const;
    float getAccuracy() const;
    float getProgress() const;
    bool getShowCombo() const;
    PlayerLevel getSkillLevel() const;
    int getToggleBack() const;
    bool getGodMode() const;
    int getNumStagesWon() const;
    int getLevelRequestRow() const;
    int getLevelRequestCol() const;
    bool hasLevelProgress(int level) const;
    bool hasLevelProgress(int row, int col) const;
    PlayerProgress getLevelProgress(int level) const;
    PlayerProgress getLevelProgress(int row, int col) const;
    bool isLevelAvailable(int level) const;
    bool isLevelAvailable(int row, int col) const;
    int getMenuRowIndex() const;
    int getTotalLevelRating(int row) const;
    float getTotalLevelScore(int row) const;
    float getTotalLevelScore() const;
    bool hasTriggeredStartup() const;
    float getScoring() const;
    TutorialManager* getTutorialMgr() const;
    
    void setRunningSpeed(int val1, int val2, int val3, int val4, int nav);
    void setSpeedParameters(int initSpeed, int minSpeed, int maxSpeed);
    void setSeed(unsigned value);
    void setName(const std::string & name);
    void setHP(int value);
    void setNumCorrectTotal(int value);
    void setNumWrongTotal(int value);
    void setNumCorrectCombo(int value);
    void setNumWrongCombo(int value);
    void setScore(float value);
	void setMouseLeft(bool value);
	void setKeyUp(bool value);
	void setKeyDown(bool value);
	void setKeyLeft(bool value);
	void setKeyRight(bool value);
	void setKeySpace(bool value);
	
	void setCamDir(Direction value);
	bool setVineDirRequest(Direction value, bool force = false);
	void setMousePos(Vector2 value);
	void setOldPos(Vector3 value);
	void setCamPos(Vector3 value);
    void setPos(Vector3 value);
	void setOldRot(Quaternion value);
    void setOldRoll(float value);
	void setCamRot(Quaternion value);
    void setCamRoll(float value);
	void setDesireRot(Quaternion value);
    void setDesireRoll(float value);
    void setBaseSpeed(float value);
    void setSkillLevel(PlayerLevel value);
    void setToggleBack(int value);
    void setGodMode(bool value);
    void setLevelRequestRow(int value);
    void setLevelRequestCol(int value);
    void setLevelRequest(int row, int col);
    void setMenuRowIndex(int value);
    void setAllProgressTo(const PlayerProgress & value);
    void saveCam();
    void revertCam();
	Vector3 getCamForward(bool combined = true) const;
	Vector3 getCamUpward(bool combined = true) const;
	Vector3 getCamRight(bool combined = true) const;
    Quaternion getRot() const;
    Quaternion getRoll() const;
    Quaternion getCombinedRotAndRoll() const;
    void playPodSound(int index) const;
    void reactGUI() const;
    float getStartMusicTimer() const;
    void playMusic() const;
    void setVolume();
    void unpause();
    void pause();
    
    void setSounds(bool mode);
    
    void unlink();
    void link(Tunnel* tunnel);
    void initToggleBack();
    void newTunnel(const std::string & nameMusic);
    void startMenu();
    void saveSpeedSettings();
    
	void move(Vector3 delta);
    void changeMovementMode();
    void setShowCombo(bool value);
	void addVine(Vine* vine);
    void removeVines();
	void checkCollisions();
    void decideFinalSpeed(float elapsed);
    
    void addAction(ActionCode actType);
    void updateTractorBeam(float elapsed);
    void performTractorBeam();
    void updateTimeWarp(float elapsed);
    void performTimeWarp();
    void updateBoost(float elapsed);
    void performBoost();
    void updateShields(float elapsed);
    bool triggerShields();
    void performShields();
    void updateGlowExtraction(float elapsed);
    void setGlowGrabParameters();
    void beginBadFuelPickUp();
    void updateBadFuelPickUp(float elapsed);
    void testPodGiveFeedback(Pod* test);
    void determineSpawnCombo();
    void offsetShip(float elapsed);
    void recordInfo();
	void update(float elapsed);
    
    void initPowerUps();
    void setPowerUp(std::string pwr, bool value);
    bool isPowerUpAvailable(std::string pwr);
    bool isPowerUpActive(std::string pwr);
    Powerup* getPowerUpPtr(std::string pwr);
    void performPowerUp(std::string pwr);
    void destroyPowerUps();
    void resetPowerUps();
    
    void incrementNumStagesWon();
    void calculateNavigationScores();
    void calculateSpeedScores();
    std::string getCurrentStats() const;
    
    void saveAllResults(Evaluation eval);
    bool saveStage(std::string file);
    bool saveActions(std::string file);
    bool saveSession(std::string file);
    //bool saveProgress(std::string file, bool updateSessionID);
    bool saveProgress(std::string file);
    bool loadProgress(std::string savePath);
    
    void initSettings();
    
    bool winFlag;
    
    ~Player();
};

#endif /* defined(__Testing__Player__) */
