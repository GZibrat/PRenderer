#version 450

uniform sampler2D bumpTexture;
in vec3 pos;
in vec3 normal;
in vec2 uv;
in float delta;

uniform mat4 cam;
uniform mat4 view;
uniform float scale;
uniform int face;

const int FSI[6] = int[] ( -1 , 1, -1, 1, -1, 1);
const int FI[18] = int[] (
	1, 2, 0,
	1, 2, 0,
	0, 1, 2,
	0, 1, 2,
	0, 2, 1,
	0, 2, 1
);

out vec2 fuv;
out float flogz;
out vec3 fnorm;
out vec3 fpos;

#define near 0.1
#define far 250000000.0
#define C 0.01
#define FC (1.0/log(far*C+1))
#define fWidth 64.0
#define fHeight fWidth

float BellFunc( float x )
{
	float f = ( x / 2.0 ) * 1.5; // Converting -2 to +2 to -1.5 to +1.5
	if( f > -1.5 && f < -0.5 )
	{
		return( 0.5 * pow(f + 1.5, 2.0));
	}
	else if( f > -0.5 && f < 0.5 )
	{
		return 3.0 / 4.0 - ( f * f );
	}
	else if( ( f > 0.5 && f < 1.5 ) )
	{
		return( 0.5 * pow(f - 1.5, 2.0));
	}
	return 0.0;
}

vec4 BiCubic( sampler2D textureSampler, vec2 TexCoord )
{
    float texelSizeX = 1.0 / fWidth; //size of one texel 
    float texelSizeY = 1.0 / fHeight; //size of one texel 
    vec4 nSum = vec4( 0.0, 0.0, 0.0, 0.0 );
    vec4 nDenom = vec4( 0.0, 0.0, 0.0, 0.0 );
    float a = fract( TexCoord.x * fWidth ); // get the decimal part
    float b = fract( TexCoord.y * fHeight ); // get the decimal part
    for( int m = -1; m <=2; m++ )
    {
        for( int n =-1; n<= 2; n++)
        {
			vec4 vecData = texture2D(textureSampler, 
                               TexCoord + vec2(texelSizeX * float( m ), 
					texelSizeY * float( n )));
			float f  = BellFunc( float( m ) - a );
			vec4 vecCooef1 = vec4( f,f,f,f );
			float f1 = BellFunc ( -( float( n ) - b ) );
			vec4 vecCoeef2 = vec4( f1, f1, f1, f1 );
            nSum = nSum + ( vecData * vecCoeef2 * vecCooef1  );
            nDenom = nDenom + (( vecCoeef2 * vecCooef1 ));
        }
    }
    return nSum / nDenom;
}

void main()
{
	float bump = texture2D(bumpTexture, uv).r;
	fuv = uv;
	fnorm = normal;
	vec4 vpos = cam * vec4(pos+normal*(bump*500+0*delta), 1.0);
	fpos = vpos.xyz;
	gl_Position = view * vpos;
	gl_Position.z = 2.0*log(gl_Position.w*C+1)/log(far*C+1)-1;
	flogz = log(gl_Position.w*C+1)*FC;
	gl_Position.z = (2*flogz-1)*gl_Position.w;
}