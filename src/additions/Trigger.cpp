#include "Trigger.hpp"

#include <Geode/Geode.hpp>
using namespace geode::prelude;

namespace Trigger {
    const short DURATION = 10;
    const short OPACITY = 35;
    const short TARGET_GROUP = 51;
    const short CENTER_GROUP = 71;
    const short EASING = 30;
    const short ITEM = 80;
}

SetupTriggerPopup* Trigger::getTriggerPopup() {
    CCArrayExt<CCNode*> children = CCDirector::sharedDirector()->getRunningScene()->getChildren();

    for (auto child : children) if (auto popup = typeinfo_cast<SetupTriggerPopup*>(child)) return popup;
    return nullptr;
}

float Trigger::getProperty(EffectGameObject* object, short property) {
    auto popup = getTriggerPopup();
    return popup->getTriggerValue(property, object);
}

void Trigger::setProperty(EffectGameObject* object, short property, float newValue) {
    auto popup = getTriggerPopup();

    // we don't have to worry about setting m_gameObject, because this function will only be called
    // when the popup is open, and the popup only opens if multiple objects are selected

    auto savedGameObjects = popup->m_gameObjects;

    popup->m_gameObjects = CCArray::createWithObject(object);
    popup->updateValue(property, newValue);

    popup->m_gameObjects = savedGameObjects;
}

bool Trigger::hasProperty(EffectGameObject* object, short property) {
    auto in = [property](const std::vector<short>& vec) {
        return std::find(vec.begin(), vec.end(), property) != vec.end();
    };

    switch (object->m_objectID) {
        case 901: return in({28, 29, 143, 144, 395, 71, 396, 10, 51});
        case 1616: return in({51});
        case 1049: return in({51});
        case 1268: return in({51, 63, 556, -1, -2});
        case 2067: return in({150, 151, 10, 71, 51});
        case 3033: return in({76, 51, 71, 520, 521, 545, 522, 523, 546});
        case 1346: return in({68, 69, 401, 402, 10, 403, 51, 71, 516, 518, 517, 519});
        case 3016: return in({51, 71, 365, 340, 363, 364, 292, 293, 298, 299, 308, 309, 366, 361, 362});
        case 3660: return in({51, 566, 567, 568, 569, 300, 301, 560, 563, 564, 565});
        case 3661: return in({51, 71});
        case 3032: return in({51, 10, 537, 71, 557, 524});
        case 3006: return in({222, 223, 220, 221, 218, 219, 231, 232, 288, 237, 238, 239, 240, 252, 253, 225, 51, 71, 341, 263, 264, 282});
        case 3007: return in({222, 223, 220, 221, 270, 271, 252, 253, 225, 51, 71, 341, 263, 264, 282});
        case 3008: return in({222, 223, 220, 221, 233, 234, 235, 236, 252, 253, 225, 51, 71, 341, 263, 264, 282});
        case 3009: return in({222, 223, 220, 221, 286, 275, 252, 253, 225, 51, 71, 341, 263, 264, 282});
        case 3010: return in({222, 223, 220, 221, 260, 265, 252, 253, 225, 51, 71, 341, 263, 264, 282});
        case 3011: return in({222, 223, 220, 221, 252, 253, 263, 264, 10, 282, 51, 218, 219, 231, 232, 288, 237, 238, 239, 240});
        case 3012: return in({222, 223, 220, 221, 252, 253, 263, 264, 10, 282, 51, 270, 271});
        case 3013: return in({222, 223, 220, 221, 252, 253, 263, 264, 10, 282, 51, 233, 234, 235, 236});
        case 3014: return in({222, 223, 220, 221, 252, 253, 263, 264, 10, 282, 51, 275});
        case 3015: return in({222, 223, 220, 221, 252, 253, 263, 264, 10, 282, 51, 265});
        case 3024: return in({51});
        case 3029: return in({});
        case 3030: return in({});
        case 3031: return in({});
        case 1817: return in({80, 77, 449});
        case 3614: return in({467, 473, 80, 470, 51});
        case 3615: return in({80, 51, 473});
        case 3617: return in({80});
        case 3619: return in({80, 95, 479, 51});
        case 3620: return in({80, 95, 479, 483, 51, 71, 484});
        case 3641: return in({80});
        case 3607: return in({-1, -2, 437, 438});
        case 3608: return in({51, 71, 547, 548, 549, 550, 552, 553, 554, 555});
        case 3618: return in({51});
        case 1913: return in({371, 10});
        case 1914: return in({71, 213, 454, 10});
        case 2901: return in({28, 29});
        case 2015: return in({68, 10});
        case 2925: return in({});
        case 2016: return in({371, 28, 29, 506});
        case 2900: return in({582, 583, 173});
        case 1934: return in({406, 404, 432, 408, 409, 410, 411});
        case 3605: return in({432, 10, 404, 406, 51, 71, 421, 424, 422, 425, 423, 426});
        case 3602: return in({404, 405, 406, 408, 409, 410, 411, 416, 455, 434, 51, 71, 421, 424, 422, 425, 423, 426});
        case 3603: return in({457, 455, 416, 10, 404, 406, 51, 71, 421, 424, 422, 425, 423, 426});
        case 3604: return in({51, 447, 525});
        case 3606: return in({143, 144});
        case 3612: return in({143, 144});
        case 1615: return in({80});
        case 3613: return in({51, 71});
        case 3662: return in({51});
        case 3609: return in({80, 95, 51, 71});
        case 3640: return in({51, 71});
        case 3643: return in({51});
        case 1812: return in({51});
        case 3600: return in({51, 71});
        case 1932: return in({});
        case 2899: return in({574});
        case 3642: return in({498, 501, 10});
        case 2903: return in({203, 204, 205, 206, 456, 209});
        case 2066: return in({148});
        case 3022: return in({51, 346, 348, 349, 350});
        case 2904: return in({});
        case 2905: return in({175, 176, 180, 179, 181, 182, 177, 512, 290, 291, 51, 183, 191, 10});
        case 2907: return in({175, 176, 180, 179, 181, 182, 177, 512, 290, 51, 10});
        case 2909: return in({10, 176, 175, 191, 179, 181, 182});
        case 2910: return in({180, 189, 10});
        case 2911: return in({175, 180, 176, 10, 179, 189, 191});
        case 2912: return in({180, 189, 10});
        case 2913: return in({179, 181, 10, 176, 290, 291, 51, 71});
        case 2914: return in({179, 10, 176, 71, 181, 290, 291, 51});
        case 2915: return in({180, 189, 10, 176, 181, 71, 191, 51});
        case 2916: return in({176, 180, 10, 290, 291, 51});
        case 2917: return in({180, 189, 10, 179, 512, 290, 291, 51});
        case 2919: return in({176, 10, 51});
        case 2920: return in({176, 10});
        case 2921: return in({176, 10, 179, 180, 189});
        case 2922: return in({176, 10});
        case 2923: return in({10, 176, 191, 175, 179, 180, 189});
        case 2924: return in({180, 189, 10});
        case 3017: return in({222, 223, 220, 221, 218, 219, 231, 232, 237, 238, 239, 240, 344, 225});
        case 3018: return in({222, 223, 220, 221, 270, 271, 344, 225});
        case 3019: return in({222, 223, 220, 221, 233, 234, 235, 236, 344, 225});
        case 3020: return in({222, 223, 220, 221, 275, 344, 225});
        case 3021: return in({222, 223, 220, 221, 260, 265, 344, 225});
        case 3023: return in({344, 225});
    }

    return false;
}

