#ifndef EZ_H
#define EZ_H

/******************************************************************************/
/**                                  TYPES                                   **/
/******************************************************************************/

#include <inttypes.h>
#include <stddef.h>

/******************************************************************************/
/**                                  GENERAL                                 **/
/******************************************************************************/

#define ez_min(x, y) (((x)<(y))?(x):(y))
#define ez_max(x, y) (((x)>(y))?(x):(y))

#ifndef NDEBUG
#define ez_assert(cond)\
    if(!(cond))\
    {\
        ez_abort();\
    }
#else
#define ez_assert(cond) ((void *)0)
#endif

void ez_abort(void);

/******************************************************************************/
/**                                  MEMORY                                  **/
/******************************************************************************/

void  ez_mem_copy(void *src, void *dest, size_t bytes);
void *ez_mem_alloc(size_t size);
void  ez_mem_free(void *ptr);
void *ez_mem_realloc(void *ptr, size_t size);

/******************************************************************************/
/**                                 STRINGS                                  **/
/******************************************************************************/

size_t ez_str_len(char *s);
size_t ez_str_len_max(char *s, size_t max);
void   ez_str_copy(char *src, char *dest);
void   ez_str_copy_max(char *src, char *dest, size_t max);

/******************************************************************************/
/**                              STD I/O & ERR                               **/
/******************************************************************************/

void ez_out_print(char *s);
void ez_out_println(char *s);

/******************************************************************************/
/**                                 FILE I/O                                 **/
/******************************************************************************/

char  *ez_file_read_text(char *pathname, size_t *size);
void  *ez_file_read_bin(char *pathname, size_t *size);
void   ez_file_free(void *file_content);

#define ez_file_read ez_file_read_text

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
ez_abort(void)
{
    ExitProcess(1);
}

void
ez_mem_copy(void *src, void *dest, size_t bytes)
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
    size_t size;
} ez_allochdr;

#define ez_get_allochdr(ptr)\
    ((ez_allochdr *)((char *)ptr - sizeof(ez_allochdr)))

void *
ez_mem_alloc(size_t size)
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
ez_mem_realloc(void *ptr, size_t size)
{
    void *newptr = 0;
    ez_allochdr *oldhdr;
    size_t min_size;
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

size_t
ez_str_len(char *s)
{
    size_t len = 0;
    while(*s++)
    {
        ++len;
    }
    return(len);
}

size_t
ez_str_len_max(char *s, size_t max)
{
    size_t len = 0;
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
ez_str_copy_max(char *src, char *dest, size_t max)
{
    size_t i = 0;
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

    WriteFile(_ez_stdout, s, (DWORD)ez_str_len(s), 0, 0);
}

void
ez_out_println(char *s)
{
    ez_out_print(s);
    ez_out_print("\n");
}

char *
ez_file_read_text(char *pathname, size_t *size)
{
    /* TODO */
    char *content = 0;
    char *content_ptr = 0;
    HANDLE file_handle;
    LARGE_INTEGER large_int;
    size_t bytes_to_read;
    uint32_t to_read, read;
    int error_occurred;

    *size = 0;

    file_handle = CreateFileA(
        pathname, GENERIC_READ, 0, 0, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL, 0);
    if(file_handle == INVALID_HANDLE_VALUE)
    {
        return(content);
    }

    if(GetFileSizeEx(file_handle, &large_int))
    {
        *size = large_int.QuadPart;

        content = (char *)ez_mem_alloc(*size);
        if(content)
        {
            bytes_to_read = *size;
            error_occurred = 0;
            content_ptr = content;
            do
            {
                if(bytes_to_read <= UINT32_MAX)
                {
                    /* TODO: */
                    to_read = (uint32_t)bytes_to_read;
                }
                else
                {
                    to_read = UINT32_MAX;
                }
                if(ReadFile(
                    file_handle, content_ptr,
                    to_read, (LPDWORD)&read, 0))
                {
                    bytes_to_read -= read;
                    content_ptr += read;
                }
                else
                {
                    error_occurred = 1;
                    ez_mem_free(content);
                    content = 0;
                    *size = 0;
                }
            } while((bytes_to_read > 0) && !(error_occurred));
        }

        CloseHandle(file_handle);
    }

    return(content);
}

void *
ez_file_read_bin(char *pathname, size_t *size)
{
    void *content = 0;
    content = (void *)ez_file_read_text(pathname, size);
    return(content);
}

void
ez_file_free(void *file_content)
{
    ez_mem_free(file_content);
}

#else

#error "Unsupported platform"

#endif

#endif
