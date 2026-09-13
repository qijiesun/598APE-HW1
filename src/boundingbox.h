#ifndef __BOUNDINGBOX_H__
#define __BOUNDINGBOX_H__
#include "vector.h"

struct AABB {
    Vector lower;
    Vector upper;

    AABB() : lower(Vector(0, 0, 0)), upper(Vector(0, 0, 0)) {}
    AABB(Vector l, Vector u) : lower(l), upper(u) {}

    bool intersects(Ray ray) {
        double entry = -inf;
        double exit = inf;

        if (ray.vector.x != 0.0) {
            double time1 = (lower.x - ray.point.x) / ray.vector.x;
            double time2 = (upper.x - ray.point.x) / ray.vector.x;
            if (time1 > time2) {
                double temp = time1;
                time1 = time2;
                time2 = temp;
            }
            if (time1 > entry) {
                entry = time1;
            }
            if (time2 < exit) {
                exit = time2;
            }
        } else if (ray.point.x < lower.x || ray.point.x > upper.x) {
            return false;
        }

        if (ray.vector.y != 0.0) {
            double time1 = (lower.y - ray.point.y) / ray.vector.y;
            double time2 = (upper.y - ray.point.y) / ray.vector.y;
            if (time1 > time2) {
                double temp = time1;
                time1 = time2;
                time2 = temp;
            }
            if (time1 > entry) {
                entry = time1;
            }
            if (time2 < exit) {
                exit = time2;
            }
        } else if (ray.point.y < lower.y || ray.point.y > upper.y) {
            return false;
        }

        if (ray.vector.z != 0.0) {
            double time1 = (lower.z - ray.point.z) / ray.vector.z;
            double time2 = (upper.z - ray.point.z) / ray.vector.z;
            if (time1 > time2) {
                double temp = time1;
                time1 = time2;
                time2 = temp;
            }
            if (time1 > entry) {
                entry = time1;
            }
            if (time2 < exit) {
                exit = time2;
            }
        } else if (ray.point.z < lower.z || ray.point.z > upper.z) {
            return false;
        }
        
        return (entry <= exit) && (exit >= 0.0);
    }
};

#endif