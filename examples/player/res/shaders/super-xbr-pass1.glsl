// GLSL shader autogenerated by cg2glsl.py.
#if defined(VERTEX)

#if __VERSION__ >= 130
#define COMPAT_VARYING out
#define COMPAT_ATTRIBUTE in
#define COMPAT_TEXTURE texture
#else
#define COMPAT_VARYING varying
#define COMPAT_ATTRIBUTE attribute
#define COMPAT_TEXTURE texture2D
#endif

#ifdef GL_ES
#define COMPAT_PRECISION mediump
#else
#define COMPAT_PRECISION
#endif
COMPAT_VARYING     vec2 _texCoord1;
COMPAT_VARYING     vec4 _color1;
COMPAT_VARYING     vec4 _position1;
COMPAT_VARYING     float _frame_rotation;
struct prev {
    vec2 _video_size;
    vec2 _texture_size;
float _placeholder23;
};
struct input_dummy {
    vec2 _video_size1;
    vec2 _texture_size1;
    vec2 _output_dummy_size;
    float _frame_count;
    float _frame_direction;
    float _frame_rotation;
};
struct out_vertex {
    vec4 _position1;
    vec4 _color1;
    vec2 _texCoord1;
};
out_vertex _ret_0;
vec4 _r0009;
COMPAT_ATTRIBUTE vec4 VertexCoord;
COMPAT_ATTRIBUTE vec4 COLOR;
COMPAT_ATTRIBUTE vec4 TexCoord;
COMPAT_VARYING vec4 COL0;
COMPAT_VARYING vec4 TEX0;
 
uniform mat4 MVPMatrix;
uniform int FrameDirection;
uniform int FrameCount;
uniform COMPAT_PRECISION vec2 OutputSize;
uniform COMPAT_PRECISION vec2 TextureSize;
uniform COMPAT_PRECISION vec2 InputSize;
void main()
{
    _r0009 = VertexCoord.x*MVPMatrix[0];
    _r0009 = _r0009 + VertexCoord.y*MVPMatrix[1];
    _r0009 = _r0009 + VertexCoord.z*MVPMatrix[2];
    _r0009 = _r0009 + VertexCoord.w*MVPMatrix[3];
    _ret_0._position1 = _r0009;
    _ret_0._color1 = COLOR;
    _ret_0._texCoord1 = TexCoord.xy;
    gl_Position = _r0009;
    COL0 = COLOR;
    TEX0.xy = TexCoord.xy;
    return;
    COL0 = _ret_0._color1;
    TEX0.xy = _ret_0._texCoord1;
} 
#elif defined(FRAGMENT)

#if __VERSION__ >= 130
#define COMPAT_VARYING in
#define COMPAT_TEXTURE texture
out vec4 gl_FragColor;
#else
#define COMPAT_VARYING varying
#define FragColor gl_FragColor
#define COMPAT_TEXTURE texture2D
#endif

