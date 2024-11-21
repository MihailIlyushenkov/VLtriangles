#include "geom.hpp"

#include <iostream>

int sign(double a){
    return (a > 1e-9)?(1):((a < -1e-9)?(-1):(0));
}

int my_iszero(double a){
    return ((a < 1e-9)&&(a > -1e-9))?(1):0;
}

int inbounds(double x, double low, double up){
    return my_iszero(x-low) || ((low < x)&&(x < up)) || my_iszero(up-x);
}

void triangle::process(){
    if (p.isvalid()){
        state = NORM;
    }
    else{
        if((vertexes[0] == vertexes[1]) && (vertexes[0] == vertexes[2])){
            state = POINT;
        }
        else{
            l.r = vertexes[0];
            l.a = (vertexes[0] == vertexes[1])?(vertexes[0]-vertexes[2]):(vertexes[0]-vertexes[1]);
            state = LINE;
        }
    }
}

int intersect_triangle_triangle(const triangle& Tr1, const triangle& Tr2){
    if(Tr1.p.n||Tr2.p.n){ //planes ||
        // std::cout << "triangles in parallell (maybe same) planes\n";
        if(Tr1.p == Tr2.p){ //same plane
            // std::cout << "triangles in same plane\n";
            int v1 = 0;
            int v2 = 0;
            for(int i = 0; i < 3; i++) {
                v1 = intersect_point_triangle(Tr1, Tr2.vertexes[i]);
                v2 = intersect_point_triangle(Tr2, Tr1.vertexes[i]);
                // std::cout << "v1: " << v1 << " v2: " << v2 << '\n';
                // std::cout << '\n';
                if(v1 || v2) {
                    // std::cout << "points inside triangles\n";
                   return 1;
                }
            }

            // std::cout << "no points inside triangles\n";

            std::pair<double, double> res{0,0};
            for(int i = 0; i < 3; i++) {
                for(int j = 0; j < 3; j++){
                    res = intersect_line_line_coeff({Tr1.vertexes[i], Tr1.sides[i]}, {Tr2.vertexes[j], Tr2.sides[j]});
                    // std::cout << line{Tr1.vertexes[i], Tr1.sides[i]} << '\n';
                    // std::cout << line{Tr2.vertexes[j], Tr2.sides[j]} << '\n';
                    // std::cout << "coefs: " << res.first << ' ' << res.second << '\n';
                    // std::cout << "bounds: " << inbounds(res.first, 0, 1) << ' ' << inbounds(res.second, 0, 1) << '\n';
                    if(inbounds(res.first, 0, 1) && inbounds(res.second, 0, 1)){
                        return 1;
                    }
                }
            }
            return 0;
        }
        else{ //diff planes
            // std::cout << "triangles in diff planes";
            return 0;
        }
    }
    else { //planes intersect
        // std::cout << "triangles in intersected planes\n";
        line intersection_line = Tr1.p&&Tr2.p;
        // std::cout << "intline: " << intersection_line << '\n';

        std::pair<double, double> res;
        vect intersection_points[3]{};
        int intersections_found = 0;
        for(int i = 0; i < 3; i++){
            res = intersect_line_line_coeff(intersection_line, {Tr1.vertexes[i], Tr1.sides[i]});
            // std::cout << res.first << " " << res.second << " from " << intersection_line << "\nand\n" << line{Tr1.vertexes[i], Tr1.sides[i]} << '\n';
            if(inbounds(res.second, 0, 1)){
                // std::cout << "this passes" << '\n';
                intersection_points[intersections_found] = Tr1.vertexes[i] + res.second*Tr1.sides[i];
                intersections_found++;
            }
        }

        vect r;
        vect a;

        // std::cout << intersections_found << " intersections" << '\n';
        // for(int i = 0; i < intersections_found; i++){
        //     std::cout << "intpoint: " << intersection_points[i] << '\n';
        // }


        switch (intersections_found)
        {
        case 0: return 0;
        case 1: std::cout << "found 1 intersection. Thats impossible." << '\n'; return -1;
        case 2: r = intersection_points[0]; a =  intersection_points[1] - intersection_points[0]; break;
        case 3: 
            if (intersection_points[0] != intersection_points[1]) {
                r = intersection_points[0]; 
                a =  intersection_points[1] - intersection_points[0]; 
                break;
            }
            else if (intersection_points[1] != intersection_points[2]) {
                r = intersection_points[1]; 
                a =  intersection_points[2] - intersection_points[1]; 
                break;
            }
            else if (intersection_points[0] != intersection_points[1]) {
                r = intersection_points[0]; 
                a =  intersection_points[1] - intersection_points[0]; 
                break;
            }
            else {
                std::cout << "something very wrong, cause all intersections are same\n";
                return -1;
            }

        default:std::cout << "WUT?? found " << intersections_found << " intersections, when 3 is max.\n"; return -1; 
            break;
        }

        line segment{r, a};
        // std::cout << "segment " << segment << '\n';
        // std::cout << Tr2 << '\n';

        if(intersect_point_triangle(Tr2, segment.r) && intersect_point_triangle(Tr2, segment.a + segment.r)) {
            return 1;
        }

        for(int i = 0; i < 3; i++){
            res = intersect_line_line_coeff({Tr2.vertexes[i], Tr2.sides[i]}, segment);
            // std::cout << "res1: " << res.first << " res2: " << res.second << '\n';
            if (inbounds(res.first, 0, 1) && (inbounds(res.second, 0, 1) || segment.a == NULL_VECT)){
                return 1;
            }
        }
    }
    return 0;
}

