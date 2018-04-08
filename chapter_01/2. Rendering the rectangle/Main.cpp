#include <SFML/Graphics.hpp>

int main(int argc, char* argv[]){
	sf::RenderWindow window(sf::VideoMode(640,480), "Rendering the rectangle.");

	// Creating our shape.
	sf::RectangleShape rectangle(sf::Vector2f(128.0f,128.0f));
	rectangle.setFillColor(sf::Color::Red);
	rectangle.setPosition(320,240);
	rectangle.setOrigin(rectangle.getSize().x / 2, rectangle.getSize().y / 2);

	while(window.isOpen()){
		sf::Event event;
		while(window.pollEvent(event)){
			if(event.type == sf::Event::Closed){
				// Close window button clicked.
				window.close();
			}
		}
		window.clear(sf::Color::Black);
		window.draw(rectangle); // Drawing our shape.
		window.display();
	}
}