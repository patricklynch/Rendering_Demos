uniform vec4 mainColor;
uniform float strength;
uniform float spread;
uniform int samples;

uniform sampler2D mainTexture;
uniform sampler2D depthTexture;

void main()
{
	float d = texture2D( depthTexture, gl_TexCoord[0].st ).r;
	float ssao = 0.0;
	float maxDist = 0.2;

	for( int i = -20; i < 20; i++ )
	{
		if ( i < samples && i >= -samples ) {
			for(int x = -1; x <= 1; x++)
			{
				for( int y = -1; y <= 1; y++) 
				{
					if ( x != 0 && y != 0 )
					{
						vec2 offset = vec2( x, y ) * float(i) * spread;
						float p = texture2D( depthTexture, gl_TexCoord[0].st + offset ).r;
						float dist = (d - p) * 2.0;
						dist = clamp( dist, 0.0, maxDist);
						float val = dist * strength;
						ssao += val * (1.0 - dist / maxDist );
					}
				}
			}
		}
	}
	
	//if ( ssao < 0.0 ) ssao = 0.0;
	ssao = clamp( ssao, 0.0, 1.0 );
	
	vec4 diffuseColor = texture2D( mainTexture, gl_TexCoord[0].st ) * mainColor;
	gl_FragColor = mix( diffuseColor, vec4( 0, 0, 0, 1 ), ssao );
}