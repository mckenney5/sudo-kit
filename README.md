sudo-kit
==============
sudo-kit — _A trojan for bash and sudo to obtain user passwords_


## Description
This program is designed to be run on a target computer, where it will
then append the user's bashrc file and be called any time sudo is called.
When a target types in their password, it is saved so it can be accessed
later.

## Project Meta
This software is in a **working alpha** state.
The software may not work and may have drastic changes in the future.

## License
This code is licensed under the MIT license. See 'LICENSE' for details. 

## Compiling and Testing
This program allows a couple of compiling options.

The simplest way is to compile by `make`

##### Make Options:
* `make debug`	: adds debugging symbols to the program, along with verbose output
* `make static`	: statically links program (note: this should be done only with musl clib)
* `make`	: makes the program and mimics sudo with its outputs

## Attack Scenario
The best attack scenario for this program is that it is used as a payload
for a trusted USB attack. The attacker would add their own payload prior
to uploading the program, a good example would be having the program call
netcat, so when a user types in their password, their password is available
by connecting to a set port where the password would be spit out then the
connection closes. With a simple script, this would be a simple way
to get root access via ssh or later for a physical attack.

## Limitations and Ways to Combat This
Currently, this program cannot check if the first password given is correct.
This means that it could grab the wrong password.

To combat this program, one could:
* require root to write to the bashrc file 
* not leaving their computer unattended (even if locked)

## Exit codes
0 - Exited normally
1 - File IO error
2 - No args supplied
3 - Could not dynamically alloc memory
4 - Could not expand dynamically alloc'd memory


If the -u option is used, the exit code is that of system("mv")

