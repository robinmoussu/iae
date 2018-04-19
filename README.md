Iacta Alea Est
==============

`iae` is a simple dice parser. Is goal is to be a (nearly) drop-in replacement of [DiceParser](https://github.com/Rolisteam/DiceParser). The main difference is that some operators were redondants and have been cleaned.

It name come from *iacta alea est* (the die is cast). It is really short, witch is conviniant to roll a lot of dices in a row!

Usage:
------

Syntax:
-------

The syntax of `iae` is similar to the one of DiceParser. It consist of a dice roll followed by a list of operation that modify it. For example `2d10` will roll 2 dices with 10 faces, and `5d6+3` will roll 5 dices with 6 faces, and add 3 to the total. By default the detail of all rolled dices is displayed, as well of the total obtained.

The complete grammar can be found in `src/grammar.cpp`

### Dices:

`ndf` or `nDf`: Roll `n` dices with `f` faces. Example: `5d6`

`nl[name1,name2,...]` or `nl[name1,name2,...]`: Roll `n` dices. The faces are describes by strings separated by a comma, inside square bracket. Example: `3l[10 cooper,a rusty sword, some rag]`

### Modifier:

TODO

### Multiple expression:

Multiple expression separated by `;` can be computed in a row. Example: `1d10+2;2d6` will roll 1 dice with 10 faces and add 2 to the total, then 2 dices with 2 faces.

Multiples dices roll can be grouped between parenthesis in a comma separated list. Example: `(2d6;1d10)k[>5]` will roll two 6-faced dice, and one 10-faced dice, and keep are dices who's result is superior to 5.

The roll and result of a previous expression can be accessed with `!n` and `$n` respectively (with `n` being the index of the expression). For example in `1d10;(2d20;2d21)/4;3d30`. The expression `1d10` is `!1`, `(2d20;2d21)/2` is `!2`, and `3d30` is `!3`. When rolling the dices if you get `6` for the 10-faced dice, `6 4` and `12 8` for the 20 and 21-faced dices, and `5 15 30` for the 30-faced dice, then `$1 = 6`, `$2 = 5` and `$3 = 60`.

`!n`: Re-use the rolls of a previous expression. You can use `!n` to replace a dice roll in the expression. Example `5d8+1;2d10;(!1,!2)k[>=6]` will roll 5 dice with 8 faces and 2 dices with 10 faces, and keep all dices whoses faces have more than 5. If you get `5 8 6 2 1` with the five 8-faced dices and `4 10` with the two 10-faced dices, then the 8-faced dices will see their result increased by one (`6 9 2 3 1`), and finally you will keep `6 9` from the 8-faced dices, and `10` from the 10-faced dices.

`$n`: Get the result of a previsous expression

Since you can only access to the result of a previous expression, the following expression are ill-formed: `$2d6;1d10`, `1d10;2d$3;$1d2`, `!2k[>8];10d10`.

### Output:

TODO

### Comments:

The beggining of an expression can be commented by a string in backticks `\``. If you need to use backticks, you can escape it with `\\`. Example: `\`Inteligence\`2d20;\`difficulty\`4d5`;!1i[>$2]{\`success\`"You get ${1d4} gold"}{\`fail\`"You got a headache"}`.

Installation:
-------------

### Dependencies

Build system (required):
- [build2](https://www.build2.org/)

Compiler (any c++17 compliant compiler). Tested with
- gcc 7.3

### build

Just use `b` in the root directory (it's the equivalent of make for build2).

The binaries are created in the `bin` directory.

To do:
------

- Add alias
- Add LICENCE.txt
- const and constexpr correctness

- Add unit tests [catch2](https://github.com/catchorg/Catch2)
- Add [integration tests](https://www.build2.org/build2/doc/build2-testscript-manual.xhtml)
- Use [standardise](https://github.com/foonathan/standardese) to generate documentation
- Use [fmt](https://github.com/fmtlib/fmt) for input and output
- Separate the library part from the tui
- Add support for clang and cl (windows compiler)
- Build on windows
- Add [command line argument parsing](https://github.com/docopt/docopt.cpp), and build-in help 
- Add man page using [help2man](https://www.gnu.org/software/help2man/), see also [quickstart](https://www.cyberciti.biz/faq/linux-unix-creating-a-manpage/) and [cli](https://stage.build2.org/cli?q=cli)
- generate system packages
- add clang-format pre-commit hook
- code coverage ([see](http://mesonbuild.com/Feature-autodetection.html))
- add transiflex translation support
- test [kythe](https://github.com/google/kythe)

- investiguate why I loosed nices error messages with print_type if needed

