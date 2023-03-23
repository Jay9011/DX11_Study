#include "stdafx.h"
#include "ThreadDemo.h"

void ThreadDemo::Initialize()
{
    //Loop();

    //function<void()> f = bind(&ThreadDemo::Function, this);
    //f();
    
    //MultiThread();

    //Join();

    //Mutex();

    //Execute();

    //ExecuteTimer();

    ExecutePerformance();
}

void ThreadDemo::Update()
{
    //MutexUpdate();
}

void ThreadDemo::Render()
{
}

void ThreadDemo::MultiThread()
{
    thread t(bind(&ThreadDemo::MultiThread1, this));
    thread t2(bind(&ThreadDemo::MultiThread2, this));

    // t가 종료될 때 까지 [대기]합니다.
    t.join();
    printf("t.join\n");

    t2.join();
    printf("t2.join\n");
}

void ThreadDemo::Join()
{
    /*
    * Lambda
    * = : 멤버변수를 대입 받아서 사용합니다.
    * & : 전역이나 멤버 변수를 참조 받아서 사용합니다.
    */
    thread t([]() 
        {
            for (int i = 0; i < 100; i++)
                printf("1 : %d\n", i);
            printf("반복문1 종료\n");
        }
    );

    thread t2([]()
        {
            int a = 0;
            while (true)
            {
                a++;
                printf("A : %d\n", a);

                Sleep(100);

                if (a > 30)
                    break;
            }
        }
    );

    printf("멀티 쓰레드 시작\n");

    t2.join();
    printf("t2.join\n");

    t.join();
    printf("t.join\n");
}

void ThreadDemo::Mutex()
{
    thread t([&]()
        {
            while (true)
            {
                Sleep(1000);

                printf("Progress : %f\n", progress);
            }
        }
    );

    // 일단, 넘어간 후 t가 완료후 join을 시켜줍니다.
    t.detach();
}

void ThreadDemo::MutexUpdate()
{
    progress += 0.01f;

    ImGui::ProgressBar(progress / 1000.0f);
}

/*
* thread간에 공유 자원을 사용하다보니 정확한 값이 나오지 않습니다.
* (t1이 값을 저장하고 증가를 수행하려는 동안 t2도 값을 저장하고 증가를 수행하기 때문입니다.)
* 이때는, mutex의 lock(), unlock()을 사용해서 자원을 잠궈줄 수 있습니다.
*/
void ThreadDemo::RaceCondition(int& counter)
{
    for (int i = 0; i < 100000; i++)
    {
        m.lock();
        {
            counter++;
        }
        m.unlock();
    }
}

void ThreadDemo::Execute()
{
    int counter = 0;

    vector<thread> threads;
    for (int i = 0; i < 4; i++)
    {
        function<void(int&)> f = bind(&ThreadDemo::RaceCondition, this, placeholders::_1);

        threads.push_back(thread(f, ref(counter)));
    }

    for (int i = 0; i < 4; i++)
        threads[i].join();

    printf("Counter : %d\n", counter);
}

void ThreadDemo::ExecuteTimer()
{
    timer.Start([]()
    {
        printf("Timer\n");
    }
    , 2000, 2);

    timer.Start([]()
    {
        printf("Timer2\n");
    }
    , 3000);
}

void ThreadDemo::ExecutePerformance()
{
    int arr[10000];
    for (int i = 0; i < 10000; i++)
        arr[i] = Math::Random(0, 100000);

    Performance p;
    p.Start();
    {
        sort(arr, arr + 10000);
    }
    float last = p.End();

    printf("총 수행시간 : %f\n", last);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* 
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void ThreadDemo::Loop()
{
    for (int i = 0; i < 100; i++)
        printf("1 : %d\n", i);
    printf("반복문1 종료\n");

    for (int i = 0; i < 100; i++)
        printf("2 : %d\n", i);
    printf("반복문2 종료\n");
}

void ThreadDemo::Function()
{
    printf("함수 포인터 호출\n");
}

void ThreadDemo::MultiThread1()
{
    for (int i = 0; i < 100; i++)
        printf("1 : %d\n", i);
    printf("반복문1 종료\n");
}

void ThreadDemo::MultiThread2()
{
    for (int i = 0; i < 100; i++)
        printf("2 : %d\n", i);
    printf("반복문2 종료\n");
}
