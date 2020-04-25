from building import *
Import('rtconfig')
src     = []
cwd     = GetCurrentDir()

# add key src files.
if GetDepend('PKG_USING_KEY'):
    src += Glob('src/key.c')
if GetDepend('PKG_USING_KEY_SAMPLE'):
    src += Glob('samples/key_sample.c')
# add key include path.
path = [cwd+'/inc']

# add src and include to group.
group = DefineGroup('Key', src, depend = ['PKG_USING_KEY'], CPPPATH = path)

Return('group')