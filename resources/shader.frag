uniform vec4 mainColor;
uniform sampler2D mainTexture;
uniform float tileResolution;

void main()
{
    gl_FragColor = texture2D( mainTexture, gl_TexCoord[0].st * tileResolution) * mainColor;
}