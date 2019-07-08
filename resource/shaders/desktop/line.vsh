//
//  Shader.vsh
//  Untitled
//
//  Created by Marcin Pedzimaz on 12/3/10.
//  Copyright 2010 PRS. All rights reserved.
//

in vec4 position;
in vec4 color;
in vec3 rotvec;

uniform	mat4 projection;
uniform mat4 modelView;
uniform vec3 pSize; // min, max, decrase

out vec4 pcolor;
out mat2 particleRot;

void main()
{
    particleRot[0] = vec2( rotvec.y , -rotvec.x );
    particleRot[1] = rotvec.xy;
    
	pcolor = color;
	
    gl_PointSize = mix( pSize.z * ( rotvec.z - 1.0 ) + pSize.y , pSize.x, rotvec.z );
	gl_Position = projection * (modelView * position);
}
