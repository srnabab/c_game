#version 460
 
layout(location = 0) out vec2 outUV;

void main()
{
    int index = gl_VertexIndex;

    float x = -1.0f + float((index & 1) << 2);
    float y = -1.0f + float((index & 2) << 1);

    gl_Position = vec4(x, y, 0.0f, 1.0);

    outUV = vec2(x * 0.5 + 0.5, y * -0.5 + 0.5);
}