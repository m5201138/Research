#ifndef IMPLICITFUNCTIONMIN
#define IMPLICITFUNCTIONMIN
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Poisson_reconstruction_function.h>
#include <vector>
#include <cmath>
#include <algorithm>

typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef Kernel::Point_3 Point;
typedef CGAL::Poisson_reconstruction_function<Kernel> Poisson_reconstruction_function;

class implicit_function_min{
public:
    std::vector<Poisson_reconstruction_function> f;
    implicit_function_min(std::vector<Poisson_reconstruction_function> fi){
        f=fi;
    }

    double blendUnion(double f,double g) const{
        double d;
        double r=0.01;
        //f*=10;
        //g*=10;
        if ((f+r)>g && (f-r)<g) {
            double dt = 4.0*(f+g)*(f+g) - 8.0*(f*f+g*g-r*r);
            double x1 = ((2.0*f+2.0*g) + std::sqrt(dt))/4.0;
            d = x1 - r;
        } else {
            d = std::min(f, g);
        }
        
        return d;
    }
    
    
    double evalUnion(std::vector<Poisson_reconstruction_function> fi,double x,double y,double z)const{
        if (fi.size() == 1) {
            Poisson_reconstruction_function f = fi[0];
            return f(Point(x,y,z));
        }
        Poisson_reconstruction_function f1 = fi[0];
        Poisson_reconstruction_function f2 = fi[1];
        double g = blendUnion(f1(Point(x,y,z)), f2(Point(x,y,z)));
        for (size_t i = 2; i < fi.size(); ++i) {
            f1 = fi[i];
            g = blendUnion(g, f1(Point(x,y,z)));
        }
        
        return g;
    }
    double operator()(Point p) const{
        return evalUnion(f,CGAL::to_double(p.x()),CGAL::to_double(p.y()),CGAL::to_double(p.z()));
    }
};

#endif