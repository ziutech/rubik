#version 330

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

uniform vec4 lp;

in vec4 normal;
in vec4 vertex;
in vec4 color;
in int group;
in vec2 texCoord;
in vec4 c1;
in vec4 c2;
in vec4 c3;

out vec4 iC;
out vec2 itexCoord;
out vec4 pointL;
out vec4 pointN;
out vec4 pointV;
flat out int igroup;

void main(void) {
    mat4 invTBN = mat4(c1, c2, c3, vec4(0, 0, 0, 1));
    // pointL = normalize(V * (lp - M * vertex));//znormalizowany wektor do światła w przestrzeni oka
    pointL = normalize(invTBN * (inverse(M) * lp - vertex));
    pointN = normalize(invTBN * normal);
    // pointV = normalize(vec4(0, 0, 0, 1) - V * M * vertex); //Wektor do obserwatora w przestrzeni oka
    pointV = normalize(invTBN * (inverse(V * M) * vec4(0, 0, 0, 1) - vertex));
    igroup = group;
    itexCoord = texCoord;
    gl_Position=P*V*M*vertex;
}
