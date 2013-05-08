#include "demo.h"
#include "mesh.h"
#include <ctime>
#include <random>
#include <functional>

std::unique_ptr<Mesh> mesh;

void _FAILTEST()
{
    LOG("* UNIT TEST FAILED");
    abort();
}

template <typename T>
void _TEST_EQ(T const &a, T const &b, const char *msg)
{
    if (!(a == b)) {
        LOG("%s", msg);
        _FAILTEST();
    }
}

template <>
void _TEST_EQ<float>(float const &a, float const &b, const char *msg)
{
    if (fabs(a - b) > 1e-7) {
        LOG("%s  (lhs = %f, rhs = %f)", msg, (double) a, (double) b);
        _FAILTEST();
    }
}

#define TEST_EQ(expr, result) \
    _TEST_EQ(expr, result, #expr " != " #result)

void demo_init(unsigned, unsigned)
{
    std::normal_distribution<float> normDist;
    std::mt19937 engine; // Mersenne twister MT19937
    auto normRand = std::bind(normDist, engine);

    engine.seed(time(NULL));

    mesh = Mesh::createRing(4, PI/4.f);
    mesh->splitVert(mesh->eEdge(0,0), mesh->eVertPrev(0,0));
    mesh->splitVert(mesh->eEdge(0,2), mesh->eVertPrev(0,2));
    mesh->splitVert(mesh->eEdge(0,4), mesh->eVertPrev(0,4));
    mesh->splitVert(mesh->eEdge(0,6), mesh->eVertPrev(0,6));
    U32 edge = mesh->splitFace(mesh->eEdge(0,1), mesh->eEdge(0,5));
    ASSERTX(edge/8 == 2 && mesh->eOpposite(edge)/8 == 0);
    ASSERTX(mesh->check());

    TEST_EQ(lerp(0.f, 1.f, -1, 1), -1.f);
    TEST_EQ(lerp(0.f, 1.f, 0, 1), 0.f);
    TEST_EQ(lerp(0.f, 1.f, 1, 2), .5f);
    TEST_EQ(lerp(0.f, 1.f, 1, 1), 1.f);
    TEST_EQ(lerp(0.f, 1.f, 2, 1), 2.f);

    TEST_EQ(lerp(1.f, 0.f, -1, 1), 2.f);
    TEST_EQ(lerp(1.f, 0.f, 0, 1), 1.f);
    TEST_EQ(lerp(1.f, 0.f, 1, 2), .5f);
    TEST_EQ(lerp(1.f, 0.f, 1, 1), 0.f);
    TEST_EQ(lerp(1.f, 0.f, 2, 1), -1.f);

    TEST_EQ(lerp(Vec3({3.f,0.f,3.f}), Vec3({0.f,3.f,0.f}), 2, 3),
            Vec3({1.f,2.f,1.f}));

    {
        TEST_EQ(Mat2::identity().det(), 1.f);
        TEST_EQ((Mat2::identity()*4.f).det(), 16.f);

        Mat2 mr = {{{normRand(), normRand()},
                    {normRand(), normRand()}}};
        TEST_EQ(mr*Mat2::identity(), mr);
        TEST_EQ(Mat2::identity()*mr, mr);
        TEST_EQ(mr.transpose().transpose(), mr);
        TEST_EQ(mr.transpose().transpose().det(), mr.det());
    }

    {
        Mat3 mr = {{{normRand(), normRand(), normRand()},
                    {normRand(), normRand(), normRand()},
                    {normRand(), normRand(), normRand()}}};

        TEST_EQ(Mat3::identity().det(), 1.f);
        TEST_EQ((Mat3::identity()*4.f).det(), 64.f);

        Mat3 ma({{{-2.f,-1.f,2.f},
                 {2.f,1.f,0.f},
                 {3.f,3.f,-1.f}}});

        TEST_EQ(ma[0][0], -2.f); TEST_EQ(ma[0][1], -1.f); TEST_EQ(ma[0][2],  2.f);
        TEST_EQ(ma[1][0],  2.f); TEST_EQ(ma[1][1],  1.f); TEST_EQ(ma[1][2],  0.f);
        TEST_EQ(ma[2][0],  3.f); TEST_EQ(ma[2][1],  3.f); TEST_EQ(ma[2][2], -1.f);
        TEST_EQ(ma[0][0] * (ma[1][1]*ma[2][2] - ma[2][1]*ma[1][2]), 2.f);
        TEST_EQ(ma[1][0] * (ma[2][1]*ma[0][2] - ma[0][1]*ma[2][2]), 10.f);
        TEST_EQ(ma[2][0] * (ma[0][1]*ma[1][2] - ma[1][1]*ma[0][2]), -6.f);
        TEST_EQ(ma.det(), 6.f);
        TEST_EQ(ma.transpose().det(), 6.f);
        TEST_EQ(ma.transpose().transpose(), ma);
    }
}

bool demo_prepareFrame()
{
    return false;
}

void demo_drawFrame()
{
}

void demo_evtMouseMove(int x, int y)
{
}
void demo_evtMouseButton(uint8_t button, bool state)
{
}
