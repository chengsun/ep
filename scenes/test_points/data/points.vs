#version 130
in float position;
uniform float time;
float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}
void main()
{
    gl_Position = vec4(rand(vec2(position, time)), rand(vec2(position, 1.f + time)), 0.f, 1.f);
    //gl_Position = vec4(gl_VertexID/2.f, gl_VertexID/2.f, 0.f, 1.f);
}
