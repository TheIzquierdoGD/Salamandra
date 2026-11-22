#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>

using namespace geode::prelude;

#ifndef SALAMANDRA_ENABLE_LEGACY_TOUCH_DISPATCHER
#define SALAMANDRA_ENABLE_LEGACY_TOUCH_DISPATCHER 0
#endif

class $modify(SalamandraBaseGameLayerTouch, GJBaseGameLayer) {
    bool init() {
        if (!GJBaseGameLayer::init()) return false;
        if (m_uiLayer) m_uiLayer->setZOrder(2);
        return true;
    }
};

#if SALAMANDRA_ENABLE_LEGACY_TOUCH_DISPATCHER

#include <Geode/modify/CCTouchDispatcher.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/CCDirector.hpp>
#include <algorithm>
#include <array>
#include <optional>
#include <type_traits>
#include <utility>
#include <vector>

class $modify(SalamandraTouchDispatcher, CCTouchDispatcher) {
    static void onModify(auto& self) {
        (void)self.setHookPriority(
            "cocos2d::CCTouchDispatcher::touches", Priority::Replace
        );
    }

    void registerForcePrio(CCObject* object, int priority, int mode) {
        CCTouchDispatcher::registerForcePrio(object, priority, mode);
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

        static std::optional<ParentPath> filtered(
            CCNode* node, Handler* handler, CCNode* filterNode
        ) {
            ParentPath result(handler);
            bool confirmed = false;
            while (node) {
                result.path.push_back(node);
                confirmed |= (node == filterNode);
                node = node->getParent();
            }
            if (!confirmed) return std::nullopt;
            return result;
        }

        CCNode* leaf() const { return path.empty() ? nullptr : path.front(); }
        CCNode* root() const { return path.empty() ? nullptr : path.back(); }
        CCNode* atDepth(size_t depth) const {
            return depth < path.size() ? path[path.size() - 1 - depth] : nullptr;
        }

        bool isStealing() const {
            if constexpr (std::is_same_v<Handler, CCTargetedTouchHandler>) {
                if (handler->m_bSwallowsTouches) return false;
                if (auto* node = leaf()) {
                    if (node->getUserFlag("steals-touch"_spr)) return true;
                    if (typeinfo_cast<TableView*>(node) ||
                        typeinfo_cast<BoomScrollLayer*>(node)) return true;
                    if (auto* scroll = typeinfo_cast<ScrollLayer*>(node)) {
                        return scroll->isStealingTouches();
                    }
                }
            }
            return false;
        }

        bool operator<(ParentPath const& other) const {
            if (root() != other.root()) {
                auto* director = CCDirector::get();
                auto roots = std::array<CCNode*, 2>{
                    director->m_pRunningScene, director->m_pNotificationNode
                };
                return std::find(roots.begin(), roots.end(), root()) >
                       std::find(roots.begin(), roots.end(), other.root());
            }
            auto maxLen = std::max(path.size(), other.path.size());
            for (size_t i = 1; i < maxLen; ++i) {
                auto* a = atDepth(i);
                auto* b = other.atDepth(i);
                if (!a && b) return isStealing();
                if (!b && a) return !other.isStealing();
                if (a != b) {
                    if (a->getZOrder() == b->getZOrder()) {
                        return a->getOrderOfArrival() > b->getOrderOfArrival();
                    }
                    return a->getZOrder() > b->getZOrder();
                }
            }
            return false;
        }
    };

    template <class Handler>
    std::vector<ParentPath<Handler>> registeredPaths(
        CCArray* handlers, std::optional<CCNode*> filterNode = std::nullopt
    ) {
        std::vector<ParentPath<Handler>> paths;
        if (!handlers) return paths;
        for (auto* handler : CCArrayExt<Handler*>(handlers)) {
            if (!handler || !handler->getDelegate()) continue;
            auto* node = typeinfo_cast<CCNode*>(handler->getDelegate());
            if (!node) continue;
            if (filterNode) {
                auto filtered = ParentPath<Handler>::filtered(node, handler, *filterNode);
                if (filtered) paths.push_back(std::move(*filtered));
            } else {
                paths.emplace_back(node, handler);
            }
        }
        std::sort(paths.begin(), paths.end());
        return paths;
    }

