#version 130

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
vec2 _c0058;
vec2 _c0060;
vec2 _c0062;
vec2 _c0064;
vec2 _c0066;
vec2 _c0068;
vec2 _c0070;
vec2 _c0072;
vec2 _c0074;
vec2 _c0076;
vec2 _c0078;
vec2 _c0080;
float _TMP81;
float _TMP85;
float _TMP89;
float _TMP93;
float _TMP97;
float _TMP101;
float _TMP105;
float _TMP109;
float _TMP113;
float _TMP121;
float _TMP129;
float _TMP137;
float _TMP147;
float _a0150;
float _TMP151;
float _a0154;
float _TMP155;
float _a0158;
float _TMP159;
float _a0162;
float _TMP201;
float _a0204;
float _TMP205;
float _a0208;
float _TMP209;
float _a0212;
float _TMP213;
float _a0216;
float _x0256;
float _TMP257;
vec3 _r0264;
vec3 _r0266;
vec3 _TMP269;
vec3 _TMP277;
vec3 _TMP285;
float _a0292;
float _t0294;
COMPAT_VARYING vec4 TEX0;
 
uniform int FrameDirection;
uniform int FrameCount;
uniform COMPAT_PRECISION vec2 OutputSize;
uniform COMPAT_PRECISION vec2 TextureSize;
uniform COMPAT_PRECISION vec2 InputSize;
void main()
{
    vec2 _dir;
    vec2 _g1;
    vec2 _g2;
    float _d_edge;
    float _edge_strength;
    vec3 _color1;
    _x0046 = (TEX0.xy*TextureSize)/2.00000000E+00;
    _TMP0 = fract(_x0046);
    _dir = _TMP0 - vec2( 5.00000000E-01, 5.00000000E-01);
    if (_dir.x*_dir.y > 0.00000000E+00) { 
        _ret_0 = COMPAT_TEXTURE(Texture, TEX0.xy);
        FragColor = _ret_0;
        return;
    } 
    _g1 = vec2(1.00000000E+00/TextureSize.x, 0.00000000E+00);
    _g2 = vec2(0.00000000E+00, 1.00000000E+00/TextureSize.y);
    _c0058 = (TEX0.xy - 2.00000000E+00*_g1) - _g2;
    _TMP5 = COMPAT_TEXTURE(Texture, _c0058);
    _c0060 = (TEX0.xy - _g1) - 2.00000000E+00*_g2;
    _TMP6 = COMPAT_TEXTURE(Texture, _c0060);
    _c0062 = (TEX0.xy - 2.00000000E+00*_g1) + _g2;
    _TMP7 = COMPAT_TEXTURE(Texture, _c0062);
    _c0064 = TEX0.xy - _g1;
    _TMP8 = COMPAT_TEXTURE(Texture, _c0064);
    _c0066 = TEX0.xy - _g2;
    _TMP9 = COMPAT_TEXTURE(Texture, _c0066);
    _c0068 = (TEX0.xy - _g1) + 2.00000000E+00*_g2;
    _TMP10 = COMPAT_TEXTURE(Texture, _c0068);
    _c0070 = TEX0.xy + _g2;
    _TMP11 = COMPAT_TEXTURE(Texture, _c0070);
    _c0072 = TEX0.xy + _g1;
    _TMP12 = COMPAT_TEXTURE(Texture, _c0072);
    _c0074 = (TEX0.xy + _g1) - 2.00000000E+00*_g2;
    _TMP13 = COMPAT_TEXTURE(Texture, _c0074);
    _c0076 = (TEX0.xy + 2.00000000E+00*_g1) - _g2;
    _TMP14 = COMPAT_TEXTURE(Texture, _c0076);
    _c0078 = TEX0.xy + _g1 + 2.00000000E+00*_g2;
    _TMP15 = COMPAT_TEXTURE(Texture, _c0078);
    _c0080 = TEX0.xy + 2.00000000E+00*_g1 + _g2;
    _TMP16 = COMPAT_TEXTURE(Texture, _c0080);
    _TMP81 = dot(_TMP5.xyz, vec3( 2.12599993E-01, 7.15200007E-01, 7.22000003E-02));
    _TMP85 = dot(_TMP6.xyz, vec3( 2.12599993E-01, 7.15200007E-01, 7.22000003E-02));
    _TMP89 = dot(_TMP7.xyz, vec3( 2.12599993E-01, 7.15200007E-01, 7.22000003E-02));
    _TMP93 = dot(_TMP8.xyz, vec3( 2.12599993E-01, 7.15200007E-01, 7.22000003E-02));
    _TMP97 = dot(_TMP9.xyz, vec3( 2.12599993E-01, 7.15200007E-01, 7.22000003E-02));
    _TMP101 = dot(_TMP10.xyz, vec3( 2.12599993E-01, 7.15200007E-01, 7.22000003E-02));
    _TMP105 = dot(_TMP11.xyz, vec3( 2.12599993E-01, 7.15200007E-01, 7.22000003E-02));
    _TMP109 = dot(_TMP12.xyz, vec3( 2.12599993E-01, 7.15200007E-01, 7.22000003E-02));
    _TMP113 = dot(_TMP14.xyz, vec3( 2.12599993E-01, 7.15200007E-01, 7.22000003E-02));
    _TMP121 = dot(_TMP16.xyz, vec3( 2.12599993E-01, 7.15200007E-01, 7.22000003E-02));
    _TMP129 = dot(_TMP15.xyz, vec3( 2.12599993E-01, 7.15200007E-01, 7.22000003E-02));
    _TMP137 = dot(_TMP13.xyz, vec3( 2.12599993E-01, 7.15200007E-01, 7.22000003E-02));
    _a0150 = _TMP93 - _TMP85;
    _TMP147 = abs(_a0150);
    _a0154 = _TMP93 - _TMP101;
    _TMP151 = abs(_a0154);
    _a0158 = _TMP109 - _TMP129;
    _TMP155 = abs(_a0158);
    _a0162 = _TMP109 - _TMP137;
    _TMP159 = abs(_a0162);
    _TMP17 = 2.00000000E+00*(_TMP147 + _TMP151 + _TMP155 + _TMP159);
    _a0204 = _TMP97 - _TMP113;
    _TMP201 = abs(_a0204);
    _a0208 = _TMP97 - _TMP81;
    _TMP205 = abs(_a0208);
    _a0212 = _TMP105 - _TMP89;
    _TMP209 = abs(_a0212);
    _a0216 = _TMP105 - _TMP121;
    _TMP213 = abs(_a0216);
    _TMP18 = 2.00000000E+00*(_TMP201 + _TMP205 + _TMP209 + _TMP213);
    _d_edge = _TMP17 - _TMP18;
    _x0256 = (_d_edge - -9.99999997E-07)/1.99999999E-06;
    _TMP38 = min(1.00000000E+00, _x0256);
    _TMP257 = max(0.00000000E+00, _TMP38);
    _edge_strength = _TMP257*_TMP257*(3.00000000E+00 - 2.00000000E+00*_TMP257);
    _r0264 = 5.00000000E-01*_TMP11.xyz;
    _r0264 = _r0264 + 5.00000000E-01*_TMP9.xyz;
    _r0266 = 5.00000000E-01*_TMP8.xyz;
    _r0266 = _r0266 + 5.00000000E-01*_TMP12.xyz;
    _color1 = _r0264 + _edge_strength*(_r0266 - _r0264);
    _TMP33 = min(_TMP11.xyz, _TMP12.xyz);
    _TMP34 = min(_TMP9.xyz, _TMP33);
    _TMP269 = min(_TMP8.xyz, _TMP34);
    _TMP35 = max(_TMP11.xyz, _TMP12.xyz);
    _TMP36 = max(_TMP9.xyz, _TMP35);
    _TMP277 = max(_TMP8.xyz, _TMP36);
    _TMP37 = min(_TMP277, _color1);
    _TMP285 = max(_TMP269, _TMP37);
    _a0292 = _edge_strength - 5.00000000E-01;
    _TMP19 = abs(_a0292);
    _t0294 = 1.00000000E+00 - 2.00000000E+00*_TMP19;
    _color1 = _color1 + _t0294*(_TMP285 - _color1);
    _ret_0 = vec4(_color1.x, _color1.y, _color1.z, 1.00000000E+00);
    FragColor = _ret_0;
    return;
} 

