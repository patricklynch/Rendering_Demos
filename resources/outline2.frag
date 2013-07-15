uniform vec4 mainColor;
uniform float depthRange;
uniform vec4 outlineColor;
uniform float outlineThickness;

uniform sampler2D mainTexture;
uniform sampler2D depthTexture;

void main()
{
	float d = texture2D( depthTexture, gl_TexCoord[0].st ).r;
	float outline = 0.0;
	float maxDist = 0.3;
	for(int x = -1; x <= 1; x++)
	{
		for( int y = -1; y <= 1; y++) 
		{
			if ( x != 0 && y != 0 )
			{
				vec2 offset = vec2( x, y ) * outlineThickness * 0.001;
				float p = texture2D( depthTexture, gl_TexCoord[0].st + offset ).r;
				float dist = ( d - p );
				outline += dist * ( dist / maxDist );
			}
		}
	}
	outline *= 500.0;
	outline = clamp( outline, 0.0, 1.0 );
	vec4 diffuseColor = texture2D( mainTexture, gl_TexCoord[0].st ) * mainColor;
	gl_FragColor = mix( vec4(1,1,1,1), outlineColor, outline );
}