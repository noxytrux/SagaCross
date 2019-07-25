#version 150

uniform vec4 mColor;
uniform sampler2D tex;
uniform float cutoff;

in vec2 tcoord;
out vec4 Out_Color;

void main()
{
	vec4 col = texture(tex,tcoord.st);
	float cut = smoothstep( cutoff-0.1 , cutoff+0.1 , col.a );
    Out_Color = ( col * mColor ) * cut;
}
