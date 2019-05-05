#include "Camera.h"

Camera::Camera(sf::Vector2f pos, sf::Vector2u windowSize, float cameraSize) : m_zoom(0.0f)
{
	SetSize(windowSize, cameraSize);
	SetPosition(pos);
}

Camera::~Camera()
{
}

void Camera::SetPosition(sf::Vector2f position)
{
	m_view.setCenter(position);
}

sf::Vector2f Camera::GetPosition() const
{
	return m_view.getCenter();
}

void Camera::SetZoom(float zoom)
{
	m_zoom = zoom;
	m_view.zoom(zoom);
}

float Camera::GetZoom() const
{
	return m_zoom;
}

void Camera::SetSize(sf::Vector2u windowSize, float cameraSize)
{
	float aspectRatio = (float)windowSize.x / (float)windowSize.y;
	m_view.setSize(cameraSize * aspectRatio, cameraSize);
	m_cameraSize = cameraSize;
}

float Camera::GetSize() const
{
	return m_cameraSize;
}

sf::FloatRect Camera::GetBounds() const
{
	return sf::FloatRect(m_view.getCenter() - m_view.getSize() / 2.f, m_view.getSize());
}

sf::Vector2f Camera::ScreenToWorldPos(sf::Vector2i screenPos, const sf::RenderWindow& window)
{
	return window.mapPixelToCoords(screenPos);
}

sf::Vector2i Camera::WorldPosToScreenPos(sf::Vector2f worldPos, const sf::RenderWindow& window)
{
	return window.mapCoordsToPixel(worldPos);
}


