
#ifndef BIRCC_LEXER_H_INCLUDED
#define BIRCC_LEXER_H_INCLUDED

typedef struct bircc_lexer_value_struct bircc_lexer_value;
typedef struct bircc_lexer_interface_struct bircc_lexer_interface;
typedef struct bircc_lexer_source_struct bircc_lexer_source;
typedef struct bircc_lexer_token_struct bircc_lexer_token;
typedef struct bircc_lexer_tokenlist_struct bircc_lexer_tokenlist;


typedef int bircc_lexer_destroy_func(bircc_lexer_value * me);
typedef int bircc_lexer_lex_func(bircc_lexer_value * me, bircc_lexer_token * token);


/* Generic interface (COM-like) */
typedef void * bircc_interface_release_func(void * me);
typedef void * bircc_interface_retain_func(void * me);
typedef void * bircc_interface_create_func(void * me, ...);

typedef struct bircc_core_interface_struct bircc_core_interface;
typedef struct bircc_factory_interface_struct bircc_factory_interface;

/* core interface for memory management. */
struct bircc_core_interface_struct {
  bircc_interface_release_func * release;
  bircc_interface_retain_func  * retain;
};

/* Factory interface for object creation. */
struct bircc_factory_interface_struct {
  bircc_core_interface core;
  bircc_interface_create_func  * create;

};



/* Interface of a lexer. */
struct bircc_lexer_interface_struct {
  bircc_factory_interface      * factory; /* Composed interface. */
  bircc_lexer_lex_func         * lex;  
   
};




#endif

