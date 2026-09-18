#include "mesh.h"
#include <algorithm>
#include <cmath>

Mesh::Bounds Mesh::emptyBounds(){
   Bounds b;
   b.minX = b.minY = b.minZ = inf;
   b.maxX = b.maxY = b.maxZ = -inf;
   return b;
}

void Mesh::expand(Bounds& dst, const Bounds& src){
   if(src.minX < dst.minX) dst.minX = src.minX;
   if(src.minY < dst.minY) dst.minY = src.minY;
   if(src.minZ < dst.minZ) dst.minZ = src.minZ;
   if(src.maxX > dst.maxX) dst.maxX = src.maxX;
   if(src.maxY > dst.maxY) dst.maxY = src.maxY;
   if(src.maxZ > dst.maxZ) dst.maxZ = src.maxZ;
}

void Mesh::expandPoint(Bounds& dst, const Vector& p){
   if(p.x < dst.minX) dst.minX = p.x;
   if(p.y < dst.minY) dst.minY = p.y;
   if(p.z < dst.minZ) dst.minZ = p.z;
   if(p.x > dst.maxX) dst.maxX = p.x;
   if(p.y > dst.maxY) dst.maxY = p.y;
   if(p.z > dst.maxZ) dst.maxZ = p.z;
}

thread_local Triangle* Mesh::lastHit = nullptr;
Mesh::Mesh(Vector* points, unsigned int* polys, int numPolygons, const Vector& offset, Texture* tex, Texture* normal)
   : Shape(offset, tex, 0., 0., 0.) {
   triangles.reserve(numPolygons);
   indices.reserve(numPolygons);
   for(int i = 0; i < numPolygons; i++){
      Vector a = points[polys[3*i]] + offset;
      Vector b = points[polys[3*i+1]] + offset;
      Vector c = points[polys[3*i+2]] + offset;
      Triangle* tri = new Triangle(a, b, c, tex);
      tri->normalMap = normal;

      TriRef ref;
      ref.tri = tri;
      ref.bounds = emptyBounds();
      expandPoint(ref.bounds, a);
      expandPoint(ref.bounds, b);
      expandPoint(ref.bounds, c);
      ref.cx = (a.x + b.x + c.x) / 3.;
      ref.cy = (a.y + b.y + c.y) / 3.;
      ref.cz = (a.z + b.z + c.z) / 3.;
      triangles.push_back(ref);
      indices.push_back(i);
   }
   if(numPolygons > 0) build(0, numPolygons);
}

int Mesh::build(int start, int count){
   Node node;
   node.bounds = emptyBounds();
   node.left = -1;
   node.right = -1;
   node.start = start;
   node.count = count;
   for(int i = start; i < start + count; i++) expand(node.bounds, triangles[indices[i]].bounds);

   int nodeIndex = (int)nodes.size();
   nodes.push_back(node);

   if(count <= 8) return nodeIndex;

   double minX = inf, minY = inf, minZ = inf;
   double maxX = -inf, maxY = -inf, maxZ = -inf;
   for(int i = start; i < start + count; i++){
      const TriRef& tri = triangles[indices[i]];
      if(tri.cx < minX) minX = tri.cx;
      if(tri.cy < minY) minY = tri.cy;
      if(tri.cz < minZ) minZ = tri.cz;
      if(tri.cx > maxX) maxX = tri.cx;
      if(tri.cy > maxY) maxY = tri.cy;
      if(tri.cz > maxZ) maxZ = tri.cz;
   }

   const double spanX = maxX - minX;
   const double spanY = maxY - minY;
   const double spanZ = maxZ - minZ;
   const int axis = (spanX >= spanY && spanX >= spanZ) ? 0 : ((spanY >= spanZ) ? 1 : 2);
   const int mid = start + count / 2;
   std::nth_element(indices.begin() + start, indices.begin() + mid, indices.begin() + start + count,
      [this, axis](int a, int b) {
         if(axis == 0) return triangles[a].cx < triangles[b].cx;
         if(axis == 1) return triangles[a].cy < triangles[b].cy;
         return triangles[a].cz < triangles[b].cz;
      });

   nodes[nodeIndex].left = build(start, mid - start);
   nodes[nodeIndex].right = build(mid, start + count - mid);
   nodes[nodeIndex].count = 0;
   return nodeIndex;
}

bool Mesh::intersectBounds(const Bounds& b, const Ray& ray, double maxT) const {
   double tmin = 0.;
   double tmax = maxT;
   const double origin[3] = {ray.point.x, ray.point.y, ray.point.z};
   const double dir[3] = {ray.vector.x, ray.vector.y, ray.vector.z};
   const double mn[3] = {b.minX, b.minY, b.minZ};
   const double mx[3] = {b.maxX, b.maxY, b.maxZ};
   for(int axis = 0; axis < 3; axis++){
      if(std::fabs(dir[axis]) < 1e-12){
         if(origin[axis] < mn[axis] || origin[axis] > mx[axis]) return false;
         continue;
      }
      double invD = 1. / dir[axis];
      double t0 = (mn[axis] - origin[axis]) * invD;
      double t1 = (mx[axis] - origin[axis]) * invD;
      if(t0 > t1) std::swap(t0, t1);
      if(t0 > tmin) tmin = t0;
      if(t1 < tmax) tmax = t1;
      if(tmax < tmin) return false;
   }
   return tmax > 0.;
}

double Mesh::getIntersection(Ray ray){
   if(nodes.empty()) return inf;
   double best = inf;
   Triangle* bestTri = NULL;
   int stack[128];
   int top = 0;
   stack[top++] = 0;
   while(top > 0){
      const Node& node = nodes[stack[--top]];
      if(!intersectBounds(node.bounds, ray, best)) continue;
      if(node.count > 0){
         for(int i = node.start; i < node.start + node.count; i++){
            Triangle* tri = triangles[indices[i]].tri;
            double t = tri->getIntersection(ray);
            if(t < best){
               best = t;
               bestTri = tri;
            }
         }
      } else {
         if(node.left >= 0) stack[top++] = node.left;
         if(node.right >= 0) stack[top++] = node.right;
      }
   }
   lastHit = bestTri;
   return best;
}

bool Mesh::getLightIntersection(Ray ray, double* fill){
   if(nodes.empty()) return false;
   int stack[128];
   int top = 0;
   stack[top++] = 0;
   while(top > 0){
      const Node& node = nodes[stack[--top]];
      if(!intersectBounds(node.bounds, ray, 1.)) continue;
      if(node.count > 0){
         for(int i = node.start; i < node.start + node.count; i++){
            if(triangles[indices[i]].tri->getLightIntersection(ray, fill)) return true;
         }
      } else {
         if(node.left >= 0) stack[top++] = node.left;
         if(node.right >= 0) stack[top++] = node.right;
      }
   }
   return false;
}

void Mesh::move(){}
unsigned char Mesh::reversible(){ return 1; }

void Mesh::getColor(unsigned char* toFill, double* am, double* op, double* ref, Autonoma* r, Ray ray, unsigned int depth){
   if(lastHit != NULL) lastHit->getColor(toFill, am, op, ref, r, ray, depth);
}

Vector Mesh::getNormal(Vector point){
   if(lastHit != NULL) return lastHit->getNormal(point);
   return Vector(0., 0., 1.);
}

void Mesh::setAngles(double a, double b, double c){ yaw = a; pitch = b; roll = c; }
void Mesh::setYaw(double d){ yaw = d; }
void Mesh::setPitch(double d){ pitch = d; }
void Mesh::setRoll(double d){ roll = d; }
