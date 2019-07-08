//
//  Shader.fsh
//  Untitled
//
//  Created by Marcin Pedzimaz on 12/3/10.
//  Copyright 2010 PRS. All rights reserved.
//

in vec4 position;
in vec2 coord;

uniform	mat4 projection;
uniform mat4 modelViewWorld;
uniform float time;

out vec2 uv1;

void main()
{
	gl_Position = projection * (modelViewWorld * position);
	uv1 = coord;
    uv1.t += time*0.5;
}