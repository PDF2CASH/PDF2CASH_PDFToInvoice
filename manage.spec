# -*- mode: python -*-

block_cipher = None


a = Analysis(['manage.py'],
             pathex=['/home/bruno/unb/PDF2CASH_PDFToInvoice'],
             binaries=[],
             datas=[],
             hiddenimports=['rest_framework', 'rest_framework.apps','corsheaders','mock','invoice.urls','rest_framework.authentication','rest_framework.permissions', 'rest_framework.negotiation','rest_framework.metadata','corsheaders.middleware','django.contrib.messages.middleware','django.contrib.auth.middleware','django.contrib.sessions.middleware','django.contrib.sessions.serializers',
             'rest_framework_jwt.authentication',
             'rest_framework_jwt.utils','rest_framework.parsers','worker.urls','apigateway.urls', '_sqlite3', 'packaging.version', 'packaging.specifiers', 'packaging.requirements','pyimod03_importers', '_ctypes','pandas'],
             hookspath=[],
             runtime_hooks=[],
             excludes=[],
             win_no_prefer_redirects=False,
             win_private_assemblies=False,
             cipher=block_cipher,
             noarchive=False)
pyz = PYZ(a.pure, a.zipped_data,
             cipher=block_cipher)
exe = EXE(pyz,
          a.scripts,
          [],
          exclude_binaries=True,
          name='manage',
          debug=False,
          bootloader_ignore_signals=False,
          strip=False,
          upx=True,
          console=True )
coll = COLLECT(exe,
               a.binaries,
               a.zipfiles,
               a.datas,
               strip=False,
               upx=True,
               name='manage')
