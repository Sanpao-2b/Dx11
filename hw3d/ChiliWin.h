#pragma once

// target Windows 7 or later
#define _WIN32_WINNT 0x0601
#include <sdkddkver.h>
/*
	������Щ#defines ������һЩû�õ�Windows.h�ж���ĺ꣬
	��������һЩwindows����ʱ������һЩδ֪�Ĵ��󣬿��Գ���ɾ����һЩ����ȫ�������µĺ꣬����������ӱ���ʱ��
*/

#define WIN32_LEAN_AND_MEAN
#define NOGDICAPMASKS
#define NOSYSMETRICS
#define NOMENUS
#define NOICONS
#define NOSYSCOMMANDS
#define NORASTEROPS
#define OEMRESOURCE
#define NOATOM
#define NOCLIPBOARD
#define NOCOLOR
#define NOCTLMGR
#define NODRAWTEXT
#define NOKERNEL
#define NONLS
#define NOMEMMGR
#define NOMETAFILE
#define NOMINMAX
#define NOOPENFILE
#define NOSCROLL
#define NOSERVICE
#define NOSOUND
#define NOTEXTMETRIC
#define NOWH
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX
#define NORPC
#define NOPROXYSTUB
#define NOIMAGE
#define NOTAPE
#define STRICT

//ǰ�涨���NO NO NO��Щ�� ������ȥ��Windows.h�е���Ӧ�ĺ궨�� ����NOMINMAX �ǽ��std::min() std::max()��ͻ��
#include <Windows.h>
