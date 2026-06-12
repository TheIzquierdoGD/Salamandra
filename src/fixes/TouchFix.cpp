#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/CCTouchDispatcher.hpp>
#include <Geode/modify/CCDirector.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <unordered_set>

using namespace geode::prelude;

static inline std::unordered_set<cocos2d::CCObject*> s_forcePriorityObjects;

struct CustomDirectorHook : Modify<CustomDirectorHook, CCDirector> {
    void setNextScene() {
        s_forcePriorityObjects.clear(); 
        CCDirector::setNextScene();
    }
};

struct CustomTouchDispatcherHook : Modify<CustomTouchDispatcherHook, CCTouchDispatcher> {
    static void onModify(auto& self) {
        (void)self.setHookPriority("cocos2d::CCTouchDispatcher::touches", Priority::Replace);
    }

    void registerForcePrio(cocos2d::CCObject* obj, int prio) {
        s_forcePriorityObjects.insert(obj);
        return CCTouchDispatcher::registerForcePrio(obj, prio);
    }

    void unregisterForcePrio(cocos2d::CCObject* obj) {
        s_forcePriorityObjects.erase(obj);
        return CCTouchDispatcher::unregisterForcePrio(obj);
    }

    template <class Handler>
    struct ParentPath {
        std::vector<CCNode*> path;
        Handler* handler = nullptr;

        ParentPath(Handler* h) : handler(h) {}
        ParentPath(CCNode* node, Handler* h) : handler(h) {
            while (node) {
                path.push_back(node);
                node = node->getParent();
            }
        }

        static std::optional<ParentPath> filtered(CCNode* node, Handler* h, CCNode* filterNode) {
            ParentPath result{h};
            bool isConfirmed = false;
            while (node) {
                result.path.push_back(node);
                if (node == filterNode) isConfirmed = true;
                node = node->getParent();
            }
            return isConfirmed ? std::make_optional(std::move(result)) : std::nullopt;
        }

        CCNode* leaf() const { return path.empty() ? nullptr : path.front(); }
        CCNode* root() const { return path.empty() ? nullptr : path.back(); }
        CCNode* getAtDepth(size_t depth) const { return (depth < path.size()) ? path[path.size() - 1 - depth] : nullptr; }

        bool isStealing() const {
            if constexpr (std::is_same_v<Handler, CCTargetedTouchHandler>) {
                if (handler->m_bSwallowsTouches) return false;
                if (auto nodeLeaf = leaf()) {
                    if (nodeLeaf->getUserFlag("steals-touch"_spr)) return true;
                    if (typeinfo_cast<TableView*>(nodeLeaf) || typeinfo_cast<BoomScrollLayer*>(nodeLeaf)) return true;
                    if (auto scrollLayer = typeinfo_cast<ScrollLayer*>(nodeLeaf)) return scrollLayer->isStealingTouches();
                }
            }
            return false;
        }

        bool operator<(ParentPath const& other) const {
            if (this->root() != other.root()) {
                auto director = CCDirector::get();
                auto const rootNodes = std::array<CCNode*, 2>{director->m_pRunningScene, director->m_pNotificationNode};
                return std::find(rootNodes.begin(), rootNodes.end(), this->root()) > std::find(rootNodes.begin(), rootNodes.end(), other.root());
            }

            size_t maxPathLength = std::max(path.size(), other.path.size());
            for (size_t i = 1; i < maxPathLength; ++i) {
                auto nodeA = this->getAtDepth(i);
                auto nodeB = other.getAtDepth(i);
                if (!nodeA && nodeB) return this->isStealing();
                if (!nodeB && nodeA) return !other.isStealing();
                if (nodeA != nodeB) {
                    if (nodeA->getZOrder() == nodeB->getZOrder()) 
                        return nodeA->getOrderOfArrival() > nodeB->getOrderOfArrival();
                    return nodeA->getZOrder() > nodeB->getZOrder();
                }
            }
            return false;
        }
    };

    template <class Handler>
    std::vector<ParentPath<Handler>> getRegisteredPaths(CCArray* handlers, std::optional<CCNode*> filterNode) {
        std::vector<ParentPath<Handler>> paths;
        if (!handlers) return paths;
        for (auto handler : CCArrayExt<Handler*>(handlers)) {
            if (!handler || !handler->getDelegate()) continue;
            auto node = typeinfo_cast<CCNode*>(handler->getDelegate());
            if (!node) continue;

            if (filterNode) {
                if (auto filteredPath = ParentPath<Handler>::filtered(node, handler, *filterNode)) 
                    paths.push_back(std::move(filteredPath.value()));
            } else {
                paths.emplace_back(node, handler);
            }
        }
        std::sort(paths.begin(), paths.end());
        return paths;
    }

