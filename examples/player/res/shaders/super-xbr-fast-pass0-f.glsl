#version 130

#ifdef PARAMETER_UNIFORM
uniform float XBR_EDGE_STR;
uniform float XBR_WEIGHT;
#else
const float XBR_EDGE_STR = 0.0;
const float XBR_WEIGHT = 0.0;
#endif



#if __VERSION__ >= 130

#define COMPAT_VARYING in
#define COMPAT_TEXTURE texture

out vec4 FragColor;

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

struct input_dummy {
    vec2 _video_size;
    vec2 _texture_size;
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
float _TMP19;
vec3 _TMP37;
vec3 _TMP36;
vec3 _TMP35;
vec3 _TMP34;
vec3 _TMP33;
float _TMP38;
float _TMP18;
float _TMP17;
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
vec4 _TMP6;
vec4 _TMP5;
vec2 _TMP0;

uniform sampler2D Texture;

input_dummy _IN1;

vec2 _x0046;
bvec2 _a0050;
vec2 _c0060;
vec2 _c0062;
vec2 _c0064;
vec2 _c0068;
vec2 _c0070;
vec2 _c0072;
vec2 _c0074;
vec2 _c0076;
vec2 _c0078;
vec2 _c0080;
vec2 _c0082;
float _TMP83;
float _TMP87;
float _TMP91;
float _TMP95;
float _TMP99;
float _TMP103;
float _TMP107;
float _TMP111;
float _TMP115;
float _TMP123;
float _TMP131;
float _TMP139;
float _TMP149;
float _a0152;
float _TMP153;
float _a0156;
float _TMP157;
float _a0160;
float _TMP161;
float _a0164;
float _TMP181;
float _a0184;
float _TMP185;
float _a0188;
float _TMP189;
float _a0192;
float _TMP203;
float _a0206;
float _TMP207;
float _a0210;
float _TMP211;
float _a0214;
float _TMP215;
float _a0218;
float _TMP235;
float _a0238;
float _TMP239;
float _a0242;
float _TMP243;
float _a0246;
float _x0258;
float _TMP259;
vec3 _r0266;
vec3 _r0268;
vec3 _TMP271;
vec3 _TMP279;
vec3 _TMP287;
float _a0294;
float _t0296;

COMPAT_VARYING vec4 TEX0;
 
uniform int FrameDirection;
uniform int FrameCount;
uniform COMPAT_PRECISION vec2 OutputSize;
uniform COMPAT_PRECISION vec2 TextureSize;
uniform COMPAT_PRECISION vec2 InputSize;


void main()
{
    vec2 _g1;
    vec2 _g2;
    float _d_edge;
    float _edge_strength;
    vec3 _color1;


    _x0046 = TEX0.xy*TextureSize;
    _TMP0 = fract(_x0046);
    _a0050 = bvec2(_TMP0.x < 5.00000000E-01, _TMP0.y < 5.00000000E-01);
    if (_a0050.x || _a0050.y) { 
        _ret_0 = COMPAT_TEXTURE(Texture, TEX0.xy);
        FragColor = _ret_0;
        return;
    } 
    _g1 = vec2(1.00000000E+00/TextureSize.x, 0.00000000E+00);
    _g2 = vec2(0.00000000E+00, 1.00000000E+00/TextureSize.y);
    _c0060 = TEX0.xy - _g2;
    _TMP5 = COMPAT_TEXTURE(Texture, _c0060);
    _c0062 = (TEX0.xy + _g1) - _g2;
    _TMP6 = COMPAT_TEXTURE(Texture, _c0062);
    _c0064 = TEX0.xy - _g1;
    _TMP7 = COMPAT_TEXTURE(Texture, _c0064);
    _TMP8 = COMPAT_TEXTURE(Texture, TEX0.xy);
    _c0068 = TEX0.xy + _g1;
    _TMP9 = COMPAT_TEXTURE(Texture, _c0068);
    _c0070 = (TEX0.xy - _g1) + _g2;
    _TMP10 = COMPAT_TEXTURE(Texture, _c0070);
    _c0072 = TEX0.xy + _g2;
    _TMP11 = COMPAT_TEXTURE(Texture, _c0072);
    _c0074 = TEX0.xy + _g1 + _g2;
    _TMP12 = COMPAT_TEXTURE(Texture, _c0074);
    _c0076 = TEX0.xy + 2.00000000E+00*_g1;
    _TMP13 = COMPAT_TEXTURE(Texture, _c0076);
    _c0078 = TEX0.xy + _g2 + 2.00000000E+00*_g1;
    _TMP14 = COMPAT_TEXTURE(Texture, _c0078);
    _c0080 = TEX0.xy + 2.00000000E+00*_g2;
    _TMP15 = COMPAT_TEXTURE(Texture, _c0080);
    _c0082 = TEX0.xy + 2.00000000E+00*_g2 + _g1;
    _TMP16 = COMPAT_TEXTURE(Texture, _c0082);
    _TMP83 = dot(_TMP5.xyz, vec3( 2.12599993E-01, 7.15200007E-01, 7.22000003E-02));
    _TMP87 = dot(_TMP6.xyz, vec3( 2.12599993E-01, 7.15200007E-01, 7.22000003E-02));
    _TMP91 = dot(_TMP7.xyz, vec3( 2.12599993E-01, 7.15200007E-01, 7.22000003E-02));
    _TMP95 = dot(_TMP8.xyz, vec3( 2.12599993E-01, 7.15200007E-01, 7.22000003E-02));
    _TMP99 = dot(_TMP9.xyz, vec3( 2.12599993E-01, 7.15200007E-01, 7.22000003E-02));
    _TMP103 = dot(_TMP10.xyz, vec3( 2.12599993E-01, 7.15200007E-01, 7.22000003E-02));
    _TMP107 = dot(_TMP11.xyz, vec3( 2.12599993E-01, 7.15200007E-01, 7.22000003E-02));
    _TMP111 = dot(_TMP12.xyz, vec3( 2.12599993E-01, 7.15200007E-01, 7.22000003E-02));
    _TMP115 = dot(_TMP14.xyz, vec3( 2.12599993E-01, 7.15200007E-01, 7.22000003E-02));
    _TMP123 = dot(_TMP16.xyz, vec3( 2.12599993E-01, 7.15200007E-01, 7.22000003E-02));
    _TMP131 = dot(_TMP15.xyz, vec3( 2.12599993E-01, 7.15200007E-01, 7.22000003E-02));
    _TMP139 = dot(_TMP13.xyz, vec3( 2.12599993E-01, 7.15200007E-01, 7.22000003E-02));
    _a0152 = _TMP95 - _TMP87;
    _TMP149 = abs(_a0152);
    _a0156 = _TMP95 - _TMP103;
    _TMP153 = abs(_a0156);
    _a0160 = _TMP111 - _TMP131;
    _TMP157 = abs(_a0160);
    _a0164 = _TMP111 - _TMP139;
    _TMP161 = abs(_a0164);
    _a0184 = _TMP107 - _TMP99;
    _TMP181 = abs(_a0184);
    _a0188 = _TMP103 - _TMP87;
    _TMP185 = abs(_a0188);
    _a0192 = _TMP131 - _TMP139;
    _TMP189 = abs(_a0192);
    _TMP17 = _TMP149 + _TMP153 + _TMP157 + _TMP161 + 2.00000000E+00*_TMP181 + -(_TMP185 + _TMP189);
    _a0206 = _TMP99 - _TMP115;
    _TMP203 = abs(_a0206);
    _a0210 = _TMP99 - _TMP83;
    _TMP207 = abs(_a0210);
    _a0214 = _TMP107 - _TMP91;
    _TMP211 = abs(_a0214);
    _a0218 = _TMP107 - _TMP123;
    _TMP215 = abs(_a0218);
    _a0238 = _TMP95 - _TMP111;
    _TMP235 = abs(_a0238);
    _a0242 = _TMP83 - _TMP115;
    _TMP239 = abs(_a0242);
    _a0246 = _TMP91 - _TMP123;
    _TMP243 = abs(_a0246);
    _TMP18 = _TMP203 + _TMP207 + _TMP211 + _TMP215 + 2.00000000E+00*_TMP235 + -(_TMP239 + _TMP243);
    _d_edge = _TMP17 - _TMP18;
    _x0258 = (_d_edge - -9.99999997E-07)/1.99999999E-06;
    _TMP38 = min(1.00000000E+00, _x0258);
    _TMP259 = max(0.00000000E+00, _TMP38);
    _edge_strength = _TMP259*_TMP259*(3.00000000E+00 - 2.00000000E+00*_TMP259);
    _r0266 = 5.00000000E-01*_TMP11.xyz;
    _r0266 = _r0266 + 5.00000000E-01*_TMP9.xyz;
    _r0268 = 5.00000000E-01*_TMP8.xyz;
    _r0268 = _r0268 + 5.00000000E-01*_TMP12.xyz;
    _color1 = _r0266 + _edge_strength*(_r0268 - _r0266);
    _TMP33 = min(_TMP11.xyz, _TMP12.xyz);
    _TMP34 = min(_TMP9.xyz, _TMP33);
    _TMP271 = min(_TMP8.xyz, _TMP34);
    _TMP35 = max(_TMP11.xyz, _TMP12.xyz);
    _TMP36 = max(_TMP9.xyz, _TMP35);
    _TMP279 = max(_TMP8.xyz, _TMP36);
    _TMP37 = min(_TMP279, _color1);
    _TMP287 = max(_TMP271, _TMP37);
    _a0294 = _edge_strength - 5.00000000E-01;
    _TMP19 = abs(_a0294);
    _t0296 = 1.00000000E+00 - 2.00000000E+00*_TMP19;
    _color1 = _color1 + _t0296*(_TMP287 - _color1);
    _ret_0 = vec4(_color1.x, _color1.y, _color1.z, 1.00000000E+00);

     FragColor = _ret_0;


    return;
} 
