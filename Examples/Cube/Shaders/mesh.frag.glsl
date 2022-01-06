﻿// Copyright 2021 SMS
// License(Apache-2.0)

#version 450

const float PI = 3.141592654;

struct Material
{
    sampler2D albedo;
    sampler2D metallic;
    sampler2D roughness;
    sampler2D ao;
    sampler2D emissive;
    sampler2D normal;
};

uniform Material mat;

layout (location = 0) out vec4 frag_color;

in Out
{
    vec3 position;
    vec3 normal;
    vec2 uv;
    vec3 tangent;
    vec3 bitangent;
} vert;

void main()
{
    // frag_color = vec4(texture(mat.albedo, vert.uv));
    frag_color = vec4(vert.uv, 0.0, 0.1);
}
