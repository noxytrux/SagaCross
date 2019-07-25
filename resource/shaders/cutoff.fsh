//
//  Shader.fsh
//  Untitled
//
//  Created by Marcin Pedzimaz on 12/3/10.
//  Copyright 2010 PRS. All rights reserved.
//

precision lowp float;

uniform vec4 mColor;
uniform sampler2D tex;
uniform float cutoff;

varying vec2 tcoord;

void main()
{
	vec4 col = texture2D(tex,tcoord.st);
	float cut = smoothstep( cutoff-0.1 , cutoff+0.1 , col.a );
    gl_FragColor = ( col * mColor ) * cut;
}




