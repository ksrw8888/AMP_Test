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

    concurrency::array_view<float> myView(COUNT, nickName_GPU); //將資料打入視訊記憶體
    concurrency::parallel_for_each(myView.extent, [=](concurrency::index<1> idx) restrict(amp)
        {
            for (int i = 0; i < 100000; i++)
            {
                myView[idx] = (myView[idx] + 0.1f) / 2.3f;
            }
        });

    myView.synchronize();//顯式等待GPU計算完成並將資料打回記憶體

    QueryPerformanceCounter(&ed);
    printf("GPU耗時: %d 毫秒\r\n", (ed.QuadPart - strt.QuadPart) * 1000 / freq.QuadPart);
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
    printf("CPU耗時: %d 毫秒\r\n", (ed.QuadPart - strt.QuadPart) * 1000 / freq.QuadPart);
    for (int idx = 0; idx < COUNT; idx++)
    {
        if (nickName_CPU[idx] != nickName_GPU[idx])
        {
            puts("CPU和GPU的計算結果不相符！");
            getchar();
            return 0;
        }
    }
    puts("測試結束");
    getchar();
    return 0;
}