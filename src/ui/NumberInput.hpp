#pragma once

#include <Geode/ui/TextInput.hpp>
#include <Geode/utils/cocos.hpp>
#include <Geode/utils/function.hpp>
using namespace geode::prelude;

class NumberInput : public CCNode {
public:
    using InputCallback = Function<void(double value)>;

    struct Options {
        double initialValue;
        int decimalPlaces;
        std::optional<ZStringView> label;
    };

private:
    Options m_options;
    InputCallback m_callback;
    double m_value;
    Ref<Label> m_label;
    Ref<TextInput> m_input;
    Ref<Button> m_incrementBtn;
    Ref<Button> m_decrementBtn;

    bool init(InputCallback callback, Options options);

public:
    void setValue(double value);
    void setEnabled(bool enabled);

    static NumberInput* create(InputCallback callback, Options options = {});
};
