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
uniform float speed;
uniform float amplitude;

out vec2 uv;

void main()
{
    vec4 vertex = position * position;
    vec4 Multiply = vec4( speed ) * time;
    vec4 SinVec = vec4( vertex.z ) * sin(Multiply);
    vec4 outVal = vec4( SinVec.x ) * amplitude;
    vec4 Add0 = vec4(0.0, outVal.x, 0.0, 0.0) + position;
    
 	gl_Position = projection * ( modelViewWorld * Add0 );
 	uv = coord;
}

