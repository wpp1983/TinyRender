#pragma once

// 通用的顶点数据结构
struct Vertex {
    float position[3];  // x, y, z
    float color[4];     // r, g, b, a
};

// 定义三角形的顶点数据
const Vertex triangleVertices[] = {
    { { 0.0f, 0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },  // 顶点1 (红色)
    { { 0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } }, // 顶点2 (绿色)
    { { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }  // 顶点3 (蓝色)
}; 