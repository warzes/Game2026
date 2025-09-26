#include "stdafx.h"
#include "Material.h"

Materialo2::Materialo2()
{
	specularIntensity = 0.0f;
	shininess = 0.0f;
}

Materialo2::Materialo2(GLfloat sIntensity, GLfloat shine)
{
	specularIntensity = sIntensity;
	shininess = shine;
}

void Materialo2::UseMaterial(GLuint specularIntensityLocation, GLuint shininessLocation)
{
	glUniform1f(specularIntensityLocation, specularIntensity);
	glUniform1f(shininessLocation, shininess);
}

Materialo2::~Materialo2()
{
}