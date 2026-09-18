#ifndef __MESH_H__
#define __MESH_H__

#include "shape.h"
#include "triangle.h"
#include <vector>

class Mesh : public Shape {
public:
   Mesh(Vector* points, unsigned int* polys, int numPolygons, const Vector& offset, Texture* texture, Texture* normalMap);
   double getIntersection(Ray ray);
   bool getLightIntersection(Ray ray, double* fill);
   void move();
   unsigned char reversible();
   void getColor(unsigned char* toFill, double* am, double* op, double* ref, Autonoma* r, Ray ray, unsigned int depth);
   Vector getNormal(Vector point);
   void setAngles(double yaw, double pitch, double roll);
   void setYaw(double d);
   void setPitch(double d);
   void setRoll(double d);

private:
   struct Bounds {
      double minX, minY, minZ;
      double maxX, maxY, maxZ;
   };

   struct TriRef {
      Triangle* tri;
      Bounds bounds;
      double cx, cy, cz;
   };

   struct Node {
      Bounds bounds;
      int left;
      int right;
      int start;
      int count;
   };

   std::vector<TriRef> triangles;
   std::vector<int> indices;
   std::vector<Node> nodes;
   static thread_local Triangle* lastHit;

   int build(int start, int count);
   bool intersectBounds(const Bounds& b, const Ray& ray, double maxT) const;
   static Bounds emptyBounds();
   static void expand(Bounds& dst, const Bounds& src);
   static void expandPoint(Bounds& dst, const Vector& p);
};

#endif
