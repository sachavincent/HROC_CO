#version 460 core
out vec4 FragColor;
uniform bool outline;

void main()
{
    if (outline){
        FragColor = vec4(0.0, 1.0, 0.451, 1.0);
    }
    else{
        FragColor = vec4(1.0,0.0,0.0,0.1);
    }
}