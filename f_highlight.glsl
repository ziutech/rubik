#version 330
uniform sampler2D texture0;
uniform sampler2D texture1;
in vec4 iColor;
in vec2 itexcoord;
out vec4 pixelColor;
void main(void) {
  vec4 renderedPixel = texture(texture0, itexcoord); 
  vec4 highlight = texture(texture1, itexcoord);
  pixelColor = renderedPixel + highlight;
}
