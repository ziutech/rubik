#version 330
uniform sampler2D texture0;
uniform sampler2D texture1;
in vec4 iColor;
in vec2 itexcoord;
out vec4 pixelColor;
void main(void) {
  vec3 renderedPixel = texture(texture0, itexcoord).rgb; 
  vec3 highlight = texture(texture1, itexcoord).rgb;

  renderedPixel += highlight;

  vec3 result = vec3(1.0) - exp(-renderedPixel * 0.7);
  result = pow(result, vec3(1.0) / 2.2);
  pixelColor = vec4(result, 1.0);
}
