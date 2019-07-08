//
//  Shader.vsh
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

out vec2 uv0;
out vec2 uv1;
out vec2 uvRegular;

const float delta = 0.001;

void main()
{
	gl_Position = projection * (modelViewWorld * position);
    
    uvRegular = coord;

    uv0 = coord;
    uv1 = coord;
    
    uv0.s += time*delta;
	uv0.t += time*delta;
	uv1.s += time*delta;
	uv1.t -= time*delta;
}
