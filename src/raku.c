
#include "raku.h"
#include <stdlib.h>
#include <string.h>
#include "bstrlib.h"
#include "bstraux.h"

/* Type info */
struct RaKindInfo_ {
  int                 kind;
  const char        * name;
  RaGenericFunction * destructor;
};

/* Slabs for the slab allocator. */
#define RA_SLAB_SIZE 1024

typedef struct RaSlab_ RaSlab;


struct RaSlab_ {
  struct RaSlab_ * next;
  int used;
  RaCell cells[RA_SLAB_SIZE];
};


/* The runtime of Raku as an abstract type */
struct Raku_  {
  /* Allocated cells of the slab allocator. */
  struct RaSlab_ * allocated_cells;
  /* Currently active slab allocator. */
  struct RaSlab_ * current_slab;

  /* List of cells that are currently unused. */
  RaCell * unused; 
  RaCell * root;
  RaCell * mark;
  RaCell * sweep;
};


/* Generic array handling functions: */
void * ra_array_alloc(int nelem, int elsize) {
  return calloc(nelem, elsize);
};

void * ra_array_get(const void * array, int nelem, int elsize, int index) {
  char * data = (char *) array;
  if(!array)            return NULL;
  if (index > nelem)    return NULL;
  return data + (elsize * index);
};


void * ra_array_put(void * array, int nelem, int elsize, int index, void * elem) {
  char * data = (char *) array;
  void * aid  = NULL;
  if(!array)            return NULL;
  if (index > nelem)    return NULL;
  aid         =  data + (elsize * index);
  memmove(aid, elem, elsize);
  return aid;
};


void * ra_array_growcopy(const void * array, int nelem, int elsize, int newnelem) {
  int index, tocopy;
  void * result = ra_array_alloc(newnelem, elsize);
  if (!result)    return result;
  tocopy        = nelem;
  if (newnelem < tocopy) tocopy = newnelem;
  if (array) { 
    memcpy(result, array, tocopy * elsize);
  }
  return result;
}

void * ra_array_walk(void * array, int nelem, int elsize, ra_array_walker * walker, void * extra) {
  int index = 0;
  for(index = 0; index < nelem; index ++) {
    void * res  = NULL;
    void * elem = ra_array_get(array, nelem, elsize, index);
    if(walker) {  res = walker(array, index, elem, extra); }
    if (res) return res;
  }
  return NULL;
}

void ra_array_through(void * array, int nelem, int elsize, int start, ra_array_eacher * each) {
  int index;
  for(index = start; index < nelem; index ++) {
    void * elem = ra_array_get(array, nelem, elsize, index);
    if(elem) each(elem);
  }
}

void ra_array_each(void * array, int nelem, int elsize, ra_array_eacher * each) {
  ra_array_through(array, nelem, elsize, 0 , each);
}


void * ra_array_free(void * array) {
  free(array);
  return NULL;
}

/* Appends other after self and returns the new list head. 
 * If self is NULL, returns other. */
RaCell * racell_append(RaCell * self, RaCell * other) {
  if (!self) return other;
  if (!other) return self;
  self->next            = other;
  other->previous       = self;
  return self;
}

/* Compares cells for equality */
int racell_compare(RaCell * self, RaCell * other) {
  if (!self) { if(!other) return 0; else return -1; }
  if (other) return 1;
  if (self->kind != other->kind) return self->kind - other->kind;
  switch (self->kind) {
    case RA_NIL: return 0;
    case RA_INTEGER: return 1;
    default: 
      return 0;
  }
}


/* Calls the destructor of a data cell. Does nothing if it's not a data cell. */
RaCell * racell_free_data(RaCell * cell) {
  RaData data;
  if (!(cell->kind & RA_DATA))  return cell;
  data = cell->value.data;
  if(!data.interface)           return NULL;
  if(!data.interface->fini)     return NULL;
  data.interface->fini(data.pointer);
  data.pointer = NULL;
  return cell;
}

