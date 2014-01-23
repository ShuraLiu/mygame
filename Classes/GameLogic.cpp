//
//  GameLogic.cpp
//  mygame
//
//  Created by 刘 潇逸 on 14-1-21.
//
//

#include "GameLogic.h"
#include "GameContext.h"
#include "PlayLayer.h"

USING_NS_CC;

GameLogic::GameLogic(IObserver& observer)
: observer_(observer)
, status_(GAME_STATUS_NONE)
, touch_(false)
{
    
}

GameLogic::~GameLogic()
{
    
}

void GameLogic::update(float delta)
{
    switch (status_) {
        case GAME_STATUS_NONE:
        {
            Texture2D::setDefaultAlphaPixelFormat(Texture2D::PixelFormat::RGBA4444);
            Director::getInstance()->getTextureCache()->addImage("game_1.pvr.ccz");
            SpriteFrameCache::getInstance()->addSpriteFramesWithFile("game_1.plist");
            AnimationCache::getInstance()->addAnimationsWithFile("animations.plist");
            GameContext::getInstance().initTiledMap("game_1.tmx");
            GameStatus prevStatus = status_;
            status_ = GAME_STATUS_INIT;
            observer_.onStatusChanged(prevStatus, status_);
        }
            break;
        case GAME_STATUS_INIT:
        {
            GameStatus prevStatus = status_;
            status_ = GAME_STATUS_PLAYING;
            observer_.onStatusChanged(prevStatus, status_);
        }
            break;
        case GAME_STATUS_PLAYING:
        {
            GameContext& context = GameContext::getInstance();
            context.updateGameContext(delta);
            GameContext::SceneObjectArray& ladderArray = GameContext::getInstance().getLadderArray();
            if (context.getRole()->getCurrentState() != ROLE_STATE_PRE_CLIMB
                && context.getRole()->getCurrentState() != ROLE_STATE_CLIMB)
            {
                for (int i = 0; i < ladderArray.size(); ++i)
                {
                    Ladder* ladder = (Ladder*) ladderArray.at(i);
                    if (!ladder->isCollisioned() && (checkCollision(context.getRole()->getBodyRect(), ladder->getCollsionRectDown()) || checkCollision(context.getRole()->getBodyRect(), ladder->getCollsionRectUp())))
                    {
                        ladder->onCollision();
                        observer_.onLadderCanClimb(ladder);
                    }
                    if (ladder->isCollisioned() && (!checkCollision(context.getRole()->getBodyRect(), ladder->getCollsionRectDown()) && !checkCollision(context.getRole()->getBodyRect(), ladder->getCollsionRectUp())))
                    {
                        ladder->noCollision();
                        observer_.onLadderCanNotClimb(ladder);
                    }
                }
            }
            else
            {
                for (int i = 0; i < ladderArray.size(); ++i)
                {
                    Ladder* ladder = (Ladder*) ladderArray.at(i);
                    ladder->noCollision();
                    observer_.onLadderCanNotClimb(ladder);
                }
            }
        }
            break;
        default:
            break;
    }
}

void GameLogic::addMapToLayer(cocos2d::Layer *layer)
{
    layer->addChild(GameContext::getInstance().getMap());
    GameContext::getInstance().addSceneObjectsToLayer(layer);
    GameContext::getInstance().addAIsToLayer(layer);
    GameContext::getInstance().getRole()->addRoleToLayer(layer);
}

void GameLogic::climbLadder(Ladder *ladder)
{
    GameContext& context = GameContext::getInstance();
    if (checkCollision(context.getRole()->getBodyRect(), ladder->getCollsionRectDown()))
    {
        context.getRole()->climb(ladder, true);
    }
    else if (checkCollision(context.getRole()->getBodyRect(), ladder->getCollsionRectUp()))
    {
        context.getRole()->climb(ladder, false);
    }
}

bool GameLogic::onTouchBegan(Layer* layer, Touch *touch, Event *unused_event)
{
    PlayLayer* pLayer = dynamic_cast<PlayLayer*>(layer);
    if (pLayer)
    {
        if (pLayer->getBoundingBox().containsPoint(touch->getLocation()))
        {
             touch_ = true;
        }
    }
    return true;
}

void GameLogic::onTouchMoved(Layer* layer, Touch *touch, Event *unused_event)
{
    PlayLayer* pLayer = dynamic_cast<PlayLayer*>(layer);
    if (pLayer)
    {
        if (pLayer->getBoundingBox().containsPoint(touch->getLocation()))
        {
            touch_ = false;
        }
    }
}

void GameLogic::onTouchEnded(Layer* layer, Touch *touch, Event *unused_event)
{
    PlayLayer* pLayer = dynamic_cast<PlayLayer*>(layer);
    if (pLayer)
    {
        if (pLayer->getBoundingBox().containsPoint(touch->getLocation()))
        {
            if (touch_)
            {
                touch_ = false;
                Point pos = pLayer->convertToNodeSpace(touch->getLocation());
                GameContext::getInstance().getRole()->moveTo(pos);
            }
        }
    }
}

void GameLogic::onTouchCancelled(Layer* layer, Touch *touch, Event *unused_event)
{
    if (touch_)
    {
        touch_ = false;
    }
}

bool GameLogic::checkCollision(const cocos2d::Rect &rect1, const cocos2d::Rect &rect2)
{
    return rect1.intersectsRect(rect2);
}
