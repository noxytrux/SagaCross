//
//  Shader.fsh
//  Untitled
//
//  Created by Marcin Pedzimaz on 12/3/10.
//  Copyright 2010 PRS. All rights reserved.
//

precision lowp float;

varying vec2 uv;
uniform sampler2D pTex;

void main()
{
    gl_FragColor = texture2D(pTex, uv );
}