/* Deallocate, puts the cell back on the unused list. Calls the 
 * destructor of the type as well if needed.
 */
RaCell * racell_free(Raku * self, RaCell * cell) {
  RaCell * aid;
  if (!self) return NULL;
  if (!cell) return NULL;
  // call data destructor.
  racell_free_data(cell);
  aid = racell_append(cell, self->unused);
  self->unused = aid;
  return NULL;
}


#define RAKU_ALLOC_SLAB_SIZE 1024


/* Marks a raku cell as used, so the GC won't collect it. 
 */
RaCell * racell_mark(RaCell * self) {
  if(!self) return NULL;
  self->flags = self->flags & RA_GC_MARK;
  return self;
}


/* Marks a raku cell as used, so the GC won't collect it. 
 * Also recursively marks, recursively, any linked next or child nodes. 
 */
RaCell * racell_mark_recursive(RaCell * self) {
  RaCell * res = racell_mark(self);
  racell_mark_recursive(self->child);
  racell_mark_recursive(self->next);
  return self;
}


/* Initializes a cell to nil value. */
RaCell * racell_initnil(RaCell * self) {
  if (!self) return NULL;
  self->kind  = RA_NIL;
  self->flags = 0;
  self->next  = self->previous = self->child = self->parent = NULL;
  self->value.data.pointer     = NULL;
  return self;
}

/* Links a cell to a next and child node. Either or both may be NULL to unlink. */
RaCell * racell_link(RaCell * self, RaCell * next, RaCell * child) {
  if (!self)         return NULL;
  self->next            = next;
  if (next) { 
    next->previous      = self;
  }
  self->child           = child;
  if (child) {
    child->parent       = self;
  } 
}

/* Initializes a cell as a tree node. */
RaCell * 
racell_init_treenode(RaCell * self, RaCell * next, RaCell * child) {
  if (!self)         return NULL;
  self->kind            = RA_TREENODE;
  self->flags           = 0;
  self->value.data.pointer     = NULL;
  racell_link(self, next, child);
  return self;
}

static void ra_bstring_fini(void * ptr) {
  bstring bstr = ptr;
  bdestroy(bstr);
}

static void * ra_bstring_copy(void * ptr) {
  bstring bstr = ptr;
  return bstrcpy(bstr);
}


static const RaDataInterface ra_bstring_interface = {
  ra_bstring_fini,
  ra_bstring_copy
};


/* Initializes a cell to contain data that will be handled using the interface 
 * functions. Returns self, or NULL if data was null. */
RaCell * 
racell_init_data
(RaCell * self, int kind, void * data, const RaDataInterface * ifa) {
  racell_initnil(self);
  self->kind                    = kind;
  self->value.data.pointer      = data;
  self->value.data.interface    = ifa;
  if(!data) return NULL;  
  return self;
}


/* Initializes a cell to a string value. */
RaCell * 
racell_init_bstring
(RaCell * self, bstring bstr) {
  return racell_init_data(self, RA_STRING, bstr, &ra_bstring_interface);
}

/* Initializes a cell to a string value. */
RaCell * 
racell_initcstr(RaCell * self, char * cstr) {
  bstring bstr;
  bstr = bfromcstr(cstr);
  return racell_init_bstring(self, bstr);
}


/* Finds a value in an unordered RaCell list  */
RaCell * racell_find(RaCell * root, RaCell * key) {
  return NULL;
}


/* Frees a slab of memory. All active cells are destroyed as well. */
void raslab_free(RaSlab * self) {
  int index;
  for(index = 0; index < RA_SLAB_SIZE; index++) {
    racell_free_data(self->cells + index);
  }
  free(self);
}

