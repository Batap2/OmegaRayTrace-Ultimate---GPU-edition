#ifndef Sphere_H
#define Sphere_H

#include "Object3D.h"
#include <cmath>

static glm::vec3 SphericalCoordinatesToEuclidean( glm::vec3 ThetaPhiR ) {
    return ThetaPhiR[2] * glm::vec3( cos(ThetaPhiR[0]) * cos(ThetaPhiR[1]) , sin(ThetaPhiR[0]) * cos(ThetaPhiR[1]) , sin(ThetaPhiR[1]) );
}
static glm::vec3 SphericalCoordinatesToEuclidean( float theta , float phi ) {
    return glm::vec3( cos(theta) * cos(phi) , sin(theta) * cos(phi) , sin(phi) );
}

static glm::vec3 EuclideanCoordinatesToSpherical( glm::vec3 xyz ) {
    float R = xyz.length();
    float phi = asin( xyz[2] / R );
    float theta = atan2( xyz[1] , xyz[0] );
    return glm::vec3( theta , phi , R );
}

class Sphere : public Object3D {
public:
    glm::vec3 m_center;
    float m_radius;

    __host__ __device__ Sphere () : Object3D() {}
    __host__ __device__ Sphere(glm::vec3 c , float r) : Object3D() , m_center(c) , m_radius(r) {}
    __host__ __device__ Sphere(const Sphere &other) : Object3D(), m_center(other.m_center), m_radius(other.m_radius) {material = Material(other.material);}
    __device__ virtual bool hit(const ray& r, float tmin, float tmax, HitData& rec) const;

};


__device__ bool Sphere::hit(const ray& r, float t_min, float t_max, HitData& rec) const {
    vec3 oc = r.origin() - m_center;
    float a = dot(r.direction(), r.direction());
    float b = dot(oc, r.direction());
    float c = dot(oc, oc) - m_radius * m_radius;
    float discriminant = b * b - a * c;

    if (discriminant > 0) {
        float temp = (-b - sqrt(discriminant)) / a;

        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.position = r.point_at_parameter(rec.t);
            rec.normal = (rec.position - m_center) / m_radius;
            rec.mat = material;
            return true;
        }

        temp = (-b + sqrt(discriminant)) / a;
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.position = r.point_at_parameter(rec.t);
            rec.normal = (rec.position - m_center) / m_radius;
            rec.mat = material;
            return true;
        }
    }

    return false;
};



#endif
