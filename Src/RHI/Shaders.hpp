#pragma once

// 顶点着色器代码
const char* vertexShader = R"(
    #version 450
    layout(location = 0) in vec3 position;
    layout(location = 1) in vec4 color;
    
    layout(location = 0) out vec4 fragColor;
    
    void main() {
        gl_Position = vec4(position, 1.0);
        fragColor = color;
    }
)";

// 片段着色器代码
const char* fragmentShader = R"(
    #version 450
    layout(location = 0) in vec4 fragColor;
    layout(location = 0) out vec4 outColor;
    
    void main() {
        outColor = fragColor;
    }
)"; 