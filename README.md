# MIPS-I CPU Emulator

This is a fully working MIPS-I CPU emulator and tester. We implemented everything except floating point and coprocessor instructions; exceptions were implemented using pre-defined return codes. This work was completed in October 2016. Learn more [here.](http://www.thomasnugent.me.uk/university/02_mips.php)

***

## How To Use

The project can be built using `make all` and the tester can be run using `./src/tn1115/test_mips`. This runs my cpu against my own tester. The coursework was set up such that the cpu and tester interacted through a defined API. This meant anyones tester can be run against anyones cpu, and allowed them to both be marked separately.

***

## Credits

The makefile, fragments and include folders were all provided to us. All other code was written by me.
