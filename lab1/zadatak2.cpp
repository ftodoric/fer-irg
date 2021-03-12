#include <iostream>
#include <glm/glm.hpp>

using namespace std;
using namespace glm;

void printVector(vec3 v);

/*
 * Program na ulazu prima sve potrebne koeficijente triju jednadzbi s tri nepoznanice
 * bez provjere ispravnosti ulaza.
 * */
int main() {
    int a1, a2, a3, b1, b2, b3, c1, c2, c3, t1, t2, t3;
    mat3 coeficientMatrix;
    vec3 vectorT, result;

    cout << "Unesite koeficijente triju jednadzbi s tri nepoznanice:\n";
    cin >> a1 >> b1 >> c1 >> t1 >> a2 >> b2 >> c2 >> t2 >> a3 >> b3 >> c3 >> t3;

    coeficientMatrix = mat3(a1, a2, a3,
                            b1, b2, b3,
                            c1, c2, c3);
    vectorT = vec3(t1, t2, t3);

    //rijesenje se dobije primjenom matricnih operacija
    result = inverse(coeficientMatrix) * vectorT;

    cout << "[x y z] = ";
    printVector(result);

    return 0;
}

void printVector(vec3 v) {
    cout << "[" << v.x << ", ";
    cout << v.y << ", ";
    cout << v.z << "]" << endl;
}