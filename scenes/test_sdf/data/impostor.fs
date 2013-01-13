#version 130
in vec4 positionFrag;
uniform float time;
float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}
void main()
{
/*
    float ball1 = length(positionFrag - vec4(sin(0.02f*time), sin(0.07f*time), 0.f, 1.f)) - 0.1;
    float ball2 = length(positionFrag - vec4(sin(0.08f*time), sin(0.03f*time), 0.f, 1.f)) - 0.1;
    float mergeball = smoothstep(0, 1, ball1 - ball2);
    //float result = mix(ball1, ball2, mergeball);
    float result = min(ball1, ball2);
    */
    float result = 1.f;
    int i;
    mat4 mod2cam = mat4( cos(time*0.01f), -sin(time*0.01f), 0.0, 0.0,
			             sin(time*0.01f),  cos(time*0.01f), 0.0, 0.0,
			             0.0,           0.0, 1.0, 0.0,
				         0.0,           0.0, 0.0, 1.0 );
    /*
    mat4 mod2cam = mat4( 
			             1.0,           0.0, 0.0, 0.0,
                        0.0, cos(time*0.01f), -sin(time*0.01f), 0.0,
			             0.0, sin(time*0.01f),  cos(time*0.01f), 0.0,
				         0.0,           0.0, 0.0, 1.0 );
                         */
    vec4 position = mod2cam * positionFrag;
    for (i = 0; i < 100; i++) {
        float ball = length(position - vec4(
            sin(0.03f*rand(vec2(i, 0.f))*time + 10.f*rand(vec2(i, 0.5f))),
            sin(0.03f*rand(vec2(i, 1.f))*time + 10.f*rand(vec2(i, 1.5f))),
            sin(0.03f*rand(vec2(i, 2.f))*time + 10.f*rand(vec2(i, 2.5f))),
            1.f
        )) - rand(vec2(i, -1.0f))*0.1f;
        result = min(result, ball);
    }
    gl_FragColor = vec4(result < 0 ? 1 : 0);
}
