#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <vector>
#include <set>
#include <sstream>

using namespace geode::prelude;

static std::vector<float> parsePercents(std::string const& raw) {
    std::vector<float> out;
    std::stringstream ss(raw);
    std::string token;
    while (std::getline(ss, token, ',')) {
        size_t start = token.find_first_not_of(" \t");
        size_t end = token.find_last_not_of(" \t");
        if (start == std::string::npos) continue;
        std::string trimmed = token.substr(start, end - start + 1);
        try {
            float val = std::stof(trimmed);
            if (val > 0.f && val <= 100.f) out.push_back(val);
        } catch (...) {}
    }
    return out;
}

class $modify(APPPlayLayer, PlayLayer) {
    struct Fields {
        std::set<int> triggered;
    };

    void resetLevel() {
        PlayLayer::resetLevel();
        m_fields->triggered.clear();
    }

    void postUpdate(float dt) {
        PlayLayer::postUpdate(dt);
        if (!m_player1 || m_player1->m_isDead) return;
        if (!Mod::get()->getSettingValue<bool>("enabled")) return;

        auto targets = parsePercents(Mod::get()->getSettingValue<std::string>("percents"));
        if (targets.empty()) return;

        float current = this->getCurrentPercent();
        for (float target : targets) {
            int key = static_cast<int>(target);
            if (m_fields->triggered.count(key)) continue;
            if (current >= target) {
                m_fields->triggered.insert(key);
                this->pauseGame(true); // VERIFY: see README
                break;
            }
        }
    }
};
