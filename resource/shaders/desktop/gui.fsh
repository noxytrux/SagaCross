#version 150

uniform sampler2D tex;
uniform vec4 mColor;
in vec2 tcoord;

out vec4 Out_Color;

void main()
{
    Out_Color = texture(tex,tcoord.st) * mColor;
}



