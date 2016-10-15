# -*- mode: python -*-

block_cipher = None

a = Analysis(['{py_filename}'],
             pathex=['../prototypes/trabant'],
             binaries=None,
             datas=[ ('../sim/trabantsim.exe','sim'), ('../sim/OpenAL.dll','sim'), ('../sim/alut.dll','sim'), ('../sim/data','sim/data') ],
             hiddenimports=[],
             hookspath=[],
             runtime_hooks=[],
             excludes=[],
             win_no_prefer_redirects=False,
             win_private_assemblies=False,
             cipher=block_cipher)
pyz = PYZ(a.pure, a.zipped_data,
             cipher=block_cipher)
exe = EXE(pyz,
          a.scripts,
          a.binaries,
          a.zipfiles,
          a.datas,
          name='{name}',
          icon='trabant.ico',
          debug=False,
          strip=False,
          upx=True,
          console={is_console})
