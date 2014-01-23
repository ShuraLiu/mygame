//
//  AI.cpp
//  mygame
//
//  Created by 刘 潇逸 on 14-1-23.
//
//

#include "AI.h"
#include "config.h"
#include "AIStateIdle.h"
#include "AIStateMove.h"
#include "Utils.h"

USING_NS_CC;

AI::AI()
: pAIStateIdle_(new AIStateIdle(this))
, pAIStateMove_(new AIStateMove(this))
, pAISprite_(0)
, speedMove_(0)
, mState_(AI_STATE_NONE)
, mPrevState_(AI_STATE_NONE)
, autoLogic_(true)
, currentIdleDuration_(0)
, idleDuration_(0)
, currentMoveDistance_(0)
{
    init();
}

AI::~AI()
{
    CC_SAFE_RELEASE_NULL(pAISprite_);
    ActionArray::iterator it = actions_.begin();
    ActionArray::iterator end = actions_.end();
    for (; it != end; ++it)
    {
        if ((*it))
        {
            (*it)->release();
        }
    }
}

void AI::init()
{
    pAISprite_ = Sprite::create();
    pAISprite_->setAnchorPoint(Point::ZERO);
    pAISprite_->setPosition(Point::ZERO);
    CC_SAFE_RETAIN(pAISprite_);
    bodyRect_ = Rect(183, 20, 131, 130);
    attackRect_ = Rect(236, 28, 300, 165);
    speedMove_ = 100;
    idleDuration_ = 2;
    autoMoveDistance_ = 200;
    
    actions_.resize(ACTION_COUNT, 0);
    RepeatForever* action = RepeatForever::create(Animate::create(AnimationCache::getInstance()->getAnimation("role_run")));
    actions_.at(ACTION_MOVE) = action;
    CC_SAFE_RETAIN(action);
    
    if (changeState(AI_STATE_IDLE))
    {
        pAIStateIdle_->enter();
    }
}

void AI::update(float delta)
{
    doAutoLogic(delta);
}

void AI::doAutoLogic(float delta)
{
    switch (mState_) {
        case AI_STATE_IDLE:
        {
            currentIdleDuration_ += delta;
            if (utils::floatGreaterEuqalCompare(currentIdleDuration_, idleDuration_))
            {
                if (changeState(AI_STATE_MOVE))
                {
                    currentIdleDuration_ = 0;
                    pAISprite_->setFlippedX(!pAISprite_->isFlippedX());
                    pAIStateMove_->enter();
                }
            }
        }
            break;
        case AI_STATE_MOVE:
        {
            float currentX = pAISprite_->getPositionX();
            float addDistance = delta * speedMove_;
            currentMoveDistance_ += addDistance;
            if (utils::floatGreaterEuqalCompare(currentMoveDistance_, autoMoveDistance_))
            {
                float nextX = currentX + (addDistance - ((currentMoveDistance_ - autoMoveDistance_))) * (pAISprite_->isFlippedX() ? -1 : 1);
                pAISprite_->setPositionX(nextX - (currentMoveDistance_ - autoMoveDistance_));
                stop();
                currentMoveDistance_ = 0;
            }
            else
            {
                float nextX = currentX + addDistance * (pAISprite_->isFlippedX() ? -1 : 1);
                pAISprite_->setPositionX(nextX);
            }
        }
        default:
            break;
    }
}

void AI::setPosition(const cocos2d::Point &pos)
{
    Point position = Point(pos.x - bodyRect_.origin.x, pos.y - bodyRect_.origin.y);
    pAISprite_->setPosition(position);
}

void AI::addAIToLayer(cocos2d::Layer *layer)
{
    layer->addChild(pAISprite_, ROLE_Z_ORDER - 1);
}

bool AI::changeState(AI_STATE state)
{
    if (mState_ == AI_STATE_DEAD)
    {
        return  false;
    }
    pAISprite_->stopAllActions();
    mPrevState_ = mState_;
    mState_ = state;
    return true;
}

void AI::stop()
{
    if (changeState(AI_STATE_IDLE))
    {
        pAIStateIdle_->enter();
    }
}