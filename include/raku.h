#ifndef RAKU_H_INCLUDED
#define RAKU_H_INCLUDED


#ifndef FALSE 
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE (!(FALSE))
#endif

#include "bstrlib.h"

/* Generic function pointer type, since the C standards do not allow void * pointers 
 * to store generic function pointers, only generic data pointers. 
 */
typedef void   * RaGenericFunction(void *); 

/* Predefine the unions and structs that we'll use and typedef them. */
typedef union  RaValue_         RaValue;
typedef struct RaCell_          RaCell;
typedef struct RaData_          RaData;
typedef struct RaParser_        RaParser;
typedef struct RaDataInterface_ RaDataInterface;

typedef void   RaFinalizer(void *);
typedef void * RaCopier(void *);

/* Interface that contains the needed function pointers to free, copy, 
 etc a wrapped RaData pointer. */
struct RaDataInterface_ {
  RaFinalizer   * fini;
  RaCopier      * copy;
};

/* Generic data type to wrap pointers with. */
struct RaData_  {
  void                  * pointer;
  const RaDataInterface * interface;
};


/* A simple value union that can contain numbers, pointers, and function pointers. */
union RaValue_ {
  int                     integer;
  double                  floating;
  RaData                  data;
  RaGenericFunction     * function;
};

/* Types of a RaCell */
enum RaCellKind_ {
  RA_NIL        = 0,
  RA_OK         = 1,
  RA_INTEGER    = 2,
  RA_FLOATING   = 3,
  RA_FUNCTION   = 4,
  RA_TREENODE   = 5,
  RA_DATA       = 128,
  RA_FAIL       = RA_DATA | 1,
  RA_STRING     = RA_DATA | 2,
  RA_TOKEN      = RA_DATA | 3,
  RA_USER       = RA_DATA | 4,
};

/* Flags for a RaCell */
enum RaCellFlags_ {
  RA_GC_MARK            = 1 << 1,
  RA_GC_CHILDMARKED     = 1 << 2,
  RA_GC_GARBAGE         = 1 << 3,
  RA_GC_LOCKED          = 1 << 4,
};

/* User type construction macro */
#define RA_USER_TYPE(A,B,C,D) \
        (((A)<<24) | ((B)<<16) | ((C)<<8) | (D))

/* The basic element of Raku is a lisp-like 
   Cell, which is however not a cons cell but a tree node.

   The value contains either a number, a function pointer or a data pointer.
   Kind is the type specifier of the cell.
   Flags contains flags for the garbage collector or the wrapping API.
   The pointers next, previous , parent and child point to the respective related 
   cells of this RaCell.
*/
struct RaCell_ {
  RaValue       value;
  int           kind;
  int           flags;
  RaCell        * next, * previous, * parent, * child;
};

/* Lexer tokens. */
typedef struct RaToken_ RaToken;

struct RaToken_ {
  bstring text;
  int kind;
  int line;
  int coln;
};

/* The runtime of Raku as an abstract type */
typedef struct Raku_ Raku;

RaCell * racell_alloc(Raku * raku);
RaCell * racell_free(Raku * raku, RaCell * cell);


void * ra_array_alloc(int nelem, int elsize);
void * ra_array_get(const void * array, int nelem, int elsize, int index);
void * ra_array_put(void * array, int nelem, int elsize, int index, void * elem);
void * ra_array_growcopy(const void * array, int nelem, int elsize, int newnelem);
typedef void * ra_array_walker(void * array, int index, void * value, void * extra);
void * ra_array_walk(void * array, int nelem, int elsize, ra_array_walker * walker, void * extra);
typedef void ra_array_eacher(void * value);
void ra_array_through(void * array, int nelem, int elsize, int start, ra_array_eacher * each);
void ra_array_each(void * array, int nelem, int elsize, ra_array_eacher * each);
void * ra_array_free(void * array);




Raku * raku_new(); 
Raku * raku_free(Raku * self);
Raku * raku_init(Raku * self); 
Raku * raku_done(Raku * self);
void raku_gc_mark_and_sweep(Raku * raku); 

RaCell *racell_alloc(Raku *self);
RaCell *racell_find(RaCell *root,RaCell *key);
RaCell *racell_init_bstring(RaCell *self,bstring bstr);
RaCell *racell_init_data(RaCell *self,int kind,void *data, const RaDataInterface *ifa);
RaCell *racell_init_treenode(RaCell *self,RaCell *next,RaCell *child);
RaCell *racell_link(RaCell *self,RaCell *next,RaCell *child);
RaCell *racell_initnil(RaCell *self);
RaCell *racell_mark(RaCell *self);
RaCell *racell_free(Raku *self,RaCell *cell);
RaCell *racell_free_data(RaCell *cell);
int racell_compare(RaCell *self, RaCell *other);
RaCell *racell_append(RaCell *self, RaCell *other);



#endif


