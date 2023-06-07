#version 330
uniform sampler2D highlight;

in vec2 itexcoord;
out vec4 blurredHighlight;

vec4 blur9(sampler2D image, vec2 uv, vec2 resolution, vec2 direction) {
  vec4 color = vec4(0.0);
  vec2 off1 = vec2(1.3846153846) * direction;
  vec2 off2 = vec2(3.2307692308) * direction;
  color += texture2D(image, uv) * 0.2270270270;
  color += texture2D(image, uv + (off1 / resolution)) * 0.3162162162;
  color += texture2D(image, uv - (off1 / resolution)) * 0.3162162162;
  color += texture2D(image, uv + (off2 / resolution)) * 0.0702702703;
  color += texture2D(image, uv - (off2 / resolution)) * 0.0702702703;
  return color;
}

void main(void) {
  vec4 color_h = blur9(highlight, itexcoord, vec2(1000, 1000), vec2(1, 0));
  vec4 color_v = blur9(highlight, itexcoord, vec2(1000, 1000), vec2(0, 1));
  blurredHighlight = (color_h + color_v) / 2;
}
