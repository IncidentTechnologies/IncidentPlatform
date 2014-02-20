// DimensionPrimitives.h is just a conglomerate combination of
// all of the Dimension Primitives into a convenient single 
// library.  

#pragma once

#include "point.h"
#include "point2.h"
#include "vector.h"
#include "color.h"      // This form of color is the theoretical type
#include "colord.h"     // Implemented as a double word color
#include "vertex.h"

// DimObjects
// *************************
#include "DimObject.h"
// *************************
#include "texture.h"
// *************************
#include "light.h"
#include "triangle.h"
#include "plane.h"
#include "sphere.h"
#include "volume.h"
#include "camera.h"
#include "player.h"
#include "Text3D.h"
#include "rectangle.h"
#include "MeshMd3.h"
// *************************

// FlatObjects
// *************************
#include "FlatObject.h"
// *************************
#include "blit.h"
#include "text.h"
#include "line.h"
// *************************

#include "hterrain.h"

#include "MeshLoader.h"

// Matrix Etc
#include "matrix.h"
#include "TransformationMatrix.h"

// Not really a primitive 
#include "FPS.h"

