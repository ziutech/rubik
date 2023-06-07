
#version 330

in vec4 vertex;
in vec2 texcoord;

out vec2 itexcoord;

void main(void) {
  itexcoord = texcoord;
  gl_Position = vertex;
}
