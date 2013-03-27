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

    TEST_EQ(Mat2::identity().det(), 1.f);
    TEST_EQ((Mat2::identity()*4.f).det(), 16.f);
    {
        Mat2 m = {{{normRand(), normRand()},
                   {normRand(), normRand()}}};
        TEST_EQ(m*Mat2::identity(), m);
        TEST_EQ(Mat2::identity()*m, m);
    }

    TEST_EQ(Mat3::identity().det(), 1.f);
    TEST_EQ((Mat3::identity()*4.f).det(), 64.f);
    TEST_EQ(Mat3({{{-2.f,2.f,3.f},
                   {-1.f,1.f,3.f},
                   {2.f,0.f,-1.f}}}).det(), 6.f);
}

bool demo_prepareFrame()
{
    return false;
}

void demo_drawFrame()
{
}
