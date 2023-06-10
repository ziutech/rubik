# version 330 core
out vec4 FragColor;
  
in vec2 itexcoord;
uniform sampler2D image;
  
uniform bool horizontal;
const float weight[7] = float[] (
  0.2255859375,
	0.1933593750,
	0.1208496094,
	0.05371093750,
	0.01611328125,
	0.002929687500,
	0.0002441406250
);
void main()
{             
    vec2 tex_offset = 1.0 / textureSize(image, 0); // gets size of single texel
    vec3 result = texture(image, itexcoord).rgb * weight[0]; // current fragment's contribution
    if(horizontal)
    {
        for(int i = 1; i < 7; ++i)
        {
            result += texture(image, itexcoord + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            result += texture(image, itexcoord - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        }
    }
    else
    {
        for(int i = 1; i < 7; ++i)
        {
            result += texture(image, itexcoord + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
            result += texture(image, itexcoord - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
        }
    }
    FragColor = vec4(result, 1.0);
}
