#version 400 core
out vec4 FragColor; // declare a vec4 variable that will contain the output color of the pixel
in vec3 ourColor; // the input variable from the vertex shader (same name and same type)
void main()
{
   FragColor = vec4(ourColor, 1.0f); // set the output variable to a dark-red color
}
