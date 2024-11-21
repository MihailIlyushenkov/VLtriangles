#include <limits>
#include <cmath>
#include <iostream>
#include <utility>
#include <vector>
#include <list>

#pragma once

enum TRSTATE{
    BAD = -1,
    NORM = 0,
    LINE = 1,
    POINT = 2 
};

inline int equal(double a, double b){
    // return (std::fabs(a-b) < std::numeric_limits<double>::epsilon())?1:0;
    return (std::fabs(a-b) < 1e-15)?1:0;
}

int sign(double a);

int my_iszero(double x);

class vect{
    public:
    vect(double a = 0, double b = 0, double c = 0): x(a), y(b), z(c) {};

    void operator*=(double a);
    void operator*=(int a);

    double size();
    double sqsize() const;
    int incube(double side) const {
        return (sign(side - fabs(x)) == 1) && (sign(side - fabs(y)) == 1) && (sign(side - fabs(z)) == 1);
    }

    void norm(){
        if(!my_iszero(x)){
            y /= x;
            z /= x;
            x = 1;
        }
        if(!my_iszero(y)){
            x /= y;
            z /= y;
            y = 1;
        }
        if(!my_iszero(z)){
            x /= z;
            y /= z;
            z = 1;
        }
    }

    double x;
    double y;
    double z;
};

vect operator+(const vect& v1, const vect& v2);
vect operator-(const vect& v1, const vect& v2);
double operator*(const vect& v1, const vect& v2);
vect operator&(const vect& v1, const vect& v2);
int operator==(const vect& v1, const vect& v2);
int operator!=(const vect& v1, const vect& v2);
double operator/(const vect& a, const vect& b);
int operator||(const vect& v1, const vect& v2);

vect operator*(const vect& v1, int coeff);
vect operator*(const vect& v1, double coeff);
vect operator*(double coeff, const vect& v1);
vect operator*(int coeff, const vect& v1);

const vect NULL_VECT(0, 0, 0);
const vect INVL_VECT(MAXFLOAT, MAXFLOAT, MAXFLOAT);

class plane{
    public:

    //rv is for radius vector
    //nv is for normal vector
    plane(vect rv, vect nv): r(rv), n(nv), D(-(rv.x*nv.x + rv.y*nv.y + rv.z*nv.z)) {}; 
    plane(): D(0) {};

    //generates plane: 
    //vects r1, r2, r3 are dots on plane
    plane(vect r1, vect r2, vect r3): r(r1), n((r3-r1)&(r2-r1)), D(-(r.x*n.x + r.y*n.y + r.z*n.z)) {};
    int isvalid();

    vect r;
    vect n;
    double D; //from Ax+By+Cz+D = 0
};

int operator==(const plane& p1, const plane&p2);


class line{
    public:

    line(vect rv = NULL_VECT, vect av = NULL_VECT): r(rv), a(av) {};
    vect r;
    vect a;

};


class triangle{
    public:
    triangle(vect rv1, vect rv2, vect rv3): 
            vertexes {rv1, rv2, rv3}, 
            sides {rv2-rv1, rv3-rv2, rv1-rv3},
            p(rv1, rv2, rv3),
            l(vect(0,0,0), vect(0,0,0)) {};

    triangle() {};

    void process();
    void set_intersected(){
        intersected = 1;
        return;
    }
    //radius-vectors to vertexes
    vect vertexes[3];

    //side vectors
    vect sides[3];

    //plane of the triangle (makes sense only if state == NORM)
    plane p;

    //line of the triangle (makes sense only if state == LINE)
    line l;

    size_t index = 0;
    int intersected = 0;
    TRSTATE state = BAD;
};

line operator&&(const plane& p1, const plane& p2);


int intersect_point_triangle(const triangle& Tr, const vect& v);
int intersect_triangles(const triangle& Tr1, const triangle& Tr2);
std::pair<double, double> intersect_line_line_coeff(const line& L1, const line& L2);
vect intersect_line_plane(const line& L, const plane& P);
int intersect_line_triangle(const triangle& Tr, const line& L);
int intersect_triangle_plane(const plane& P, const triangle& Tr);

std::ostream& operator<<(std::ostream& stream, const vect& v);
std::ostream& operator<<(std::ostream& stream, const plane& p);
std::ostream& operator<<(std::ostream& stream, const line& l);
std::ostream& operator<<(std::ostream& stream, const triangle& Tr);

std::ostream& operator<<(std::ostream& stream, const std::list<triangle*>& TrList);



class octtree{
    public:
        octtree(vect center, double lenght): _center(center), _lenght(lenght) {
            // std::cout << "created tree with c: " << center << " l: " << lenght << '\n';
        };

        vect _center{0,0,0}; //центр куба подпространства
        double _lenght = 0; //длина ребра куба подпространства

        //(z > 0): 
        //  x > 0, y > 0 -> 0
        //  x > 0, y < 0 -> 1
        //  x < 0, y < 0 -> 2
        //  x < 0, y > 0 -> 3
        //(z < 0): 
        //  x > 0, y > 0 -> 4
        //  x > 0, y < 0 -> 5
        //  x < 0, y < 0 -> 6
        //  x < 0, y > 0 -> 7
        octtree* subspace[8]{nullptr}; //fully insude bounds
        
