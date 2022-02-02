#version 440 core

out vec4 FragColor;

in vec3 FragPos_in;
in vec2 TexCoords_in;
in vec3 Normal_in;
in int Depth_in;

#define ALPHA = 0.2;
#define MAX_DEPTH = 16;

const vec3 colors[MAX_DEPTH] = vec3[] {vec3(244, 67, 54),vec3(233, 30, 99),vec3(156, 39, 176),vec3(103, 58, 183),
                                vec3(63, 81, 181),vec3(33, 150, 243),vec3(3, 169, 244),vec3(0, 188, 212),
                                vec3(0, 150, 136),vec3(76, 175, 80),vec3(139, 195, 74),vec3(205, 220, 57),
                                vec3(255, 235, 59),vec3(255, 193, 7),vec3(255, 152, 0),vec3(255, 87, 34)};


void main()
{
    vec3 color = colors[min(Depth_in, MAX_DEPTH)];

    FragColor = vec4(color, ALPHA);
}