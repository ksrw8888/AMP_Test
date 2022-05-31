#include <iostream>
#include <amp.h>
#include <WinBase.h>

#define COUNT 10000

float nickName_GPU[COUNT];
float nickName_CPU[COUNT];

int main(void)
{
    LARGE_INTEGER freq;
    LARGE_INTEGER strt;
    LARGE_INTEGER ed;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&strt);

    concurrency::array_view<float> myView(COUNT, nickName_GPU); //�N��ƥ��J���T�O����
    concurrency::parallel_for_each(myView.extent, [=](concurrency::index<1> idx) restrict(amp)
        {
            for (int i = 0; i < 100000; i++)
            {
                myView[idx] = (myView[idx] + 0.1f) / 2.3f;
            }
        });

    myView.synchronize();//�㦡����GPU�p�⧹���ñN��ƥ��^�O����

    QueryPerformanceCounter(&ed);
    printf("GPU�Ӯ�: %d �@��\r\n", (ed.QuadPart - strt.QuadPart) * 1000 / freq.QuadPart);
    QueryPerformanceCounter(&strt);

#pragma omp parallel
    for (int idx = 0; idx < COUNT; idx++)
    {
        for (int i = 0; i < 100000; i++)
        {
            nickName_CPU[idx] = (nickName_CPU[idx] + 0.1f) / 2.3f;
        }
    }
    QueryPerformanceCounter(&ed);
    printf("CPU�Ӯ�: %d �@��\r\n", (ed.QuadPart - strt.QuadPart) * 1000 / freq.QuadPart);
    for (int idx = 0; idx < COUNT; idx++)
    {
        if (nickName_CPU[idx] != nickName_GPU[idx])
        {
            puts("CPU�MGPU���p�⵲�G���۲šI");
            getchar();
            return 0;
        }
    }
    puts("���յ���");
    getchar();
    return 0;
}