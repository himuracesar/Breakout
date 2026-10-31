#include "math.h"

/**
 * Get the Distance between two points in 2D space.
 * 
 * @param a Vector2 point A.
 * @param b Vector2 point B.
 * @return float Distance between point A and point B.
 */
float GetDistance(Vector2 a, Vector2 b) {
    float dx = b.x - a.x;
    float dy = b.y - a.y;

    float distance = sqrtf(dx * dx + dy * dy);

    return distance;
}

/**
 * Compute the dot product for two vectors.
 * 
 * @param a Vector A.
 * @param b Vector B.
 * @return float Dot product of two vectors.
 */
float Dot(Vector2 a, Vector2 b) {
    return a.x * b.x + a.y * b.y;
}

/**
 * Compute the reflect vector between velocity and normal.
 * 
 * @param velocity Velocity of the object
 * @param normal Normal of the surface when hit the object. Must be normalized.
 * @return Vector2 Reflect vector.
 */
Vector2 reflect(Vector2 velocity, Vector2 normal) {
    float dot = Dot(velocity, normal);

    Vector2 result;
    result.x = velocity.x - 2.0f * dot * normal.x;
    result.y = velocity.y - 2.0f * dot * normal.y;

    return result;
}

/**
 * Normalize the vector
 * 
 * @param v Vector to normalize.
 * @return Vector2 Normalizez vector.
 */
Vector2 normalized(Vector2 v) {
    float l = length(v);
	Vector2 norm = { v.x / l, v.y / l };

	return norm;
}

/**
 * Compute the length of a vector.
 * 
 */
float length(Vector2 v)  {
    return sqrtf(powf(v.x, 2.0f) + powf(v.y, 2.0f));
}

/**
 * Linear interpolation.
 * 
 * @param start Start point.
 * @param end End point.
 * @param amount Amount of the interpolation.
 * @return float Result interpolation.
 */
float lerp(float start, float end, float amount) {
    return start + (end - start) * amount;
}