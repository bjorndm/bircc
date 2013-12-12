
#include <ctype.h>
#include <string.h>

/* Helper functions for RAKU. */

/** Helper for calling any of the isXXX functions 
based on the first character expression string
a call isalnum on ch and return the result.
A call isalpha on ch and return the result.
b call isblank on ch and return the result.
c call iscntrl on ch and return the result.
d call isdigit on ch and return the result.
g call isgraph on ch and return the result.
l call islower on ch and return the result.
p call isprint on ch and return the result.
P call ispunct on ch and return the result.
u call isspace on ch and return the result.
x call isxdigit on ch and return the result.
any other first character: return FALSE.
*/
int cstr_charis(const char * expression, int ch) {
  char first;
  if (!expression) return FALSE;
  first = expression[0];
  switch(first) {
    case 'a': return isalnum(ch);
    case 'A': return isalpha(ch);
    case 'b': return isblank(ch);
    case 'c': return iscntrl(ch);
    case 'd': return isdigit(ch);
    case 'g': return isgraph(ch);
    case 'l': return islower(ch);
    case 'p': return isprint(ch);
    case 'P': return ispunct(ch);
    case 's': return isspace(ch);
    case 'u': return isupper(ch);
    case 'x': return isxdigit(ch);
    default: return FALSE;
  }
  // cannot reach this place
}



/** Ultra-simple matching of a single character ch
versus a string expression of allowed characters or instructions. 
The expression is interpreted as follows: 
$a call isalnum on ch and return the result.
$A call isalpha on ch and return the result.
$b call isblank on ch and return the result.
$c call iscntrl on ch and return the result.
$d call isdigit on ch and return the result.
$g call isgraph on ch and return the result.
$l call islower on ch and return the result.
$p call isprint on ch and return the result.
$P call ispunct on ch and return the result.
$s call isspace on ch and return the result.
$u call isupper on ch and return the result.
$x call isxdigit on ch and return the result.
@ must be followed by a list of characters, strchr is called on that list.
^ must be followed by a list of characters, strchr is called on that list,
  and it's logical opposite is returned.
. matches any character and always returns true.
empty string or NULL: matches nothing and always returns false.
A ! prefix to any of these means to apply the 
C ! operator to the rest of the expression's result.
If expression starts with any other character, FALSE is returned as well.
*/
int cstr_simplematch(const char * expression, int ch) {
  char first;
  if (!expression) return FALSE;
  first = expression[0];
  switch(first) {
    case '\0': return FALSE;
    case '!' : return !cstr_simplematch(expression+1, ch);
    case '@' : return (strchr(expression+1, ch) != NULL);
    case '^' : return (strchr(expression+1, ch) == NULL);
    case '.' : return TRUE;
    case '$' : return cstr_charis(expression+1, ch);
    default: return FALSE;
  }
  // can't reach here    
}


