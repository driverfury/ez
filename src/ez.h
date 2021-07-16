#ifndef EZ_H
#define EZ_H

/******************************************************************************/
/**                                  GENERAL                                 **/
/******************************************************************************/

#define ez_min(x, y) (((x)<(y))?(x):(y))
#define ez_max(x, y) (((x)>(y))?(x):(y))

/******************************************************************************/
/**                                  MEMORY                                  **/
/******************************************************************************/

void  ez_mem_copy(void *src, void *dest, unsigned long bytes);
void *ez_mem_alloc(unsigned long size);
void  ez_mem_free(void *ptr);
void *ez_mem_realloc(void *ptr, unsigned long size);

/******************************************************************************/
/**                                 STRINGS                                  **/
/******************************************************************************/

unsigned long ez_str_len(char *s);
unsigned long ez_str_len_max(char *s, unsigned long max);
void          ez_str_copy(char *src, char *dest);
void          ez_str_copy_max(char *src, char *dest, unsigned long max);

/******************************************************************************/
/**                                I/O & ERR                                 **/
/******************************************************************************/

void ez_out_print(char *s);
void ez_out_println(char *s);

#endif


/******************************************************************************/
/**                              IMPLEMENTATION                              **/
/******************************************************************************/

#ifdef EZ_IMPLEMENTATION

#ifndef EZ_NO_CRT_LIB

/* TODO: C std lib imlementation */
#error "Not implemented yet with C standard library"

#elif defined(WIN32) || defined(_WIN32) ||\
      defined(__WIN32__) || defined(__NT__)

/* Linker stuff to not complaining about not using C runtime library */
int _fltused = 0;

#include <windows.h>

void
ez_mem_copy(void *src, void *dest, unsigned long bytes)
{
    unsigned int i;

    char *srcp  = (char *)src;
    char *destp = (char *)dest;

    for(i = 0;
        i < bytes;
        ++i)
    {
        *destp++ = *srcp++;
    }
}

typedef struct
{
    unsigned long size;
} ez_allochdr;

#define ez_get_allochdr(ptr)\
    ((ez_allochdr *)((char *)ptr - sizeof(ez_allochdr)))

void *
ez_mem_alloc(unsigned long size)
{
    ez_allochdr *hdr;
    void *ptr = 0;
    hdr = (ez_allochdr *)VirtualAlloc(
        0, size+sizeof(ez_allochdr),
        MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    if(hdr)
    {
        hdr->size = size;
        ptr = (void *)((char *)hdr + sizeof(ez_allochdr));
    }
    return(ptr);
}

void
ez_mem_free(void *ptr)
{
    void *real_ptr;
    if(ptr)
    {
        real_ptr = ez_get_allochdr(ptr);
        VirtualFree(real_ptr, 0, MEM_RELEASE|MEM_DECOMMIT);
    }
}

void *
ez_mem_realloc(void *ptr, unsigned long size)
{
    void *newptr = 0;
    ez_allochdr *oldhdr;
    unsigned long min_size;
    if(ptr)
    {
        if(size == 0)
        {
            ez_mem_free(ptr);
        }
        else
        {
            newptr = ez_mem_alloc(size);
            oldhdr = ez_get_allochdr(ptr);
            min_size = ez_min(size, oldhdr->size);
            ez_mem_copy(ptr, newptr, min_size);
        }
    }
    return(newptr);
}

unsigned long
ez_str_len(char *s)
{
    unsigned long len = 0;
    while(*s++)
    {
        ++len;
    }
    return(len);
}

unsigned long
ez_str_len_max(char *s, unsigned long max)
{
    unsigned long len = 0;
    while(*s++ && len < max)
    {
        ++len;
    }
    return(len);
}

void
ez_str_copy(char *src, char *dest)
{
    while(*src)
    {
        *dest++ = *src++;
    }
}

void
ez_str_copy_max(char *src, char *dest, unsigned long max)
{
    unsigned long i = 0;
    while(*src && i < max)
    {
        *dest++ = *src++;
    }
}

HANDLE *_ez_stdout;
HANDLE *_ez_stdout;
HANDLE *_ez_stdout;

void
ez_out_print(char *s)
{
    if(!_ez_stdout)
    {
        _ez_stdout = GetStdHandle(STD_OUTPUT_HANDLE);
    }

    WriteFile(_ez_stdout, s, ez_str_len(s), 0, 0);
}

void
ez_out_println(char *s)
{
    ez_out_print(s);
    ez_out_print("\n");
}

#else

#error "Unsupported platform"

#endif

#endif
