//
//  Shader.vsh
//  Untitled
//
//  Created by Marcin Pedzimaz on 12/3/10.
//  Copyright 2010 PRS. All rights reserved.
//

attribute vec4 position;
attribute vec2 coord;

uniform	mat4 projection;
uniform mat4 modelViewWorld;

varying vec2 tcoord;

void main()
{
    tcoord.st = coord.st;
    gl_Position = projection * (modelViewWorld * position);
}
