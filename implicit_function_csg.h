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
    implicit_function_csg(std::vector<Poisson_reconstruction_function>& fi){
        f=fi;
    }
    double rf_intersection(double f1, double f2) const{
        return f1 + f2 + sqrt(f1*f1 + f2*f2);
    }
    double rf_union(double f1,double f2) const{
        double alpha=0.1;
        return f1 + f2 - sqrt(f1*f1 + f2*f2);//+alpha/(1+(f1/alpha)*(f1/alpha)+(f2/alpha)*(f2/alpha));
    }
    

    double evalUnion(std::vector<Poisson_reconstruction_function> fi,double x,double y,double z)const{
        if (fi.size() == 1) {
            Poisson_reconstruction_function f = fi[0];
            return f(Point(x,y,z));
        }
        Poisson_reconstruction_function f1 = fi[0];
        Poisson_reconstruction_function f2 = fi[1];
        int c=0;
        double g = rf_union(f1(Point(x,y,z)), f2(Point(x,y,z)));
        for (size_t i = 2; i < fi.size(); ++i) {
            f1 = fi[i];
            g = rf_union(g, f1(Point(x,y,z)));
            
            if(g<-1.0){
                std::cout<<"func0="<<fi[0](Point(x,y,z))<<"func1="<<fi[1](Point(x,y,z))<<"func2="<<fi[2](Point(x,y,z))<<"func3="<<fi[3](Point(x,y,z))<<"func4="<<fi[4](Point(x,y,z))<<"func5="<<fi[5](Point(x,y,z))<<"func6="<<fi[6](Point(x,y,z))<<"func7="<<fi[7](Point(x,y,z))<<"func8="<<fi[8](Point(x,y,z))<<"func9="<<fi[9](Point(x,y,z))<<" g="<<g<<std::endl;
            }
            c++;
        }

        return g;
    }
    double operator()(Point p) const{
        return evalUnion(f,CGAL::to_double(p.x()),CGAL::to_double(p.y()),CGAL::to_double(p.z()));
    }
}; 

#endif