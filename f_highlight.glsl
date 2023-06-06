#version 330
uniform sampler2D cb0;
uniform sampler2D cb1;
in vec4 iColor;
in vec2 itexcoord;
out vec4 pixelColor;
void main(void) {

  pixelColor = texture(cb1, itexcoord);
}
