#include <algorithm>
#include <cmath>
#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>
#include <random>
#include <vector>

#include <SFML/Graphics.hpp>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;
const int FPS_LIMIT = 30;
const int TOTAL_FRAMES = 90;

// global tween function
std::function<float(float, float, float)> tween = [](float a, float b, float t) {
    return (1 - t) * a + t * b;
};

float animationTime = 0.f;

void handleInput(sf::Window& window, bool& shouldQuit) {
    while (const std::optional<sf::Event> event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
            shouldQuit = true;
        }

        // ====== ====== ======
        // TODO: (Q2)
        //  implement key presses (1-9) that replace the tween function
        //  with different alternate tween functions.
        //  Functions can be from lecture or from https://easings.net/#
        // ====== ====== ======
        if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
            switch (key->code) {
                case sf::Keyboard::Key::Num1:  // linear
                    tween = [](float a, float b, float t) {
                        float eased = t;
                        return (1.f - eased) * a + eased * b;
                    };
                    break;
                case sf::Keyboard::Key::Num2:  // ease in quad
                    tween = [](float a, float b, float t) {
                        float eased = t * t;
                        return (1.f - eased) * a + eased * b;
                    };
                    break;
                case sf::Keyboard::Key::Num3:  // ease out quad
                    tween = [](float a, float b, float t) {
                        float eased = 1.f - (1.f - t) * (1.f - t);
                        return (1.f - eased) * a + eased * b;
                    };
                    break;
                case sf::Keyboard::Key::Num4:  // ease in-out quad
                    tween = [](float a, float b, float t) {
                        float eased = (t < 0.5f) ? (2.f * t * t)
                                                 : (1.f - std::pow(-2.f * t + 2.f, 2.f) / 2.f);
                        return (1.f - eased) * a + eased * b;
                    };
                    break;
                case sf::Keyboard::Key::Num5:  // ease in cubic
                    tween = [](float a, float b, float t) {
                        float eased = t * t * t;
                        return (1.f - eased) * a + eased * b;
                    };
                    break;
                case sf::Keyboard::Key::Num6:  // ease out cubic
                    tween = [](float a, float b, float t) {
                        float eased = 1.f - std::pow(1.f - t, 3.f);
                        return (1.f - eased) * a + eased * b;
                    };
                    break;
                case sf::Keyboard::Key::Num7:  // ease in-out cubic
                    tween = [](float a, float b, float t) {
                        float eased = (t < 0.5f) ? (4.f * t * t * t)
                                                 : (1.f - std::pow(-2.f * t + 2.f, 3.f) / 2.f);
                        return (1.f - eased) * a + eased * b;
                    };
                    break;
                case sf::Keyboard::Key::Num8:  // ease in-out sine
                    tween = [](float a, float b, float t) {
                        float eased = -(std::cos(3.14159265f * t) - 1.f) / 2.f;
                        return (1.f - eased) * a + eased * b;
                    };
                    break;
                case sf::Keyboard::Key::Num9:  // ease out bounce
                    tween = [](float a, float b, float t) {
                        const float n1 = 7.5625f;
                        const float d1 = 2.75f;
                        float eased = 0.f;
                        if (t < 1.f / d1) {
                            eased = n1 * t * t;
                        } else if (t < 2.f / d1) {
                            float x = t - 1.5f / d1;
                            eased = n1 * x * x + 0.75f;
                        } else if (t < 2.5f / d1) {
                            float x = t - 2.25f / d1;
                            eased = n1 * x * x + 0.9375f;
                        } else {
                            float x = t - 2.625f / d1;
                            eased = n1 * x * x + 0.984375f;
                        }
                        return (1.f - eased) * a + eased * b;
                    };
                    break;
                default:
                    break;
            }
        }
    }
}

void render(sf::RenderWindow& window) {
    // Clear with blue background (sky)
    window.clear(sf::Color::Black);
    // ====== ====== ======
    // TODO: (Q1) Draw circle that moves between
    // the left/right half of the screen.
    // Movement should be governed by the tween function.
    // ====== ====== ======
    static int elapsedFrames = 0;
    ++elapsedFrames;
    animationTime =
        static_cast<float>(elapsedFrames % TOTAL_FRAMES) / static_cast<float>(TOTAL_FRAMES);
    const float radius = 24.f;
    const float x = tween(radius, static_cast<float>(WINDOW_WIDTH) - radius, animationTime);
    const float y = static_cast<float>(WINDOW_HEIGHT) / 3.f;
    sf::CircleShape circle(radius);
    circle.setFillColor(sf::Color::Cyan);
    circle.setOrigin({radius, radius});
    circle.setPosition({x, y});
    window.draw(circle);

    // ====== ====== ======
    // TODO: (Q3) Draw tween function graph with a dot
    // on the current portion of the curve
    // ====== ====== ======

    // Plot region at the bottom of the window
    const float plotLeft = 200.f;
    const float plotBottom = static_cast<float>(WINDOW_HEIGHT) - 50.f;
    const float plotWidth = 400.f;
    const float plotHeight = 400.f;
    const float plotTop = plotBottom - plotHeight;
    const float plotRight = plotLeft + plotWidth;
    sf::VertexArray axes(sf::PrimitiveType::Lines);
    axes.append({{plotLeft, plotTop}, sf::Color::White});
    axes.append({{plotLeft, plotBottom}, sf::Color::White});
    axes.append({{plotLeft, plotBottom}, sf::Color::White});
    axes.append({{plotRight, plotBottom}, sf::Color::White});
    window.draw(axes);
    // Sample the current tween from 0 to 1 and connect the samples
    const int sampleCount = 200;
    sf::VertexArray curve(sf::PrimitiveType::LineStrip);
    for (int i = 0; i < sampleCount; ++i) {
        float t = static_cast<float>(i) / static_cast<float>(sampleCount - 1);
        float value = tween(0.f, 1.f, t);
        float px = plotLeft + t * plotWidth;
        float py = plotBottom - value * plotHeight;
        curve.append({{px, py}, sf::Color::Cyan});
    }
    window.draw(curve);

    const float markerRadius = 5.f;
    const float markerValue = tween(0.f, 1.f, animationTime);
    sf::CircleShape marker(markerRadius);
    marker.setFillColor(sf::Color::Yellow);
    marker.setOrigin({markerRadius, markerRadius});
    marker.setPosition(
        {plotLeft + animationTime * plotWidth, plotBottom - markerValue * plotHeight});
    window.draw(marker);

    window.display();
}

int main() {
    sf::RenderWindow window;

    try {
        // Initialize window
        window.create(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Tween");
        window.setFramerateLimit(FPS_LIMIT);
        // Prevent key repeats.
        window.setKeyRepeatEnabled(false);

        bool shouldQuit = false;
        // Main game loop
        while (window.isOpen()) {
            handleInput(window, shouldQuit);
            if (shouldQuit) {
                break;
            }
            render(window);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}
