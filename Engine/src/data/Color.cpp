#include "data/Color.h"

#pragma region Statics

Color Color::Black = Color(glm::vec3(0));
Color Color::White = Color(glm::vec3(1));
Color Color::Red = Color(glm::vec3(1, 0, 0));
Color Color::Green = Color(glm::vec3(0, 1, 0));
Color Color::Blue = Color(glm::vec3(0, 0, 1));
Color Color::Yellow = Color(glm::vec3(1, 1, 0));
Color Color::Magenta = Color(glm::vec3(1, 0, 1));
Color Color::Cyan = Color(glm::vec3(0, 1, 1));
Color Color::Orange = Color(glm::vec3(1, 0.5, 0));

#pragma endregion

#pragma region Constructors

Color::Color()
{
	Value = glm::vec3(1);
}

Color::Color(const Color& color)
{
	this->Value = color.Value;
}

Color::Color(const glm::vec3& color)
{
	this->Value = color;
}

Color::~Color()
{
}

#pragma endregion

#pragma region Operators

Color& Color::operator=(const Color& other)
{
	Value = other.Value;
	return *this;
}

Color Color::operator+(const Color& other) const
{
	return Color(Value + other.Value);
}

Color Color::operator-(const Color& other) const
{
	return Color(Value - other.Value);
}

Color Color::operator*(const Color& other) const
{
	return Color(Value * other.Value);
}

Color Color::operator*(float scalar) const
{
	return Color(Value * scalar);
}

Color Color::operator/(float scalar) const
{
	return Color(Value / scalar);
}

#pragma endregion

#pragma region Public Methods

nlohmann::ordered_json Color::Serialize() const
{
	nlohmann::ordered_json json;

	json["r"] = Value.r;
	json["g"] = Value.g;
	json["b"] = Value.b;
	
	return json;
}

#pragma endregion