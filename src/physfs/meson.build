physfs_srcs = files([
	'physfs.c',
	'physfs_archiver_7z.c',
	'physfs_archiver_dir.c',
	'physfs_archiver_grp.c',
	'physfs_archiver_hog.c',
	'physfs_archiver_iso9660.c',
	'physfs_archiver_mvl.c',
	'physfs_archiver_qpak.c',
	'physfs_archiver_slb.c',
	'physfs_archiver_unpacked.c',
	'physfs_archiver_vdf.c',
	'physfs_archiver_wad.c',
	'physfs_archiver_zip.c',
	'physfs_byteorder.c',
	'physfs_unicode.c',

	'physfs_platform_haiku.cpp',
	'physfs_platform_os2.c',
	'physfs_platform_posix.c',
	'physfs_platform_qnx.c',
	'physfs_platform_unix.c',
	'physfs_platform_windows.c',
	'physfs_platform_winrt.cpp',
])

if target_machine.system() == 'darwin' # untested
	physfs_srcs += file('physfs_platform_apple.m')
endif
