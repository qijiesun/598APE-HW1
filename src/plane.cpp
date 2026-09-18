#include "plane.h"

Plane::Plane(const Vector &c, Texture* t, double ya, double pi, double ro, double tx, double ty) : Shape(c, t, ya, pi, ro), vect(c), right(c), up(c){
   textureX = tx; textureY = ty;
   setAngles(yaw, pitch, roll);
   normalMap = NULL;
   mapX = textureX; mapY = textureY;
}

void Plane::setAngles(double a, double b, double c){
   yaw =a; pitch = b; roll = c;
   xcos = cos(yaw);
   xsin = sin(yaw);
   ycos = cos(pitch);
   ysin = sin(pitch);
   zcos = cos(roll);
   zsin = sin(roll);
   vect.x = xsin*ycos*zcos+ysin*zsin;
   vect.y = ysin*zcos-xsin*ycos*zsin;
   vect.z = xcos*ycos;
   up.x = -xsin*ysin*zcos+ycos*zsin;
   up.y = ycos*zcos+xsin*ysin*zsin;
   up.z = -xcos*ysin;
   right.x = xcos*zcos;
   right.y = -xcos*zsin;
   right.z = -xsin;
   d = -vect.dot(center);
}

void Plane::setYaw(double a){
   yaw =a;
   xcos = cos(yaw);
   xsin = sin(yaw);
   
   vect.x = xsin*ycos*zcos+ysin*zsin;
   vect.y = ysin*zcos-xsin*ycos*zsin;
   vect.z = xcos*ycos;
   up.x = -xsin*ysin*zcos+ycos*zsin;
   up.y = ycos*zcos+xsin*ysin*zsin;
   up.z = -xcos*ysin;   
   right.x = xcos*zcos;
   right.y = -xcos*zsin;
   right.z = -xsin;
   d = -vect.dot(center);
}

void Plane::setPitch(double b){
   pitch = b;
   ycos = cos(pitch);
   ysin = sin(pitch);
   vect.x = xsin*ycos*zcos+ysin*zsin;
   vect.y = ysin*zcos-xsin*ycos*zsin;
   vect.z = xcos*ycos;
   up.x = -xsin*ysin*zcos+ycos*zsin;
   up.y = ycos*zcos+xsin*ysin*zsin;
   up.z = -xcos*ysin;
   d = -vect.dot(center);
}

void Plane::setRoll(double c){
   roll = c;
   zcos = cos(roll);
   zsin = sin(roll);
   vect.x = xsin*ycos*zcos+ysin*zsin;
   vect.y = ysin*zcos-xsin*ycos*zsin;
//   vect.z = xcos*ycos;
   up.x = -xsin*ysin*zcos+ycos*zsin;
   up.y = ycos*zcos+xsin*ysin*zsin;
  // up.z = -xcos*ysin;
   right.x = xcos*zcos;
   right.y = -xcos*zsin;
   //right.z = -xsin;
   d = -vect.dot(center);
}

double Plane::getIntersection(Ray ray){
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
   return (r>0)?r:inf;
}

Vector Plane::localScalers(const Vector& offset) {
	return Vector(
      offset.x*right.x + offset.y*right.y + offset.z*right.z,
      offset.x*up.x + offset.y*up.y + offset.z*up.z,
      offset.x*vect.x + offset.y*vect.y + offset.z*vect.z
	);
}

bool Plane::getLightIntersection(Ray ray, double* fill){
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

   if(texture->opacity>1-1E-6) return true;
	Vector dist(0, 0, 0);
	if (optimizations.o4) {
		dist = localScalers(ray.point-center);
	} else {
		dist = solveScalers(right, up, vect, ray.point-center);
	}
   unsigned char temp[4];
   double amb, op, ref;
   texture->getColor(temp, &amb, &op, &ref,fix(dist.x/textureX-.5), fix(dist.y/textureY-.5));
   if(op>1-1E-6) return true;
   fill[0]*=temp[0]/255.;
   fill[1]*=temp[1]/255.;
   fill[2]*=temp[2]/255.;
   return false;
}

void Plane::move(){
   d = -vect.dot(center);
}
void Plane::getColor(unsigned char* toFill,double* am, double* op, double* ref, Autonoma* r, Ray ray, unsigned int depth){
   Vector dist(0, 0, 0);
	if (optimizations.o4) {
		dist = localScalers(ray.point-center);
	} else {
		dist = solveScalers(right, up, vect, ray.point-center);
	}
   texture->getColor(toFill, am, op, ref, fix(dist.x/textureX-.5), fix(dist.y/textureY-.5));
}
unsigned char Plane::reversible(){ 
   return 1; }

Vector Plane::getNormal(Vector point){
   if(normalMap==NULL)
      return vect;
   else{
      Vector dist(0, 0, 0);
		if (optimizations.o4) {
			dist = localScalers(point-center);
		} else {
			dist = solveScalers(right, up, vect, point-center);
		}
      double am, ref, op;
      unsigned char norm[3];
      normalMap->getColor(norm, &am, &op, &ref, fix(dist.x/mapX-.5+mapOffX), fix(dist.y/mapY-.5+mapOffY));
      Vector ret = ((norm[0]-128)*right+(norm[1]-128)*up+norm[2]*vect).normalize();
      return ret;
   }
}