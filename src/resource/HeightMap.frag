#version 130
#extension GL_EXT_gpu_shader4 : enable
uniform usampler2D tex;

void main()
{
    uvec4 texture = texture2D(tex, gl_TexCoord[0].xy);
    float asd=texture.r;
    asd/=4500;
    float col=float(mod(texture.r,256))/256.0;
    gl_FragColor = vec4(col,asd,col,1);//vec4(texture.r,texture.r,texture.r,1);
    // gl_FragColor = vec4( gl_TexCoord[0].xy,0,1);
}
