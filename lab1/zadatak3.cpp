#include <iostream>
#include <glm/glm.hpp>

using namespace std;
using namespace glm;

void printVector(vec3 v);

/*
 * Program na ulazu prima koordinate tri tocke trokuta u 3d ravnini,
 * kao i koordinate tocke T za koju ispisuje njezine baricentricne koordinate.
 * */

int main() {
    int Ax, Ay, Az, Bx, By, Bz, Cx, Cy, Cz, Tx, Ty, Tz;
    mat3 vertexMatrix;
    vec3 vectorT, baricentricCoordinates;

    cout << "Unesite koordinate triju vrhova trokuta (A, B, C) i tocke T:\n";
    cin >> Ax >> Ay >> Az >> Tx >> Bx >> By >> Bz >> Ty >> Cx >> Cy >> Cz >> Tz;

    vertexMatrix = mat3(Ax, Bx, Cx,
                        Ay, By, Cy,
                        Az, Bz, Cz);
    vectorT = vec3(Tx, Ty, Tz);

    //isti matricni postupak kao i u prethodnome zadataku
    baricentricCoordinates = inverse(vertexMatrix) * vectorT;

    cout << "Baricentricne koordinate tocke T:\nT";
    printVector(baricentricCoordinates);

    return 0;
}

void printVector(vec3 v) {
    cout << "(" << v.x << ", ";
    cout << v.y << ", ";
    cout << v.z << ")" << endl;
}