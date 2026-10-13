#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;

struct StatDef {
    const char* label;
    const char* key;   // GameStatsManager stat id
};

// Stat IDs are what GD uses internally. If one edits the wrong stat,
// change its key here. You can add more rows too.
static const std::vector<StatDef> STATS = {
    {"Stars",        "6"},
    {"Moons",        "28"},
    {"Diamonds",     "14"},
    {"Mana Orbs",    "29"},
    {"User Coins",   "8"},
    {"Secret Coins", "13"},
};

class StatPopup : public Popup {
protected:
    std::vector<TextInput*> m_inputs;

    bool init() {
        if (!Popup::init(300.f, 110.f + STATS.size() * 34.f)) return false;

        this->setTitle("Stat Editor");
        auto gsm = GameStatsManager::sharedState();
        auto size = m_mainLayer->getContentSize();

        float y = size.height - 55.f;
        for (auto const& s : STATS) {
            auto label = CCLabelBMFont::create(s.label, "bigFont.fnt");
            label->setScale(0.45f);
            label->setAnchorPoint({0.f, 0.5f});
            label->setPosition({20.f, y});
            m_mainLayer->addChild(label);

            auto input = TextInput::create(110.f, "0", "bigFont.fnt");
            input->setFilter(CommonFilter::Uint);
            input->setMaxCharCount(9);
            input->setString(std::to_string(gsm->getStat(s.key)));
            input->setPosition({size.width - 80.f, y});
            m_mainLayer->addChild(input);
            m_inputs.push_back(input);

            y -= 34.f;
        }

        auto applySpr = ButtonSprite::create("Apply");
        applySpr->setScale(0.8f);
        auto applyBtn = CCMenuItemExt::createSpriteExtra(
            applySpr, [this](CCObject*) { this->apply(); }
        );
        applyBtn->setPosition({size.width / 2 + 55.f, 30.f});
        m_buttonMenu->addChild(applyBtn);

        auto maxSpr = ButtonSprite::create("Max All");
        maxSpr->setScale(0.8f);
        auto maxBtn = CCMenuItemExt::createSpriteExtra(
            maxSpr, [this](CCObject*) {
                for (auto input : m_inputs) input->setString("1000000");
                this->apply();
            }
        );
        maxBtn->setPosition({size.width / 2 - 55.f, 30.f});
        m_buttonMenu->addChild(maxBtn);

        return true;
    }

    void apply() {
        auto gsm = GameStatsManager::sharedState();
        for (size_t i = 0; i < STATS.size(); i++) {
            std::string str = m_inputs[i]->getString();
            if (str.empty()) continue;
            try {
                gsm->setStat(STATS[i].key, std::stoi(str));
            } catch (...) {}
        }
        FLAlertLayer::create("Done", "Stats updated! Quit GD from the menu so it saves.", "OK")->show();
        this->onClose(nullptr);
    }

public:
    static StatPopup* create() {
        auto ret = new StatPopup();
        if (ret->init()) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }
};

class $modify(StatMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        auto spr = ButtonSprite::create("Stats");
        spr->setScale(0.7f);
        auto btn = CCMenuItemExt::createSpriteExtra(spr, [](CCObject*) {
            StatPopup::create()->show();
        });

        if (auto menu = this->getChildByID("bottom-menu")) {
            menu->addChild(btn);
            menu->updateLayout();
        }
        return true;
    }
};
