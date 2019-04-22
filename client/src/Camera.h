#pragma once
#include <SFML/Graphics/View.hpp>

class Camera
{
public:
	Camera(sf::Vector2f pos, sf::Vector2u windowSize, float cameraSize);
	~Camera();

	void SetPosition(sf::Vector2f position);
	sf::Vector2f GetPosition() const;
	void SetZoom(float zoom);
	float GetZoom() const;
	void SetSize(sf::Vector2u windowSize, float cameraSize);
	float GetSize() const;

	const sf::View& GetView() const { return m_view; }
private:
	sf::View m_view;
	float m_zoom{0.0f};
	float m_cameraSize{1.0f};
};

