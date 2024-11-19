// #include <Geode/modify/LevelEditorLayer.hpp>
// #include "../buddy/IconLayer.hpp"

// #include <Geode/Geode.hpp>
// using namespace geode::prelude;

// class $modify(LevelEditorLayer) {
//     bool init(GJGameLevel* p0, bool p1) {
//         if (!LevelEditorLayer::init(p0, p1)) return false;

//         if (Mod::get()->getSettingValue<bool>("should-show-icon")) {
//             // setTouchEnabled(false);
//             // EditorUI::get()->setTouchEnabled(false);

//             auto iconLayer = IconLayer::create();
//             iconLayer->setZOrder(10000);
//             addChild(iconLayer);
            
//             // setTouchMode(kCCTouchesOneByOne);
//             iconLayer->setTouchMode(kCCTouchesOneByOne);

//             iconLayer->setTouchEnabled(true);
//             iconLayer->setTouchPriority(-1000000000);
//             // iconLayer->setTouchPriority(-100);

//             // auto isLocked = reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(EditorUI::get()) + 0x3c8);
//             // *isLocked = false;
//         }

//         return true;
//     }
// };