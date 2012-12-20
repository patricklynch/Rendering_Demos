uniform sampler2D imageTexture;

void main()
{
	float edge = 0.0;
	vec4 white = vec4( 1, 1, 1, 1 );
	float maxDist = 0.5;
	vec4 c = texture2D( imageTexture, gl_TexCoord[0].st );
	for(int x = -1; x <= 1; x++)
	{
		for( int y = -1; y <= 1; y++) 
		{
			if ( x != 0 && y != 0 )
			{
				for( int i = -3; i < 3; i++ )
				{
					vec2 offset = vec2( x, y ) * 0.0001 + float(i) * 0.0005;
					float p = texture2D( imageTexture, gl_TexCoord[0].st + offset ).r;
					float dist = (c.r - p) * 1.1;
					float val = dist;
					edge += val * (dist / maxDist );
				}
			}
		}
	}
	edge = clamp( edge, 0.0, 1.0 );
	gl_FragColor = mix( c, vec4(0,0,0,1), edge );
}