#pragma once

//#include "DimensionPrimitives.h"
#include "point.h"
#include "vector.h"
#include "plane.h"

// Nuke functions
// ***************************************
/*
// Volume
bool Nuke(point P, volume V);       // Point, Volume
bool Nuke(sphere S, volume V);      // Sphere, Volume
bool Nuke(volume A, volume B);      // Volume, Volume
bool Nuke(polygon P, volume V);     // Polygon, Volume
bool Nuke(mesh M, volume V);        // Mesh, Volume

// Sphere
bool Nuke(point P, sphere S);       // Point, Sphere
bool Nuke(sphere A, sphere B);      // Sphere, Sphere
bool Nuke(polygon P, sphere S);     // Polygon, Sphere
bool Nuke(mesh M, sphere S);        // Mesh, Sphere

// Cylinder ?

// Triangle ?

// Polygon ?

// Mesh ?

*/


// Minimum Distance Functions
// Two Points
float MinDistancePointPoint(point a, point b);

// Line and Point
float MinDistancePointLine(point a1, point a2, point P);

// Plane and Point
// Points a1, a2, a3 define the plane and point P is the point
float DistancePointPlane(point a1, point a2, point a3, point P);

// For use with our plane class
float DistancePointPlane(point, plane);

float VectorPlaneIntersect(point a1, point a2, point a3, point P2, point P1);
float VectorPlaneIntersect(point a1, point a2, point a3, point P, vector v);

float LineSphereIntersect(point a1, point a2, point P, float radius);
