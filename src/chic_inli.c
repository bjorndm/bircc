
#include "inli.h"
#include <stdlib.h>

/*
  Title: Intrusive Linked Lists
*/


/**
* Struct: ChicInli
*
* Intrusive linked lists.
*/

/**
* Function: chic_inli_initall
*
* Fully initializes a non-NULL intrusive linked list.
*
* Parameters:
*   self - ChicInli to initialize
*   next - Next ChicInli list node to link to or NULL
*   prev - Previous ChicInli list node to link to or NULL
*
* Returns: 
*   self
*/
ChicInli * chic_inli_initall(ChicInli * self, ChicInli * next, ChicInli * prev) {
  if (!self) return NULL;
  self->next = next;
  self->prev = prev;
  return self;
}

/**
* Function: chic_inli_init
*
* Initializes the intrusive linked list. Next and prev are set to NULL.
*
* Parameters:
*   self - ChicInli to initialize
*
* Returns: 
*   self
*/
ChicInli * chic_inli_init(ChicInli * self) {
  return chic_inli_initall(self, NULL, NULL);
}

/** 
* Function: chic_inli_remove
*
* Removes self from the list it is part of. 
* Does NOT clean up any data asssociated with the container of the intrusive 
* list and also doesn't free self, since self should be part of the container 
* of the intrusive list. 
*
* Parameters:
*   self - ChicInli to remove from whole of list
*
* Returns: 
*   self
*/
ChicInli * chic_inli_remove(ChicInli * self) {  
  if(!self) return NULL;
  if(self->prev) { self->prev->next = self->next; }
  if(self->next) { self->next->prev = self->prev; }
  self->prev = NULL;
  self->next = NULL;
  return self;
}

/** 
* Function: chic_inli_add
*
* Appends other after self. 
*
* Parameters:
*   self - ChicInli to append to
*   other - ChicInli to append to self.
*
* Returns:
*   other if all went OK, NULL on error
*/
ChicInli * chic_inli_add(ChicInli * self, ChicInli * other) {  
  if(!self || !other) return NULL;
  self->next  = other;
  other->prev = self;
  return other;
}

/** 
* Function: chic_inli_next
*
* Returns the next element in the list
*
* Parameters:
*   self - ChicInli
*
* Returns: 
*   the next element in the list, or NULL if no next item. 
*/
ChicInli * chic_inli_next(ChicInli * self) {
  if(!self) return NULL;
  return self->next;
}

/** 
* Function: chic_inli_prev
*
* Returns the previous element in the list
*
* Parameters:
*   self - ChicInli
*
* Returns: 
*   the next element in the list, or NULL if no next item. 
*/
ChicInli * chic_inli_prev(ChicInli * self) {
  if(!self) return NULL;
  return self->prev;
}

/** 
* Function: chic_inli_first
*
* Returns the first element in the list, by dumb iteration.
*
* Parameters:
*   self - ChicInli
*
* Returns: 
*   the first link in the list, or NULL if self is NULL. 
*/
ChicInli * chic_inli_first(ChicInli * self) {
  ChicInli * aid = self; 
  if(!aid) return NULL;
  while (aid->prev) {
    aid = aid->prev;
  }
  return aid;  
}

/** 
* Function: chic_inli_last
*
* Returns the last element in the list, by dumb iteration.
*
* Parameters:
*   self - ChicInli
*
* Returns: 
*   the last link in the list, or NULL if self is NULL. 
*/
ChicInli * chic_inli_last(ChicInli * self) {
  ChicInli * aid = self; 
  if(!aid) return NULL;
  while (aid->next) {
    aid = aid->next;
  }
  return aid;  
}

/** 
* Function: chic_inli_push
*
* Appends other to the end of the list by dumb iteration.
*
* Parameters:
*   self - ChicInli
*
* Returns: 
*   other, or NULL if self or other is NULL. 
*/
ChicInli * chic_inli_push(ChicInli * self, ChicInli * other) {  
  ChicInli * aid;
  aid = chic_inli_last(self);
  return chic_inli_add(aid, other);
}


/** 
* Function: chic_inli_unshift 
*                
* Prepends other to the beginning of the list by dumb iteration.
*
* Parameters:
*   self - ChicInli
*
* Returns: 
*   other, or NULL if self or other is NULL. 
*/
ChicInli * chic_inli_unshift(ChicInli * self, ChicInli * other) {  
  ChicInli * aid;
  aid = chic_inli_first(self);
  return chic_inli_add(other, self);
}


/** 
* Function: chic_inli_shift
*
* Removes the first element from the list by dumb iteration.
*
* Parameters:
*   self - ChicInli
*
* Returns: 
*   list node that was removed, or NULL if self is NULL. 
*/
ChicInli * chic_inli_shift(ChicInli * self) {  
  ChicInli * aid;
  aid = chic_inli_first(self);
  return chic_inli_remove(aid);
}


/** 
* Function: chic_inli_pop
*
* Removes the last element from the list by dumb iteration.
*
* Parameters:
*   self - ChicInli
*
* Returns: 
*   list node that was removed, or NULL if self is NULL. 
*/
ChicInli * chic_inli_pop(ChicInli * self) {  
  ChicInli * aid;
  aid = chic_inli_last(self);
  return chic_inli_remove(aid);
}


/** 
* Function: chic_inli_data
* 
* Parameters:
* self - list node to get the data of.
* offset - Use offsetof to calculate this offset.
*
* Returns:  
*   the data for this node of the singly linked list. 
* 
*/
void * chic_inli_data(ChicInli * self, int offset) {
  if(!self) return NULL;
  return (void *)((char *)self - offset);
}




