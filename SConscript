Import('RTT_ROOT')
Import('rtconfig')
from building import *

cwd     = GetCurrentDir()

# add button src files.
if GetDepend('PKG_USING_BUTTON'):
    src += Glob('src/button.c')
if GetDepend('PKG_USING_BUTTON_SAMPLE'):
    src += Glob('samples/button_sample.c')
# add button include path.
path = [cwd+'/inc']

# add src and include to group.
group = DefineGroup('button', src, depend = ['PKG_USING_BUTTON'], CPPPATH = path)

Return('group')