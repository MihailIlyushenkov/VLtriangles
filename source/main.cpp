#include "geom.hpp"

void count_usual(triangle* Triangles, size_t count){

    for(size_t i = 0; i < count; i++){
        Triangles[i].intersected = 0;
    }

    std::cout << '\n';

    for(size_t i = 0; i < count - 1; i++){
        for(size_t j = i + 1; j < count; j++){
            if(intersect_triangles(Triangles[i], Triangles[j]) == 1){
                std::cout << i << " and " << j;
                if( Triangles[i].vertexes[0] == Triangles[j].vertexes[0] &&
                    Triangles[i].vertexes[1] == Triangles[j].vertexes[1] &&
                    Triangles[i].vertexes[2] == Triangles[j].vertexes[2]) {
                        std::cout << " same vert\n";
                }
                Triangles[i].intersected = 1;
                Triangles[j].intersected = 1;
                std::cout << Triangles[i] << Triangles[j] << '\n';
            }
            else if (intersect_triangles(Triangles[i], Triangles[j]) == -1){
                std::cout << "ERROR:\n" << Triangles[i] << '\n' << Triangles[j] << "\n\n";
            }
        }
    }

    std::cout << "usual intersections: ";
    for(size_t i = 0; i < count; i++){
        if(Triangles[i].intersected == 1){
            std::cout << i << ' ';
        }
    }
    std::cout << '\n';
}

int main(){
    int count = 0; 

    std::cin >> count;
    

    if(count <= 0) {
        std::cout << "invalid count of triangles\n";
        return -1;
    }

    // std::cout<<count <<'\n';

    triangle* Triangles = new triangle [count];

    vect trvert[3];
    double x, y, z;

    vect sum_vect = {0,0,0};
    double total_distance_between_vertexes = 0;
    double total_dispersion = 0;

    for(int i = 0; i < count; i++){
        for(int j = 0; j < 3; j++){
            std::cin >> x;
            std::cin >> y;
            std::cin >> z;

            trvert[j] = vect{x, y, z};
        }

        sum_vect = sum_vect + trvert[0] + trvert[1] + trvert[2];
        total_distance_between_vertexes += (trvert[0] - trvert[1]).size() + (trvert[1] - trvert[2]).size() + (trvert[0] - trvert[2]).size();

        Triangles[i] = triangle{trvert[0], trvert[1], trvert[2]};
        Triangles[i].process();
        Triangles[i].index = i;
    }

    for(int i = 0; i < count; i++){
        total_dispersion += ((Triangles[i].vertexes[0] + Triangles[i].vertexes[1] + Triangles[i].vertexes[2]).size() - sum_vect.size());
        // std::cout << Triangles[i] << '\n';
    }

    // std::cout << "average vect: " << sum_vect/count << '\n';
    // std::cout << "average_distance_between_vertexes: " << total_distance_between_vertexes/(3*count) << '\n';
    // std::cout << "dispersion: " << total_dispersion/count << '\n';
    

    octtree root{sum_vect/count, total_dispersion/count};
    // octtree root{{0,0,0}, 40};
    

    for(int i = 0; i < count; i++) {
        root.interior.push_back(&Triangles[i]);
    }

    double average_distance_between_vertexes = total_distance_between_vertexes/(3*count);

    if (average_distance_between_vertexes > 1){
        root.process_tree(total_distance_between_vertexes/(3*count));
    }
    else {
        root.process_tree(1);
    }

    root.count_internal_intersections();
    
    // std::cout << "tree intersections: ";
    for(int i = 0; i < count; i++){
        if(Triangles[i].intersected == 1){
            std::cout << i << ' ';
        }
    }
    std::cout << '\n';
    
    count_usual(Triangles, count);

    delete Triangles;
    return 0;
}

/*
агрегат - все, похожее на сишные массивы и структуры (без конструкторов)
struct S {int x = 0, y = 0;};
S s{1, 2}; - агрегатная инициализация.

struct T {int x = 0, y = 0; T(int a, int b): x(a+b) {}}
T t{1, 2}; // вызов конструктора

основные виды инициализации

int global; //zero-inint

int foo(){
    std::vector<int> v; //default-init
    int j{}; //value-init
    int k = 7; //copy-init
    int i{7}; //direct-init
    std::vector<int> w {1, 2, 3}; //direct-list-init
    std::vector<int> z = {1, 2, 3}; //copy-list-init
}
*/