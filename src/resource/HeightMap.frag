//#define NO_KINECT

//1200 1400


#ifndef NO_KINECT

#version 130
#extension GL_EXT_gpu_shader4 : enable
uniform usampler2D height;

#else
uniform sampler2D height;

#endif

uniform float minH;
uniform float maxH;

uniform sampler2D level0;
uniform sampler2D level1;
uniform sampler2D level2;
uniform sampler2D level3;
uniform sampler2D level4;

uniform sampler2D gameTexture;


float weight(float heightV, int region)
{
    // 0 water: [0, 50]  
    // 1 sand : [51, 101] 
    // 2 grass: [102, 152]
    // 3 rock : [153, 203]
    // 4 snow : [204, 255]

    float rmax, rrng;
    if(region==0)
    {
        rmax=50.0;
        rrng=50.0-0.0;
    }
    else if(region == 1)
    {
        rmax=101.0;
        rrng=101.0-51.0;
    }
    else if(region == 2)
    {
        rmax=152.0;
        rrng=152.0-102.0;
    }
    else if(region == 3)
    {
        rmax=203.0;
        rrng=203.0-153.0;
    }
    else //if(region == 4)
    {
        rmax=255.0;
        rrng=255.0-204.0;
    }

    rmax/=255.0;
    rrng/=255.0;


    float w = (rrng - abs(heightV - rmax)) / rrng;
    return (w < 0.0) ? 0.0 : w;
}

void main()
{
    vec2 txtH=gl_TexCoord[0].xy/gl_TexCoord[0].z;
    if(txtH.x < 0.0 || txtH.x>1.0 || txtH.y <0.0 || txtH.y > 1.0)
    {
        gl_FragColor=vec4(0,0,0,1);
        return;
    }

    vec4 gameTxt = texture2D(gameTexture, txtH);



#ifdef NO_KINECT
    vec4 heightTxt = texture2D(height, txtH.xy);
#else
    uvec4 heightTxt = texture2D(height, txtH.xy);
#endif

    float heightV=heightTxt.r;

    heightV=(heightV-minH)/(maxH-minH);
    heightV=min(1.0,heightV);
    heightV=max(0.0,heightV);

#ifndef NO_KINECT
    heightV=1-heightV;
#endif


    vec2 txt=gl_TexCoord[1].xy/gl_TexCoord[1].z;


    vec4 level0Txt = texture2D(level0, txt);
    vec4 level1Txt = texture2D(level1, txt);
    vec4 level2Txt = texture2D(level2, txt);
    vec4 level3Txt = texture2D(level3, txt);
    vec4 level4Txt = texture2D(level4, txt);

    vec4 bgCol=
    weight(heightV,0)*level0Txt+
    weight(heightV,1)*level1Txt+
    weight(heightV,2)*level2Txt+
    weight(heightV,3)*level3Txt+
    weight(heightV,4)*level4Txt;


    gl_FragColor = gameTxt.a*vec4(gameTxt.rgb,1)+(1.0-gameTxt.a)*bgCol;
}

