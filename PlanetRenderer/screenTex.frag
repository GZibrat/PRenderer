#version 450

uniform sampler2D sampler;

in vec3 fuv;
in float fface;

void main()
{
	gl_FragColor =  vec4(texture2D(sampler, fuv.xy).r,0,0,1);
	//gl_FragColor = vec4(fuv.xy, 0, 1);
}