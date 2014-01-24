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
#include "AIStateAttack.h"
#include "AIStateDead.h"
#include "Utils.h"
#include "ActorProperty.h"
#include "GameContext.h"

USING_NS_CC;

AI::AI(ActorProperty* property, const cocos2d::Point& initialPosition, const std::string& direction, float speed, float idleDuration, float moveDistance)
: pAIStateIdle_(new AIStateIdle(this))
, pAIStateMove_(new AIStateMove(this))
, pAIStateAttack_(new AIStateAttack(this))
, pAIStateDead_(new AIStateDead(this))
, pAISprite_(0)
, speedMove_(speed)
, idleDuration_(idleDuration)
, autoMoveDistance_(moveDistance)
, mState_(AI_STATE_NONE)
, mPrevState_(AI_STATE_NONE)
, autoLogic_(true)
, currentIdleDuration_(0)
, currentMoveDistance_(0)
, property_(property)
{
    init(initialPosition, direction);
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

void AI::init(const cocos2d::Point& initialPosition, const std::string& direction)
{
    pAISprite_ = Sprite::create();
    pAISprite_->setAnchorPoint(Point::ZERO);
    setPosition(initialPosition);
    CC_SAFE_RETAIN(pAISprite_);
    
    actions_.resize(ACTION_COUNT, 0);
    RepeatForever* move = RepeatForever::create(Animate::create(AnimationCache::getInstance()->getAnimation(property_->action_move.c_str())));
    actions_.at(ACTION_MOVE) = move;
    CC_SAFE_RETAIN(move);
    RepeatForever* idle = RepeatForever::create(Animate::create(AnimationCache::getInstance()->getAnimation(property_->action_idle.c_str())));
    actions_.at(ACTION_IDLE) = idle;
    CC_SAFE_RETAIN(idle);
    Animate* attack = Animate::create(AnimationCache::getInstance()->getAnimation(property_->action_attack.c_str()));
    actions_.at(ACTION_ATTACK) = attack;
    CC_SAFE_RETAIN(attack);
    Animate* dead = Animate::create(AnimationCache::getInstance()->getAnimation(property_->action_dead.c_str()));
    actions_.at(ACTION_DEAD) = dead;
    CC_SAFE_RETAIN(dead);
    
    if (0 == std::strcmp(direction.c_str(), "left"))
    {
        direction_ = LEFT;
    }
    else
    {
        direction_ = RIGHT;
    }
    
    initialPosition_ = pAISprite_->getPosition();
    float x = (direction_ == LEFT) ? (initialPosition_.x + autoMoveDistance_) : (initialPosition_.x - autoMoveDistance_);
    float y = initialPosition_.y;
    autoMoveEndPosition_ = Point(x, y);
    currentTargetPosition_ = initialPosition_;
    
    if (0 != std::strcmp(direction.c_str(), property_->direction.c_str()))
    {
        pAISprite_->setFlippedX(true);
    }
    
    if (changeState(AI_STATE_IDLE))
    {
        pAIStateIdle_->enter();
    }
}

void AI::update(float delta)
{
//    doAutoLogic(delta);
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
                    switchDirection(direction_ == LEFT ? RIGHT : LEFT);
                    currentTargetPosition_ = currentTargetPosition_ == initialPosition_ ? autoMoveEndPosition_ : initialPosition_;
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
                pAISprite_->setPositionX(currentTargetPosition_.x);
                stop();
                currentMoveDistance_ = 0;
            }
            else
            {
                float nextX = currentX + addDistance * (direction_ == LEFT ? -1 : 1);
                pAISprite_->setPositionX(nextX);
            }
        }
        default:
            break;
    }
}

void AI::setPosition(const cocos2d::Point &pos)
{
    Point position = Point(pos.x - property_->bodyRect.origin.x, pos.y - property_->bodyRect.origin.y);
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

void AI::attack()
{
    if (changeState(AI_STATE_ATTACK))
    {
        pAIStateAttack_->enter();
    }
}

void AI::dead()
{
    if (changeState(AI_STATE_DEAD))
    {
        pAIStateDead_->enter();
    }
    GameContext::getInstance().cleanAICanAttack(this);
}

void AI::switchDirection(Actor::Direction direction)
{
    if (direction_ != direction)
    {
        direction_ = direction;
        pAISprite_->setFlippedX(!pAISprite_->isFlippedX());
    }
}

Rect AI::getAttackRect()
{
    Rect rect = direction_ == RIGHT ? property_->rightAttackRect : property_->leftAttackRect;
    return Rect(pAISprite_->getPositionX() + rect.origin.x, pAISprite_->getPositionY() + rect.origin.y, rect.size.width, rect.size.height);
}

Rect AI::getCollisionBodyRect()
{
    return cocos2d::Rect(pAISprite_->getPositionX() + property_->bodyRect.origin.x, pAISprite_->getPositionY() + property_->bodyRect.origin.y, property_->bodyRect.size.width, property_->bodyRect.size.height);
}
