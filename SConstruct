# This is a Python script that the SCons tool uses to build your source code.
# Author: Vishesh Dokania <vdokania@ucla.edu>

# Initialize build environment
env = Environment()

# Customize build settings
env.Append(MODE = 'release') # If you want to build in debug mode you can do that as well
env.Append(CPPFLAGS = '-g -O3') # Feel free to add some other compiler optimizations
env.Append(CPPPATH = ['src', '/w/class/ee201a/ee201ata/oa/include/oa']) # Tell SCons where to find C/C++ header files
env.Append(LIBPATH = '/w/class/ee201a/ee201ata/oa/lib/linux_rhel40_64/opt') # Tell SCons where to find OA shared libraries
env.Append(LIBS = ['oaBase', 'oaCommon', 'oaDesign', 'oaDM', 'oaPlugIn', 'oaTech', 'dl']) # Tell SCons which shared libraries to link

# List all C++ source files that should be compiled
sources = [
	'src/main.cpp',
	'src/InputOutputHandler.cpp',
	'src/ProjectInputRules.cpp',
	'src/OAHelper.cpp',
]

# Set up default build
PinAssignBuild = env.Program(target = 'PinAssign', source = sources)

# Build it!
Default(PinAssignBuild)


#===================================================================
# Post-pin assignment verification

sources_paverify = [
	'PAVerification/PAVerify.cpp',
	'PAVerification/InputOutputHandler.cpp',
	'PAVerification/ProjectInputRules.cpp',
	'PAVerification/OAHelper.cpp',
	'PAVerification/Verify.cpp',
]

# Set up default build
PAVerifyBuild = env.Program(target = 'PAVerify', source = sources_paverify)

# Build it!
Default(PAVerifyBuild)
