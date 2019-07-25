//
//  Shader.fsh
//  Untitled
//
//  Created by Marcin Pedzimaz on 12/3/10.
//  Copyright 2010 PRS. All rights reserved.
//

precision lowp float;

uniform sampler2D tex;
uniform vec4 mColor;
varying vec2 tcoord;

void main()
{
    gl_FragColor = texture2D(tex,tcoord.st) * mColor;
}