    bool handleTargeted(
        CCSet* touches, CCTouch* touch, CCEvent* event, unsigned int type,
        std::vector<ParentPath<CCTargetedTouchHandler>> const& paths
    ) {
        for (auto const& path : paths) {
            if (touches && !touches->containsObject(touch)) break;
            auto* delegate = path.handler->getDelegate();
            auto* claimed = path.handler->m_pClaimedTouches;
            if (!delegate || !claimed) continue;
            bool wasClaimed = false;

            if (type == CCTOUCHBEGAN) {
                wasClaimed = delegate->ccTouchBegan(touch, event);
                if (wasClaimed) claimed->addObject(touch);
            } else if (claimed->containsObject(touch)) {
                wasClaimed = true;
                if (type == CCTOUCHMOVED) delegate->ccTouchMoved(touch, event);
                if (type == CCTOUCHENDED) {
                    delegate->ccTouchEnded(touch, event);
                    claimed->removeObject(touch);
                }
                if (type == CCTOUCHCANCELLED) {
                    delegate->ccTouchCancelled(touch, event);
                    claimed->removeObject(touch);
                }
            }

            if (wasClaimed && path.handler->m_bSwallowsTouches) {
                if (touches) touches->removeObject(touch);
                return true;
            }
        }
        return false;
    }

    void touches(CCSet* touches, CCEvent* event, unsigned int type) {
        if (!touches) return;
        m_bLocked = true;

        if (m_pTargetedHandlers && m_pTargetedHandlers->count() > 0) {
            auto paths = registeredPaths<CCTargetedTouchHandler>(m_pTargetedHandlers);
            std::vector<CCTouch*> snapshot;
            for (auto* touchObject : *touches) {
                snapshot.push_back(static_cast<CCTouch*>(touchObject));
            }
            for (auto* touch : snapshot) {
                if (touches->containsObject(touch)) {
                    handleTargeted(touches, touch, event, type, paths);
                }
            }
        }

        if (m_pStandardHandlers && touches->count() > 0) {
            auto paths = registeredPaths<CCStandardTouchHandler>(m_pStandardHandlers);
            for (auto const& path : paths) {
                auto* delegate = path.handler->getDelegate();
                if (!delegate) continue;
                if (type == CCTOUCHBEGAN) delegate->ccTouchesBegan(touches, event);
                if (type == CCTOUCHMOVED) delegate->ccTouchesMoved(touches, event);
                if (type == CCTOUCHENDED) delegate->ccTouchesEnded(touches, event);
                if (type == CCTOUCHCANCELLED) delegate->ccTouchesCancelled(touches, event);
            }
        }
        m_bLocked = false;
    }

public:
    bool processTargetedHandlersWithFilter(
        CCTouch* touch, CCEvent* event, unsigned int type, CCNode* filterNode
    ) {
        return handleTargeted(
            nullptr, touch, event, type,
            registeredPaths<CCTargetedTouchHandler>(m_pTargetedHandlers, filterNode)
        );
    }
};

class SalamandraObjectLayerTouchListener : public CCLayer {
    CCLayer* m_targetObjectLayer = nullptr;
public:
    static SalamandraObjectLayerTouchListener* create(CCLayer* layer) {
        auto* ret = new SalamandraObjectLayerTouchListener();
        if (ret && ret->init(layer)) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }

    bool init(CCLayer* layer) {
        if (!CCLayer::init() || !layer) return false;
        m_targetObjectLayer = layer;
        setTouchEnabled(true);
        setID("object-layer-touch-listener"_spr);
        return true;
    }

    void registerWithTouchDispatcher() override {
        CCTouchDispatcher::get()->addTargetedDelegate(this, 0, true);
    }

    bool ccTouchBegan(CCTouch* touch, CCEvent* event) override {
        return static_cast<SalamandraTouchDispatcher*>(CCTouchDispatcher::get())
            ->processTargetedHandlersWithFilter(touch, event, CCTOUCHBEGAN, m_targetObjectLayer);
    }
    void ccTouchMoved(CCTouch* touch, CCEvent* event) override {
        static_cast<SalamandraTouchDispatcher*>(CCTouchDispatcher::get())
            ->processTargetedHandlersWithFilter(touch, event, CCTOUCHMOVED, m_targetObjectLayer);
    }
    void ccTouchEnded(CCTouch* touch, CCEvent* event) override {
        static_cast<SalamandraTouchDispatcher*>(CCTouchDispatcher::get())
            ->processTargetedHandlersWithFilter(touch, event, CCTOUCHENDED, m_targetObjectLayer);
    }
    void ccTouchCancelled(CCTouch* touch, CCEvent* event) override {
        static_cast<SalamandraTouchDispatcher*>(CCTouchDispatcher::get())
            ->processTargetedHandlersWithFilter(touch, event, CCTOUCHCANCELLED, m_targetObjectLayer);
    }
};

class $modify(SalamandraEditorTouch, EditorUI) {
    bool init(LevelEditorLayer* editorLayer) {
        if (!EditorUI::init(editorLayer)) return false;
        if (editorLayer && editorLayer->m_objectLayer) {
            if (auto* listener = SalamandraObjectLayerTouchListener::create(editorLayer->m_objectLayer)) {
                addChild(listener, -1000);
            }
        }
        return true;
    }
};

#endif
