//
//  Shader.fsh
//  Untitled
//
//  Created by Marcin Pedzimaz on 12/3/10.
//  Copyright 2010 PRS. All rights reserved.
//

uniform sampler2D texWater;
uniform sampler2D texWaterCaustics;
uniform sampler2D texWaterCausticsSecond;

in vec2 uv0;
in vec2 uv1;
in vec2 uvRegular;

out vec4 Out_Color;

void main(void)
{
    vec4 outcolor = texture(texWater, uvRegular);
    vec4 color0   = texture(texWaterCaustics, uv0 * 16.0);
    vec4 color1   = texture(texWaterCausticsSecond, uv1 * 16.0);
    vec4 caustic  = (color0 + color1);

	Out_Color = (0.5 * outcolor) * (outcolor.a * 0.5) + caustic;
}

