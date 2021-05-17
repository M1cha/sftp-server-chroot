This allows using openssh's `sftp-server` in chroot mode without `sshd` or
modifying their source-code.

# Why?
When doing reverse-sshfs using `ncat` and `ssh` port forwarding you have to
trust the server to not hijack your `sshfs` call to gain access to your whole
system. This prevents that by limiting `sftp-server`'s access using kernel
functionality.

# How does it work?
`sshd` includes the same code as `sftp-server` in their binary but neither
provides the sftp-functionality as a library.

Both call the function `sftp_server_main` passing the current user's
`struct passwd` as an argument. `sshd` does the chroot before calling that
function. You could also do the chroot before starting `sftp-server` but that
would require making all the dependencies (linker, shared library, /dev/null)
available in there.

So instead, I wrote a small preload library which uses `sftp-server`'s initial
`getpwuid` call to do the chroot and return a `struct passwd` for the user that
you initially inteded to run the server at(chroot needs root permissions).

# How to run
The specifics differ depending on your setup but the minimum you have to do is
set some environment variables:
- `export LD_PRELOAD="/path/to/libsftp_server_chroot_preload.so"`
- `export SFTP_CHROOT_DIR="/path/to/shared/directory"`
- `export SFTP_UID="DESIRED_UID"`

After chrooting, the process drops it's user and group privileges to that of the
user with the UID `DESIRED_UID`.
