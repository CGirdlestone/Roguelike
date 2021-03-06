#ifndef CAMERA_H
#define CAMERA_H

class Camera
{
public:
	Camera(int width, int height, int zoom, int mapWidth, int mapHeight, int _xBuffer, int _yBuffer);
	~Camera();
	int calculateOffset(int x, int y);
	void updatePosition(int x, int y);
	constexpr int getWidth() const {return m_width;};
	constexpr int getHeight() const {return m_height;};
	constexpr int getX() const {return m_x;};
	constexpr int getY() const {return m_y;};
	constexpr int getPos() const {return m_x + m_y * m_mapWidth;};
	constexpr int getXBuffer() const { return m_xBuffer; };
	constexpr int getYBuffer() const { return m_yBuffer; };
	constexpr int getZoom() const { return m_zoom; };
	void setZoom(int zoom) { m_zoom = std::min(4, std::max(1, zoom) == 3 ? (m_zoom == 4 ? 2 : 4) : std::max(1, zoom)); };

	void serialise(std::ofstream& file);
	int deserialise(char* buffer, int i);

private:
	int m_width;
	int m_height;
	int m_zoom;
	int m_mapWidth;
	int m_mapHeight;
	int m_xBuffer;
	int m_yBuffer;
	int m_x;
	int m_y;
};

#endif