#ifdef GL_ES
#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif
#define COMPAT_PRECISION mediump
#else
#define COMPAT_PRECISION
#endif
COMPAT_VARYING     vec2 _texCoord;
COMPAT_VARYING     vec4 _color2;
COMPAT_VARYING     float _frame_rotation;
struct prev {
    vec2 _video_size;
    vec2 _texture_size;
float _placeholder29;
};
struct input_dummy {
    vec2 _video_size1;
    vec2 _texture_size1;
    vec2 _output_dummy_size;
    float _frame_count;
    float _frame_direction;
    float _frame_rotation;
};
struct out_vertex {
    vec4 _color2;
    vec2 _texCoord;
};
vec4 _ret_0;
vec3 _TMP55;
vec3 _TMP54;
vec3 _TMP53;
vec3 _TMP52;
vec3 _TMP51;
vec3 _TMP27;
vec3 _TMP25;
float _TMP26;
float _TMP24;
float _TMP23;
float _TMP56;
float _TMP22;
float _TMP21;
float _TMP20;
float _TMP19;
vec4 _TMP18;
vec4 _TMP17;
vec4 _TMP16;
vec4 _TMP15;
vec4 _TMP14;
vec4 _TMP13;
vec4 _TMP12;
vec4 _TMP11;
vec4 _TMP10;
vec4 _TMP9;
vec4 _TMP8;
vec4 _TMP7;
vec2 _TMP2;
vec2 _TMP1;
vec4 _TMP0;
uniform sampler2D Texture0;
prev _PASSPREV21;
input_dummy _IN1;
vec2 _x0067;
vec2 _c0081;
vec2 _c0083;
vec2 _c0085;
vec2 _c0087;
vec2 _c0089;
vec2 _c0091;
vec2 _c0093;
vec2 _c0095;
vec2 _c0097;
vec2 _c0099;
vec2 _c0101;
vec2 _c0103;
float _TMP104;
float _TMP108;
float _TMP112;
float _TMP116;
float _TMP120;
float _TMP124;
float _TMP128;
float _TMP132;
float _TMP136;
float _TMP144;
float _TMP152;
float _TMP160;
float _TMP170;
float _a0173;
float _TMP174;
float _a0177;
float _TMP178;
float _a0181;
float _TMP182;
float _a0185;
float _TMP224;
float _a0227;
float _TMP228;
float _a0231;
float _TMP232;
float _a0235;
float _TMP236;
float _a0239;
float _TMP286;
float _a0289;
float _TMP290;
float _a0293;
float _TMP294;
float _a0297;
float _TMP298;
float _a0301;
float _TMP328;
float _a0331;
float _TMP332;
float _a0335;
float _TMP336;
float _a0339;
float _TMP340;
float _a0343;
float _x0365;
float _TMP366;
vec3 _r0373;
vec3 _r0375;
vec3 _r0377;
vec3 _r0379;
float _t0389;
vec3 _TMP390;
vec3 _TMP398;
vec3 _TMP406;
COMPAT_VARYING vec4 TEX0;
 
