#version 150

in vec4 position;
in vec2 coord;

uniform	mat4 projection;
uniform mat4 modelViewWorld;

out vec2 tcoord;

void main()
{
    tcoord.st = coord.st;
    gl_Position = projection * (modelViewWorld * position);
}
