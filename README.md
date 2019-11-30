# Myfind

Simplified version of [find(1)](http://man7.org/linux/man-pages/man1/find.1.html) command.

This is college project aimed to get to know on parsing, ASTs, writing complex commands.

## Available options and expressions:

The following descriptions are from the Linux manual of  [find(1)](http://man7.org/linux/man-pages/man1/find.1.html).

### Options

-P: Never follow symbolic links. This is the default behaviour. When **find** examines or prints information a file, and the file is a symbolic link, the information used shall be taken from the properties of the symbolic link itself.

-L: Follow symbolic links. When **find** examines or prints information about files, the information used shall be taken from the properties of the file to which the link points, not from the link itself (unless it is a broken symbolic link or **find** is unable to examine the file to which the link points). If -L is in effect and **find** discovers a symbolic link to a subdirectory during its search, the subdirectory pointed to by the symbolic link will be searched.

-H: Cause the file information and file type returned for each symbolic link specified on the command line to be those of the file referenced by the link not the link itself. If the referenced file does not exist, the file information and type will be for the link itself. File information of all symbolic links not on the command line is that of the link itself.

-d (Free-BSD feature): Cause **find** to perform a depth-first traversal.

### Tests

-group _gname_: File belongs to group _gname_.

-name _pattern_: Base of file name matches shell pattern _pattern_. Because the leading directories are removed, the file names considered for a match with **-name** will never include a slash, so `-name a/b' will never match anything.

-newer _reference_:
Time of the last data modification of the current file is more recent than that of the last data modification of the reference file. If _reference_ is a symbolic link and the -H option or the -L option is in effect, then the time of the last data modification of the file it points to is always used.

-perm _mode_: File's permission bits are exactly _mode_ (octal).

-perm _-mode_: All of the permission bits mode are set for the file. (octal)

-perm _/mode_: Any of the permission bits mode are set for the file. (octal)

-type _c_

    b      block (buffered) special

    c      character (unbuffered) special

    d      directory

    p      named pipe (FIFO)

    f      regular file

    l      symbolic link; this is never true if the -L option is in effect
           unless the symbolic link is broken.

    s      socket

-user _uname_: File is owned by user _uname_

### Actions

-delete:
Delete files; true if removal succeeded. If the removal failed, an error message is issued.  If **-delete** fails, **find**'s exit status will be 1.

-exec _command_:
Execute command; true if 0 status is returned. All following arguments to **find** are taken to be arguments to the command until an argument consisting of *;* is encountered. The string *{}* is replaced by the current file name being processed everywhere it occurs in the arguments to the command not just in arguments where it is alone, as in some versions of **find**. Both of these constructions might need to be escaped (with a *\\*) or quoted to protect them from expansion by the shell.

-exec _command_ {} +:
This variant of the **-exec** action runs the specified command on the selected files, but the command line is built by appending each selected file name at the end; the total number of invocations of the command will be much less than the number of matched files. The command line is built in much the same way that xargs builds its command lines. Only one instance of '{}' is allowed within the command, and (when **find** is being invoked from a shell) it should be quoted (for example, '{}') to protect it from interpretation by shells.  The command is executed in the starting directory. If any invocation with the + form returns a non-zero value as exit status, then **find** returns a non-zero exit status.

-execdir _command_:
Like **-exec**, but the specified command is run from the subdirectory containing the matched file, which is not normally the directory in which you started **find**. As with **-exec**, the {} should be quoted if **find** is being invoked from a shell. This a much more secure method for invoking commands, as it avoids race conditions during resolution of the paths to the matched files.

## Examples

```bash
./myfind
./myfind -name foo
./myfind -type d
./myfind -perm 636
./myfind -exec echo {} \;
```
