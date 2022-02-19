#version 460 core

layout(early_fragment_tests) in;

flat in uint pass_objectID;

layout(std430, binding = 0) buffer visibilityBuffer
{
    int visibility[];
};

void main()
{
    visibility[pass_objectID] = 1;
}