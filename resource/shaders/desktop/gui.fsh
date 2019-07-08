//
//  Shader.fsh
//  Untitled
//
//  Created by Marcin Pedzimaz on 12/3/10.
//  Copyright 2010 PRS. All rights reserved.
//

uniform sampler2D tex;
uniform vec4 mColor;
in vec2 tcoord;

out vec4 Out_Color;

void main()
{
    Out_Color = texture(tex,tcoord.st) * mColor;
}



