
CUTest - The C Unit Test framework
==================================

Author: AiO (AiO Secure Teletronics - https://www.aio.nu)

Project site: https://github.com/aiobofh/cutest

Thank you for downloading the CUTest framework! I hope it will make
your software development, using test-driven design an easier task.

CUTest is a C testing framework written in pure C. The idea behind
CUTest is to provide a platform independent C Unit Test framework,
but I guess it will only work in Linux for GCC anyway :). It's the
thought that counts. Please join me and port it to other enviroments.

The CUTest framework is tightly bound to a very specific build
system layout too. So let's admit that GNU Make is also needed.

Features
--------

* Automated generation of controllable mocks for all C-functions
  called by the design under test.
* C-Function stubbing
* Generic asserts in 1, 2 and 3 argument flavors.
* JUnit XML reports for Jenkins integration
* Very few dependencies to other tools (`echo`, `gcc`, `as`, `make`,
  `which`, `grep`, `sed`, `rst2html`, `less`, 'nm` and `cproto`)
* In-line documentation to ReSTructured Text or HTML
  (requires additional tools: `grep`, `sed` and `rst2html`)

Organize your directories
-------------------------

The CUTest framework make some expecations but should be fairly
flexible by default the paths are set to support a flat structure
with test-case source files and design under test source files in
the same folder.

However you MUST name your test-case source file as the
corresponding design under test source file.

So... If you have a file dut.c you need a dut_test.c file to test
the functions in the dut.c file.

Here is a flat example::

  src/dut.c       <- your program (design under test)
  src/dut_test.c  <- test suite for dut.c (must #include cutest.h)
  src/Makefile

... So keep your clean:-target clean ;).

Here is a more complex example::

  my_project/src/dut.c
  my_project/src/Makefile
  my_project/test/dut_test.c
  my_project/test/Makefile

In this case you need to set the CUTEST_SRC_DIR=../src in the test
Makefile in my_project/test/Makefile.

Include paths
-------------

If you have many -I../path/to/somewhere passed to the build of your
project collect all -I-flags into the CUTEST_IFLAGS variable before
inclusion of cutest.mk and the include paths will be passed on to
cproto and the test-runner build automatically. Hopefully easing
your integration a bit.

Example
-------

foo_test.c::

  #include "cutest.h"

  test(adder_shall_add_two_arguments_and_return_their_sum) {
    assert_eq(3, adder(1, 2), "adder shall return 3 = 1 + 2");
    assert_eq(6, adder(3, 3), "adder shall return 6 = 3 + 3");
  }

  test(foo_shall_call_the_adder_function_once_with_correct_args) {
    // When calling foo() the adder(i, j) funciton call will call a
    // mock.
    foo(1, 2);
    assert_eq(1, cutest_mock.adder.call_count,
              "adder shall be called once");
    assert_eq(1, cutest_mock.adder.args.arg0,
              "first argument shall be 1");
    assert_eq(2, cutest_mock.adder.args.arg1,
              "second argument shall be 2");
  }

  test(foo_shall_return_the_adder_functions_result_unmodified) {
    cutest_mock.adder.retval = 123456;
    assert_eq(123456, foo(1, 2),
              "foo shall return adder's return value");
  }

foo.c::

  int adder(int a, int b) { return a + b; }
  int foo(int i, int j) { return adder(a, b); }

Makefile for a simple directory structure::

  CUTEST_SRC_DIR=./ # If you have a flat directory structure
  include /path/to/cutest/src/cutest.mk


Makefile for automatically downloading cutest into your project::

  CUTEST_SRC_DIR=./ # If you have a flat directory structure
  include cutest/src/cutest.mk
  cutest:
     git clone https://github.com/aiobofh/cutest.git
  clean::
     rm -rf cutest

Command line to build a test runner and execute it::

  $ make foo_test
  $ ./foo_test
  ...

Command line to run all test suites::

  $ make check
  ...


There are more examples available in the examples folder.

In-line documentation to ReSTructured Text and/or HTML
------------------------------------------------------

You can always read the cutest.h file, since it's the only one
around.

When you have inclued the cutest.mk makefile in your own Makefile
you can build the documentation using::

  $ make cutest_help       # Will print out the manual to console
  $ make cutest_help.html  # Generate a HTML document
  $ make cutest_help.rst   # Generate a RST document

To compile the test runner you should never ever have
`CUTEST_RUN_MAIN` nor `CUTEST_MOCK_MAIN` defined to the compiler.
They are used to compile the *CUTest test runner generator* and
the *CUTest mock generator* respectively.

The test() macro
----------------

Every test is defined with this macro.

Example::

  test(main_should_return_0_on_successful_execution)
  {
    ... Test body ...
  }

The assert_eq() macro
---------------------

This macro makes it easy to understand the test-case flow, it is a
variadic macro that takes two or three arguments. Use the form you
feel most comfortable with.

Example::

  ...
  assert_eq(1, 1, "1 should be eqial to 1");
  ...
  assert_eq(1, 1);
  ...

Test initialization
-------------------

In between every test() macro the CUTest framework will clear all
the mock controls and test framwork state so that every test is
run in isolation.

Test execution
--------------

When executing tests the elapsed time for execution is sampled and
used in the JUnit report. Depending on command line options an
output is printed to the console, either as a short version with
'.' for successful test run and 'F' for failed test run, but if set
to verbose '-v' '[PASS]' and '[FAIL]' output is produced. What
triggers a failure is if an assert_eq() is not fulfilled.

If the test runner is started with verbose mode '-v' the offending
assert will be printed to the console directly after the fail. If
in normal mode all assert-failures will be collected and printed
in the shutdown process.

Shutdown process
----------------

At the end of the execution the CUTest test-runner program will
output a JUnit XML report if specified with the -j command line
option.


CUTest mock generator
=====================

This is a tool that can be used to generate mock-up functions. It
inspects a specified source-code file (written i C language) and
looks for uses of the funcitons listed in a file which list all
function that is replaceable with a mock when developing code using
test-driven design.

Requirements
------------

To be able to generate well formatted function declarations to
mutate into mock-ups this tool make use of the ``cproto`` tool.

How to compile the tool
-----------------------

Just include the cutest.mk makefile in your own Makefile in your
folder containing the source code for the *_test.c files.

The tool is automatically compiled when making the check target
But if you want to make the tool explicitly just call::

  $ make cutest_mock

Usage
-----

If you *need* to run the tool manually this is how::

  $ ./cutest_mock design_under_test.c mockables.lst /path/to/cutest

And it will scan the source-code for mockable functions and
output a header file-style text, containing everything needed to
test your code alongside with the `cutest.h` file.

The mockables.lst is produced by `nm dut.o | sed 's/.* //g'`.

However, if you use the Makefile targets specified in the beginning
of this document you will probably not need to run it manually.

Mock-ups
--------

The cutest_mock tool scans the design under test for call() macros,
and create a mock-up control stucture, unique for every callable
mockable function, so that tests can be fully controlled.

The control structures are encapsulated in the global struct
instance called 'mocks'.

In a test they can be accessed like this::

  mocks.<name_of_called_function>.<property>...

If you have::

  FILE* fp = fopen("filename.c", "r");

in your code, a mock called cutest_mock_fopen() will be generated.
It will affect the cutest_mock.fopen mock-up control structure.

For accurate information please build your <dut>_mocks.h file and
inspect the structs yourself.

Stubbing
--------

To stub a function in your design under test you can easily write
your own stub in your test-file, just pointing the
cutest_mock.<dut>.func function pointer to your stub.


CUTest proxification tool
=========================

The cutest_prox tool reads an elaborated assembler source file and
a file containing a list of mockable functions to produce a new
assembler output with all calls to local (or other) functions
replaced by CUTest mocks.

How to build the tool
---------------------

Makefile::

Just include the cutest.mk makefile in your own Makefile in your
folder containing the source code for the *_test.c files.

The tool is automatically compiled when making the check target.
But if you want to make the tool explicitly just call::

  $ make cutest_prox

Usage
-----

If you *need* to run the tool manually this is how::

  $ ./cutest_prox dut_mockables.s dut_mockables.lst

And an assembler file will be outputed to stdout.


CUTest test runner generator
============================

The cutest_run tool will parse your test suite and produce an
executable program with some command line options to enable you to
control it a little bit.

How to build the tool
---------------------

Makefile::

Just include the cutest.mk makefile in your own Makefile in your
folder containing the source code for the *_test.c files.

The tool is automatically compiled when making the check target.
But if you want to make the tool explicitly just call::

  $ make cutest_run

Usage
-----

If you *need* to run the tool manually this is how::

  $ ./cutest_run dut_test.c dut_mocks.h

And it will scan the test suite source-code for uses of the `test()`
macro and output a C program containing everything needed to test
your code alongside with the `cutest.h` file.

However, if you use the Makefile targets specified in the
beginning of this document you will probably not need to run it
manually.

The test runner program
-----------------------

The generated test runner program will inventory all the tests in
the specified suite and run them in the order that they appear in
the suite.

The first thing that happens is the Startup process, then all
tests are run in isolation, followed by the Shutdown process.
