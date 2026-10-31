#pragma once
#ifndef MATH_H
#define MATH_H

#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#define M_PI 3.14159265358979323846f

typedef struct {
    float x;
    float y;
} Vector2;

/**
 * Get the Distance between two points in 2D space.
 * 
 * @param a Vector2 point A.
 * @param b Vector2 point B.
 * @return float Distance between point A and point B.
 */
float GetDistance(Vector2 a, Vector2 b);

/**
 * Compute the dot product for two vectors.
 * 
 * @param a Vector A.
 * @param b Vector B.
 * @return float Dot product of two vectors.
 */
float Dot(Vector2 a, Vector2 b);

/**
 * Compute the reflect vector between velocity and normal.
 * 
 * @param velocity Velocity of the object
 * @param normal Normal of the surface when hit the object.
 * @return Vector2 Reflect vector.
 */
Vector2 reflect(Vector2 velocity, Vector2 normal);

/**
 * Normalize the vector
 * 
 * @param v Vector to normalize.
 * @return Vector2 Normalizez vector.
 */
Vector2 normalized(Vector2 v);

/**
 * Compute the length of a vector.
 * 
 */
float length(Vector2 v);

/**
 * Linear interpolation.
 * 
 * @param start Start point.
 * @param end End point.
 * @param amount Amount of the interpolation.
 * @return float Result interpolation.
 */
float lerp(float start, float end, float amount);

#endif