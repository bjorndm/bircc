#ifndef BIRCC_CPP_H_INCLUDED
#define BIRCC_CPP_H_INCLUDED

typedef struct bircc_sourcefile_struct bircc_sourcefile;


bircc_sourcefile * bircc_sourcefile_read(const char * filename);
bircc_sourcefile * bircc_sourcefile_free(bircc_sourcefile * me);
int bircc_sourcefile_expand_trigraphs(bircc_sourcefile * me);
int bircc_sourcefile_debugprint(bircc_sourcefile * me);

typedef struct bircc_pp_token_struct bircc_pp_token;
typedef struct bircc_pp_tokenlist_struct bircc_pp_tokenlist;

bircc_pp_tokenlist * birc_sourcefile_tokenize(bircc_sourcefile * me);







#endif