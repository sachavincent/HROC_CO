#version 460 core

layout(early_fragment_tests) in;

buffer visibilityBuffer
{
    int visibility[];
};

flat in int pass_objectID;

void main()
{
    visibility[pass_objectID] = 1;
}