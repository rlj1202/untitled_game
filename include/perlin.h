#ifndef H_PERLIN
#define H_PERLIN

#include <glm/glm.hpp>

int permutation[] = { 151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 
                      103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23, 190, 6, 148, 247, 120, 234, 75, 0, 
                      26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33, 88, 237, 149, 56, 
                      87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166, 
                      77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 
                      46, 245, 40, 244, 102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 
                      187, 208, 89, 18, 169, 200, 196, 135, 130, 116, 188, 159, 86, 164, 100, 109, 
                      198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123, 5, 202, 38, 147, 118, 126, 
                      255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42, 223, 183, 
                      170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 
                      172, 9, 129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 
                      104, 218, 246, 97, 228, 251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 
                      241, 81, 51, 145, 235, 249, 14, 239, 107, 49, 192, 214, 31, 181, 199, 106, 
                      157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254, 138, 236, 205, 
                      93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180 };

float Perlin(glm::vec2 point);

glm::vec2 RandomGradient(glm::ivec2 point) {
    // Random float. No precomputed gradients mean this works for any number of grid coordinates
    float random = 2920.f
        * sin(point.x * 21942.f + point.y * 171324.f + 8912.f)
        * cos(point.x * 23157.f * point.y * 217832.f + 9758.f);

    return glm::vec2(cos(random), sin(random));
}

float Interpolate(float a, float b, float w) {
    return (b - a) * w + a;
}

float Perlin(glm::vec2 point) {
    // Determine grid cell coordinates
    glm::ivec2 lb_point = glm::ivec2(floor(point.x), floor(point.y));
    glm::ivec2 rb_point = lb_point + glm::ivec2(1, 0);
    glm::ivec2 lu_point = lb_point + glm::ivec2(0, 1);
    glm::ivec2 ru_point = lb_point + glm::ivec2(1, 1);

    // Determine interpolation weights
    // Could also use higher order polynomial/s-curve here
    glm::vec2 s = point - (glm::vec2) lb_point;

    // Interpolate between grid point gradients
    float n0 = glm::dot(RandomGradient(lb_point), point - (glm::vec2) lb_point);
    float n1 = glm::dot(RandomGradient(rb_point), point - (glm::vec2) rb_point);
    float n2 = glm::dot(RandomGradient(lu_point), point - (glm::vec2) lu_point);
    float n3 = glm::dot(RandomGradient(ru_point), point - (glm::vec2) ru_point);

    float value = Interpolate(
        Interpolate(n0, n1, s.x),
        Interpolate(n2, n3, s.x),
        s.y
    );

    return value;
}

#endif