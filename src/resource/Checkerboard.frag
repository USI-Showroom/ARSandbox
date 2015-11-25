

void main()
{
    vec2 txt=gl_TexCoord[1].xy/gl_TexCoord[1].z;

    float nSquares=10.0;    
    txt*=nSquares;

    int tx=int(floor(txt.x));
    int ty=int(floor(txt.y));

    int sum=tx+ty;
    int div=sum/2;
    int modulus=sum-2*div;

    float val = float((modulus < 1) ? 0 : 1);
    gl_FragColor = vec4(val,val,val,1);
}