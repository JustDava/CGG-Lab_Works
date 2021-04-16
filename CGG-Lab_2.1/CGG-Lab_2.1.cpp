#include <iostream>
#include <vector>

using namespace std;

//int pnpoly(int nvert, float* vertx, float* verty, float testx, float testy)
//{
//    int i, j, c = 0;
//    for (i = 0, j = nvert - 1; i < nvert; j = i++) {
//        if (((verty[i] > testy) != (verty[j] > testy)) &&
//            (testx < (vertx[j] - vertx[i]) * (testy - verty[i]) /
//                (verty[j] - verty[i]) + vertx[i]))
//            c = !c;
//    }
//    return c;
//}

int pnpoly(int nvert, float* vertx, float* verty, float testx, float testy)
{
    int i, j, c = 0;
    for (i = 0, j = nvert - 1; i < nvert; j = i++) {
        //if ((verty[i] > testy) != (verty[j] > testy))
        //{
        if ((verty[i] == testy && verty[j] == testy) && (vertx[i] > testx) != (vertx[j] > testx))
        {
            c++;
            break;
        }
        if ((vertx[i] == testx && vertx[j] == testx) && (verty[i] > testy) != (verty[j] > testy))
        {
            c++;
            break;
        }

        float t2 = (verty[i] - testy) / (verty[i] - verty[j]);
        float t1 = (vertx[i] - testx) + ((vertx[j] - vertx[i]) * t2);
        if (t2 == 0 && t1 == 0)
        {
            c = !c;
            break;
        }
        if ((t2 >= 0 && t2 <= 1) && t1 >= 0)
        {
            if ((0 * (vertx[i] - vertx[j]) + 1 * (verty[i] - verty[j])) > 0)
            {
                c++;
            }
            else c--;
        }
        //}
        //if (((verty[i] > testy) != (verty[j] > testy)) &&
        //    (testx < (vertx[j] - vertx[i]) * (testy - verty[i]) /
        //        (verty[j] - verty[i]) + vertx[i]))
        //    c = !c;
    }
    return c;
}

int main()
{
    setlocale(LC_ALL, "ru");

    const int polygN = 8;

    float polygX[polygN] = { 2, 2, 5, 10, 13, 13, 10, 5 };
    float polygY[polygN] = { 5, 8, 11, 11, 8, 5, 2, 2 };

    float testpointX = 7;
    float testpointY = 10;
    
    int c = pnpoly(polygN, polygX, polygY, testpointX, testpointY);

    if (c != 0)
    {
        cout << "Точка внутри многоугольника";
    }
    else cout << "Точка вне многоугольника";

    return 0;
}
