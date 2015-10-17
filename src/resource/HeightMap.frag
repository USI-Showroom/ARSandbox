uniform sampler2D tex;

void main()
{
    gl_FragColor = texture2D(tex, gl_TexCoord[0].xy);
    // gl_FragColor = vec4( gl_TexCoord[0].xy,0,1);
}
