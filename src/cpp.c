
#include "bstrlib.h"

typedef struct RaToken_ RaToken;
typedef struct RaLexer_ RaLexer;




/*
comment
ob
cb
op
cp
oa
ca
eos
string
float
integer
symbol
*/

enum RaLexTokenKind {
  RA_LEX_STRING         = '"',
  RA_LEX_COMMENT        = '#',
  RA_LEX_OPEN_PAREN     = '(',
  RA_LEX_CLOSE_PAREN    = ')',
  RA_LEX_OPEN_BRACKET   = '[',
  RA_LEX_CLOSE_BRACKET  = ']',
  RA_LEX_OPEN_BRACE     = '{',
  RA_LEX_CLOSE_BRACE    = '}',
  RA_LEX_STATEMENT_END  = ';',
  RA_LEX_NUMBER         = '%',
  RA_LEX_INTEGER        = '&',
  RA_LEX_SYMBOL         = ':',
  RA_LEX_ERROR          = '!',
  RA_LEX_END            = '$',
};

enum RaLexState {
  RA_LEX_SINGLE_STRING   = '\'',
  RA_LEX_DOUBLE_STRING   = '"',
  RA_LEX_BACKTICK_STRING = '`',
  RA_LEX_ANY_NUMBER      = 'd',
  RA_LEX_FLOAT_NUMBER    = 'f',
  RA_LEX_SWALLOW         =  2,
  RA_LEX_CONTINUE        =  1,
  RA_LEX_DONE            =  0,
};


struct RaToken_ {
  int        kind;
  int        line;
  int        col;
  bstring    value;
};


/* Function type that gets a character of input from the input stream. */
typedef int RaLexerGetc(void * input);

struct RaLexer_ {
  RaToken         token;
  int             suspend;
  int             kind;
  int             state;
  int             line;
  int             col;
  int             ch;
  int             ch_prev;
  void          * input;
  bstring         buffer;
  RaLexerGetc   * get;
};

#include <stdlib.h>
#include <stdio.h>

/* Cleans up a token. */
RaToken * ratoken_done(RaToken * self) {
  if(!self) return NULL;
  bdestroy(self->value);
  return self;
}

/* Allocates a new token. */
RaToken * ratoken_init(RaToken * self, int kind, int line, int col, const char * value) {
  bstring baid;
  if (value) { 
    baid = bfromcstr(value);
  } else {
    baid = bfromcstr("");
  }
  if(!baid) return NULL;
  self->kind    = kind;
  self->line    = line;
  self->col     = col;
  self->value   = baid;

  return self;
}


/* Initialize lexer with given input and getc function. */
RaLexer * ralexer_init(RaLexer * self, void * input, RaLexerGetc * get) {
  if (!self) return NULL;
  self->buffer = bfromcstralloc(128, "");
  if(!self->buffer) {
                 return NULL;
  }
  self->kind   = -1;
  self->line   =  1;
  self->col    =  1;
  self->get    = get;
  self->input  = input;
  self->ch_prev= -1;
  self->ch     = -1;
  self->state  = -1;
  self->suspend = FALSE;
  return self;
}

/* Cleans up lexer. */
RaLexer * ralexer_done(RaLexer * self) {
  if (!self) return NULL;
  bdestroy(self->buffer);
  self->kind    = -1;
  self->line    =  1;
  self->col     =  1;
  self->get     = NULL;
  self->input   = NULL;
  self->ch_prev = -1;
  self->ch      = -1;
  self->state   = -1;
  self->suspend = FALSE;
  return self;
}

/* Wrapper for fgetc. */
int ralexer_fgetc(void * data) {
  return fgetc((FILE*) data);
}


/* Gets the next character, fills in ch and updates line and column. 
  If self->suspend is true, does not actually get the character, but returns 
  the last-gotten character in stead, and unsets suspend. 
 */
int ralexer_getnextchar(RaLexer * self) {
  int ch;
  
  if (self->suspend) {
    self->suspend = FALSE.
    return self->ch;
  }
  ch = self->get(self->data);
  self->column++;
  /* Newline, unless preceded by a cr gives us a new line. 
   *CR alone or followed by \n too. */
  if (((ch == '\n') && (self->ch_prev != '\r')) || (ch == '\r') ) {
    self->line++;
    self->column = 1;
  }
  self->ch_prev = self->ch;
  self->ch      = ch;
  return self->ch;
}