    bool processSingleTargetedHandler(CCSet* touches, CCTouch* touch, CCEvent* event, unsigned int typeIndex, std::vector<ParentPath<CCTargetedTouchHandler>> const& paths) {
        for (auto const& path : paths) {
            auto delegate = path.handler->getDelegate();
            auto claimedTouches = path.handler->m_pClaimedTouches;
            bool isClaimed = false;

            if (typeIndex == CCTOUCHBEGAN) {
                if ((isClaimed = delegate->ccTouchBegan(touch, event))) {
                    claimedTouches->addObject(touch);
                }
            } else if (claimedTouches->containsObject(touch)) {
                isClaimed = true;
                if (typeIndex == CCTOUCHMOVED) delegate->ccTouchMoved(touch, event);
                else if (typeIndex == CCTOUCHENDED) { delegate->ccTouchEnded(touch, event); claimedTouches->removeObject(touch); }
                else if (typeIndex == CCTOUCHCANCELLED) { delegate->ccTouchCancelled(touch, event); claimedTouches->removeObject(touch); }
            }

            if (isClaimed && path.handler->m_bSwallowsTouches) {
                if (touches) touches->removeObject(touch);
                return true;
            }
        }
        return false;
    }

    void touches(CCSet* touches, CCEvent* event, unsigned int typeIndex) {
        m_bLocked = true;
        if (m_pTargetedHandlers->count() > 0) {
            auto paths = getRegisteredPaths<CCTargetedTouchHandler>(m_pTargetedHandlers, std::nullopt);
            std::vector<CCTouch*> touchesSnapshot;
            for (auto t : *touches) touchesSnapshot.push_back(static_cast<CCTouch*>(t));
            for (auto t : touchesSnapshot) processSingleTargetedHandler(touches, t, event, typeIndex, paths);
        }

        if (m_pStandardHandlers->count() > 0 && touches->count() > 0) {
            for (auto const& path : getRegisteredPaths<CCStandardTouchHandler>(m_pStandardHandlers, std::nullopt)) {
                auto delegate = path.handler->getDelegate();
                if (typeIndex == CCTOUCHBEGAN) delegate->ccTouchesBegan(touches, event);
                else if (typeIndex == CCTOUCHMOVED) delegate->ccTouchesMoved(touches, event);
                else if (typeIndex == CCTOUCHENDED) delegate->ccTouchesEnded(touches, event);
                else if (typeIndex == CCTOUCHCANCELLED) delegate->ccTouchesCancelled(touches, event);
            }
        }
        m_bLocked = false;
        CCTouchDispatcher::touches(touches, event, typeIndex);
    }

    bool processTargetedHandlersWithFilter(CCTouch* touch, CCEvent* event, unsigned int typeIndex, CCNode* filterNode) {
        return processSingleTargetedHandler(nullptr, touch, event, typeIndex, getRegisteredPaths<CCTargetedTouchHandler>(m_pTargetedHandlers, filterNode));
    }
};

struct CustomBaseGameLayerHook : Modify<CustomBaseGameLayerHook, GJBaseGameLayer> {
    bool init() {
        if (!GJBaseGameLayer::init()) return false;
        if (m_uiLayer) m_uiLayer->setZOrder(2);
        return true;
    }
};

class ObjectLayerTouchListener : public CCLayer {
public:
    CCLayer* m_targetObjectLayer;
    static ObjectLayerTouchListener* create(CCLayer* objectLayer) {
        auto ret = new ObjectLayerTouchListener;
        if (ret && ret->init(objectLayer)) { ret->autorelease(); return ret; }
        delete ret; return nullptr;
    }
    bool init(CCLayer* objectLayer) {
        m_targetObjectLayer = objectLayer;
        this->setTouchEnabled(true);
        this->setID("object-layer-touch-listener"_spr);
        return true;
    }
    void registerWithTouchDispatcher() override {
        CCTouchDispatcher::get()->addTargetedDelegate(this, 0, true);
    }
    bool ccTouchBegan(CCTouch* t, CCEvent* e) override { return static_cast<CustomTouchDispatcherHook*>(CCTouchDispatcher::get())->processTargetedHandlersWithFilter(t, e, CCTOUCHBEGAN, m_targetObjectLayer); }
    void ccTouchMoved(CCTouch* t, CCEvent* e) override { static_cast<CustomTouchDispatcherHook*>(CCTouchDispatcher::get())->processTargetedHandlersWithFilter(t, e, CCTOUCHMOVED, m_targetObjectLayer); }
    void ccTouchEnded(CCTouch* t, CCEvent* e) override { static_cast<CustomTouchDispatcherHook*>(CCTouchDispatcher::get())->processTargetedHandlersWithFilter(t, e, CCTOUCHENDED, m_targetObjectLayer); }
    void ccTouchCancelled(CCTouch* t, CCEvent* e) override { static_cast<CustomTouchDispatcherHook*>(CCTouchDispatcher::get())->processTargetedHandlersWithFilter(t, e, CCTOUCHCANCELLED, m_targetObjectLayer); }
};

struct CustomEditorUIHook : Modify<CustomEditorUIHook, EditorUI> {
    bool init(LevelEditorLayer* editorLayer) {
        if (!EditorUI::init(editorLayer)) return false;
        this->addChild(ObjectLayerTouchListener::create(editorLayer->m_objectLayer), -1000);
        return true;
    }
};