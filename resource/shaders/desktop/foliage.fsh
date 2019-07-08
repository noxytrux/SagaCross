//
//  Shader.fsh
//  Untitled
//
//  Created by Marcin Pedzimaz on 12/3/10.
//  Copyright 2010 PRS. All rights reserved.
//

in vec2 uv;
uniform sampler2D pTex;

out vec4 Out_Color;

void main()
{
    Out_Color = texture(pTex, uv );
}