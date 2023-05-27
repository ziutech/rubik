#version 330

out vec4 pixelColor; //Zmienna wyjsciowa fragment shadera. Zapisuje sie do niej ostateczny (prawie) kolor piksela

const vec3 lightAmbient       = vec3(0.9);
const vec3 lightDiffusion     = vec3(0.9);
const vec3 lightSpecular      = vec3(0.9);

const vec3 ambientReflection  = vec3(1.0);
const vec3 diffuseReflection  = vec3(1.0);
const vec3 specularReflection = vec3(1.0);
const float shininess = 60;

in vec4 pointL;
in vec4 pointN;
in vec4 pointV;
in vec4 iC;

void main(void) {
	vec4 pointLn = normalize(pointL);
	vec4 pointNn = normalize(pointN);
	vec4 pointVn = normalize(pointV);
	vec4 r = reflect(-pointLn, pointNn); //Wektor odbity

	float nl = clamp(dot(pointNn, pointLn), 0, 1); //Kosinus kąta pomiędzy wektorami n i l.
	float rv = pow(clamp(dot(r,  pointVn), 0, 1), shininess); // Kosinus kąta pomiędzy wektorami r i v podniesiony do 25 potęgi

  vec3 ip = ambientReflection * lightAmbient;
  ip += diffuseReflection * lightDiffusion * nl;
  ip += specularReflection * lightSpecular * rv;

	pixelColor = vec4(ip * iC.rgb, iC.a);
}
