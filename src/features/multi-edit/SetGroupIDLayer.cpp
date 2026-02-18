// #include <Geode/modify/SetGroupIDLayer.hpp>
// #include "MultiEditContext.hpp"

// #include <Geode/Geode.hpp>
// using namespace geode::prelude;

// enum class Property {
//     GroupID,
//     EditorLayer,
//     EditorLayer2,
//     ZOrder,
//     Channel,
//     Order
// };

// class $modify(SetGroupIDLayer) {
//     struct Fields : MultiEditContext {
//         SetGroupIDLayer* popup = nullptr;

//         void init(SetGroupIDLayer* popup, CCMenu* buttonMenu) {
//             this->popup = popup;
//             registerSelf(popup, buttonMenu);
//         }

//         float getProperty(GameObject* object, int intProperty) override {
//             Property property = static_cast<Property>(intProperty);

//             switch (property) {
//                 case Property::GroupID:
//                     return 0;
//                 case Property::EditorLayer:
//                     return object->m_editorLayer;
//                 case Property::EditorLayer2:
//                     return object->m_editorLayer2;
//                 case Property::ZOrder:
//                     return object->m_zOrder != 0 ? object->m_zOrder : object->m_defaultZOrder;
//                 default:
//                     break;
//             }

//             if (auto ego = typeinfo_cast<EffectGameObject*>(object)) {
//                 switch (property) {
//                     case Property::Channel:
//                         return ego->m_channelValue;
//                     case Property::Order:
//                         return ego->m_ordValue;
//                     default:
//                         break;
//                 }
//             }

//             return 0;
//         }

//         void setProperty(GameObject* object, int intProperty, float newValue) override {
//             Property property = static_cast<Property>(intProperty);

//             switch (property) {
//                 case Property::GroupID:
//                     object->addToGroup(newValue);
//                     break;
//                 case Property::EditorLayer:
//                     object->m_editorLayer = newValue;
//                     break;
//                 case Property::EditorLayer2:
//                     object->m_editorLayer2 = newValue;
//                     break;
//                 case Property::ZOrder:
//                     object->m_zOrder = newValue;
//                     break;
//                 default:
//                     break;
//             }

//             if (auto ego = typeinfo_cast<EffectGameObject*>(object)) {
//                 switch (property) {
//                     case Property::Channel:
//                         ego->m_channelValue = newValue;
//                         break;
//                     case Property::Order:
//                         ego->m_ordValue = newValue;
//                         break;
//                     default:
//                         break;
//                 }
//             }
//         }

//         bool hasProperty(GameObject* object, int intProperty) override {
//             Property property = static_cast<Property>(intProperty);

//             switch (property) {
//                 case Property::GroupID:
//                 case Property::EditorLayer:
//                 case Property::EditorLayer2:
//                 case Property::ZOrder:
//                     return true;
//                 case Property::Channel:
//                 case Property::Order:
//                     return typeinfo_cast<EffectGameObject*>(object) != nullptr;
//             }
//         }

//         int getPropertyDecimalPlaces(int intProperty) override {
//             return 0;
//         }

//         PropertyBounds getPropertyBounds(int intProperty) override {
//             Property property = static_cast<Property>(intProperty);

//             switch (property) {
//                 case Property::GroupID:
//                     return PropertyBounds(0, 9999);
//                 case Property::EditorLayer:
//                 case Property::EditorLayer2:
//                 case Property::Channel:
//                 case Property::Order:
//                     return PropertyBounds::toInf(0);
//                 case Property::ZOrder:
//                     return PropertyBounds::negInfToInf();
//             }
//         }

//         void onMixedInputApplied(int intProperty, std::optional<float> value) override {
//             Property property = static_cast<Property>(intProperty);

//             switch (property) {
//                 case Property::GroupID:
//                     popup->updateGroupIDButtons();
//                 case Property::EditorLayer:
//                 case Property::EditorLayer2:
//                 case Property::Channel:
//                 case Property::Order:
//                 case Property::ZOrder:
//                     break;
//             }
//         }

//         CCArray* getObjectArray() override {
//             if (popup->m_targetObject) {
//                 return CCArray::createWithObject(popup->m_targetObject);
//             } else {
//                 return popup->m_targetObjects;
//             }
//         }
//     };

//     bool init(GameObject* obj, CCArray* objs) {
//         if (!SetGroupIDLayer::init(obj, objs)) return false;

//         if (auto be = Loader::get()->getLoadedMod("hjfod.betteredit")) {
//             for (auto hook : be->getHooks()) {
//                 if (hook->getDisplayName() != "SetGroupIDLayer::init") continue;
//                 (void)hook->disable();
//             }
//         }

//         // node ids replaces the default button menu so i have to create my own here

//         CCMenu* buttonMenu = CCMenu::create();
//         buttonMenu->setID("button-menu"_spr);
//         m_mainLayer->addChild(buttonMenu);
//         // handleTouchPriority(this);

//         m_fields->init(this, buttonMenu);

//         m_fields->addInput(m_groupIDInput, static_cast<int>(Property::GroupID));
//         m_fields->addInput(m_editorLayerInput, static_cast<int>(Property::EditorLayer));
//         m_fields->addInput(m_editorLayer2Input, static_cast<int>(Property::EditorLayer2));
//         m_fields->addInput(m_zOrderInput, static_cast<int>(Property::ZOrder));
//         if (m_channelInput) m_fields->addInput(m_channelInput, static_cast<int>(Property::Channel));
//         if (m_orderInput) m_fields->addInput(m_orderInput, static_cast<int>(Property::Order));

//         m_fields->setupMixed();

//         return true;
//     }
// };
