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

    // t�� ����� �� ���� [���]�մϴ�.
    t.join();
    printf("t.join\n");

    t2.join();
    printf("t2.join\n");
}

void ThreadDemo::Join()
{
    /*
    * Lambda
    * = : ��������� ���� �޾Ƽ� ����մϴ�.
    * & : �����̳� ��� ������ ���� �޾Ƽ� ����մϴ�.
    */
    thread t([]() 
        {
            for (int i = 0; i < 100; i++)
                printf("1 : %d\n", i);
            printf("�ݺ���1 ����\n");
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

    printf("��Ƽ ������ ����\n");

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

    // �ϴ�, �Ѿ �� t�� �Ϸ��� join�� �����ݴϴ�.
    t.detach();
}

void ThreadDemo::MutexUpdate()
{
    progress += 0.01f;

    ImGui::ProgressBar(progress / 1000.0f);
}

/*
* thread���� ���� �ڿ��� ����ϴٺ��� ��Ȯ�� ���� ������ �ʽ��ϴ�.
* (t1�� ���� �����ϰ� ������ �����Ϸ��� ���� t2�� ���� �����ϰ� ������ �����ϱ� �����Դϴ�.)
* �̶���, mutex�� lock(), unlock()�� ����ؼ� �ڿ��� ����� �� �ֽ��ϴ�.
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

    printf("�� ����ð� : %f\n", last);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* 
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void ThreadDemo::Loop()
{
    for (int i = 0; i < 100; i++)
        printf("1 : %d\n", i);
    printf("�ݺ���1 ����\n");

    for (int i = 0; i < 100; i++)
        printf("2 : %d\n", i);
    printf("�ݺ���2 ����\n");
}

void ThreadDemo::Function()
{
    printf("�Լ� ������ ȣ��\n");
}

void ThreadDemo::MultiThread1()
{
    for (int i = 0; i < 100; i++)
        printf("1 : %d\n", i);
    printf("�ݺ���1 ����\n");
}

void ThreadDemo::MultiThread2()
{
    for (int i = 0; i < 100; i++)
        printf("2 : %d\n", i);
    printf("�ݺ���2 ����\n");
}