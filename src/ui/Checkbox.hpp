#pragma once

#include <Geode/Geode.hpp>
using namespace geode::prelude;

class Checkbox : public CCMenu {
public:
    using ToggleCallback = Function<void(bool enabled)>;

    struct Options {
        bool initialValue;
        std::string label;
    };

private:
    Options m_options;
    ToggleCallback m_callback;
    bool m_value;
    Ref<CCMenuItemToggler> m_toggler;
    Ref<Label> m_label;

    bool init(ToggleCallback callback, Options options);

public:
    static Checkbox* create(ToggleCallback callback, Options options = {});
};
