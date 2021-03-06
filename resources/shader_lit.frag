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
	
	vec4 mainColor;
	vec4 textureColor = texture2D( mainTexture, gl_TexCoord[0].st );
	if ( textured == true )
		mainColor = textureColor * diffuseColor;
	else
		mainColor = diffuseColor;
	
	vec4 diffuse		= vec4( mainColor.xyz * df, textureColor.a );
	vec4 specular		= vec4( specularColor.xyz * sf, textureColor.a );
	vec4 ambient		= vec4( mainColor.xyz * ambientColor.xyz, textureColor.a );
	
	gl_FragColor		= ambient * diffuseColor + diffuse + specular;
}
