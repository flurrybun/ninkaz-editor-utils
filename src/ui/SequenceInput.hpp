#pragma once

#include <Geode/ui/TextInput.hpp>
#include <Geode/utils/cocos.hpp>
#include <Geode/utils/function.hpp>
using namespace geode::prelude;

class SequenceInput : public CCNode {
public:
    using InputCallback = Function<void(const std::string& text)>;

    struct Options {
        std::string initialValue;
        std::optional<ZStringView> label;
    };

private:
    Options m_options;
    InputCallback m_callback;
    Ref<TextInput> m_input;

    bool init(InputCallback callback, Options options);

public:
    static SequenceInput* create(InputCallback callback, Options options = {});
};
