#version 330
in vec2 f_texcoord;

out vec4 outputColor;

uniform sampler2D tex;

void main() {
    vec4 col = texture(tex, vec2(f_texcoord.x, -f_texcoord.y));
    outputColor = col;
}
