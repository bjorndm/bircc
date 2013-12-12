#ifndef chic_inli_H_INCLUDED
#define chic_inli_H_INCLUDED

/**
* ChicInli is an intrusive doubly linked list. 
* You will notice it has no reference to it's element,
* because it should be used like this: 
* struct Data_ { ... } 
* struct DataNode_ ( struct Data_ self; ChicChicInli list; } 
* Then use 
* CHIC_INLI_DATA(&list, DataNode, list)   to fetch the data node;
*/

typedef struct ChicInli_ ChicInli;

struct ChicInli_ {
  ChicInli * next;
  ChicInli * prev;
};

/** Returns the list node for this list element is part of, 
for the give list element, and data type*/
#define CHIC_INLI_DATA(LIST, TYPE, MEMBER)  \
       chic_inli_data(LIST, offsetof(TYPE, MEMBER))  

/** Shorthand for INLI_DATA(LIST, DATA, list) */
#define CHOC_INLI_LISTDATA(LIST, TYPE) INLI_DATA(LIST, TYPE, list)


ChicInli * chic_inli_initall (ChicInli * self , ChicInli * next , ChicInli * prev );

ChicInli * chic_inli_init (ChicInli * self );

ChicInli * chic_inli_remove (ChicInli * self );

ChicInli * chic_inli_add (ChicInli * self , ChicInli * other );

ChicInli * chic_inli_next (ChicInli * self );

ChicInli * chic_inli_prev (ChicInli * self );

ChicInli * chic_inli_first (ChicInli * self );

ChicInli * chic_inli_last (ChicInli * self );

ChicInli * chic_inli_push (ChicInli * self , ChicInli * other );

ChicInli * chic_inli_unshift (ChicInli * self , ChicInli * other );

ChicInli * chic_inli_shift (ChicInli * self );

ChicInli * chic_inli_pop (ChicInli * self );

void * chic_inli_data (ChicInli * self , int offset );

#ifndef CHIC_INLI_NO_ABBREVIATION

#define INLI ChicInli
#define INLI_INITALL(ME, NEXT, PREV) chic_inli_initall((ME), (NEXT), (PREV))
#define INLI_INITL(ME)          chic_inli_init((ME))
#define INLI_REMOVE(ME)         chic_inli_remove((ME))
#define INLI_ADD(ME, OTHER)     chic_inli_add((ME), (OTHER))
#define INLI_NEXT(ME)           chic_inli_next((ME))
#define INLI_PREV(ME)           chic_inli_prev((ME))
#define INLI_PUSH(ME, OTHER)    chic_inli_push((ME)), (OTHER)
#define INLI_UNSHIFT(ME, OTHER) chic_inli_unshift((ME), (OTHER))
#define INLI_SHIFT(ME)          chic_inli_shift(ME)
#define INLI_POP(ME)            chic_inli_pop(ME)
#define INLI_DATA(LIST, TYPE, MEMBER)   CHIC_INLI_DATA((LIST), (TYPE), (MEMBER))

#endif 


#endif




