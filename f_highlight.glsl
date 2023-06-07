#version 330
uniform sampler2D texture0;
in vec4 iColor;
in vec2 itexcoord;
out vec4 pixelColor;
void main(void) {
  pixelColor = texture(texture0, itexcoord);
}