/* Appends cheracter ch to the lexer's buffer. */
void ralexer_append(RaLexer * lexer, int ch) {
  bformata(lexer->buffer, "%c", ch);
}

/* Initializes the new token from the current lexer state and. */
RaToken * ralexer_inittoken(RaLexer * self, RaToken * token, int kind) {
  token->line = self
  return ratoken_init(token, kind, self->line, self->col,  bdata(self->buffer));
}


typedef int LexerHelper(Lexer * lex, int ch, void * data);

/* Keeps on lexing until helper returns negative ,or if CH is 0. 
 * The returned value is appended to the  buffer as a character. */
int ralexer_get_until_cond(RaLexer * self, LexerHelper * helper, void * data) {
  int ch, newch;
  while (TRUE) { 
    ch     = lex_getnextchar(self);
    if(ch == EOF) {
      self->suspend = TRUE;
      return          TRUE;
    }
    newch  = helper(self, ch, data);
    if(newch < 0) {
      self->suspend = TRUE;
      return          TRUE;
    }
    ralexer_append(self, newch);
  }
}

int lex_until_ch_helper(Lexer * lex, int ch, void * data) {
  int chend = (*((int*)data));
  if (ch != chend) return ch;
  return -1;
}

int lex_until_in_helper(Lexer * lex, int ch, void * data) {
  char * strend = data;
  if (!strchr(strend, ch)) return ch;
  return -1;
}

int lex_until_not_in_helper(Lexer * lex, int ch, void * data) {
  char * strok = data;
  if (strchr(strok, ch)) return ch;
  return -1;
}

/* XXX: not too good this... */
int lex_until_escaped_ch_helper(Lexer * lex, int ch, void * data) {
  int chend = (*((int*)data));
  if (lex->ch_prev == '\\') return ch;
  if (ch != chend) return ch;
  return -1;
}



/* Keeps on lexing until chend is found, the characters are added to the buffer.  */
int ralexer_get_until_ch(RaLexer * self, int chend) {
  return ralexer_get_until_cond(self, lex_until_ch_helper, &chend);
}

/* Keeps on lexing until chend is found, the characters are added to the buffer. 
  a backslash \ is an escape for chend. */
int ralexer_get_until_escaped_ch(RaLexer * self, int chend) {
  return ralexer_get_until_cond(self, lex_until_escaped_ch_helper, &chend);
}

/* Keeps on lexing until any character in strend is found, the characters are added 
 *to the buffer.  */
int ralexer_get_until_in(RaLexer * self, const char * strend) {
  return ralexer_get_until_cond(self, lex_until_in_helper, strend);
}

/* Keeps on lexing until any character NOT in strpk is found, the characters are added 
 *to the buffer.  */
int ralexer_get_until_not_in(RaLexer * self, const char * strok) {
  return ralexer_get_until_cond(self, lex_until_not_in_helper, strend);
}


int ralexer_lex_word(RaLexer * self, RaToken * token, int ch) {
  ralexer_append(self, ch); 
  ralexer_get_until_in(self, " \t\r\n;(){}[]\#");
  lexer_inittoken(self, token, RA_LEX_WORD);
  lexer->suspend = TRUE;
  return RA_LEX_CONTINUE;
}


int ralexer_lex_string_with_escapes(RaLexer * self, RaToken * token, int ch) {
  ralexer_get_until_escaped_ch(self, ch);
  /// XXX: should process escapes here
  lexer_inittoken(self, token, RA_LEX_STRING);
  return RA_LEX_CONTINUE;
}

int ralexer_lex_string(RaLexer * self, RaToken * token, int ch) {
  ralexer_get_until_ch(self, ch);
  lexer_inittoken(self, token, RA_LEX_STRING);
  return RA_LEX_CONTINUE;
}