int intersect_point_triangle(const triangle& Tr, const vect& v){
    // std::cout << "INTERSECTING POINT AND TRIANGLE:\n";
    // std::cout << "point: " << v << '\n';
    // std::cout << Tr << '\n';

    if(my_iszero((v - Tr.p.r)*Tr.p.n) == 0) {
        // std::cout << "vect not in plane: " << (v - Tr.p.r)*Tr.p.n << '\n';
        // std::cout << "res = 0\n";
        return 0; //point not in triangle's plane
    }

    if((v == Tr.vertexes[0]) || (v == Tr.vertexes[1]) || (v == Tr.vertexes[2])) {
        // std::cout << "point == tr.vertex\n";
        // std::cout << "res = 1\n";
        return 1;
    }
    vect surf_area1(0, 0, 0);
    vect dirs[3];

    for(int i = 0; i < 3; i++){
        dirs[i] = (Tr.sides[i]&(v - Tr.vertexes[i]));
        // std::cout << "dir" << i << ": " << dirs[i] << '\n';
    }

    // std::cout << dirs[0]/Tr.p.n << ' '<< dirs[1]/Tr.p.n << ' ' << dirs[2]/Tr.p.n << '\n';
    // std::cout << sign(dirs[0]/Tr.p.n) << ' '<< sign(dirs[1]/Tr.p.n) << ' ' << sign(dirs[2]/Tr.p.n) << '\n';

    return (sign(dirs[0]/Tr.p.n) == sign(dirs[1]/Tr.p.n) && (sign(dirs[0]/Tr.p.n) == sign(dirs[2]/Tr.p.n)))?(1):(0);
}

int intersect_line_line_check(const line& L1, const line& L2){ //lines are in one plane
    // std::cout << ((L1.r - L2.r)&(L1.a)) << '\n' << L2.a << '\n';
    // std::cout << "check: " <<((L1.r - L2.r)&(L1.a))*L2.a << '\n';
    // std::cout << "res: " << my_iszero(((L1.r - L2.r)&(L1.a))*L2.a) << '\n';
    
    return (my_iszero(((L1.r - L2.r)&(L1.a))*L2.a))?(1):(0);
}

vect intersect_line_plane(const line& L, const plane& P){
    if ((L.a*P.n) == 0){ //line || plane
        if ((L.r - P.r)*P.n == 0) {
            return L.r;
        }
        else return INVL_VECT;
    }
    // std::cout << -1*((L.r-P.r)*P.n)/(L.a*P.n) << '\n';
    return L.r - (((L.r-P.r)*P.n)/(L.a*P.n)*L.a);
}

