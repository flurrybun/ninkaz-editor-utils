#pragma once

#include <Geode/Geode.hpp>
using namespace geode::prelude;

// class DialogueLine : public CCObject {
// public:
//     enum class IconAction {
//         None,
//         Bounce,
//     };
    
//     static DialogueLine* create(std::string, IconAction = IconAction::None);
// protected:
//     std::string m_text;
//     IconAction m_action;
    
//     bool init(std::string, IconAction);
// };

struct DialogueLine {
    std::string text;
    enum class IconAction {
        None,
        Bounce,
    } action;

    DialogueLine(std::string text, IconAction action = IconAction::None) : text(text), action(action) {}
};