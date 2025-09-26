#pragma once

class Materialo2
{
public:
	Materialo2();
	Materialo2(GLfloat sIntensity, GLfloat shine);

	void UseMaterial(GLuint specularIntensityLocation, GLuint shininessLocation);

	~Materialo2();

private:
	GLfloat specularIntensity;
	GLfloat shininess;
};