int ralexer_lex_comment_hash(RaLexer * self, RaToken * token, int ch) {
  ralexer_append(self, ch);
  ch = ralexer_getnextchar(self);
  if (ch == '{') {
    ralexer_get_until_ch(self, '}');
    ch = ralexer_getnextchar(self);
    ralexer_append(self, ch);
    lexer
  }
  ralexer_get_until_ch(self, '\n');
  return RA_LEX_CONTINUE;
}

int ralexer_lex_eof(RaLexer * self, RaToken * token, int ch) {
  
  return RA_LEX_DONE;
}

int ralexer_lex_number(RaLexer * self, RaToken * token, int ch) {
  return RA_LEX_CONTINUE;
}

int ralexer_lex_eos(RaLexer * self, RaToken * token, int ch) {
  return RA_LEX_CONTINUE;
}


int ralexer_lex_blockstart(RaLexer * self, RaToken * token, int ch) {
  return RA_LEX_CONTINUE;
}

int ralexer_lex_blockend(RaLexer * self, RaToken * token, int ch) {
  return RA_LEX_CONTINUE;
}


/* Lexes the input at the beginning of the token. */
int ralexer_lexchar(RaLexer * self, RaToken * token, int ch, int ch_prev) {
  /* There was an escape character before now. */
  if (ch_prev == '\\') {
    /* Ignore escaped newlines. */
    if (ch == '\n') return RA_LEX_SWALLOW;
    /* Ignore escaped newlines. */
    if (ch == '\r') return RA_LEX_SWALLOW;
  }
  /* In \r\n only let \r have effect. */
  if (ch_prev == '\r') {
    if (ch == '\n') return RA_LEX_SWALLOW;
  }
  
 switch (ch) {
    case '"'    :   return ralexer_lex_string_with_escapes(self, token, ch);
    case '\''   :   return ralexer_lex_string(self, token, ch);
    case '`'    :   return ralexer_lex_string(self, token, ch);
    case '#'    :   return ralexer_lex_comment_hash(self, token, ch);
    case EOF    :   return ralexer_lex_eof(self, token, ch);
    /* Fallthrough list for numbers. Numbers must beginbwn with a digit or - 0.0 is 
     * OK  and so is -0xbabe, but .0 is not.  */
    case '-'    :   /* fallthrough */
    case '0'    :   /* fallthrough */
    case '1'    :   /* fallthrough */
    case '2'    :   /* fallthrough */
    case '3'    :   /* fallthrough */
    case '4'    :   /* fallthrough */
    case '5'    :   /* fallthrough */
    case '6'    :   /* fallthrough */
    case '7'    :   /* fallthrough */
    case '8'    :   /* fallthrough */
    case '9'    :   return ralexer_lex_number(self, token, ch);
    case '\n'   :   return ralexer_lex_eos(self, token, ch);
    case '\r'   :   return ralexer_lex_eos(self, token, ch);
    case ';'    :   return ralexer_lex_eos(self, token, ch);
    case '('    :   return ralexer_lex_blockstart(self, token, ch);
    case ')'    :   return ralexer_lex_blockend(self, token, ch);
    case '['    :   return ralexer_lex_blockstart(self, token, ch);
    case ']'    :   return ralexer_lex_blockend(self, token, ch);
    case '{'    :   return ralexer_lex_blockstart(self, token, ch);
    case '}'    :   return ralexer_lex_blockend(self, token, ch);
    /* Wild escape character, */
    case '\\'   :   return RA_LEX_SWALLOW;
    /* Ignore whitespace. */
    case ' '    :   /* fallthrough */
    case '\t'   :   return RA_LEX_SWALLOW;
    default     :   return ralexer_lex_word(self, token, ch);
  }
}

/* Gets the next token from the lexer. This will call
* the get that was given n initialization until a token is read, eof is reached, or an error 
* is detected. Token may not be null, but may not be initialized. 
* 
* Returns RA_LEX_CONTINUE if lmexing may continue or RA_LEX_DONE if lexing is done.
* 
*/
int ralexer_lex(RaLexer * self, RaToken * token) {
  int ch, res;
  while (TRUE) { 
    ch  = lexer_getnetxchar(self); 
    res = lexer_lexchar(ch, self->ch_prev);
    if (res != RA_LEX_SWALLOW) {
      /* Empty buffer and return. */
      bassigncstr(self->buffer, "");
      return res;
    }
  }
}


























