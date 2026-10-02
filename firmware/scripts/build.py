# GC-01 Pro RU: C-only startup is implemented in gc01.c. MIT.
Import("env")
env.Append(LINKFLAGS=["-nostartfiles", "-flto"])
