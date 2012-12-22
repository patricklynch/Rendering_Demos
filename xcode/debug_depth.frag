uniform sampler2D depthBufferTexture;
uniform float rangeMultiplier;

void main()
{
	float d = texture2D( depthBufferTexture, gl_TexCoord[0].st ).r * rangeMultiplier - rangeMultiplier * 0.25;
	gl_FragColor = vec4( d, d, d, 1 );
}
