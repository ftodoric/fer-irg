#include <iostream>
#include <glm/glm.hpp>

using namespace std;
using namespace glm;

//pomocni ispisi objekata
void printVector(vec3 v);
void printMatrix(mat3 m);

int main()
{
    vec3 v1, s, v2, v3, v4;
    mat3 M1, M2, M3;
    vec4 V;

    v1 = vec3(2, 3, -4) + vec3(-1, 4, -1);
    cout << "v1 = ";
    printVector(v1);

    s = v1 * vec3(-1, 4, -1);
    cout << "s = ";
    printVector(s);

    v2 = cross(v1, vec3(2, 2, 4));
    cout << "v2 = ";
    printVector(v2);

    v3 = normalize(v2);
    cout << "v3 = ";
    printVector(v3);

    v4 = -1.0f * v2;
    cout << "v4 = ";
    printVector(v4);

    //konstruktori matrice unose stupce slijedno
    M1 = mat3(1, 2, 4, 2, 1, 5, 3, 3, 1) + mat3(-1, 5, -4, 2, -2, -1, -3, 7, 3);
    cout << endl
         << "m1 = \n";
    printMatrix(M1);

    M2 = mat3(1, 2, 4, 2, 1, 5, 3, 3, 1) * transpose(mat3(-1, 5, -4, 2, -2, -1, -3, 7, 3));
    cout << endl
         << "m2 = \n";
    printMatrix(M2);

    M3 = mat3(1, 2, 4, 2, 1, 5, 3, 3, 1) * inverse(mat3(-1, 5, -4, 2, -2, -1, -3, 7, 3));
    cout << endl
         << "m3 = \n";
    printMatrix(M3);

    V = vec4(1, 2, 3, 1) * mat4(1, 0, 0, 2,
                                0, 2, 0, 3,
                                0, 0, 1, 3,
                                0, 0, 0, 1);
    cout << endl
         << "V = [";
    cout << V.x << ", " << V.y << ", " << V.z << ", " << V.w << "]\n";

    return 0;
}

void printVector(vec3 v)
{
    cout << "[" << v.x << ", ";
    cout << v.y << ", ";
    cout << v.z << "]" << endl;
}

void printMatrix(mat3 m)
{
    //provjera je li doslo do operacije s neinverzibilnom matricom
    if (isnan(m[0][0]))
    {
        cout << "Invertible matrix operation!\n";
        return;
    }
    //stupci su pohranjeni u redovima
    for (int i = 0; i < glm::mat3::length(); ++i)
    {
        for (int j = 0; j < glm::mat3::length(); ++j)
        {
            cout << m[j][i] << " ";
        }
        cout << endl;
    }
}