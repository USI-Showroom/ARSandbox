#version 130
// #extension GL_EXT_gpu_shader4 : enable
// uniform usampler2D tex1;

uniform sampler2D height;

uniform sampler2D level0;
uniform sampler2D level1;
uniform sampler2D level2;
uniform sampler2D level3;
uniform sampler2D level4;


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
	vec4 heightTxt = texture2D(height, gl_TexCoord[0].xy);
    // uvec4 heightTxt = texture2D(tex1, gl_TexCoord[0].xy);

    float heightV=heightTxt.r;
    // height=float(mod(height,256.0))/256.0;

    
    vec4 level0Txt = texture2D(level0, gl_TexCoord[1].xy);
    vec4 level1Txt = texture2D(level1, gl_TexCoord[1].xy);
    vec4 level2Txt = texture2D(level2, gl_TexCoord[1].xy);
    vec4 level3Txt = texture2D(level3, gl_TexCoord[1].xy);
    vec4 level4Txt = texture2D(level4, gl_TexCoord[1].xy);

    gl_FragColor=   weight(heightV,0)*level0Txt+
                    weight(heightV,1)*level1Txt+
                    weight(heightV,2)*level2Txt+
                    weight(heightV,3)*level3Txt+
                    weight(heightV,4)*level4Txt;
}

