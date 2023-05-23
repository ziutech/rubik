#version 330

uniform vec4 wall_colors[7];
out vec4 pixelColor;
in vec4 iColor;
flat in uint igroup;

void main(void) {
	pixelColor = wall_colors[igroup];
}
