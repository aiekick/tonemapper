#pragma once

#include <tonemap.h>

class SchlickOperator : public TonemapOperator {
public:
	SchlickOperator() : TonemapOperator() {
		parameters["p"] = Parameter(200.f, 1.f, 1000.f, "p", "Rational mapping curve parameter");

		name = "Schlick";
		description = "Schlick Mapping\n\nProposed in \"Quantization Techniques for Visualization of High Dynamic Range Pictures\" by Schlick 1994.";

		shader->init(
			"Schlick",

			"#version 330\n"
			"in vec2 position;\n"
			"out vec2 uv;\n"
			"void main() {\n"
			"    gl_Position = vec4(position.x*2-1, position.y*2-1, 0.0, 1.0);\n"
			"    uv = vec2(position.x, 1-position.y);\n"
			"}",

			"#version 330\n"
			"uniform sampler2D source;\n"
			"uniform float exposure;\n"
			"uniform float Lmax;\n"
			"uniform float p;\n"
			"in vec2 uv;\n"
			"out vec4 out_color;\n"
			"\n"
			"vec4 clampedValue(vec4 color) {\n"
			"	 color.a = 1.0;\n"
			"	 return clamp(color, 0.0, 1.0);\n"
			"}\n"
			"\n"
			"void main() {\n"
			"    vec4 color = exposure * texture(source, uv);\n"
			"	 color = p * color / (p * color - color + exposure * Lmax);\n"
			"    out_color = clampedValue(color);\n"
			"}"
		);
	}

	virtual void setParameters(const Image *image) override {
		parameters["Lmax"] = Parameter(image->getMaximumLuminance(), "Lmax");
	};

protected:
	virtual float map(float value, float exposure) const override {
		float Lmax = parameters.at("Lmax").value;
		float p = parameters.at("p").value;

		value *= exposure;
		value = p * value / (p * value - value + exposure * Lmax);
		return value;
	}
};