/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2011      Zynga Inc.

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#include "CCScene.h"
#include "CCDirector.h"
#include "CCLayer.h"
#include "CCSprite.h"
#include "CCSpriteBatchNode.h"
#include "CCPhysicsWorld.h"

NS_CC_BEGIN

Scene::Scene()
#ifdef CC_USE_PHYSICS
: _physicsWorld(nullptr)
#endif
{
    _ignoreAnchorPointForPosition = true;
    setAnchorPoint(Point(0.5f, 0.5f));
}

Scene::~Scene()
{
#ifdef CC_USE_PHYSICS
    CC_SAFE_DELETE(_physicsWorld);
#endif
}

bool Scene::init()
{
    bool bRet = false;
     do 
     {
         Director * pDirector;
         CC_BREAK_IF( ! (pDirector = Director::getInstance()) );
         this->setContentSize(pDirector->getWinSize());
         // success
         bRet = true;
     } while (0);
     return bRet;
}

Scene *Scene::create()
{
    Scene *pRet = new Scene();
    if (pRet && pRet->init())
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        CC_SAFE_DELETE(pRet);
        return NULL;
    }
}

std::string Scene::getDescription() const
{
    return StringUtils::format("<Scene | tag = %d>", _tag);
}

#ifdef CC_USE_PHYSICS
Scene *Scene::createWithPhysics()
{
    Scene *pRet = new Scene();
    if (pRet && pRet->initWithPhysics())
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        CC_SAFE_DELETE(pRet);
        return NULL;
    }
}

bool Scene::initWithPhysics()
{
    bool bRet = false;
    do
    {
        Director * pDirector;
        CC_BREAK_IF( ! (pDirector = Director::getInstance()) );
        this->setContentSize(pDirector->getWinSize());
        CC_BREAK_IF(! (_physicsWorld = PhysicsWorld::construct(*this)));
        
        this->scheduleUpdate();
        // success
        bRet = true;
    } while (0);
    return bRet;
}

void Scene::addChild(Node* child)
{
    Node::addChild(child);
}

void Scene::addChild(Node* child, int zOrder)
{
    Node::addChild(child, zOrder);
}

void Scene::addChild(Node* child, int zOrder, int tag)
{
    Node::addChild(child, zOrder, tag);
    
    addChildToPhysicsWorld(child);
}

void Scene::addChildToPhysicsWorld(Node* child)
{
    if (_physicsWorld)
    {
        std::function<void(Node*)> addToPhysicsWorldFunc = nullptr;
        addToPhysicsWorldFunc = [this, &addToPhysicsWorldFunc](Node* node) -> void
        {
            if (node->getPhysicsBody())
            {
                _physicsWorld->addBody(node->getPhysicsBody());
            }
            
            node->getChildren().forEach([addToPhysicsWorldFunc](Node* n){
                addToPhysicsWorldFunc(n);
            });
        };
        
        addToPhysicsWorldFunc(child);
    }
}

void Scene::update(float delta)
{
    Node::update(delta);
    
    if (nullptr != _physicsWorld)
    {
        _physicsWorld->update(delta);
    }
    
}
#endif


NS_CC_END
