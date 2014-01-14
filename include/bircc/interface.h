#ifndef BIRCC_INTERFACE_H_INCLUDED
#define BIRCC_INTERFACE_H_INCLUDED


/* Structs to help implement a basic interface pattern.
Not sure if it will be used. */

/* Generic interface (COM-like) */
typedef int bircc_on_get_error(void * me);
typedef int bircc_on_set_error(void * me, int error);

/** Basic interface is reference counting. */
struct bircc_refcounter;

/** Generic reference counting callbacks. */
typedef void * bircc_on_release(struct bircc_refcounter * ref);
typedef void * bircc_on_retain(struct bircc_refcounter * ref);
/** refcounter can get pointer to the refcounted object. */
typedef void * bircc_on_get_me(struct bircc_refcounter * ref);

/** Next up is a creator that can create objects as well as refcount them. */
struct bircc_creator;

/** Generic creation callback. */
typedef void * bircc_on_create(bircc_creator * creator, ...);

/* Now define bircc_refcounter... */
struct bircc_refcounter {
  bircc_on_release  * release;
  bircc_on_retain   * retain;
  bircc_on_get_me   * get_me;
};

/* actions */
/* And a concrete implementation. */
struct bircc_refcount {
  void                    * me;
  struct bircc_refcounter * acts;
};

/* Factory interface for object creation. */
struct bircc_creator {
  struct bircc_refcounter * refcounter;
  bircc_on_create         * create;
};








#endif

