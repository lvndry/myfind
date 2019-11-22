# TODO

* -exec \+
* -perm
* memory leak
* coding style
* -name print either twice or non because of check for options.d in ls
* execute / executedir could be one function
* Better error handling

# Enhancement

Instead of doing one parse function per expression I can create 3 functions: parse_type, parse_operand, parse_test with same protopyes
struct token parse_x(char *argv[], int *cursor, enum_token_type);

as the function returns a token I can aggregate all the function to way less
seems like still need a seperate function for exec-family expressions

Il y a un probleme quand il y a un exec il ne faut jamais print. Sinon il faut print

e.g find -name bar -o -exec {} \; ne dois faire le exec que si le -name bar return 0; Pour autant le -name bar ne pas etre afficher