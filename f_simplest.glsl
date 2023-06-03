#version 330

uniform vec4 wall_colors[7];
uniform int wall_mapping[7];
out vec4 pixelColor; //Zmienna wyjsciowa fragment shadera. Zapisuje sie do niej ostateczny (prawie) kolor piksela

uniform sampler2D edgeBase;
uniform sampler2D edgeAmbient;
uniform sampler2D edgeHeight;
uniform sampler2D edgeNormal;
uniform sampler2D edgeRoughness;

const vec3 lightAmbient       = vec3(0.1);
const vec3 lightDiffusion     = vec3(0.9);
const vec3 lightSpecular      = vec3(0.3);

const vec3 ambientReflection  = vec3(1.0);
const vec3 diffuseReflection  = vec3(1.0);
const vec3 specularReflection = vec3(0.5);
const float shininess = 60;

in vec4 pointL;
in vec4 pointN;
in vec4 pointV;
in vec4 iC;
in vec2 itexCoord;
in vec4 dirLight;

flat in int igroup;

void main(void) {
  int wmap = wall_mapping[igroup];
  if(wmap == 0) {
    vec4 diffuse  = texture(edgeBase, itexCoord);
    vec4 ambient  = texture(edgeAmbient, itexCoord);
    vec4 height = texture(edgeHeight, itexCoord);
    vec4 normal = texture(edgeNormal, itexCoord);
    vec4 roughness = texture(edgeRoughness, itexCoord);

    vec4 pointLn = normalize(pointL);
    vec4 pointNn = normalize(vec4(normal.rgb * 2 - 1, 0));
    vec4 pointVn = normalize(pointV);
    vec4 r = reflect(-pointLn, pointNn); //Wektor odbity
    vec4 iC = wall_colors[wmap];

    float nl = clamp(dot(pointNn, pointLn), 0, 1); //Kosinus kąta pomiędzy wektorami n i l.
    // roughness -> shiness: https://computergraphics.stackexchange.com/a/1517
    float rv = pow(clamp(dot(r,  pointVn), 0, 1), 2.0 / pow(roughness.r, 2) - 2); 
    vec3 ip = vec3(1.0) * lightAmbient;
    ip += diffuse.rgb * lightDiffusion * nl;
    ip += specularReflection * lightSpecular * rv;

    nl = clamp(dot(pointNn, dirLight), 0, 1);
    r = reflect(-dirLight, pointNn);
    rv = pow(clamp(dot(r, pointVn), 0, 1), shininess);
    ip += diffuse.rgb * lightDiffusion * nl;
    ip += specularReflection * vec3(0.1) * rv;
    pixelColor = vec4(ip.rgb, 1.0);
  } else {
    vec4 pointLn = normalize(pointL);
    vec4 pointNn = normalize(pointN);
    vec4 pointVn = normalize(pointV);
    vec4 r = reflect(-pointLn, pointNn); //Wektor odbity
    vec4 iC = wall_colors[wmap];

    float nl = clamp(dot(pointNn, pointLn), 0, 1); //Kosinus kąta pomiędzy wektorami n i l.
    float rv = pow(clamp(dot(r,  pointVn), 0, 1), shininess); // Kosinus kąta pomiędzy wektorami r i v podniesiony do 25 potęgi

    vec3 ip = iC.rgb * lightAmbient;
    ip += iC.rgb * lightDiffusion * nl;
    ip += specularReflection * lightSpecular * rv;

    nl = clamp(dot(pointNn, dirLight), 0, 1);
    r = reflect(-dirLight, pointNn);
    rv = pow(clamp(dot(r, pointVn), 0, 1), shininess);
    ip += vec3(244, 233, 155)/255 * lightDiffusion * nl;
    ip += specularReflection * vec3(0.01) * rv;
    pixelColor = vec4(ip.rgb, 1.0);
  }
}
