#version 330

// Input
in vec2 vUv;
in float noise;

uniform sampler2D tExplosion;

// Output
layout(location = 0) out vec4 out_color;

float random( vec3 scale, float seed ){
  return fract( sin( dot( gl_FragCoord.xyz + seed, scale ) ) * 43758.5453 + seed ) ;
}

void main()
{
    float r = .01 * random( vec3( 12.9898, 78.233, 151.7182 ), 0.0 );

    vec3 color = vec3( vUv * ( 1. - 2. * noise + r), 0.0 );
    out_color = vec4( color.rgb, 1.0 );

}
