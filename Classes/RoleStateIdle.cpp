//
//  RoleStateIdle.cpp
//  mygame
//
//  Created by 刘 潇逸 on 14-1-21.
//
//

#include "RoleStateIdle.h"
#include "Role.h"
#include "cocos2d.h"
#include "GameContext.h"

USING_NS_CC;

RoleStateIdle::RoleStateIdle(Role* pRole)
: pRole_(pRole)
{
    init();
}

RoleStateIdle::~RoleStateIdle()
{
    
}

void RoleStateIdle::enter()
{
    Sprite* pRoleSprite = pRole_->getRoleSprite();
    pRoleSprite->setSpriteFrame("walk_02.png");
}

void RoleStateIdle::exit()
{
    
}

void RoleStateIdle::init()
{
    
}

void RoleStateIdle::update(float delta)
{

}