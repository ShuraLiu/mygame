//
//  AIStateIdle.cpp
//  mygame
//
//  Created by 刘 潇逸 on 14-1-23.
//
//

#include "AIStateIdle.h"
#include "AI.h"
#include "cocos2d.h"
#include "GameContext.h"

USING_NS_CC;

AIStateIdle::AIStateIdle(AI* pAI)
: pAI_(pAI)
{
    init();
}

AIStateIdle::~AIStateIdle()
{
    
}

void AIStateIdle::enter()
{
    pAI_->getAISprite()->setSpriteFrame("player_0_run_0.png");
//    Sprite*  = pRole_->getRoleSprite();
//    pRoleSprite->setSpriteFrame("player_0_run_0.png");
}

void AIStateIdle::exit()
{
    
}

void AIStateIdle::init()
{
    
}

void AIStateIdle::update(float delta)
{
    
}