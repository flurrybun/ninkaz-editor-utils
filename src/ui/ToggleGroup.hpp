#pragma once

#include <Geode/Geode.hpp>
using namespace geode::prelude;

template <class T>
class ToggleGroup : public CCMenu {
    using BaseFactory = Function<CCNode*(bool active)>;
    using IconFactory = Function<CCNode*(T value)>;
    using Callback = Function<void(const T&)>;

    struct Entry {
        T value;
        Ref<CCMenuItemToggler> btn;
    };

    Callback m_callback;
    std::vector<Entry> m_entries;
    size_t m_selected = 0;

    bool init(
        std::vector<T>&& values, BaseFactory baseFactory, IconFactory iconFactory, Callback callback, float gap
    ) {
        if (!CCMenu::init()) return false;

        m_callback = std::move(callback);

        this->ignoreAnchorPointForPosition(false);
        this->setLayout(
            SimpleRowLayout::create()
                ->setGap(gap)
                ->setMainAxisScaling(AxisScaling::Fit)
                ->setCrossAxisScaling(AxisScaling::Fit)
        );

        m_entries.reserve(values.size());

        for (const T& value : values) {
            size_t index = m_entries.size();

            auto on = baseFactory(true);
            on->addChildAtPosition(iconFactory(value), Anchor::Center);
            auto off = baseFactory(false);
            off->addChildAtPosition(iconFactory(value), Anchor::Center);

            auto btn = CCMenuItemExt::createToggler(
                on,
                off,
                [this, index](CCObject*) {
                    onSelect(index);
                }
            );

            this->addChild(btn);
            m_entries.emplace_back(value, btn);
        }

        this->updateLayout();
        if (m_entries.size() > 0) m_entries[0].btn->toggle(true);

        return true;
    }

    size_t indexOf(const T& value) const {
        for (size_t i = 0; i < m_entries.size(); i++) {
            if (m_entries[i].value == value) return i;
        }

        return 0;
    }

    size_t firstEnabled() const {
        for (size_t i = 0; i < m_entries.size(); i++) {
            if (m_entries[i].btn->isEnabled()) return i;
        }

        return 0;
    }

    void applySelection(size_t index, bool notify, bool fromClick) {
        if (m_selected != index) {
            m_entries[m_selected].btn->toggle(false);
        }

        if (!fromClick) m_entries[index].btn->toggle(true);
        m_selected = index;

        if (notify && m_callback) m_callback(m_entries[index].value);
    }

    void onSelect(size_t index) {
        if (m_selected == index) {
            m_entries[index].btn->toggle(false);
            return;
        } else {
            applySelection(index, true, true);
        }
    }

public:
    static ToggleGroup* create(
        std::vector<T>&& values, BaseFactory baseFactory, IconFactory iconFactory, Callback callback, float gap = 5.f
    ) {
        auto ret = new ToggleGroup();

        if (ret->init(std::move(values), std::move(baseFactory), std::move(iconFactory), std::move(callback), gap)) {
            ret->autorelease();
            return ret;
        }

        delete ret;
        return nullptr;
    }

    static ToggleGroup* create(
        const std::vector<T>& values, BaseFactory baseFactory, IconFactory iconFactory, Callback callback, float gap = 5.f
    ) {
        auto ret = new ToggleGroup();

        if (ret->init(std::vector<T>(values), std::move(baseFactory), std::move(iconFactory), std::move(callback), gap)) {
            ret->autorelease();
            return ret;
        }

        delete ret;
        return nullptr;
    }

    void select(const T& value, bool notify = false) {
        auto index = this->indexOf(value);
        if (m_selected == index || !m_entries[index].btn->isEnabled()) return;

        this->applySelection(index, notify, false);
    }

    void runCallback() {
        if (m_callback) m_callback(m_entries[m_selected].value);
    }

    void setEnabled(const T& value, bool enabled) {
        auto index = this->indexOf(value);

        auto btn = m_entries[index].btn;
        if (btn->isEnabled() == enabled) return;

        btn->setEnabled(enabled);

        auto onSpr = static_cast<CCSprite*>(btn->m_onButton->getNormalImage());
        auto offSpr = static_cast<CCSprite*>(btn->m_offButton->getNormalImage());

        onSpr->setColor(enabled ? ccWHITE : ccGRAY);
        onSpr->setOpacity(enabled ? 255 : 150);
        offSpr->setColor(enabled ? ccWHITE : ccGRAY);
        offSpr->setOpacity(enabled ? 255 : 150);

        if (!enabled && m_selected == index) {
            btn->toggle(false);
            this->applySelection(firstEnabled(), true, false);
        } else if (enabled && !m_entries[m_selected].btn->isEnabled()) {
            this->applySelection(index, true, false);
        }
    }

    T getSelected() const {
        return m_entries[m_selected].value;
    }

    CCMenuItemToggler* getButton(const T& value) const {
        for (const auto& entry : m_entries) {
            if (entry.value == value) return entry.btn;
        }

        return nullptr;
    }
};
