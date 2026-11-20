#include <algorithm>
#include <cstdint>
#include <sstream>
#include <string>
#include <vector>
#include <yaml-cpp/yaml.h>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/screen/string.hpp>
#include "include/UI.hpp"

static int hexDigit(char value) {
    if (value >= '0' && value <= '9') {
        return value - '0';
    } else if (value >= 'a' && value <= 'f') {
        return value - 'a' + 10;
    } else if (value >= 'A' && value <= 'F') {
        return value - 'A' + 10;
    } else {
        return -1;
    }
}

static ftxui::Color parseHexColor(const std::string& hex) {
    std::string value = hex;
    if (!value.empty() && value[0] == '#') {
        value = value.substr(1);
    }

    if (value.size() != 6) {
        return ftxui::Color::RGB(0x94, 0xeb, 0xff);
    }

    int red = hexDigit(value[0]) * 16 + hexDigit(value[1]);
    int green = hexDigit(value[2]) * 16 + hexDigit(value[3]);
    int blue = hexDigit(value[4]) * 16 + hexDigit(value[5]);

    if (red < 0 || green < 0 || blue < 0) {
        return ftxui::Color::RGB(0x94, 0xeb, 0xff);
    }

    return ftxui::Color::RGB(
        static_cast<uint8_t>(red),
        static_cast<uint8_t>(green),
        static_cast<uint8_t>(blue)
    );
}

std::string loadViewColor() {
    try {
        YAML::Node config = YAML::LoadFile("escan.yaml");
        if (config["color"]) {
            return config["color"].as<std::string>();
        }
    } catch (...) {
    }

    return "#94ebff";
}

static std::vector<std::string> splitLines(const std::string& text) {
    std::vector<std::string> lines;
    std::string current;

    for (char value : text) {
        if (value == '\n') {
            lines.push_back(current);
            current.clear();
        } else if (value != '\r') {
            current += value;
        }
    }

    if (!current.empty() || lines.empty()) {
        lines.push_back(current);
    }

    return lines;
}

std::string renderView(const std::string& title, const std::string& content) {
    ftxui::Color background = parseHexColor(loadViewColor());
    ftxui::Elements lines;
    int width = ftxui::string_width(title);

    for (const auto& line : splitLines(content)) {
        lines.push_back(ftxui::text(line));
        width = std::max(width, ftxui::string_width(line));
    }

    int height = static_cast<int>(lines.size()) + 3;

    ftxui::Element element = ftxui::vbox({
        ftxui::text(title) | ftxui::bold,
        ftxui::vbox(std::move(lines))
    }) | ftxui::borderStyled(ftxui::LIGHT, background);

    ftxui::Screen screen = ftxui::Screen::Create(
        ftxui::Dimension::Fixed(width + 2),
        ftxui::Dimension::Fixed(height)
    );
    ftxui::Render(screen, element);

    return screen.ToString() + '\n';
}
