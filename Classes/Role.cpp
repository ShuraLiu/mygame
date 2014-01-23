//
//  Role.cpp
//  mygame
//
//  Created by 刘 潇逸 on 14-1-21.
//
//

#include "Role.h"
#include "RoleStateIdle.h"
#include "RoleStateMove.h"
#include "RoleStatePreClimb.h"
#include "RoleStateClimb.h"
#include "config.h"
#include "Ladder.h"
#include "ActorProperty.h"

USING_NS_CC;

Role::Role(ActorProperty* property)
: pRoleStateIdle_(new RoleStateIdle(this))
, pRoleStateMove_(new RoleStateMove(this))
, pRoleStatePreClimb_(new RoleStatePreClimb(this))
, pRoleStateClimb_(new RoleStateClimb(this))
, pCurrentState_(0)
, pRoleSprite_(0)
, speedClimb_(0)
, speedMove_(0)
, mState_(ROLE_STATE_NONE)
, mPrevState_(ROLE_STATE_NONE)
, positionToMove_(Point::ZERO)
, property_(property)
{
    init();
}

Role::~Role()
{
    CC_SAFE_RELEASE_NULL(pRoleSprite_);
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

void Role::init()
{
    pRoleSprite_ = Sprite::create();
    pRoleSprite_->setAnchorPoint(Point::ZERO);
    pRoleSprite_->setPosition(Point::ZERO);
    CC_SAFE_RETAIN(pRoleSprite_);
    speedMove_ = ROLE_MOVE_SPEED;
    speedClimb_ = ROLE_CLIMB_SPEED;
    
    actions_.resize(ACTION_COUNT, 0);
    RepeatForever* action = RepeatForever::create(Animate::create(AnimationCache::getInstance()->getAnimation(property_->action_move.c_str())));
    actions_.at(ACTION_MOVE) = action;
    CC_SAFE_RETAIN(action);
    
    if (std::strcmp(property_->direction.c_str(), "left"))
    {
        direction_ = LEFT;
    }
    else
    {
        direction_ = RIGHT;
    }
    
    if (changeState(ROLE_STATE_IDLE))
    {
        pRoleStateIdle_->enter();
    }
}

void Role::update(float delta)
{
    switch (mState_) {
        case ROLE_STATE_MOVE:
        {
            float currentX = pRoleSprite_->getPositionX();
            float nextX = currentX + delta * speedMove_ * (positionToMove_.x > currentX ? 1 : -1);
            if ((currentX - positionToMove_.x) * (nextX - positionToMove_.x) < 0)
            {
                pRoleSprite_->setPositionX(positionToMove_.x);
                stop();
            }
            else
            {
                pRoleSprite_->setPositionX(nextX);
            }
        }
            break;
        case ROLE_STATE_PRE_CLIMB:
        {
            float currentX = pRoleSprite_->getPositionX();
            float nextX = currentX + delta * speedMove_ * (positionBeforeClimb_.x > currentX ? 1 : -1);
            if ((currentX - positionBeforeClimb_.x) * (nextX - positionBeforeClimb_.x) < 0)
            {
                pRoleSprite_->setPositionX(positionBeforeClimb_.x);
                if (changeState(ROLE_STATE_CLIMB))
                {
                    pRoleStateClimb_->enter();
                }
            }
            else
            {
                pRoleSprite_->setPositionX(nextX);
            }
        }
            break;
        case ROLE_STATE_CLIMB:
        {
            float currentY = pRoleSprite_->getPositionY();
            float nextY = currentY + delta * speedClimb_ * (positionAfterClimb_.y > currentY ? 1 : -1);
            if ((currentY - positionAfterClimb_.y) * (nextY - positionAfterClimb_.y) < 0)
            {
                pRoleSprite_->setPositionY(positionAfterClimb_.y);
                stop();
            }
            else
            {
                pRoleSprite_->setPositionY(nextY);
            }
        }
            break;
        default:
            break;
    }
    if (mState_ == ROLE_STATE_MOVE)
    {
        
    }
    else if (mState_ == ROLE_STATE_PRE_CLIMB)
    {
        
    }
}

void Role::setRoleState(RoleState *state)
{
    pCurrentState_ = state;
}

void Role::setPosition(const cocos2d::Point &pos)
{
    Point position = Point(pos.x - property_->bodyRect.origin.x, pos.y - property_->bodyRect.origin.y);
    pRoleSprite_->setPosition(position);
}

void Role::addRoleToLayer(cocos2d::Layer *layer)
{
    layer->addChild(pRoleSprite_, ROLE_Z_ORDER);
}

void Role::moveTo(const cocos2d::Point &pos)
{
    if (changeState(ROLE_STATE_MOVE))
    {
        const Rect& bodyRect = getBodyRect();
        if (pos.y >= bodyRect.origin.y && pos.y < bodyRect.origin.y + bodyRect.size.height * 2)
        {
            positionToMove_.x = pos.x - (bodyRect.origin.x + bodyRect.size.width / 2) + pRoleSprite_->getPositionX();
            positionToMove_.y = pRoleSprite_->getPositionY();
            switchDirection(positionToMove_.x < pRoleSprite_->getPositionX() ? LEFT : RIGHT);
            pRoleStateMove_->enter();
        }
    }
}

void Role::climb(Ladder *ladder, bool isClimbUp)
{
    auto pSpriteLadder = ladder->getSpriteLadder();
    Point midDown = Point(pSpriteLadder->getPositionX() + pSpriteLadder->getContentSize().width / 2, pSpriteLadder->getPositionY());
    Point midUp = Point(pSpriteLadder->getPositionX() + pSpriteLadder->getContentSize().width / 2, pSpriteLadder->getPositionY() + pSpriteLadder->getContentSize().height);
    
    Point p1 = Point(midDown.x - property_->bodyRect.origin.x - property_->bodyRect.size.width / 2, midDown.y - property_->bodyRect.origin.y);
    Point p2 = Point(p1.x, midUp.y - property_->bodyRect.origin.y);
    positionBeforeClimb_ = isClimbUp ? p1 : p2;
    positionAfterClimb_ = isClimbUp ? p2 : p1;
    if (changeState(ROLE_STATE_PRE_CLIMB))
    {
        switchDirection(positionBeforeClimb_.x < pRoleSprite_->getPositionX() ? LEFT : RIGHT);
        pRoleStatePreClimb_->enter();
    }
}

bool Role::changeState(ROLE_STATE state)
{
    if (mState_ == ROLE_STATE_DEAD)
    {
        return  false;
    }
    pRoleSprite_->stopAllActions();
    mPrevState_ = mState_;
    mState_ = state;
    return true;
}

void Role::stop()
{
    if (changeState(ROLE_STATE_IDLE))
    {
        pRoleStateIdle_->enter();
    }
}

Rect Role::getCollisionBodyRect()
{
    const Rect& bodyRect = property_->bodyRect;
    return cocos2d::Rect(pRoleSprite_->getPositionX() + bodyRect.origin.x, pRoleSprite_->getPositionY() + bodyRect.origin.y, bodyRect.size.width, bodyRect.size.height);
}

void Role::switchDirection(Direction direction)
{
    direction_ = direction;
    pRoleSprite_->setFlippedX(direction_ == LEFT);
}