std::string Trigger::getEasingString(EasingType easing) {
    switch (easing) {
        case EasingType::None: return "None";
        case EasingType::EaseInOut: return "Ease In Out"; case EasingType::EaseIn: return "Ease In"; case EasingType::EaseOut: return "Ease Out";
        case EasingType::ElasticInOut: return "Elastic In Out"; case EasingType::ElasticIn: return "Elastic In"; case EasingType::ElasticOut: return "Elastic Out";
        case EasingType::BounceInOut: return "Bounce In Out"; case EasingType::BounceIn: return "Bounce In"; case EasingType::BounceOut: return "Bounce Out";
        case EasingType::ExponentialInOut: return "Exponential In Out"; case EasingType::ExponentialIn: return "Exponential In"; case EasingType::ExponentialOut: return "Exponential Out";
        case EasingType::SineInOut: return "Sine In Out"; case EasingType::SineIn: return "Sine In"; case EasingType::SineOut: return "Sine Out";
        case EasingType::BackInOut: return "Back In Out"; case EasingType::BackIn: return "Back In"; case EasingType::BackOut: return "Back Out";
    }
    return "";
}

short Trigger::getPropertyDecimalPlaces(short property) {
    int twoDecimalPlaces[] = {10, 35, 75, 84, 143, 144};

    if (std::find(std::begin(twoDecimalPlaces), std::end(twoDecimalPlaces), property)
        != std::end(twoDecimalPlaces)) return 2;
    return 0;
}

bool Trigger::canPropertyBeNegative(short property) {
    short negativeProperties[] = {28, 29, 143, 144};

    return std::find(std::begin(negativeProperties), std::end(negativeProperties), property)
        != std::end(negativeProperties);
}