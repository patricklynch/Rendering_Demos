uniform sampler2D	mainTexture;
uniform mat4		transformMatrix;

uniform vec4		diffuseColor;
uniform vec4		ambientColor;
uniform vec4		specularColor;

uniform vec3		lightPos;
uniform vec3		eyePos;

varying vec3		worldPos;
varying vec3		normal;

uniform bool		textured;

float toonify( float value, int numValueRanges )
{
	float toon = 0.0;
	float unit = 1.0 / float( numValueRanges );
	for( int i = 0; i < 20; i++) {
		if ( i < numValueRanges ) {
			toon += step( value, float(i+1) * unit ) * unit;
		}
	}
	return 1.0 - toon;
}

vec4 toonify4( vec4 value, int numValueRanges )
{
	vec4 toon = vec4(0,0,0,0);
	float unit = 1.0 / float( numValueRanges );
	for( int i = 0; i < 20; i++) {
		if ( i < numValueRanges ) {
			float v = float(i+1) * unit;
			toon = toon + step( value, vec4(v,v,v,v) ) * unit;
		}
	}
	return vec4(1,1,1,1) - toon;
}

void main()
{
	vec3 viewDir		= eyePos - worldPos;
	vec3 lightDir		= lightPos - worldPos;
	
    vec3 N				= normalize( normal );
    vec3 L				= normalize( lightDir );
    vec3 E				= normalize( viewDir );
    vec3 H				= normalize( L + E );
	
	float df			= max( 0.0, dot(N, L) );
	float sf			= pow( max( 0.0, dot(N, H) ), 30.0 );

	df *= toonify( df, 3 );
	sf *= toonify( sf, 2 );

	vec4 mainColor;
	vec4 textureColor = texture2D( mainTexture, gl_TexCoord[0].st );
	if ( textured == true )
		mainColor = textureColor * diffuseColor;
	else
		mainColor = diffuseColor;
	
	vec4 diffuse		= vec4( mainColor.xyz * df, textureColor.a );
	vec4 specular		= vec4( specularColor.xyz * sf, textureColor.a );
	vec4 ambient		= vec4( mainColor.xyz * ambientColor.xyz, mainColor.a );
	
	gl_FragColor		= ambient * diffuseColor + diffuse + specular;
}
