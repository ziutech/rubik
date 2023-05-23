#version 330

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

in vec4 vertex;
in vec4 color;
in uint group;

out vec4 iColor;
flat out uint igroup;

void main(void) {
	iColor = color;
  igroup = group;
	gl_Position = P * V * M * vertex;
}
