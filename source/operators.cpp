#include "geom.hpp"

std::ostream& operator<<(std::ostream& stream, const vect& v)
{
    stream << '(' << v.x << ',' << v.y << ',' << v.z << ')';
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const plane& p)
{
    stream << '[' << p.r << ", " << p.n << "], D: " << p.D;
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const line& l)
{
    stream << '[' << l.r << ", " << l.a << ']';
    return stream;
}

const char* state_to_char(TRSTATE a){
    switch (a)
    {
    case NORM: return "NORM";
    case LINE: return "LINE";
    case POINT: return "POINT";
    case BAD: return "BAD";
    default:
        return 0;
    }
}

std::ostream& operator<<(std::ostream& stream, const triangle& Tr)
{
    stream  << state_to_char(Tr.state) << '\n'
            << "r: {" << Tr.vertexes[0] << ", " << Tr.vertexes[1] << ", " << Tr.vertexes[2] << "}\n";
            // << "s: {" << Tr.sides[0] << ", " << Tr.sides[1] << ", " << Tr.sides[2] << "}\n"
            // << "p: " << Tr.p << '\n'
            // << "l: " << Tr.l;
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const std::list<triangle*>& TrList)
{
    for(auto &x: TrList){
        stream << x->index << ' ';
    }
    stream << '\n';
    return stream;
}


vect operator+(const vect& v1, const vect& v2){
    vect res;
    res.x = v1.x + v2.x;
    res.y = v1.y + v2.y;
    res.z = v1.z + v2.z;
    return res;
}

vect operator-(const vect& v1, const vect& v2){
    vect res;
    res.x = v1.x - v2.x;
    res.y = v1.y - v2.y;
    res.z = v1.z - v2.z;
    return res;
}

double operator*(const vect& v1, const vect& v2){
    return (v1.x*v2.x) + (v1.y*v2.y) + (v1.z*v2.z);
}   

void vect::operator*=(int coeff){
    this->x *= coeff;
    this->y *= coeff;
    this->z *= coeff;
}

void vect::operator*=(double coeff){
    this->x *= coeff;
    this->y *= coeff;
    this->z *= coeff;
}

vect operator&(const vect& v1, const vect& v2){
    vect res;
    res.x = v1.y*v2.z - v1.z*v2.y;
    res.y = -v1.x*v2.z + v1.z*v2.x;
    res.z = v1.x*v2.y - v1.y*v2.x;
    return res;
}

int operator==(const vect& v1, const vect& v2) {
    return (equal(v1.x, v2.x) && equal(v1.y, v2.y) && equal(v1.z, v2.z))?1:0; 
}

int operator!=(const vect& v1, const vect& v2) {
    return (equal(v1.x, v2.x) && equal(v1.y, v2.y) && equal(v1.z, v2.z))?0:1;
}

//x/y
double operator/(const vect& a, const vect& b) {
    return (b.x != 0)?(a.x/b.x):((b.y != 0)?(a.y/b.y):(((b.z != 0)?(a.z/b.z):(__DBL_MAX__)))); 
}

int operator||(const vect& v1, const vect& v2) {
    return ((v1&v2) == NULL_VECT); 
}

int operator==(const plane& p1, const plane&p2){
    return (p1.n||p2.n)?((my_iszero((p1.r-p2.r)*p1.n))?(1):(0)):(0);
}



int plane::isvalid(){
    return (n == NULL_VECT)?0:1;
}

double vect::size(){
    return sqrt(x*x + y*y + z*z);
}

double vect::sqsize() const {
    return (x*x + y*y + z*z);
}

vect operator*(const vect& v1, int coeff){
    vect res = v1;
    res *= coeff;
    return res;
}

vect operator*(const vect& v1, double coeff){
    vect res = v1;
    res *= coeff;
    return res;
}

vect operator*(double coeff, const vect& v1){
    vect res = v1;
    res *= coeff;
    return res;
}

vect operator*(int coeff, const vect& v1){
    vect res = v1;
    res *= coeff;
    return res;
}

line operator&&(const plane& p1, const plane& p2){
    vect a = p1.n&p2.n;
    a.norm();
    // std::cout << "a = " << a << '\n';
    vect r;
    if (a == NULL_VECT){ //|| planes
        if(my_iszero(p1.D-p2.D)){ //same plane
            r = p1.r;
        }
        else {
            r = INVL_VECT; //not same plane
        }
    }
    else{ //planes intersect by line
        double A1 = p1.n.x;
        double B1 = p1.n.y;
        double C1 = p1.n.z;
        double D1 = -p1.D;

        double A2 = p2.n.x;
        double B2 = p2.n.y;
        double C2 = p2.n.z;
        double D2 = -p2.D;

        // std::cout << "A1 = " << A1 << '\n'
        // << "B1 = " << B1 << '\n'
        // << "C1 = " << C1 << '\n'
        // << "D1 = " << D1 << '\n'
        // << "A2 = " << A2 << '\n'
        // << "B2 = " << B2 << '\n'
        // << "C2 = " << C2 << '\n'
        // << "D2 = " << D2 << '\n';
        //remove these variables later;

        if(!iszero(a.x)){
            // std::cout << "supposing x = 0\n";
            //suppose x = 0
            //Ax + By + Cz = D -> need to solve By + Cz = D
            double det = B1*C2 - C1*B2;
            double dety = D1*C2 - C1*D2;
            double detz = B1*D2 - D1*B2;

            if(iszero(det)) {
                std::cout << "smth wrong...";
                r = INVL_VECT;
                return line{r, a};
            }
            
            return line(vect{0, dety/det, detz/det}, a);
        }
        else if(!iszero(a.y)){
            //suppose y = 0; -> need to solve Ax + Cz = D
            // std::cout << "supposing y = 0\n";   
            double det = A1*C2 - A2*C1;
            double detx = D1*C2 - D2*C1;
            double detz = A1*D2 - A2*D1;

            if(iszero(det)) {
                std::cout << "smth wrong...";
                r = INVL_VECT;
                return line{r, a};
            }
            
            return line(vect{detx/det, 0, detz/det}, a);
        }
        else if(!iszero(a.z)){
            //suppose z = 0; -> need to solve Ax + By = D
            // std::cout << "supposing z = 0\n";

            double det = A1*B2 - A2*B1;
            double detx = D1*B2 - D2*B1;
            double dety = A1*D2 - A2*D1;

            if(iszero(det)) {
                std::cout << "smth wrong...";
                r = INVL_VECT;
                return line{r, a};
            }
            
            return line(vect{detx/det, dety/det, 0}, a);
        }
        else {
            std::cout << "Thms very bad. How tf is vect a == 0??\n";
        }
    }
    return line{r, a};
}
