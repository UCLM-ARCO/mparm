#include "simctrl.h"

extern "C" {
    unsigned loops_per_jiffy = 500000000;

    void udelay(int);
    void free(void*);
    void sleep(int);
    char* strcat(char* dst, const char* src);
    void sprintf(char* buf, char* fmt, ...);
    int strlen(const char* dst);

    void __gxx_personality_sj0() {}
    void _Unwind_SjLj_Unregister() {}
    void _Unwind_SjLj_Resume() {}
    void _Unwind_SjLj_Register() {}
    void memcpy(void* dest, void* orig, int len);
    void memset(void* dest, int c, int len);

    void rtems_gxx_once(void *mutex) {}
    void rtems_gxx_mutex_init(void *mutex) {}
    int  rtems_gxx_mutex_lock(void *mutex) { return 1; }
    int  rtems_gxx_mutex_trylock(void *mutex) { return 1; }
    int  rtems_gxx_mutex_unlock(void *mutex) { return 1; }
    void rtems_gxx_recursive_mutex_init(void *mutex) {}
    int rtems_gxx_recursive_mutex_lock(void *mutex) { return 1; }
    int rtems_gxx_recursive_mutex_trylock(void *mutex) { return 1; }
    int rtems_gxx_recursive_mutex_unlock(void* mutex) { return 1; }
    int rtems_gxx_setspecific(int key, void *pointer) { return 0; }
    void* rtems_gxx_getspecific(int key) { return 0; }
    int rtems_gxx_key_create(void *key, void *) { return 0; }
    int rtems_gxx_key_delete(int key) { return 0; }

    int __cxa_guard_acquire (void *g) { return 1; }
    void __cxa_guard_release (void *g) { }
    void __cxa_allocate_exception () { }
    void __cxa_throw () { }
}

int
strlen(const char* dst)
{
    int i=0;
    while (dst[i++] != '\0');
    return i;
}

char*
strcat(char* dst, const char* src)
{
    const char* p = src;
    char* q = dst;

    while(*q != '\0') ++q;
    while (*p != '\0')
	*q++ = *p++;
    *q='\0';
    return dst;
}

void
sleep(int i)
{
    udelay(1);
}

void
sprintf(char* buf, char* fmt, ...)
{
    *buf='\0';
}

void
abort()
{
    for(;;);
}

void
memset(void* dest, int c, int len)
{
}

static char* heap = reinterpret_cast<char*>(0x200000);

void*
malloc (unsigned long sz)
{
    void* ret = heap;
    heap += sz;
    return ret;
}

void
free(void*)
{
}

void*
operator new (unsigned long sz)
{
    return malloc(sz);
}

void
operator delete (void*)
{
}
