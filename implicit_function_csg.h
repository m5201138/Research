#ifndef IMPLICITFUNCTIONCSG
#define IMPLICITFUNCTIONCSG
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Poisson_reconstruction_function.h>
#include <vector>
typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef Kernel::Point_3 Point;
typedef CGAL::Poisson_reconstruction_function<Kernel> Poisson_reconstruction_function;

class implicit_function_csg{
public:
    std::vector<Poisson_reconstruction_function> f;
    implicit_function_csg(std::vector<Poisson_reconstruction_function> fi){
        f=fi;
    }
    double rf_union(double f1, double f2) {
        return f1 + f2 + sqrt(f1*f1 + f2*f2);
    }
    double rf_intersection(double f1,double f2){
        return f1 + f2 - sqrt(f1*f1 + f2*f2);
    }
    

    double evalUnion(std::vector<Poisson_reconstruction_function> fi,double x,double y,double z){
        if (fi.size() == 1) {
            Poisson_reconstruction_function f = fi[0];
            return f(Point(x,y,z));
        }
        Poisson_reconstruction_function f1 = fi[0];
        Poisson_reconstruction_function f2 = fi[1];
        double g = rf_intersection(f1(Point(x,y,z)), f2(Point(x,y,z)));
        for (size_t i = 2; i < fi.size(); ++i) {
            f1 = fi[i];
            g = rf_intersection(g, f1(Point(x,y,z)));
        }
        return g;
    }
    double operator()(Point p){
        return evalUnion(f,CGAL::to_double(p.x()),CGAL::to_double(p.y()),CGAL::to_double(p.z()));
    }
}; 

#endif