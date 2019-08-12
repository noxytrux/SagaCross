//
//  Shader.fsh
//  Untitled
//
//  Created by Marcin Pedzimaz on 12/3/10.
//  Copyright 2010 PRS. All rights reserved.
//

precision highp float;

uniform sampler2D texWater;
uniform sampler2D texWaterCaustics;
uniform sampler2D texWaterCausticsSecond;

varying vec2 uv0;
varying vec2 uv1;
varying vec2 uvRegular;

void main(void)
{
    vec4 outcolor = texture2D(texWater, uvRegular);
    vec4 color0   = texture2D(texWaterCaustics, uv0 * 16.0);
    vec4 color1   = texture2D(texWaterCausticsSecond, uv1 * 16.0);
    vec4 caustic  = (color0 + color1);

	gl_FragColor = (0.5 * outcolor) + (0.5 * caustic);
}