uniform sampler2D PassPrev2Texture;
uniform int FrameDirection;
uniform int FrameCount;
uniform COMPAT_PRECISION vec2 OutputSize;
uniform COMPAT_PRECISION vec2 TextureSize;
uniform COMPAT_PRECISION vec2 InputSize;
void main()
{
    vec2 _fp;
    vec2 _dir;
    float _d_edge;
    float _hv_edge;
    float _edge_strength;
    vec3 _color1;
    vec3 _TMP62[4];
    vec3 _TMP63[4];
    _x0067 = TEX0.xy*TextureSize;
    _fp = fract(_x0067);
    _dir = _fp - vec2( 5.00000000E-01, 5.00000000E-01);
    if (_dir.x*_dir.y > 0.00000000E+00) { 
        if (_fp.x > 5.00000000E-01) { 
            _TMP0 = COMPAT_TEXTURE(Texture0, TEX0.xy);
        } else {
            _TMP0 = COMPAT_TEXTURE(PassPrev2Texture, TEX0.xy);
        } 
        gl_FragColor = _TMP0;
        return;
    } 
    if (_fp.x > 5.00000000E-01) { 
        _TMP1 = vec2(5.00000000E-01/TextureSize.x, 0.00000000E+00);
    } else {
        _TMP1 = vec2(0.00000000E+00, 5.00000000E-01/TextureSize.y);
    } 
    if (_fp.x > 5.00000000E-01) { 
        _TMP2 = vec2(0.00000000E+00, 5.00000000E-01/TextureSize.y);
    } else {
        _TMP2 = vec2(5.00000000E-01/TextureSize.x, 0.00000000E+00);
    } 
    _c0081 = (TEX0.xy - 2.00000000E+00*_TMP1) - _TMP2;
    _TMP7 = COMPAT_TEXTURE(Texture0, _c0081);
    _c0083 = (TEX0.xy - _TMP1) - 2.00000000E+00*_TMP2;
    _TMP8 = COMPAT_TEXTURE(PassPrev2Texture, _c0083);
    _c0085 = (TEX0.xy - 2.00000000E+00*_TMP1) + _TMP2;
    _TMP9 = COMPAT_TEXTURE(Texture0, _c0085);
    _c0087 = TEX0.xy - _TMP1;
    _TMP10 = COMPAT_TEXTURE(PassPrev2Texture, _c0087);
    _c0089 = TEX0.xy - _TMP2;
    _TMP11 = COMPAT_TEXTURE(Texture0, _c0089);
    _c0091 = (TEX0.xy - _TMP1) + 2.00000000E+00*_TMP2;
    _TMP12 = COMPAT_TEXTURE(PassPrev2Texture, _c0091);
    _c0093 = TEX0.xy + _TMP2;
    _TMP13 = COMPAT_TEXTURE(Texture0, _c0093);
    _c0095 = TEX0.xy + _TMP1;
    _TMP14 = COMPAT_TEXTURE(PassPrev2Texture, _c0095);
    _c0097 = (TEX0.xy + _TMP1) - 2.00000000E+00*_TMP2;
    _TMP15 = COMPAT_TEXTURE(PassPrev2Texture, _c0097);
    _c0099 = (TEX0.xy + 2.00000000E+00*_TMP1) - _TMP2;
    _TMP16 = COMPAT_TEXTURE(Texture0, _c0099);
    _c0101 = TEX0.xy + _TMP1 + 2.00000000E+00*_TMP2;
    _TMP17 = COMPAT_TEXTURE(PassPrev2Texture, _c0101);
    _c0103 = TEX0.xy + 2.00000000E+00*_TMP1 + _TMP2;
    _TMP18 = COMPAT_TEXTURE(Texture0, _c0103);
    _TMP104 = dot(_TMP7.xyz, vec3( 2.12599993E-01, 7.15200007E-01, 7.22000003E-02));
    _TMP108 = dot(_TMP8.xyz, vec3( 2.12599993E-01, 7.15200007E-01, 7.22000003E-02));
    _TMP112 = dot(_TMP9.xyz, vec3( 2.12599993E-01, 7.15200007E-01, 7.22000003E-02));
    _TMP116 = dot(_TMP10.xyz, vec3( 2.12599993E-01, 7.15200007E-01, 7.22000003E-02));
    _TMP120 = dot(_TMP11.xyz, vec3( 2.12599993E-01, 7.15200007E-01, 7.22000003E-02));
    _TMP124 = dot(_TMP12.xyz, vec3( 2.12599993E-01, 7.15200007E-01, 7.22000003E-02));
    _TMP128 = dot(_TMP13.xyz, vec3( 2.12599993E-01, 7.15200007E-01, 7.22000003E-02));
    _TMP132 = dot(_TMP14.xyz, vec3( 2.12599993E-01, 7.15200007E-01, 7.22000003E-02));
    _TMP136 = dot(_TMP16.xyz, vec3( 2.12599993E-01, 7.15200007E-01, 7.22000003E-02));
    _TMP144 = dot(_TMP18.xyz, vec3( 2.12599993E-01, 7.15200007E-01, 7.22000003E-02));
    _TMP152 = dot(_TMP17.xyz, vec3( 2.12599993E-01, 7.15200007E-01, 7.22000003E-02));
    _TMP160 = dot(_TMP15.xyz, vec3( 2.12599993E-01, 7.15200007E-01, 7.22000003E-02));
    _a0173 = _TMP116 - _TMP108;
    _TMP170 = abs(_a0173);
    _a0177 = _TMP116 - _TMP124;
    _TMP174 = abs(_a0177);
    _a0181 = _TMP132 - _TMP152;
    _TMP178 = abs(_a0181);
    _a0185 = _TMP132 - _TMP160;
    _TMP182 = abs(_a0185);
    _TMP19 = 2.00000000E+00*(_TMP170 + _TMP174 + _TMP178 + _TMP182);
    _a0227 = _TMP120 - _TMP136;
    _TMP224 = abs(_a0227);
    _a0231 = _TMP120 - _TMP104;
    _TMP228 = abs(_a0231);
    _a0235 = _TMP128 - _TMP112;
    _TMP232 = abs(_a0235);
    _a0239 = _TMP128 - _TMP144;
    _TMP236 = abs(_a0239);
    _TMP20 = 2.00000000E+00*(_TMP224 + _TMP228 + _TMP232 + _TMP236);
    _d_edge = _TMP19 - _TMP20;
    _a0289 = _TMP120 - _TMP108;
    _TMP286 = abs(_a0289);
    _a0293 = _TMP132 - _TMP144;
    _TMP290 = abs(_a0293);
    _a0297 = _TMP116 - _TMP104;
    _TMP294 = abs(_a0297);
    _a0301 = _TMP128 - _TMP152;
    _TMP298 = abs(_a0301);
    _TMP21 = 2.00000000E+00*(_TMP286 + _TMP290 + _TMP294 + _TMP298);
    _a0331 = _TMP116 - _TMP112;
    _TMP328 = abs(_a0331);
    _a0335 = _TMP120 - _TMP160;
    _TMP332 = abs(_a0335);
    _a0339 = _TMP128 - _TMP124;
    _TMP336 = abs(_a0339);
    _a0343 = _TMP132 - _TMP136;
    _TMP340 = abs(_a0343);
    _TMP22 = 2.00000000E+00*(_TMP328 + _TMP332 + _TMP336 + _TMP340);
    _hv_edge = _TMP21 - _TMP22;
    _TMP23 = abs(_d_edge);
    _x0365 = _TMP23/9.99999997E-07;
    _TMP56 = min(1.00000000E+00, _x0365);
    _TMP366 = max(0.00000000E+00, _TMP56);
    _edge_strength = _TMP366*_TMP366*(3.00000000E+00 - 2.00000000E+00*_TMP366);
    _r0373 = 5.00000000E-01*_TMP13.xyz;
    _r0373 = _r0373 + 5.00000000E-01*_TMP11.xyz;
    _r0375 = 5.00000000E-01*_TMP10.xyz;
    _r0375 = _r0375 + 5.00000000E-01*_TMP14.xyz;
    _TMP63[1] = _TMP10.xyz + _TMP13.xyz;
    _TMP63[2] = _TMP11.xyz + _TMP14.xyz;
    _r0377 = 2.50000000E-01*_TMP63[1];
    _r0377 = _r0377 + 2.50000000E-01*_TMP63[2];
    _TMP62[1] = _TMP11.xyz + _TMP10.xyz;
    _TMP62[2] = _TMP14.xyz + _TMP13.xyz;
    _r0379 = 2.50000000E-01*_TMP62[1];
    _r0379 = _r0379 + 2.50000000E-01*_TMP62[2];
    _TMP24 = float((_d_edge >= 0.00000000E+00));
    _TMP25 = _r0373 + _TMP24*(_r0375 - _r0373);
    _TMP26 = float((_hv_edge >= 0.00000000E+00));
    _TMP27 = _r0377 + _TMP26*(_r0379 - _r0377);
    _t0389 = 1.00000000E+00 - _edge_strength;
    _color1 = _TMP25 + _t0389*(_TMP27 - _TMP25);
    _TMP51 = min(_TMP13.xyz, _TMP14.xyz);
    _TMP52 = min(_TMP11.xyz, _TMP51);
    _TMP390 = min(_TMP10.xyz, _TMP52);
    _TMP53 = max(_TMP13.xyz, _TMP14.xyz);
    _TMP54 = max(_TMP11.xyz, _TMP53);
    _TMP398 = max(_TMP10.xyz, _TMP54);
    _TMP55 = min(_TMP398, _color1);
    _TMP406 = max(_TMP390, _TMP55);
    _ret_0 = vec4(_TMP406.x, _TMP406.y, _TMP406.z, 1.00000000E+00);
    gl_FragColor = _ret_0;
    return;
} 
#endif
