#version 330 core  
layout(location = 0) in vec3 position;  
layout(location = 1) in vec2 texcoord;  

out vec2 Texcoord;  

uniform mat4 model; // если планируете использовать  
uniform mat4 view;  
uniform mat4 projection;  

void main()  
{  
    gl_Position = projection * view * model * vec4(position, 1.0);  
    Texcoord = texcoord;   
}  