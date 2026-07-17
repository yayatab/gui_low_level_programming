#include <SFML/Graphics.hpp>

const int WIDTH = 800;
const int HEIGHT = 600;

int main() {
  sf::RenderWindow window(sf::VideoMode({800, 600}), "FractalGenerator");

  while (window.isOpen()) {
    while (auto ev = window.pollEvent()) {
      if (ev->is<sf::Event::Closed>())
        window.close();
    }
    window.clear();
    window.display();
  }
}