        std::list<triangle*> interior; //all elements inside node (including subnodes)
        std::list<triangle*> bound_intersections; //elements, inside node, but not in any subnode 
        

        int insert_in_subspace(triangle* tr, int index){
            if(0 <= index && index < 8){
                if(subspace[index] == nullptr) {
                    vect shift{0, 0, 0};
                    switch (index)
                    {
                    case 0: shift = vect{1, 1, 1};
                        break;
                    case 1: shift = vect{1, -1, 1};
                        break;
                    case 2: shift = vect{-1, -1, 1};
                        break;
                    case 3: shift = vect{-1, 1, 1};
                        break;
                    case 4: shift = vect{1, 1, -1};
                        break;
                    case 5: shift = vect{1, -1, -1};
                        break;
                    case 6: shift = vect{-1, -1, -1};
                        break;
                    case 7: shift = vect{-1, 1, -1};
                        break;
                    default:
                        break;
                    }

                    vect new_node_center = _center + shift*(_lenght/2);

                    if( (!(tr->vertexes[0]-new_node_center).incube(_lenght/2))|| 
                        (!(tr->vertexes[1]-new_node_center).incube(_lenght/2))||
                        (!(tr->vertexes[2]-new_node_center).incube(_lenght/2))) {  
                        //triangle has points outside new node's subspace interior  
                        std::cout << "smth bad in " << _center << " with\n" << *tr << '\n';
                        std::cout << new_node_center << '\n';
                        std::cout << tr->vertexes[0]-new_node_center << ' ' << tr->vertexes[0]-new_node_center << ' ' << tr->vertexes[0]-new_node_center << ' ' << _lenght/2 << "\n\n";
                        return 0;
                    }

                    subspace[index] = new octtree{new_node_center, _lenght/2};
                    subspace[index]->interior.push_back(tr);
                }
                else {
                    if( (!(tr->vertexes[0]-subspace[index]->_center).incube(_lenght/2))|| 
                        (!(tr->vertexes[1]-subspace[index]->_center).incube(_lenght/2))||
                        (!(tr->vertexes[2]-subspace[index]->_center).incube(_lenght/2))) {  
                        //triangle has points outside node's subspace interior  
                        std::cout << "bad";
                        return 0;
                    }
                    subspace[index]->interior.push_back(tr);
                }
                return 1;
            }
            return -1;
        }

        void dump_octtree(){
            std::list<octtree*> printqueue;
            printqueue.push_back(this);

            while(printqueue.begin() != printqueue.end()){
                octtree* elem = *(printqueue.begin());
                printqueue.pop_front();
            
                std::cout << "center " << elem->_center << " lenght " << elem->_lenght << '\n';
                std::cout << "subtrees:\n";
                for(int i = 0; i < 8; i++){
                    if(elem->subspace[i] == nullptr) continue;
                    std::cout << "  " << i << ": " << elem->subspace[i]->_center << ", " << elem->subspace[i]->_lenght << '\n';
                }
                std::cout << "++interior++ (" << elem->interior.size() << ")\n" << elem->interior;
                std::cout << "++bound intersections++\n" << elem->bound_intersections;
                std::cout << '\n';
                for(int i = 0; i < 8; i++){
                    if(elem->subspace[i] == nullptr) continue;
                    printqueue.push_back(elem->subspace[i]);
                }
            }   
        }

        int count_internal_intersections(){
            std::list<octtree*> queue;
            int count = 0;
            queue.push_back(this);

            octtree* elem = nullptr;
            while(queue.begin() != queue.end()){
                elem = queue.front();

                for(auto i = elem->bound_intersections.begin(); i != elem->bound_intersections.end(); i++){
                    auto j = i;
                    j++;
                    while(j != elem->bound_intersections.end()){                        
                        if (intersect_triangles(**i, **j)) {
                            count++;
                            (*i)->intersected = 1;
                            (*j)->intersected = 1;
                        }
                        j++;
                    }
                }

                for(auto& tr: elem->bound_intersections){
                    for(int i = 0; i < 8; i++){
                        if (elem->subspace[i] == nullptr) continue;
                        for(auto &subspace_tr: elem->subspace[i]->interior){
                            if(intersect_triangles(*tr, *subspace_tr)){
                                count++;
                                tr->intersected = 1;
                                subspace_tr->intersected = 1;
                            }
                        }
                    }
                }

                for(int i = 0; i < 8; i++){
                    if(elem->subspace[i] == nullptr) continue;
                    queue.push_back(elem->subspace[i]);
                }
                queue.pop_front();
            }
            return count;
        }

        int process_tree(double min_size);

        ~octtree(){
            for(int i = 0; i < 8; i++){
                delete subspace[i];
            }
        }
};

void process_node(octtree* node, std::list<octtree*>* octoqueue, double min_lenght);
    
