#pragma once

#include <glm/glm.hpp>

class Color
{
public:
	Color();
	Color(const glm::vec3& color);
	Color(const Color& color);
	~Color();

	Color& operator=(const Color& color);
	Color operator+(const Color& color) const;
	Color operator-(const Color& color) const;
	Color operator*(const Color& color) const;
	Color operator*(float scalar) const;
	Color operator/(float scalar) const;

    // default colors
	static Color White;
	static Color Black;
	static Color Red;
	static Color Green;
	static Color Blue;
	static Color Yellow;
	static Color Cyan;
	static Color Magenta;
	static Color Orange;

	glm::vec3 Value;
};