/// @brief calculates intersection of lines
/// @param L1 
/// @param L2 
/// @return {k1, k2}: solutions of "L1.r + k1*L1.a = L2.r + k2*L2.a"
std::pair<double, double> intersect_line_line_coeff(const line& L1, const line& L2){
    // std::cout << "checking " << L1 << " and " << L2 << '\n';
    if ((intersect_line_line_check(L1, L2) == 0)){
        // std::cout << "not in same plane"<<'\n';
        return std::pair<double, double> {__DBL_MAX__, __DBL_MAX__};
    }
    else if (L1.r == L2.r){
        // std::cout << "have same begining\n";
        return std::pair<double, double> {0, 0};
    }
    else if (L1.a || L2.a){
        // std::cout << "parallell lines\n";
        if((L2.r - L1.r) || L1.a ){
            // std::cout << "same line\n";
            return std::pair<double, double> {(L2.r-L1.r)/L1.a, (L1.r-L2.r)/L2.a};
        }
        return std::pair<double, double> {__DBL_MAX__, __DBL_MAX__};
    }
    
         
    double a1, a2;
    vect dr = L2.r-L1.r;
    vect a1p = L1.a - ((L1.a * L2.a)/(L2.a.sqsize()))*L2.a;
    vect a2p = L2.a - ((L1.a * L2.a)/(L1.a.sqsize()))*L1.a;

    // std::cout << "dr: " << dr << '\n';
    // std::cout << "a1p: " << a1p << '\n';
    // std::cout << "a2p: " << a2p << '\n';

    a2 = -(dr*a2p)/(L2.a*a2p);
    a1 = (dr*a1p)/(L1.a*a1p);

    // printf("%f %f\n", a1, a2);
    // std::cout << (dr*a1p) << ' ' << (a1*a1p) << '\n';


    // std::cout << a1 << '\n' << a2 << '\n';

    return std::pair<double, double> {a1, a2};
}

int intersect_line_triangle(const triangle& Tr, const line& L){
    double c = 0;
    int ok = 0;
    line side{};
    if(L.r*Tr.p.n == 0) { // line || Tr.plane
        if((L.r - Tr.vertexes[0])*Tr.p.n == 0) { //line is in Tr.plane
            for(int i = 0; i < 3; i++){
                side = {Tr.vertexes[i], Tr.sides[i]};
                std::pair<double, double> res = intersect_line_line_coeff(L, side);
                c = res.second;
                if (inbounds(c, 0, 1)) ok++;
            }
            return (ok >= 2)?1:0; //need two real intersections of line with sides
        }
        else { //line || Tr.plane
            return 0;
        }
    }
    else { //line intersects Tr.p
        vect r0 = intersect_line_plane(L, Tr.p);
        // std::cout << "inters at " << r0 << '\n';
        return intersect_point_triangle(Tr, r0);
    }

    return 0;
}

int intersect_point_line(const vect& v, const line& L){
    return (v-L.r || L.a);
}

int intersect_triangles(const triangle& Tr1, const triangle& Tr2){

    // std::cout << Tr1.index << ' ' << Tr2.index << '\n';

    if(Tr1.intersected && Tr2.intersected){
        return 1;
    }
    
    switch (Tr1.state)
    {
    case NORM: 
        switch (Tr2.state)
        {
            case NORM: return intersect_triangle_triangle(Tr1, Tr2);
            case LINE: return intersect_line_triangle(Tr1, Tr2.l);
            case POINT: return intersect_point_triangle(Tr1, Tr2.vertexes[0]);
            case BAD: return -1;
        }
    case LINE: 
        switch (Tr2.state)
        {
            case NORM: return intersect_line_triangle(Tr2, Tr1.l);
            case LINE: return intersect_line_line_check(Tr1.l, Tr2.l);
            case POINT: return intersect_point_line(Tr2.vertexes[0], Tr1.l);
            case BAD: return -1;
        }
    case POINT: 
        switch (Tr2.state)
        {
            case NORM: return intersect_point_triangle(Tr2, Tr1.vertexes[0]);
            case LINE: return intersect_point_line(Tr1.vertexes[0], Tr2.l);
            case POINT: return (Tr1.vertexes[0] == Tr2.vertexes[0]);
            case BAD: return -1;
        }

    case BAD:
        return -1;
    }

    return -1;
}

