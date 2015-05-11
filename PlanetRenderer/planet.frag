#version 450
#extension GL_ARB_conservative_depth : enable 

in vec2 fuv;
in float flogz;
in vec3 fnorm;
in vec3 fpos;

uniform sampler2D bumpTexture;
uniform mat4 cam;

#define Fcoef_half 0.047051080048



layout(depth_less) out float gl_FragDepth;

void main()
{
	//gl_FragColor = vec4(.4,.5, 0.4, 1.0);

	vec3 wi = -normalize(vec3(.25, .25, -.6));
	wi = (cam*vec4(wi,0)).xyz;
	vec3 n = normalize(cross(dFdx(fpos), dFdy(fpos)));
	float diff = max(dot(n,wi), 0.0);
	float r = texture2D(bumpTexture, fuv).r;
	gl_FragColor =  vec4(r, 0, 0.0, 1.0);
	gl_FragColor =  vec4(diff, diff, 0.0, 1.0);
	gl_FragDepth = flogz;
}