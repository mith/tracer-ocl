#version 410

in vec3 f_normal;
in vec3 f_texcoord;

out vec4 outputColor;

void main() 
{
    //float shade = gl_FragCoord.z / 500.0f;
    //outputColor = vec4(shade, shade, shade, 1.0f);
    outputColor = vec4((normalize(f_normal) + 1.0f) * 0.5f, 1.0f);
}
