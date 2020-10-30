
#include <algorithm>
#include <fstream>
#include "Camera.h"
#include "Utils.h"

Camera::Camera(int width, int height, int zoom, int mapWidth, int mapHeight, int _xBuffer, int _yBuffer):
m_width(width), m_height(height), m_zoom(zoom), m_mapWidth(mapWidth), m_mapHeight(mapHeight), m_xBuffer(_xBuffer), m_yBuffer(_yBuffer), m_x(0), m_y(0)
{

}

Camera::~Camera()
{

}

int Camera::calculateOffset(int x, int y)
{
	int i, j;

  i = x - m_x;
  j = y - m_y;

  return i + j * m_width;
}

void Camera::updatePosition(int x, int y)
{
  m_x = x - (m_width / m_zoom) / 2;
  m_y = y - (m_height / m_zoom) / 2;

  m_x = std::min(m_x, m_mapWidth - (m_width / m_zoom));
  m_x = std::max(m_x, 0);

  m_y = std::min(m_y, m_mapHeight - (m_height / m_zoom));
  m_y = std::max(m_y, 0);
}

void Camera::serialise(std::ofstream& file) 
{
	utils::serialiseInt(file, m_zoom);
}

int Camera::deserialise(char* buffer, int i)
{
	m_zoom = utils::deserialiseInt(buffer, i);
	i = utils::advanceFourBytes(i);

	return i;
}
