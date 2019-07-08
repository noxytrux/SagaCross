//
//  Shader.fsh
//  Untitled
//
//  Created by Marcin Pedzimaz on 12/3/10.
//  Copyright 2010 PRS. All rights reserved.
//

in vec4 pcolor;
uniform sampler2D pTex;

in mat2 particleRot;

out vec4 Out_Color;

void main()
{
	vec2 texCoord = particleRot * ( gl_PointCoord - vec2( 0.5 ) );
    Out_Color = texture( pTex , texCoord + vec2( 0.5 ) ) * pcolor;
}