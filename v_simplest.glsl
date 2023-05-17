#version 330

//Zmienne jednorodne
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform float divisor;

//Atrybuty
in vec4 vertex; 
in vec4 color;

out vec4 iC;

void main(void) {
  float d = distance(V * M *vertex, vec4(0, 0, 0, 1));
  d = (d - 4.0) / divisor;
  iC=color * (1-d);
  gl_Position=P*V*M*vertex;
}
