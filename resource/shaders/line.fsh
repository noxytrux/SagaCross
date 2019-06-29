//
//  Shader.fsh
//  Untitled
//
//  Created by Marcin Pedzimaz on 12/3/10.
//  Copyright 2010 PRS. All rights reserved.
//

precision mediump float;

varying vec4 pcolor;
uniform sampler2D pTex;

varying mat2 particleRot;

void main()
{
	vec2 texCoord = particleRot * ( gl_PointCoord - vec2( 0.5 ) );
    gl_FragColor = texture2D( pTex , texCoord + vec2( 0.5 ) ) * pcolor;
}