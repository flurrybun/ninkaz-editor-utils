#include <Geode/modify/LevelEditorLayer.hpp>

#include <Geode/Geode.hpp>
using namespace geode::prelude;

// instead of trying to port how gd shows the backs of portals into the editor, i just reimplemented it differently.
// this is because portal backs are their own object (id 38) created when the level is loaded with all the portal's
// properties duplicated over. if you've ever wondered why portals can't be used as center points, that's because
// the portal and portal back are two separate objects but with the same groups.

// trying to replicate that behavior would open up a whole can of worms, so i just do it this way.

class $modify(LevelEditorLayer) {
    struct Fields {
        std::unordered_map<GameObject*, CCSprite*> portalBacks;
    };

    $override
    bool init(GJGameLevel* p0, bool p1) {
        if (!LevelEditorLayer::init(p0, p1)) return false;

        for (const auto& object : CCArrayExt<GameObject>(m_objects)) {
            tryAddPortalBack(object);
        }

        return true;
    }

    $override
    GameObject* createObject(int p0, CCPoint p1, bool p2) {
        GameObject* object = LevelEditorLayer::createObject(p0, p1, p2);

        tryAddPortalBack(object);

        return object;
    }

    $override
    void updateVisibility(float dt) {
        LevelEditorLayer::updateVisibility(dt);

        for (const auto& [portal, back] : m_fields->portalBacks) {
            back->setPosition(portal->getPosition());
            back->setRotationX(portal->getRotationX());
            back->setRotationY(portal->getRotationY());
            back->setScaleX(portal->getScaleX());
            back->setScaleY(portal->getScaleY());
            back->setFlipX(portal->isFlipX());
            back->setFlipY(portal->isFlipY());

            back->setVisible(portal->isVisible());
            back->setOpacity(portal->getOpacity());
            back->setColor(portal->getColor());
        }
    }

    void tryAddPortalBack(GameObject* object) {
        if (!LevelEditorLayer::get()) return;

        std::optional<const char*> backFrameName = getPortalBackFrameName(object->m_objectID);
        if (!backFrameName.has_value()) return;

        auto back = CCSprite::createWithSpriteFrameName(backFrameName.value());
        LevelEditorLayer::get()->m_game2LayerB0->addChild(back);
        back->setZOrder(object->getZOrder() - 100);

        m_fields->portalBacks[object] = back;
    }

    std::optional<const char*> getPortalBackFrameName(int objectID) {
        switch (objectID) {
            case 10: return "portal_01_back_001.png"; // blue gravity
            case 11: return "portal_02_back_001.png"; // yellow gravity
            case 12: return "portal_03_back_001.png"; // cube
            case 13: return "portal_04_back_001.png"; // ship
            case 45: return "portal_05_back_001.png"; // orange mirror
            case 46: return "portal_06_back_001.png"; // blue mirror
            case 47: return "portal_07_back_001.png"; // ball
            case 99: return "portal_08_back_001.png"; // green size
            case 101: return "portal_09_back_001.png"; // pink size
            case 111: return "portal_10_back_001.png"; // ufo
            case 286: return "portal_11_back_001.png"; // dual
            case 287: return "portal_12_back_001.png"; // exit dual
            case 660: return "portal_13_back_001.png"; // wave
            case 745: return "portal_14_back_001.png"; // robot
            case 747: // linked blue teleport
            case 2902: // standalone blue teleport
                return "portal_15_back_001.png";
            case 749: // linked orange teleport
            case 2064: // standalone orange teleport
                return "portal_16_back_001.png";
            case 1331: return "portal_17_back_001.png"; // spider
            case 1933: return "portal_18_back_001.png"; // swing
            case 2926: return "portal_19_back_001.png"; // green gravity
            default: return std::nullopt;
        }
    }
};
