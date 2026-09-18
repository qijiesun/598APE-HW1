#include "disk.h"
Disk::Disk(const Vector &c, Texture* t, double ya, double pi, double ro, double tx, double ty):Plane(c, t, ya, pi, ro, tx, ty){}


double Disk::getIntersection(Ray ray){
   double time = Plane::getIntersection(ray);
   if(time==inf) 
      return time;
	Vector dist(0, 0, 0);
	if (optimizations.o4) {
		dist = localScalers(ray.point+ray.vector*time-center);
	} else {
		dist = solveScalers(right, up, vect, ray.point+ray.vector*time-center);
	}
   return (  dist.x*dist.x/(textureX*textureX)+dist.y*dist.y/(textureY*textureY)>1  )?inf:time;
}

bool Disk::getLightIntersection(Ray ray, double* fill){
   double t;
   double norm;
	if (optimizations.o5) {
		t = ray.vector.x*vect.x + ray.vector.y*vect.y + ray.vector.z*vect.z;
		norm = vect.x*ray.point.x + vect.y*ray.point.y + vect.z*ray.point.z + d;
	} else {
		t = ray.vector.dot(vect);
   	norm = vect.dot(ray.point)+d;
	}
   const double r = -norm/t;
   if(r<=0. || r>=1.) return false;
   Vector dist(0, 0, 0);
	if (optimizations.o4) {
		dist = localScalers(ray.point+ray.vector*r-center);
	} else {
		dist = solveScalers(right, up, vect, ray.point+ray.vector*r-center);
	}
   if(  dist.x*dist.x/(textureX*textureX)+dist.y*dist.y/(textureY*textureY)>1  )return false;
   if(texture->opacity>1-1E-6) return true;   
   unsigned char temp[4];
   double amb, op, ref;
   texture->getColor(temp, &amb, &op, &ref,fix(dist.x/textureX-.5), fix(dist.y/textureY-.5));
   if(op>1-1E-6) return true;
   fill[0]*=temp[0]/255.;
   fill[1]*=temp[1]/255.;
   fill[2]*=temp[2]/255.;
   return false;
}