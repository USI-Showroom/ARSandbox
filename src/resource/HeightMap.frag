#version 130
#extension GL_EXT_gpu_shader4 : enable
uniform usampler2D tex;

//uniform sampler2D tex;

void main()
{
	//vec4 texture = texture2D(tex, gl_TexCoord[0].xy);
    uvec4 texture = texture2D(tex, gl_TexCoord[0].xy);
    float col=texture.r;
    
    col=float(mod(col,256.0))/256.0;
    gl_FragColor = vec4(col,col,col,1);//vec4(texture.r,texture.r,texture.r,1);
    // gl_FragColor = vec4( gl_TexCoord[0].xy,0,1);
}