int intersect_triangle_plane(const plane& P, const triangle& Tr){
    double c0 = (Tr.vertexes[0]*P.n + P.D);
    double c1 = (Tr.vertexes[1]*P.n + P.D);
    double c2 = (Tr.vertexes[2]*P.n + P.D);

    if((sign(c0) == sign(c1)) &&  (sign(c0) == sign(c2))){
        return sign(c0); //no intersection with plane -> returning 1 or -1 to one hemispace and other (1 where P.n points)
    }
    else return 0;
}


void process_node(octtree* node, std::list<octtree*>* octoqueue, double min_lenght){
    int r1, r2, r3 = 0;

    int res[3][3];

    if(node->_lenght/2 < min_lenght){
        // maybe need to rewrite logic of inserting in nodes
        for(auto &x: node->interior){
            node->bound_intersections.push_back(x);
        }
        return;
    }

    for(auto &x: node->interior){
        for(int i = 0; i < 3; i++){
            res[i][0] = sign((x->vertexes[i]-node->_center).x);
            res[i][1] = sign((x->vertexes[i]-node->_center).y);
            res[i][2] = sign((x->vertexes[i]-node->_center).z);
        }

        if( (res[0][0] == res[1][0]) && (res[0][0] == res[2][0]) &&
            (res[0][1] == res[1][1]) && (res[0][1] == res[2][1]) &&
            (res[0][2] == res[1][2]) && (res[0][2] == res[2][2])){

            if(res[0][0] == 0 || res[0][1] == 0 || res[0][2] == 0){
                node->bound_intersections.push_back(x);
                continue;
            }

            int insertion_res = 0;

            switch (100*res[0][2] + 10*res[0][0] + res[0][1])
            {
            case (100 + 10 + 1): insertion_res = node->insert_in_subspace(x, 0);
                break;
            case (100 + 10 - 1): insertion_res = node->insert_in_subspace(x, 1);
                break;
            case (100 - 10 - 1): insertion_res = node->insert_in_subspace(x, 2);
                break;
            case (100 - 10 + 1): insertion_res = node->insert_in_subspace(x, 3);
                break;
            case (-100 + 10 + 1): insertion_res = node->insert_in_subspace(x, 4);
                break;
            case (-100 + 10 - 1): insertion_res = node->insert_in_subspace(x, 5);
                break;
            case (-100 - 10 - 1): insertion_res = node->insert_in_subspace(x, 6);
                break;
            case (-100 - 10 + 1): insertion_res = node->insert_in_subspace(x, 7);
                break;
            default:
                std::cout << "error, got: " << 100*res[0][3] + 10*res[0][1] + res[0][2] << '\n';
                break;
            }

            if(insertion_res == 0) {
                node->bound_intersections.push_back(x);
            }
            else if(insertion_res == -1){
                std::cout << "error in inserting triangle in node\n";
            }

        }
        else{
            node->bound_intersections.push_back(x);
        }
    }

    for(int i = 0; i < 8; i++){
        if(node->subspace[i] != nullptr) {
            octoqueue->push_back(node->subspace[i]);
        }
    }
    
}

int octtree::process_tree(double min_size) {
    std::list<octtree*> octoqueue;
    octoqueue.push_back(this);
    octtree* subtree;
    int count = 0;

    while(octoqueue.begin() != octoqueue.end()){
        subtree = octoqueue.front();
        process_node(subtree, &octoqueue, min_size);
        octoqueue.pop_front();
        count++;
    }
    return count;
}