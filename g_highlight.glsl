#version 330

layout(triangles) in;
layout(triangle_strip) out;
layout(max_vertices = 12) out;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
in vec4 gColor[];
in vec4 gnormal[];
in int ggroup[];
out vec4 iColor;

void main(void) {
  for (int i = 0; i < gl_in.length(); i++) {
    gl_Position = P*V*M*gl_in[i].gl_Position;
    iColor = gColor[i];
    EmitVertex();
  }
  EndPrimitive();
}
