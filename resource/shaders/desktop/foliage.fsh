#version 150

in vec2 uv;
uniform sampler2D pTex;

out vec4 Out_Color;

void main()
{
    Out_Color = texture(pTex, uv );
}