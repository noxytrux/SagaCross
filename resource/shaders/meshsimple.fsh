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

varying vec2 tcoord;

void main()
{
    gl_FragColor =  texture2D(tex,tcoord.st) * mColor;
}



