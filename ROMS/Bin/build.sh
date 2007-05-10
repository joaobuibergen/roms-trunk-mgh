#!/bin/csh -f
# 
# svn $Id$
#:::::::::::::::::::::::::::::::::::::::::::::::::::::::::: John Wilkin :::
# Copyright (c) 2002-2007 The ROMS/TOMS Group                           :::
#   Licensed under a MIT/X style license                                :::
#   See License_ROMS.txt                                                :::
#::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#                                                                       :::
# ROMS/TOMS Compiling Script                                            :::
#                                                                       :::
# Script to compile an user application where the application-specific  :::
# files are kept separate from the ROMS source code.                    :::
#                                                                       :::
# Q: How/why does this script work?                                     :::
#                                                                       :::
# A: The ROMS makefile configures user-defined options with a set of    :::
#    flags such as ROMS_APPLICATION. Browse the makefile to see these.  :::
#    If an option in the makefile uses the syntax ?= in setting the     :::
#    default, this means that make will check whether an environment    :::
#    variable by that name is set in the shell that calls make. If so   :::
#    the environment variable value overrides the default (and the      :::
#    user need not maintain separate makefiles, or frequently edit      :::
#    the makefile, to run separate applications).                       :::
#                                                                       :::
# The only pitfall (so far) to this approach is that if the makefile    :::
# never changes, GNU "make" does not necessarily know to update the     :::
# dependencies that are written by the "make depend" command into       :::
# Compilers/MakeDepend. To guard against this a "make depend" step      :::
# is forced in the script below. Even if the MakeDepend file changes,   :::
# "make" will only recompile a file if it determines an object is out   :::
# of date, so there overhead in re-evaluating the dependencies small.   :::
#                                                                       :::
#::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

# Set the CPP option defining the particular application. This will
# determine the name of the ".h" header file with the application 
# CPP definitions.

setenv ROMS_APPLICATION     CBLAST

# Set a local environmental variable to define the path to the directories
# where all this project's files are kept.

setenv MY_ROOT_DIR          /home/arango/ocean/toms/repository
setenv MY_PROJECT_DIR       ${MY_ROOT_DIR}/Projects/cblast

# The path to the user's local current ROMS source code. 
#
# If using svn locally, this would be the user's Working Copy Path (WCPATH). 
# Note that one advantage of maintaining your source code locally with svn 
# is that when working simultaneously on multiple machines (e.g. a local 
# workstation, a local cluster and a remote supercomputer) you can checkout 
# the latest release and always get an up-to-date customized source on each 
# machine. This script is designed to more easily allow for differing paths 
# to the code and inputs on differing machines. 

setenv MY_ROMS_SRC          ${MY_ROOT_DIR}/branches/arango

# Other user defined environmental variables. See the ROMS makefile for
# details on other options the user might want to set here. 

 setenv USE_MPI             on
 setenv USE_MPIF90          on
 setenv FORT                pgi

#setenv USE_DEBUG           on
#setenv USE_ARPACK          on
 setenv USE_LARGE           on

# The rest of this script sets the path to the users header file and
# analytical source files, if any. See the templates in User/Functionals.

 setenv MY_HEADER_DIR       ${MY_PROJECT_DIR}/Forward

#setenv MY_ANALYTICAL_DIR   ${MY_PROJECT_DIR}/Functionals

# Put the binary to execute in the following directory.

 setenv BINDIR              ${MY_PROJECT_DIR}/Forward

# Put the f90 files in a project specific Build directory to avoid conflict
# with other projects. 

 setenv SCRATCH_DIR         ${MY_PROJECT_DIR}/Build

# Go to the users source directory to compile. The options set above will
# pick up the application-specific cdoe from the appropriate place.

 cd ${MY_ROMS_SRC}

# Force re-evaluting the make dependencies. 
#
# WARNING: Be aware that if you compile two different applications at once
# models at once you might have an inconsistent MakeDepend file).

 make clean
 make depend

# Compile (the binary will go to BINDIR set above).  
# (-j option distributes compilation to multiple processors)

 make