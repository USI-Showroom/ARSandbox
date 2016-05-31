#define NO_KINECT
//#version 130

#ifndef NO_KINECT

#extension GL_EXT_gpu_shader4 : enable

uniform usampler2D height0;
uniform usampler2D height1;
uniform usampler2D height2;
uniform usampler2D height3;

#else
uniform sampler2D height0;
uniform sampler2D height1;
uniform sampler2D height2;
uniform sampler2D height3;

#endif

uniform float minH;
uniform float maxH;

uniform sampler2D level0;
uniform sampler2D level1;
uniform sampler2D level2;
uniform sampler2D level3;
uniform sampler2D level4;

uniform sampler2D gameTexture;

// min and max values for Water and Terrain
uniform float maxW;
uniform float minW;
uniform float maxS;
uniform float minS;


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
    else
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
        gl_FragColor=vec4(1,1,1,1);
        return;
    }

    vec4 gameTxt = texture2D(gameTexture, txtH);

#ifdef NO_KINECT
    float heightV = 
                texture2D(height0, txtH.xy).r+
        0.5 *   texture2D(height1, txtH.xy).r+
        0.25 *  texture2D(height2, txtH.xy).r+
        0.125 * texture2D(height3, txtH.xy).r;

    heightV*=255.0/1.875;
#else
    float heightV = 
                float(texture2D(height0, txtH.xy).r)+
        0.5 *   float(texture2D(height1, txtH.xy).r)+
        0.25 *  float(texture2D(height2, txtH.xy).r)+
        0.125 * float(texture2D(height3, txtH.xy).r);

    heightV/=1.875;
#endif

    heightV=(heightV-minH)/(maxH-minH);

    

    heightV=min(1.0,heightV);
    heightV=max(0.0,heightV);

#ifndef NO_KINECT
    heightV=1-heightV;
#endif
    vec4 waterColor = vec4(0.0, 0.0, 1.0, 1.0);

    // red channel holds sediment amount
    // TODO: rescale back
    heightV += gameTxt.r * (maxS - minS) + minS;

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

    // TODO: rescale back
    float waterHeight = gameTxt.b * (maxW - minW) + minW;
    waterHeight = min(waterHeight, 1.0);
    waterHeight = max(waterHeight, 0.0);
    
    gl_FragColor = bgCol * (1.0 - waterHeight) + waterColor * waterHeight;
    // gl_FragColor = vec4(gameTxt.a,0,0,1);
    // isolines
    // float isoH = 0.0025;
    // for ( int i=0; i<10; i++) {
    //     float iF=float (i);
    //     float minIso = iF * (1.0 - isoH) / 9.0;
    //     float maxIso = minIso+isoH;

    //     if (heightV > minIso && heightV < maxIso) {
    //         gl_FragColor = vec4(0.64, 0.35, 0.25, 0.2);
    //         break;
    //     }
    // }
}

