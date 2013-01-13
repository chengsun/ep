#version 130
in vec4 position;
out vec4 positionFrag;
void main()
{
   positionFrag = position;
   gl_Position = position;
}
