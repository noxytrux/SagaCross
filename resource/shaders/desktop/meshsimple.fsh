#version 150

uniform vec4 mColor;
uniform sampler2D tex;

in vec2 tcoord;

out vec4 Out_Color;

void main()
{
    Out_Color =  texture(tex,tcoord.st) * mColor;
}



