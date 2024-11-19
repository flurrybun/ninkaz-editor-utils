#include "DialogueLine.hpp"

#include <Geode/Geode.hpp>
using namespace geode::prelude;

// bool DialogueLine::init(std::string text, IconAction action) {
//     m_text = text;
//     m_action = action;
//     return true;
// }

// DialogueLine* DialogueLine::create(std::string text, IconAction action) {
//     auto ret = new DialogueLine();
//     if (ret && ret->init(text, action)) {
//         ret->autorelease();
//         return ret;
//     }
//     CC_SAFE_DELETE(ret);
//     return nullptr;
// }