/* Allocates and initializes a new slab of memory. All cells are initializes to NIL. */
struct RaSlab_ * raslab_new() {
  int index;
  struct RaSlab_ * self;
  self = calloc(1, sizeof(struct RaSlab_));
  if(!self) return NULL;
  self->next = NULL;
  self->used = 0;
  for(index = 0; index < RA_SLAB_SIZE; index++) {
    racell_initnil(self->cells + index);
  }
  return self;
}


/* Slab allocator. */
RaCell * raku_slab_alloc(Raku * self) {
  RaCell * result; 
  if (!self->allocated_cells) { 
    self->allocated_cells    = raslab_new();
  }
  if (!self->allocated_cells) return NULL;
  if(!self->current_slab) { 
    self->current_slab       = self->allocated_cells;
  }
  if (self->current_slab->used >= RA_SLAB_SIZE) {
    struct RaSlab_ * newslab;
    newslab = raslab_new();
    if (!newslab) return NULL;
    self->current_slab->next = newslab;
    self->current_slab       = newslab;
  }
  result                     = self->current_slab->cells + self->current_slab->used;
  self->current_slab->used++;
  return result;
}



Raku * raku_init(Raku * self) {
  if (!self) return NULL;
  self->allocated_cells = NULL;
  self->current_slab    = NULL;
  self->unused = NULL;
  self->root   = NULL;
  return self;
}


Raku * raku_done(Raku * self) {
  RaSlab * aid, * next;
  if (!self) return NULL;
  for(aid = self->allocated_cells; aid; aid = next) {
    next = aid->next;
    raslab_free(aid);
  }
  self->allocated_cells = NULL;
  self->current_slab    = NULL;
  self->unused = NULL;
  self->root   = NULL;
  return self;
}

/*  */
Raku * raku_new() {
  Raku * self;
  self = malloc(sizeof(*self));
  return raku_init(self);
}

/*  */
Raku * raku_free(Raku * self) {
  raku_done(self);
  free(self);
  return NULL;
}


/* Slab allocator for cells with "unused list". */
RaCell * racell_alloc(Raku * self) {
  RaCell * result;
  if (!self) return NULL;
  if (!self->unused) {
    return raku_slab_alloc(self);
  }
  result                 = self->unused;
  self->unused           = result->next;
  if (self->unused) { 
    self->unused->previous = NULL;
  }
  return racell_initnil(result);
}




/* Runs the garbage collector's mark phase, doing at most n steps. 
 Starts from RaCell, and returns the RaCell last marked.
 Continues from previous mark. */
RaCell * raku_gc_mark_n(Raku * raku, int stop) {
  int amount = 0;
  RaCell * child, * now, *next;
  if(!raku->mark)  { 
    raku->mark = raku->root;
    racell_mark(raku->mark);
  }
  now   = raku->mark;
  next  = now->next; 
  /* breadth-first traversal */
  while (next) {
    racell_mark(next);
    
    
    next = now->next;
    amount++;
  }
  /* Now mark the next cells */
  /* Finaly go up, and take the next one, */
  
  
}

/* Runs the garbage collector's mark phase fully. */
void raku_gc_mark(Raku * raku) {
  if ((!raku) || (!raku->root)) return;
  racell_mark_recursive(raku->root);
}

/* Runs the garbage collector's sweep phase fully.  */
void raku_gc_sweep(Raku * raku) {
  RaCell * child;
  int index;
  RaSlab * slab = raku->allocated_cells;
  while (slab) {
    for(index = 0; index < RA_SLAB_SIZE; index ++) {
      RaCell * aid = slab->cells + index;
      // put unmarked entries in the unused list after freeing them. racel_free
      // handles this already
      if ((aid->flags & RA_GC_MARK) != RA_GC_MARK) {
        racell_free(raku, aid);
      }      
    } 
    slab = slab->next;
  }
}

/* Runs the garbage collector's mark and sweep.  */
void raku_gc_mark_and_sweep(Raku * raku) {
  raku_gc_mark(raku);
  raku_gc_sweep(raku);
}



