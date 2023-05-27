#version 330

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

uniform vec4 wall_colors[7];
uniform vec4 lp;

in vec4 normal;
in vec4 vertex;
in vec4 color;
in uint group;

out vec4 iC;
out vec4 pointL;
out vec4 pointN;
out vec4 pointV;

void main(void) {
    pointL = normalize(V * (lp - M * vertex));//znormalizowany wektor do światła w przestrzeni oka
    pointN = normalize(V * M * normal);//znormalizowany wektor normalny w przestrzeni oka
    pointV = normalize(vec4(0, 0, 0, 1) - V * M * vertex); //Wektor do obserwatora w przestrzeni oka
    iC = wall_colors[group];
    gl_Position=P*V*M*vertex;
}
