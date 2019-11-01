# TODO

I need to think about the fact that:

```bash
find [option] [path] [expression]
```

I have to handle of every case where I only got one them (wich is the easy part);
However if an option is invalid and then add path let's say

```bash
find -LP . -print
```

I can't directly tell if the option isn't correct only by parsing the options because it could be an expression
So the strategy is that we only handle errors in expression side. It means that for example if the first argument isn't an option the optend is at **0**, the pathend is at **0** and from there I start analyzing my expression with the AST.


We have to handle the following expressions:

* -name <name>
* -type <type>
* - o
* - a
* -newer </path/to/file>
* -perm
* -group <groupname>
* -user <username>
* -print
* -delete (implies -d option)
