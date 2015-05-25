#version 410

in vec3 f_normal;
in vec3 f_texcoord;

out vec4 outputColor;

void main() 
{
    outputColor = vec4(gl_FragCoord.z, gl_FragCoord.z, gl_FragCoord.z, 1.0f);
}
