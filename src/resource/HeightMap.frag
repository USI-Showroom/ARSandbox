uniform sampler2D tex;

void main()
{
    vec4 texture = texture2D(tex, gl_TexCoord[0].xy);
    gl_FragColor = texture;//vec4(texture.r,texture.r,texture.r,1);
    // gl_FragColor = vec4( gl_TexCoord[0].xy,0,1);
}
