#include <SFML/Graphics.hpp>

int main() {
  sf::RenderWindow window(sf::VideoMode({900, 800}, 600), "Fractals - Step 1");
  window.setVerticalSyncEnabled(true);

  // Draw a visible shape
  sf::CircleShape circle(120.f);
  circle.setFillColor(sf::Color(30, 200, 120));
  circle.setPosition({200.f, 150.f});

  while (window.isOpen()) {
    while (auto o_event = window.pollEvent()) {
      if (!o_event.has_value()) {
        continue;
      }
      sf::Event event = o_event.value();
      if (event.is<sf::Event::Closed>()) {
        window.close();
      }
      if (const auto* kp = event.getIf<sf::Event::KeyPressed>()) {
        if (kp->code == sf::Keyboard::Key::Escape ||
            kp->code == sf::Keyboard::Key::Q) {
          window.close();
            }
      }
    }

    window.clear(sf::Color(15, 15, 20));
    window.draw(circle);
    window.display();
  }
}