#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <sys/prctl.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include <mqueue.h>
#include <system_server.h>
#include <gui.h>
#include <input.h>
#include <web_server.h>

static int toy_timer = 0;
pthread_mutex_t system_loop_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t system_loop_cond = PTHREAD_COND_INITIALIZER;
bool system_loop_exit = false;

void *watchThread(void *arg){

	printf("watchdog_Thread");
}


void *monitorThread(void *arg){

	printf("moitor_Thread");
}


void *diskThread(void *arg){
    char *s = arg;

    printf("%s", s);

    while (1) {
        posix_sleep_ms(5000);
    }

    return 0;
}


void *cameraThread(void *arg){

	printf("camera_Thread");
}

int posix_sleep_ms(unsigned int timeout_ms)
{
    struct timespec sleep_time;

    sleep_time.tv_sec = timeout_ms / MILLISEC_PER_SECOND;
    sleep_time.tv_nsec = (timeout_ms % MILLISEC_PER_SECOND) * (NANOSEC_PER_USEC * USEC_PER_MILLISEC);

    return nanosleep(&sleep_time, NULL);
}

static void timer_handler(int sig){
	toy_timer++;
	printf("timeer %d\n",toy_timer);

}
void set_periodic_timer(long sec_delay, long usec_delay)
{
	struct itimerval itimer_val = {
		 .it_interval = { .tv_sec = sec_delay, .tv_usec = usec_delay },
		 .it_value = { .tv_sec = sec_delay, .tv_usec = usec_delay }
    };

	setitimer(ITIMER_REAL, &itimer_val, (struct itimerval*)0);
}

void signal_exit(void)
{
    /* 여기에 구현하세요..  종료 메시지를 보내도록.. */

    pthread_mutex_lock(&system_loop_mutex);
    printf("no loop\n");
    system_loop_exit = true;
    pthread_cond_signal(&system_loop_cond);
    pthread_mutex_unlock(&system_loop_mutex);
    
}


int system_server()
{
    struct itimerval ts;
    struct sigaction  sa;
    struct sigevent   sev;
    timer_t *tidlist;
    int retcode;
    pthread_t watchdog_thread_tid, monitor_thread_tid, disk_service_thread_tid, camera_service_thread_tid;

    sigemptyset(&sa.sa_mask);
    sa.sa_flags=0;
    sa.sa_handler=timer_handler;
    if(sigaction(SIGALRM,&sa,NULL)==-1)
    {
	perror("sigaction:Timer");
	exit(-1);
    }
    printf("나 system_server 프로세스!\n");


    /* 10초 타이머 등록 */
    set_periodic_timer(10, 0);

    /* 스레드를 생성한다. */
    retcode = pthread_create(&watchdog_thread_tid, NULL, watchThread, "watchdog thread\n");
    assert(retcode == 0);
    retcode = pthread_create(&monitor_thread_tid, NULL, monitorThread, "monitor thread\n");
    assert(retcode == 0);
    retcode = pthread_create(&disk_service_thread_tid, NULL, diskThread, "disk service thread\n");
    assert(retcode == 0);
    retcode = pthread_create(&camera_service_thread_tid, NULL, cameraThread, "camera service thread\n");
    assert(retcode == 0);

    printf("system init done.  waiting...");

    // 여기에 구현하세요... 여기서 cond wait로 대기한다. 10초 후 알람이 울리면 <== system 출력
    
    pthread_mutex_lock(&system_loop_mutex);
    while(system_loop_exit==false)
    {
	pthread_cond_wait(&system_loop_cond,&system_loop_mutex);
    }
    pthread_mutex_unlock(&system_loop_mutex);
    printf("sysyem\n");

    //
    //
    /* 1초 마다 wake-up 한다 */
    while (system_loop_exit == false) {
        sleep(1);
    }

    while (1) {
        sleep(1);
    }

    return 0;
    /*
     과제를 위해 이전에 한 건 주석처리로 남겼습니다. 
    ts.it_interval.tv_sec=5;
    ts.it_interval.tv_usec=0;
    ts.it_value.tv_sec=5;
    ts.it_value.tv_usec=0;
    if(setitimer(ITIMER_REAL,&ts,NULL)==-1){
	    perror("setTimer error");
	    exit(-1);
    }
    pthread_create(&watchdog_thread_tid, NULL, watchThread, (void *)NULL);
    pthread_create(&monitor_thread_tid, NULL, monitorThread, (void *)NULL);
    pthread_create(&disk_service_thread_tid, NULL, diskThread, (void *)NULL);
    pthread_create(&camera_service_thread_tid, NULL, camaraThread, (void *)NULL);
    pthread_join(watchdog_thread_tid, NULL);
    pthread_join(monitor_thread_tid, NULL);
    pthread_join(disk_service_thread_tid, NULL);
    pthread_join(camera_service_thread_tid, NULL);
    */
    
}
