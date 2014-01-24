//
//  AI.h
//  mygame
//
//  Created by 刘 潇逸 on 14-1-23.
//
//

#ifndef __mygame__AI__
#define __mygame__AI__

#include "cocos2d.h"
#include "Actor.h"

class RoleState;

enum AI_STATE
{
    AI_STATE_NONE,
    AI_STATE_IDLE,
    AI_STATE_MOVE,
    AI_STATE_ATTACK,
    AI_STATE_ATTACKED,
    AI_STATE_DEAD,
};

class AI
: public Actor
{
    typedef std::vector<cocos2d::Action*> ActionArray;
    
public:
    void setPosition(const cocos2d::Point& pos);
    void addAIToLayer(cocos2d::Layer* layer);
    void stop();
    
public:
    enum Action
    {
        ACTION_IDLE = 0,
        ACTION_MOVE,
        ACTION_ATTACK,
        ACTION_ATTACKED,
        ACTION_DEAD,
        ACTION_COUNT,
    };
    
public:
    void init();
    void update(float delta);
    
public:
    AI();
    virtual ~AI();
    
    cocos2d::Sprite* getAISprite()
    {
        return pAISprite_;
    }
    
    float getSpeedMove() const
    {
        return speedMove_;
    }
    
    cocos2d::Rect getBodyRect()
    {
        return getCollisionBodyRect();
    }
    
    cocos2d::Rect getCollisionBodyRect()
    {
        return cocos2d::Rect(pAISprite_->getPositionX() + bodyRect_.origin.x, pAISprite_->getPositionY() + bodyRect_.origin.y, bodyRect_.size.width, bodyRect_.size.height);
    }
    
    const cocos2d::Rect& getAttackRect()
    {
        return attackRect_;
    }
    
    const ActionArray& getActions()
    {
        return actions_;
    }
    
    AI_STATE getCurrentState() const
    {
        return mState_;
    }
    
    AI_STATE getPrevState() const
    {
        return mPrevState_;
    }
    
private:
    bool changeState(AI_STATE state);
    void doAutoLogic(float delta);
    
private:
    RoleState* pAIStateIdle_;
    RoleState* pAIStateMove_;
    RoleState* pAIStateAttack_;
    RoleState* pAIStateAttacked_;
    RoleState* pAIStateDead_;
    
    AI_STATE mState_;
    AI_STATE mPrevState_;
    
    ActionArray actions_;
    
    cocos2d::Sprite* pAISprite_;
    cocos2d::Rect bodyRect_;
    cocos2d::Rect attackRect_;
    float speedMove_;
    
    float currentIdleDuration_;
    float currentMoveDistance_;
    float autoMoveDistance_;
    float idleDuration_;
    bool autoLogic_;
};

#endif /* defined(__mygame__AI__) */
